#!env python

import os,sys
from glob import glob
from subprocess import call, check_output
import re
import threading
import time

threads=[]

def drange(start, stop, step):
        ''' Return a floating range list. Start and stop are included in the list if possible.'''
        r = start
        while r <= stop:
                yield r
                r += step

from optparse import OptionParser, OptionGroup

usage = "usage: %prog [options]"
parser=OptionParser(usage=usage)
parser.add_option("-i","--input" ,dest='input',type='string',help="Input Datacard [Default=%default]",default="")
parser.add_option("-d","--dir" ,dest='dir',type='string',help="Directory where to write the configuration [Default=%default]",default="submit")
#parser.add_option("-n","--njobs" ,dest='njobs',type='int',help="Number of Job to submit [Default=%default]",default=50)
parser.add_option("","--begin" ,dest='begin',type='string',help="Begin Mass [Default=%default] or comma separeted list of mass points",default="200")
parser.add_option("","--end" ,dest='end',type='float',help="End Mass [Default=%default]",default=900)
parser.add_option("","--step" ,dest='step',type='float',help="Step [Default=%default]",default=100)
parser.add_option("-q","--queue" ,dest='queue',type='string',help="Queue [Default=%default]",default="1nh")
parser.add_option("","--dryrun" ,dest='dryrun',action='store_true',help="Do not Submit [Default=%default]",default=False)
parser.add_option("-e","--expected" ,dest='exp',action='store_true',help="Run Expected [Default=%default]",default=True)
parser.add_option("-u","--unblind" ,dest='exp',action='store_false',help="Unblind Results")
parser.add_option("","--ncore",type='int',help="num. of core. [%default]",default=4)
parser.add_option("","--rmax",type='float',help="rmax. [%default]",default=1000000)
parser.add_option("","--rmin",type='float',help="rmin. [%default]",default=-1)
parser.add_option("","--onews",action='store_true',help="Only one ws. names do not depend on the higgs mass. [%default]",default=False)
parser.add_option("","--nosyst",action='store_true',help="No Syst.. [%default]",default=False)
parser.add_option("-M","--method" ,dest='method',type='string',help="Method [%default]",default="Asymptotic")


asym_grid = OptionGroup(parser,"Asymptotic Grid/MultiDimFit options","")
asym_grid.add_option("","--npoints" ,dest='npoints',type='int',help="Number of points between rMin and rMax [%default]",default=100)
asym_grid.add_option("","--njobs" ,dest='njobs',type='int',help="Number of job per mass point [%default]",default=10)
asym_grid.add_option("","--hadd" ,dest='hadd',action='store_true',help="Hadd and collect for Asymptotic Grid [%default]",default=False)

multidim = OptionGroup(parser,"MultiDimFit options","")
multidim.add_option("","--floatmh",action='store_true',help="Float MH. [%default]",default=False)
multidim.add_option("","--rvrf",action='store_true',help="rVrF scan. [%default]",default=False)

parser.add_option_group(asym_grid)
parser.add_option_group(multidim)
(opts,args)=parser.parse_args()

onews=0 ## for onews

EOS='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select'

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python",
status=sys.path.insert(0,os.getcwd()+'/python')
print status
if status == None:
    print "-> Looking for basepath"
    basepath = ""
    mypath = os.path.abspath(os.getcwd())
    while mypath != "" and mypath != "/":
    	if "ChargedHiggs" in os.path.basename(mypath):
    		basepath = os.path.abspath(mypath)
    	mypath = os.path.dirname(mypath)
    print "-> Base Path is " + basepath
    status=sys.path.insert(0,basepath)
    status=sys.path.insert(0,basepath+'/python')
    

from ParseDat import chunkIt

print "inserting combine in path"
if not os.path.exists(os.path.expandvars('$CMSSW_BASE/bin/$SCRAM_ARCH/combine')):
        sys.exit('ERROR - CombinedLimit package must be installed')
if not os.path.exists(os.path.expandvars('$CMSSW_BASE/bin/$SCRAM_ARCH/text2workspace.py')):
        sys.exit('ERROR - CombinedLimit package must be installed')
if not os.path.exists(os.path.expandvars('$CMSSW_BASE/bin/$SCRAM_ARCH/combineCards.py')):
        sys.exit('ERROR - CombinedLimit package must be installed')


call("[ -d %s ] && rmdir %s"%(opts.dir,opts.dir),shell=True)
call("mkdir -p %s"%opts.dir,shell=True)

#for iJob in range(0,opts.njobs):
cmdFile = open("%s/cmdFile.sh"%opts.dir,"w")
#write args in cmdFile
cmdFile.write("### Automatically created by: combine.py\n")
cmdFile.write("### Args: " + ' '.join(sys.argv) + "\n")

iJob = -1

def parallel(text2ws, bsub=""):
	print "-> Parallel command :'"+text2ws+"'"
	if text2ws != "/bin/true":
		st = call(text2ws,shell=True);
	else: 
		st =0

	if st !=0 :
		print "ERROR in executing txt2ws"
		if opts.onews:
			print "-> Refusing to continue"
			raise RuntimeError
		return 0
	if bsub !="":
		print "-> submitting",bsub
		call(bsub,shell=True)
	return 0 

if ',' not in opts.begin:
	massList=drange(float(opts.begin),opts.end,opts.step)
else:
	massList=[ float(m) for m in opts.begin.split(',')]

nJobsPerMassPoint=1
if opts.method=='AsymptoticGrid' or opts.method=='MultiDimFit' :
	nJobsPerMassPoint = opts.njobs
	muList = [ x for x in drange(opts.rmin,opts.rmax+0.001, (opts.rmax-opts.rmin)/opts.npoints) ] ## no generators
	listOfMus = chunkIt( muList, opts.njobs)  

if opts.method=='AsymptoticGrid' and opts.hadd:
	print "-> Collecting results"
	totalList=glob(opts.dir +"/higgs_limitgrid_mass*_chunk*.root")
	#print "DEBUG: totalList is",totalList,"from ",opts.dir +"/higgs_limitgrid_mass*_chunk*.root"
	massList=[]
	while True:
		myList=[] ## dont contains already processed masses
		for f in totalList:
		    ok=True
		    for m in massList:
			if re.search('mass'+m,f):
				ok=False
		    if ok: myList.append(f)

		if len(myList)==0: break
		## hadd a single mass point
		proto = re.sub('_chunk.*','',myList[0])
		mass=re.sub('.*mass','',proto)
		massList.append(mass)

		#derive datacard name, as below
		datacard= re.sub( ".txt","_M%s.root"%mass,opts.input.split("/")[-1])
		if opts.onews:
			datacard= re.sub( ".txt",".root",opts.input.split("/")[-1])

		if onews >0 : cmd="/bin/true"
		onews+=1
		replace= {"dir":opts.dir,"name":"higgs_limitgrid_mass%s_merged.root"%mass,"proto":proto,"datacard":datacard}
		hadd = "hadd -f %(dir)s/%(name)s %(proto)s_chunk*root"%replace
		print "-> calling",hadd
		status=0
		if not opts.dryrun: 
			status=call(hadd,shell=True)
		if status == 0 :
			rm = "rm %(proto)s_chunk*root"%replace
			print "->",rm
			if not opts.dryrun:
				call(rm,shell=True)
		combine = "combine -M Asymptotic" +" -m "+ mass
		if opts.exp:
			combine += " -t -1 --run=expected --expectSignal=0 --expectSignalMass="+str(mass) + " "
		combine += "--getLimitFromGrid=%(dir)s/%(name)s" %replace
		#combine += datacard
		combine += " -n AsymptoticGrid %(dir)s/%(datacard)s"% replace
		mv = "mv higgs*AsymptoticGrid* %(dir)s/" % replace
		print "-> calling",combine
		if not opts.dryrun:
			status = call(combine,shell=True)
		print "-> calling",mv
		if not opts.dryrun:
			status += call(mv,shell=True)
		if status==0: 
			print "OK"
		else:
			print "FAIL"

	## and finally merge also the limit files
	if opts.dir[-1] == '/':dir = opts.dir[:-1]
	else: dir  = opts.dir[:]
	dirname = re.sub('.*/','',dir)

	replace["dirname"] = dirname
	hadd = "hadd -f %(dir)s/%(dirname)s.root %(dir)s/higgs*AsymptoticGrid*"%replace
	if not opts.dryrun:
		status=call(hadd,shell=True)
	exit(0)

for mass in massList:
   for j_chunk in range(0,nJobsPerMassPoint):
	iJob += 1
	basedir=opts.dir
	if basedir[0] != '/': basedir=os.environ['PWD'] + "/" + opts.dir

	sh=open("%s/sub%d.sh"%(opts.dir,iJob),"w")

	call(["chmod","u+x","%s/sub%d.sh"%(opts.dir,iJob)])

	## Open File and write Preamble
	sh.write('#!/bin/bash\n')
	sh.write('[ "$WORKDIR" == "" ] && export WORKDIR="/tmp/%s/" \n'%(os.environ['USER']))
	sh.write('rm -v $WORKDIR/higgs*root\n')  ## make sure there is no residual in the WORKDIR
	sh.write('cd %s\n'%(os.getcwd() ) )
	sh.write('LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH\n'%os.getcwd())

	#print " WORK-AROUND COMBINE: FIXME!!! "
	#sh.write('cd ~amarini/work/ProductionJanuary2014/CMSSW_6_1_1_CategoryFull/src\n')
	sh.write('eval `scramv1 runtime -sh`\n') # cmsenv

	## Touch control files
	touch = "touch " + basedir + "/sub%d.pend"%iJob
	call(touch,shell=True)
	cmd = "rm " + basedir + "/sub%d.run 2>/dev/null >/dev/null"%iJob
	call(cmd,shell=True)
	cmd = "rm " + basedir + "/sub%d.done 2>/dev/null >/dev/null"%iJob
	call(cmd,shell=True)
	cmd = "rm " + basedir + "/sub%d.fail 2>/dev/null >/dev/null"%iJob
	call(cmd,shell=True)

	sh.write('date > %s/sub%d.run\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.done\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.pend\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.fail\n'%(basedir,iJob))

	sh.write('cd $WORKDIR\n')

	## Construct Datacard
	## rVrF: -P HiggsAnalysis.CombinedLimit.PhysicsModel:rVrFXSHiggs
	#  [--PO higgsMassRange=122,128]
	# -P RV -P RF --floatOtherPOIs=1 (MH)
	## float Mass: -P HiggsAnalysis.CombinedLimit.PhysicsModel:floatingHiggsMass
	#  --PO higgsMassRange=122,128

	datacard= re.sub( ".txt","_M%.0f.root"%mass,opts.input.split("/")[-1])
	cmd = "text2workspace.py -m " + str(mass) + " -o " + opts.dir + "/" + datacard +  " " + opts.input

	if opts.onews:
		datacard= re.sub( ".txt",".root",opts.input.split("/")[-1])
		cmd = "text2workspace.py -o "+ opts.dir+"/"+datacard +" " + opts.input
		## only for onews
		if opts.floatmh or opts.rvrf:
			cmd +=" -P HiggsAnalysis.CombinedLimit.PhysicsModel:rVrFXSHiggs"
		if opts.floatmh:
			cmd += " --PO higgsMassRange=122,128 "

		if onews >0 : cmd="/bin/true"
		onews+=1

	if not opts.onews and (opts.floatmh or opts.rvrf):
		print "-> Float MH and RVRF are available only in the onews mode"
		raise ValueError


	if opts.ncore <2:
		if cmd != "/bin/true":
			print "-> Running command :'"+cmd+"'"
			call(cmd,shell=True)
	else:
		text2ws = cmd[:]
	
	## copy datacard in workdir
	sh.write('cp -v '+ basedir + "/"+  datacard + " ./ \n" )
	##Write combine line
	#combine -M Asymptotic -m 200 -S 0 --run=expected --expectSignal=1 --expectSignalMass=200  cms_datacard_chhiggs_taunu.txt

	combine = "combine -M "+ opts.method +" -m "+ str(mass)
	if opts.method=="AsymptoticGrid": 
		combine = "combine -M Asymptotic" +" -m "+ str(mass)


	if opts.nosyst: combine += " -S 0 "
	combine += "  --cminDefaultMinimizerType=Minuit2 "
	#combine += " -H ProfileLikelihood " ## hint, it's not working

	########## EXPECTED ? #############
	if opts.exp:
		if opts.method=="AsymptoticGrid":  combine += " -t -1 --expectSignal=0 --expectSignalMass="+str(mass) + " "
		elif opts.method == "Asymptotic":  combine += " -t -1 --run=expected --expectSignal=0 --expectSignalMass="+str(mass) + " "
		else : combine += " -t -1 --expectSignal=1 --expectSignalMass="+str(mass) + " "

	########## METHOD DEPENDENT #############
	if opts.method=="MultiDimFit": 
		#combine += " --algo=grid  --points=%d --firstPoint=0 --lastPoint=49 --squareDistPoi"%opts.npoints
		combine += " --algo=grid  --points=%d --squareDistPoi"%opts.npoints
		if opts.njobs !=1:
			combine += " --firstPoint=%d --lastPoint=%d "%(int(opts.npoints*j_chunk/opts.njobs),int(opts.npoints*(j_chunk+1)/opts.njobs-1))
			combine += " -n MultiDimFit_mass%s_chunk%d "%(str(mass),j_chunk)

	if not opts.method=="MultiDimFit" and not opts.method=="AsymptoticGrid": 
		combine += " --rMax=%f "%opts.rmax ## already taken into account
		combine += " --rMin=%f "%opts.rmin ## already taken into account

	####
	if opts.method=="MultiDimFit" and opts.rvrf:
		combine += " -P RV -P RF " ## float MH
		if opts.floatmh: combine+=" --floatOtherPOIs=1"
		combine += " --setPhysicsModelParameterRanges RV=-1,8:RF=-1,8 "
	if opts.method=="MultiDimFit" and opts.floatmh:
		if not opts.rvrf: combine += " -P MH "

	if opts.method == "ProfileLikelihood":
		combine += " --signif --pval "

	### ADD DATACARD #############
	combine += datacard

	### WRITE COMMANDS ###
	if opts.method == "AsymptoticGrid":
		for mu in listOfMus[j_chunk]:
			sh.write( combine + " --singlePoint %f  -n limitgrid_mass%s_chunk%d_mu%f\n"%(mu,str(mass),j_chunk,mu) )
		sh.write( "hadd -f higgs_limitgrid_mass%(mass)s_chunk%(chunk)d.root  higgs*limitgrid_mass%(mass)s_chunk%(chunk)d_mu*.root\n"%{"mass":str(mass),"chunk":j_chunk} )
		sh.write('EXITCODE=$?\n')
		sh.write( "rm -v  higgs*limitgrid_mass%(mass)s_chunk%(chunk)d_mu*.root\n"%{"mass":str(mass),"chunk":j_chunk} )
	else:
		sh.write( combine + "\n" )
		sh.write('EXITCODE=$?\n')

	sh.write('[ $EXITCODE == 0 ] && touch %s/sub%d.done\n'%(basedir,iJob))
	sh.write('[ $EXITCODE != 0 ] && echo $EXITCODE > %s/sub%d.fail\n'%(basedir,iJob))
	
	sh.write('cp -v higgs*root %s/\n'%basedir)
	sh.write('rm %s/sub%d.run\n'%(basedir,iJob))

	### SUBMIT COMMANDS ###
	cmdline = "bsub -q " + opts.queue + " -o %s/log%d.txt"%(basedir,iJob) + " -J " + "%s/Job_%d"%(opts.dir,iJob) + " %s/sub%d.sh"%(basedir,iJob)
	cmdFile.write(cmdline+"\n")

	bsub=""
	if not opts.dryrun: 
		if opts.ncore<2:
			print cmdline
			call(cmdline,shell=True)
		else:
			bsub=cmdline[:]

	if opts.ncore>=2:
		if not opts.onews:
			while threading.activeCount() >= opts.ncore:
				print "sleep ....",
				time.sleep(1)
				print "wake up"
		t= threading.Thread(target=parallel,args=(text2ws,bsub,) )
		t.start()
		threads.append(t)

		if opts.onews and text2ws !="/bin/true":
			print "-> for onews I'll wait the text2ws to end"
			t.join()

for t in threads:
	t.join()

print "Done"
