import os,sys
import math
from subprocess import call,check_output
#### Threading
import threading
import time
from Queue import Queue

from optparse import OptionParser

usage=''' Optimization scripts for Hmm analysis'''
parser = OptionParser()
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)
parser.add_option("-i","--infile",help="InputFile. [%default]",default='Hmm.root')
parser.add_option("-o","--output",type='string' ,help="OutputFile. [%default]",default='optimization.txt')
parser.add_option("","--histname",type='string',help="HistName. [%default]",default='BtdMass_BB')
parser.add_option("-l","--lumi"	 ,dest="lumi"	,type='float' ,help="Target Luminosity. [%default]",default=None)
parser.add_option("-m","--mass"	 ,dest="mass"	,type='int' ,help="Target Mass [%default]",default=125)
parser.add_option("-p","--ncore"	 ,dest="ncore"	,type='int' ,help="Number of cores to use [%default]",default=10)
parser.add_option("","--boundaries" ,help="Boundaries. Include last boundary (first is 0). [%default]",default="0,0.2,0.3,1")
parser.add_option("-n","--n"	 ,dest="n"	,type='int' ,help="Optimize boundary n. [0,n-1] [%default]",default=1)
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")
(opts,args) = parser.parse_args()

##############################
print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")
from hmm import *
config= eval(opts.hmm)
config.Print()

############# Extra ###############
bkgMonteCarlos=["ZZZ","WZZ","WWZ","WWW","TTW","TTZ","TTTT","ZZ","WW","WZ","ST","TT","DY"]
#processes=["GluGlu","VBF","ZH","WPlusH","WMinusH","ttH"]
sigMonteCarlos = [ proc+"_HToMuMu_M%d"%opts.mass for proc in config.processes]
basedir="HmumuAnalysis/Vars"


import ROOT
fIn = ROOT.TFile.Open(opts.infile)
if fIn == None:print "'"+opts.infile+"': No such file or directory"
config.br()
#config.lumi() #target lumi
if opts.lumi!= None: config.lumi = lambda : opts.lumi
if opts.verbose: print "-> Using target luminosity",opts.lumi

def GetSigAndBkg(histname):
    ''' Take and Resum Sig and Bkg. This is a function in order to do simultaneously BB XO and XE categories. at some point'''
    sig_histo = None
    bkg_histo = None
    br=config.br();
    lumi=config.lumi()
    for mc in sigMonteCarlos:
        xsec=config.xsec(re.sub("_HToMuMu.*","",mc))
        if sig_histo== None: 
            sig_histo= fIn.Get(basedir+"/"+histname+"_"+mc)
            if sig_histo == None: 
                for hname in histname.split(','):
                    if sig_histo==None:
                        sig_histo= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if sig_histo==None:raise IOError("Unable to get: "+mc + ":"+basedir+"/"+hname+"_"+mc)
                    else:
                        tmp= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if tmp==None:raise IOError("Unable to get: "+mc + ":"+basedir+"/"+hname+"_"+mc)
                        sig_histo.Add(tmp)
            sig_histo.Scale(xsec*br*lumi)
        else:
            tmp= fIn.Get(basedir+"/"+histname+"_"+mc)
            if tmp == None: 
                for hname in histname.split(','):
                    if tmp == None:
                        tmp= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if tmp==None:raise IOError("Unable to get: "+mc + ":"+basedir+"/"+hname+"_"+mc)
                    else:
                        tmp2= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if tmp2==None:raise IOError("Unable to get: "+mc + ":"+basedir+"/"+hname+"_"+mc)
                        tmp.Add(tmp2)
                raise IOError("Unable to get: "+mc + ":"+basedir+"/"+histname+"_"+mc)
            tmp.Scale(xsec*br*lumi)
            sig_histo.Add(tmp)

    for mc in bkgMonteCarlos:
        if bkg_histo== None: 
            bkg_histo= fIn.Get(basedir+"/"+histname+"_"+mc)
            if bkg_histo==None:
                for hname in histname.split(','):
                    if bkg_histo==None:
                        bkg_histo= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if bkg_histo == None: raise IOError("Unable to get: "+mc+":"+basedir+"/"+histname+"_"+mc)
                    else:
                        tmp= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if tmp == None: raise IOError("Unable to get: "+mc+":"+basedir+"/"+histname+"_"+mc)
                        bkg_histo.Add(tmp)
            bkg_histo.Scale(lumi)
        else:
            tmp= fIn.Get(basedir+"/"+histname+"_"+mc)
            if tmp==None:
                for hname in histname.split(','):
                    if tmp == None: 
                        tmp= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if tmp == None: raise IOError("Unable to get: "+mc+":"+basedir+"/"+histname+"_"+mc)
                    else:
                        tmp2= fIn.Get(basedir+"/"+hname+"_"+mc)
                        if tmp2==None:raise IOError("Unable to get: "+mc + ":"+basedir+"/"+hname+"_"+mc)
                        tmp.Add(tmp2)
            tmp.Scale(lumi)
            bkg_histo.Add(tmp)
    data_histo= fIn.Get(basedir+"/"+histname+"_Data")
    if data_histo==None:
        for hname in histname.split(','):
            if data_histo==None:
                data_histo= fIn.Get(basedir+"/"+hname+"_Data")
                if data_histo == None: raise IOError("Unable to get: "+mc+":"+basedir+"/"+histname+"_Data")
            else:
                tmp= fIn.Get(basedir+"/"+hname+"_Data")
                if tmp == None: raise IOError("Unable to get: "+mc+":"+basedir+"/"+histname+"_Data")
                data_histo.Add(tmp)
    return sig_histo,bkg_histo,data_histo

# I have a sig_histo and a bkg_histo 2D in mass and bdt
def runFit(data,model,ntries=3):
    fit=model.fitTo(data,
        ROOT.RooFit.Minimizer("Minuit2","minimize"),
        ROOT.RooFit.Save(1),
        ROOT.RooFit.SumW2Error(True),
        ROOT.RooFit.PrintEvalErrors(-1),
        ROOT.RooFit.PrintLevel(-1),
        ROOT.RooFit.Warnings(0)
        )
    stat = fit.status();
    ntry=0
    while stat !=0:
        fit=model.fitTo(data,
            ROOT.RooFit.Minimizer("Minuit2","minimize"),
            ROOT.RooFit.Save(1),
            ROOT.RooFit.SumW2Error(True),
            ROOT.RooFit.PrintEvalErrors(-1),
            ROOT.RooFit.PrintLevel(-1),
            ROOT.RooFit.Warnings(0)
            )
        stat = fit.status();
        ntry+=1
        if ntry > ntries : break
    return

def FitSignal(h,x,prefix="sig"):
    ''' Return Three gaus model for sig, and garbage'''
    if opts.verbose: print "-> Running fit signal for",h.GetName()
    obj=[]
    means=[ROOT.RooRealVar(prefix+"_m%d"%i,"m%d"%i,125,120,130) for i in range(0,3)]
    sigmas=[ROOT.RooRealVar(prefix+"_s%d"%i,"s%d"%i,2,0.5,10) for i in range(0,3)]
    fracs=[ROOT.RooRealVar(prefix+"_f%d"%i,"f%d"%i,.9,0,1) for i in range(0,3)]
    gaus=[ROOT.RooGaussian(prefix+"_g%d"%i,"g%d"%i,x,means[i],sigmas[i]) for i in range(0,3)]

    means[0].setVal(h.GetBinCenter(h.GetMaximumBin()))
    sigmas[0].setVal(h.GetRMS()*0.5)

    means[1].setVal(h.GetMean()*.95)
    sigmas[1].setVal(h.GetRMS())

    means[2].setVal(h.GetMean()*.9)
    sigmas[2].setVal(h.GetRMS()*2)

    fracs[0].setVal(0.6)
    fracs[1].setVal(0.2)

    dh = ROOT.RooDataHist("dh_"+prefix+"_sig","dh",ROOT.RooArgList(x),h)
    model = ROOT.RooAddPdf(prefix+"_sig","model",ROOT.RooArgList(gaus[0],gaus[1],gaus[2]),ROOT.RooArgList(fracs[0],fracs[1]), True ) 

    runFit(dh,model)

    for v in means + sigmas + fracs: v.setConstant()

    if opts.verbose:
        for idx,v in enumerate(means):
            print "means",idx,"->",v.getVal()
   
    ## avoid this to be deleted
    obj.extend(means)
    obj.extend(sigmas)
    obj.extend(fracs)
    obj.extend(gaus)
    obj.extend([model,dh,x])
    if opts.verbose: print "-> Done sig",h.GetName()
    return model,obj

def BkgModel(h,x,prefix="bkg"):
    if opts.verbose: print "-> Running fit bkg for",h.GetName()
    obj=[]
    plist =ROOT.RooArgList()
    plist.add(x)

    pname = prefix + "_a";
    a = ROOT.RooRealVar(pname,pname,1.,-5.0, 5.0 );
    plist.add(a)

    pname = prefix + "_b";
    b = ROOT.RooRealVar(pname,pname,.1,-2, 2 );
    plist.add(b)

    pname = prefix + "_c";
    c = ROOT.RooRealVar(pname,pname,-.1,-2, 2.0 );
    plist.add(c)


    dh = ROOT.RooDataHist("dh_"+prefix+"_bkg","dh",ROOT.RooArgList(x),h)
    zmod = ROOT.RooGenericPdf(prefix+"_bkg",prefix+"_bkg","TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) ",plist)
    runFit(dh,zmod)
    b.setConstant() ## from combination

    obj.extend([a,b,c,zmod])
    if opts.verbose: print "-> Done bkg",h.GetName()
    return zmod,obj


xobs=ROOT.RooRealVar("x","x",config.xmin,config.xmax)
ROOT.RooMsgService.instance().setSilentMode(True);
## 2D
sig_histo,bkg_histo,data_histo=GetSigAndBkg(opts.histname)

#BtdMass_BB_
## move one boundary (opts.n)
bounds=[ float(x) for x in opts.boundaries.split(',')]

bin0=bkg_histo.GetYaxis().FindBin( bounds[opts.n-1]) if opts.n>0 else 1
bin1=bkg_histo.GetYaxis().FindBin( bounds[opts.n+1]) if len(bounds)<opts.n+1 else bkg_histo.GetYaxis().GetNbins() +2

results=Queue()
lock = threading.Lock()

def Import(w,obj):
    getattr(w,'import')(obj, ROOT.RooFit.Silence(), ROOT.RooFit.RecycleConflictNodes())
    return

def job(wdir,ibin):
    #mkdir wdir
    with lock:
        status=call("mkdir -p "+wdir,shell=True)
        dat=open(wdir+"/"+"datacard.txt","w")
        # prepare sig and bkg 1d distributions for the boundaries
        sig_1d = []
        bkg_1d = []
        data_dh=[]
        y0,y1=0,0
        w = ROOT.RooWorkspace("w","w")
        sOsb=0.
        for ib,b in enumerate(bounds):
            y1=bkg_histo.GetYaxis().FindBin(b)
            if ib==opts.n: y1=ibin
            px_s = sig_histo.ProjectionX("s_px_ibin%d_cat%d"%(ibin,ib), y0, y1)
            px_b = bkg_histo.ProjectionX("b_px_ibin%d_cat%d"%(ibin,ib), y0, y1)
            px_d = bkg_histo.ProjectionX("d_px_ibin%d_cat%d"%(ibin,ib), y0, y1)
            sig_1d.append(px_s)
            bkg_1d.append(px_b)
            dh = ROOT.RooDataHist("cat%d_data_obs"%(ib),"dh data",ROOT.RooArgList(xobs),px_d)
            #getattr(w,'import')(dh)
            Import(w,dh)
            data_dh.append(dh)
            y0=y1+1
            sOsb += px_s.Integral()**2 /  px_b.Integral()
        # fit sig
        gb=[]
        for ib,b in enumerate(bounds):
            sig, ig = FitSignal(sig_1d[ib],xobs,"cat%d"%ib)
            sig_norm=ROOT.RooRealVar(sig.GetName()+"_norm","norm",sig_1d[ib].Integral())
            gb.extend(ig)
            if opts.verbose: 
                print "-> importing:",sig.GetName(),"and",sig_norm.GetName()
                sig.Print("V")
            #getattr(w,'import')(sig)
            #getattr(w,'import')(sig_norm)
            Import(w,sig)
            Import(w,sig_norm)
        # fit bkg with floating norm
        for ib,b in enumerate(bounds):
            bkg, ig = BkgModel(bkg_1d[ib],xobs,"cat%d"%ib)
            nevt=bkg_1d[ib].Integral()
            bkg_norm=ROOT.RooRealVar(bkg.GetName()+"_norm","norm",nevt,nevt/2.,2*nevt)
            gb.extend(ig)
            #getattr(w,'import')(bkg)
            #getattr(w,'import')(bkg_norm)
            Import(w,bkg)
            Import(w,bkg_norm)
        #
        w.writeToFile(wdir+"/templates.root")
        # write datacard
        dat.write("-------------------------------------\n")
        dat.write("imax *                               \n")
        dat.write("jmax *                               \n")
        dat.write("kmax *                               \n")
        dat.write("-------------------------------------\n")
        dat.write("shapes *   *  templates.root  w:$CHANNEL_$PROCESS\n")
        dat.write("-------------------------------------\n")
        dat.write("bin\t" + '\t'.join(['cat%d'%ib for ib,b in enumerate(bounds)]) + '\n')
        dat.write("observation\t"+'\t'.join(['-1' for ib,b in enumerate(bounds)]) +"\n")
        dat.write("-------------------------------------\n")
        #dat.write("bin     cat0    cat0                 \n")
        dat.write("bin\t" + '\t'.join(['cat%d'%ib for ib,b in enumerate(bounds) for p in ['sig','bkg']]) +'\n')
        #dat.write("process sig     bkg                  \n")
        dat.write("process\t" + '\t'.join([ p for ib,b in enumerate(bounds) for p in ['sig','bkg']]) +'\n')
        #dat.write("process 0       1                    \n")
        dat.write("process\t" + '\t'.join([ "0" if p=='sig' else "1" for ib,b in enumerate(bounds) for p in ['sig','bkg']]) +'\n')
        #dat.write("rate    -1      -1                   \n")
        dat.write("rate\t" + '\t'.join([ "1" for ib,b in enumerate(bounds) for p in ['sig','bkg']]) +'\n')
        dat.write("-------------------------------------\n")
        
        dat.close()
    #releasing lock
    # run combine
    cmd = "cd " + wdir + " && " 
    cmd += "combine -M FitDiagnostics -t -1 --robustFit=1 -m 125 -S 0  --cminDefaultMinimizerType=Minuit --expectSignal=1  --rMax=50 --rMin=-2  datacard.txt 2>&1 >log_combine.txt"

    if opts.verbose: 
        print "-> Running combine:",cmd
    
    status = call(cmd, shell=True)
    
    fR=ROOT.TFile.Open(wdir+"/fitDiagnostics.root")
    tR=fR.Get("tree_fit_sb")
    tR.GetEntry(0)
    
    if tR.fit_status != 0: 
        print "<W> Fit Failed."
        return 100

    # get fig of merits
    fom=(tR.rHiErr + tR.rLoErr)/2.0
    results.put( (ibin ,fom,sOsb) ) 

    if opts.verbose:
        print "-> Returning result:",ibin,fom,sOsb

    return fom

## prepare parallelization
threads = []
call('rm -r /tmp/'+os.environ['USER']+"/optimization/",shell=True)

for ibin in range(bin0,bin1):
    wdir='/tmp/'+os.environ['USER']+"/optimization/bin_%d"%ibin

    if opts.ncore>0:
        while threading.activeCount() > opts.ncore : 
            print "sleep ...",
            time.sleep(3)
            print "wake up"
        t = threading.Thread(target=job, args= (wdir,ibin)  )
        #job(wdir,ibin)
        t.start()
        threads.append( t )
    else:
        print "-> Running job non-parallel",wdir,ibin
        job(wdir,ibin)

for t in threads:
    t.join()

### sort
l=[]
while not results.empty():
    res=results.get()
    #ibin=res[0]
    #fom=res[1]
    #sOsb=res[2]
    l.append( res )
l.sort()

## output
fOut=ROOT.TFile.Open(re.sub('.txt','.root',opts.output),"RECREATE")
fOut.cd()
g=ROOT.TGraph()
g.SetName("scan")

g2=ROOT.TGraph()
g2.SetName("sOsb")
out=open(opts.output,"w")
for res in l:
    ibin=res[0]
    fom=res[1]
    sOsb=res[2]
    print >>out, ibin, bkg_histo.GetYaxis().GetBinCenter(ibin),"->", fom,"|",sOsb
    if fom > 10: continue
    g.SetPoint(g.GetN(),bkg_histo.GetYaxis().GetBinCenter(ibin),fom)
    g2.SetPoint(g2.GetN(),bkg_histo.GetYaxis().GetBinCenter(ibin),sOsb)
g.Write()
g2.Write()
print " ************* Done ***************"
