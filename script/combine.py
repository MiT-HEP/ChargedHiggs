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
parser.add_option("","--begin" ,dest='begin',type='float',help="Begin Mass [Default=%default]",default=200)
parser.add_option("","--end" ,dest='end',type='float',help="End Mass [Default=%default]",default=900)
parser.add_option("","--step" ,dest='step',type='float',help="Step [Default=%default]",default=100)
parser.add_option("-q","--queue" ,dest='queue',type='string',help="Queue [Default=%default]",default="1nh")
parser.add_option("","--dryrun" ,dest='dryrun',action='store_true',help="Do not Submit [Default=%default]",default=False)
parser.add_option("-e","--expected" ,dest='exp',action='store_true',help="Run Expected [Default=%default]",default=True)
parser.add_option("-u","--unblind" ,dest='exp',action='store_false',help="Unblind Results")
parser.add_option("","--ncore",type='int',help="num. of core. [%default]",default=4)
parser.add_option("","--rmax",type='float',help="rmax. [%default]",default=1000000)
parser.add_option("","--onews",action='store_true',help="Only one ws. names do not depend on the higgs mass. [%default]",default=False)
parser.add_option("","--nosyst",action='store_true',help="No Syst.. [%default]",default=False)
parser.add_option("-M","--method" ,dest='method',type='string',help="Method [%default]",default="Asymptotic")

(opts,args)=parser.parse_args()

onews=0 ## for onews

EOS='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select'

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

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
iJob = -1

def parallel(text2ws, bsub=""):
	print "-> Parallel command :'"+text2ws+"'"
	if text2ws != "/bin/true":
		st = call(text2ws,shell=True);
	else: 
		st =0

	if st !=0 :
		print "ERROR in executing txt2ws"
		return 0
	if bsub !="":
		print "-> submitting",bsub
		call(bsub,shell=True)
	return 0 

for mass in drange(opts.begin,opts.end,opts.step):
	iJob += 1
	basedir=opts.dir
	if basedir[0] != '/': basedir=os.environ['PWD'] + "/" + opts.dir

	sh=open("%s/sub%d.sh"%(opts.dir,iJob),"w")

	call(["chmod","u+x","%s/sub%d.sh"%(opts.dir,iJob)])

	## Open File and write Preamble
	sh.write('#!/bin/bash\n')
	sh.write('[ "$WORKDIR" == "" ] && export WORKDIR="/tmp/%s/" \n'%(os.environ['USER']))
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
	datacard= re.sub( ".txt","_M%.0f.root"%mass,opts.input.split("/")[-1])
	cmd = "text2workspace.py -m " + str(mass) + " -o " + opts.dir + "/" + datacard +  " " + opts.input
	if opts.onews:
		datacard= re.sub( ".txt",".root",opts.input.split("/")[-1])
		cmd = "text2workspace.py -o "+ opts.dir+"/"+datacard +" " + opts.input
		if onews >0 : cmd="/bin/true"
		onews+=1

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
	if opts.nosyst: combine += " -S 0 "
	combine += "  --cminDefaultMinimizerType=Minuit2 "
	combine += " -H ProfileLikelihood " ## hint
	if opts.exp : combine += " --run=expected --expectSignal=1 --expectSignalMass="+str(mass) + " "
	combine += " --rMax=%f "%opts.rmax
	combine += datacard
	sh.write( combine + "\n" )
	sh.write('EXITCODE=$?\n')
	sh.write('[ $EXITCODE == 0 ] && touch %s/sub%d.done\n'%(basedir,iJob))
	sh.write('[ $EXITCODE != 0 ] && echo $EXITCODE > %s/sub%d.fail\n'%(basedir,iJob))
	
	sh.write('cp higgs*root %s/\n'%basedir)
	sh.write('rm %s/sub%d.run\n'%(basedir,iJob))

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
