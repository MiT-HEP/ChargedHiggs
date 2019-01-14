import sys,os
from optparse import OptionParser
from subprocess import call, check_output
import math


parser=OptionParser()
feyndir="/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/FeynHiggs-2.14.0"
parser.add_option("","--exdir",default=feyndir+"/example/LHCHXSWG",help="LHCHXSWG dir [%default]")
parser.add_option("","--infile",default="mhmodm-LHCHXSWG.in",help="FeynHiggs input file [%default]")
parser.add_option("","--feyn",default=feyndir+"/build/FeynHiggs",help="feyn higgs [%default]")
parser.add_option("","--tmpdir",default="/tmp/"+os.environ["USER"]+"/" ,help="tmp dir [%default]")
parser.add_option("-x","--xvar",default="MHp" ,help="x variable [%default]")
parser.add_option("-y","--yvar",default="TB" ,help="y variable [%default]")
parser.add_option("-z","--zvar",default="GammaTot-Hp" ,help="z variable (@field) [%default]")
parser.add_option("--flags",default="42423110" ,help="FeynHiggs Flags [%default]")


parser.add_option("","--xrange",default="175,1025,50" ,help="x range [%default]")
parser.add_option("","--yrange",default="0.5,60.5" ,help="y range [%default]")
parser.add_option("-o","--outfile",default="width.root",help="output file [%default]")
parser.add_option("-a","--append",action='store_true',default=False,help="open output file in append mode [%default]")
parser.add_option("","--sqrtS",default=13,type='float',help="sqrtS [%default]")
opts,args=parser.parse_args()

import ROOT

xmin = float(opts.xrange.split(',')[0])
xmax = float(opts.xrange.split(',')[1]) 
try:
    xstep=float(opts.xrange.split(',')[2])
except IndexError:
    xstep=1

ymin = float(opts.yrange.split(',')[0])
ymax = float(opts.yrange.split(',')[1])
try:
    ystep=float(opts.yrange.split(',')[2])
except IndexError:
    ystep=1

# -v
verb = "-v"
zvar = opts.zvar.split('@')[0]
field=0
if '@' in opts.zvar: field=int(opts.zvar.split("@")[1])

#check if external tables are needed
#cmd=" ".join(["cp",opts.exdir+"/"+opts.infile,infile ])
hasTable=False
forig= open(opts.exdir+"/"+opts.infile)
for line in forig:
    if 'table' in line:
        to_copy=line.split()[1]
        cmd=" ".join(["cp","-v",opts.exdir+"/"+to_copy,opts.tmpdir+"/"+to_copy ])
        call(cmd,shell=True)
        hasTable=True
forig.close()

# define histogram name
name = zvar
if field>0: name+= "_%d"%field
fOut=ROOT.TFile.Open(opts.outfile,"RECREATE" if not opts.append else "UPDATE")
h=ROOT.TH2D(name ,"From FeynHiggs:" + opts.infile,int((xmax-xmin)/xstep),xmin,xmax,int((ymax-ymin)/ystep),ymin,ymax)
hrel=ROOT.TH2D(name + "_over_" + opts.xvar,"From FeynHiggs:" + opts.infile,int((xmax-xmin)/xstep),xmin,xmax,int((ymax-ymin)/ystep),ymin,ymax)
for xbin in range(0,h.GetXaxis().GetNbins()):
    for ybin in range(0,h.GetYaxis().GetNbins()):
        trial=0
        failed=True
        try:
          while failed and trial < 3:
            x=h.GetXaxis().GetBinCenter(xbin+1)
            y=h.GetYaxis().GetBinCenter(ybin+1)
            infile=opts.tmpdir+"/tmp.in"

            cmd=" ".join(["cp",opts.exdir+"/"+opts.infile,infile ])
            st=call(cmd,shell=True)
            if st != 0: print "[ERROR] while executing cmd:",cmd


            if hasTable and opts.xvar != 'MA0':
                if opts.xvar != "MHp":  raise ValueError("Unimplemented: Table + Xvar != MHp")
                mW=80.398
                print "-> Using Approximate xvar: MHp=sqrt(MW^2+MA0^2): mHp=",x,"mW=",80.398
                ma0 = math.sqrt(x*x-mW*mW)
                print "     ma0=",ma0
                cmd=" ".join(["sed","-i''","'s:^MA0.*:" +'MA0' +"     %.1f"%ma0 + ":g'",infile])
                st=call(cmd,shell=True)
                if st != 0: print "[ERROR] while executing cmd:",cmd
            else:
                cmd=" ".join(["sed","-i''","'s:^MA0.*:" +opts.xvar +"     %f"%x + ":g'",infile])
                st=call(cmd,shell=True)
                if st != 0: print "[ERROR] while executing cmd:",cmd

            cmd=" ".join(["sed","-i''","'s:^TB.*:" +opts.yvar +"     %f"%y + ":g'",infile])
            st=call(cmd,shell=True)

            fin=open(infile,"a")
            fin.write("\nprodSqrts        %f\n"%opts.sqrtS) ## 13TeV
            fin.close()
            if st != 0: print "[ERROR] while executing cmd:",cmd

            #%  Channel                  Gamma          BR             GammaSM        BRSM
            #%| A0-mu-mu             =    1.688382E-03   1.998031E-04   4.563789E-06   3.739643E-08
            cmd="cd %s &&"%opts.tmpdir + " ".join([opts.feyn,infile,opts.flags,"|", "grep",zvar ,"|" ,"tr -s ' '"])
            print "DEBUG:","calling Feyn cmd:   ",cmd
            out=check_output(cmd,shell=True)
            #print "DEBUG out:",out
            z=float(out.split('=')[1].split()[field])
            failed=False
        except:
            failed=True
            trial+=1
            print "[EXCEPTION] caught: trial",trial

        if verb !="": print out,z
        h.SetBinContent(xbin+1,ybin+1,z)
        try:
            hrel.SetBinContent(xbin+1,ybin+1,z/x)
        except ZeroDivisionError:
            pass

h.Write()
hrel.Write()
fOut.Close()
