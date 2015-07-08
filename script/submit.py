#!env python

import os,sys
from glob import glob
from subprocess import call, check_output
import re

from optparse import OptionParser, OptionGroup

usage = "usage: %prog [options]"
parser=OptionParser(usage=usage)
parser.add_option("-i","--input" ,dest='input',type='string',help="Input Configuration file",default="")
parser.add_option("-d","--dir" ,dest='dir',type='string',help="Directory where to write the configuration",default="submit")
parser.add_option("-v","--debug" ,dest='debug',type='int',help="Debug Verbosity. From 0-3. Default=%default",default=0)
parser.add_option("-n","--njobs" ,dest='njobs',type='int',help="Number of Job to submit",default=50)
parser.add_option("-q","--queue" ,dest='queue',type='string',help="Queue",default="1nh")

job_opts= OptionGroup(parser,"Job options:","these options modifies the job specific")
job_opts.add_option("-t","--no-tar" ,dest='tar',action='store_false',help="Do not Make Tar",default=True)
job_opts.add_option("","--dryrun" ,dest='dryrun',action='store_true',help="Do not Submit",default=False)
job_opts.add_option("","--no-compress" ,dest='compress',action='store_false',help="Don't compress",default=True)
job_opts.add_option("","--compress"    ,dest='compress',action='store_true',help="Compress stdout/err")
job_opts.add_option("-m","--mount-eos" ,dest='mount',action='store_true',help="Mount eos file system.",default=False)
job_opts.add_option("-c","--cp" ,dest='cp',action='store_true',help="cp Eos file locally. Do not use xrootd. ",default=False)
job_opts.add_option("","--hadd" ,dest='hadd',action='store_true',help="Hadd Directory.",default=False)

summary= OptionGroup(parser,"Summary","these options are used in case of summary is wanted")
summary.add_option("-s","--status",dest="status", action='store_true', help = "Display status information for a submission.", default=False)
summary.add_option("","--resubmit",dest="resubmit", action='store_true', help = "Resubmit failed jobs.", default=False)
summary.add_option("-j","--joblist",dest="joblist", type='string', help = "Resubmit this job list. '' or 'fail' will submit the failed jobs. 'run' will submit the running jobs. Job list = 3,5,6-10", default="")

parser.add_option_group(job_opts)
parser.add_option_group(summary)

(opts,args)=parser.parse_args()

EOS='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select'

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

def PrintLine(list):
	''' convert list in list of int number, sort and compress consecutive numbers. Then print the result:
	4,5,8,3 -> 3-5,8
	'''
	nums = [ int(s) for s in list ]
	nums.sort()
	compress = []
	last = None
	blockinit = None

	for n in nums:
		#first if it is not consecutive
		if last == None: ## INIT
			blockinit = n

		elif last != n-1:
			#close a block and open a new one
			if last != blockinit:
				compress.append( str(blockinit) + "-" + str(last) ) 
			else:
				compress.append( str(last) ) 
			blockinit = n

		last = n

	#consider also the last number
	#close a block and open a new one
	if last != blockinit:
		compress.append( str(blockinit) + "-" + str(last) ) 
	else:
		compress.append( str(last) ) 

	return ",".join(compress)

def PrintSummary(dir, doPrint=True):
	''' Print summary informations for dir'''
	run  = glob(dir + "/*run")
	fail = glob(dir + "/*fail")
	done = glob(dir + "/*done")
	pend = glob(dir + "/*pend")

	## bash color string
	red="\033[01;31m"
	green = "\033[01;32m"
	yellow = "\033[01;33m"
	cyan = "\033[01;36m"
	white = "\033[00m"

	run = [ re.sub('\.run','' , re.sub('.*/sub','', r) ) for r in run ] 	
	fail = [ re.sub('\.fail','' , re.sub('.*/sub','', r) ) for r in fail ] 	
	done = [ re.sub('\.done','' , re.sub('.*/sub','', r) ) for r in done ] 	
	pend = [ re.sub('\.pend','' , re.sub('.*/sub','', r) ) for r in pend ] 	

	tot = len(run) + len(fail) + len(done) + len(pend)

	color = red
	if len(run) > len(fail) and len(run) >len(pend)  : color= yellow
	if len(pend) > len(run) and len(pend) >len(fail): color= cyan
	if len(done) == tot and tot >0 : color = green
	
	if doPrint:
		print " ----  Directory "+ color+opts.dir+white+" --------"
		print " Pend: " + cyan   + "%3d"%len(pend) + " / " + str(tot) + white + " : " + PrintLine(pend)  ###
		print " Run : " + yellow + "%3d"%len(run) + " / "  + str(tot) + white + " : " + PrintLine(run)  ### + ",".join(run)  + "|" 
		print " Fail: " + red    + "%3d"%len(fail) + " / " + str(tot) + white + " : " + PrintLine(fail) ### + ",".join(fail) + "|" 
		print " Done: " + green  + "%3d"%len(done) + " / " + str(tot) + white + " : " + PrintLine(done) ### + ",".join(done) + "|" 
		print " -------------------------------------"

	return ( done, run, fail)

if opts.status:
	PrintSummary(opts.dir,doPrint=True)
	exit(0)

if opts.resubmit:
	( done, run, fail) = PrintSummary(opts.dir,False)

	if opts.joblist == '' or opts.joblist.lower() == 'fail':
		joblist = fail
	elif opts.joblist =='run':
		joblist = run
	else:
		joblist = opts.joblist.split(',')

	for job in joblist:
		if '-' in job: 
		   iBegin= int(job.split('-')[0])
		   iEnd = int(job.split('-')[1])
		else: 
		   iBegin= int(job)
		   iEnd = int(job)
	   	for iJob in range(iBegin,iEnd+1):
			#iJob= int(job)
			basedir = os.environ['PWD'] + "/" + opts.dir
			touch = "touch " + basedir + "/sub%d.pend"%iJob
			call(touch,shell=True)
			cmdline = "bsub -q " + opts.queue + " -o %s/log%d.txt"%(basedir,iJob) + " -J " + "%s/Job_%d"%(opts.dir,iJob) + " %s/sub%d.sh"%(basedir,iJob)
			print cmdline
			call (cmdline,shell=True)
	exit(0)

if opts.hadd:
	filelist = glob(opts.dir + "/*.root")
	if opts.dir[-1] == '/':dir = opts.dir[:-1]
	else: dir  = opts.dir[:]
	name = re.sub('.*/','',dir)
	cmd = "hadd -f %s/%s.root "%(opts.dir, name ) + " ".join(filelist)
	call(cmd,shell=True)
	exit(0)

# import Parser
from ParseDat import *
config=ParseDat(opts.input)

call("[ -d %s ] && rm -r %s"%(opts.dir,opts.dir),shell=True)
call("mkdir -p %s"%opts.dir,shell=True)
cmdFile=open("%s/submit_cmd.sh"%opts.dir,"w")
cmdFile.write("##Commands used to submit on batch. Automatic written by python/submit.py script\n")

if opts.tar:
	cmd=["tar","-czf","%s/package.tar.gz"%opts.dir]
	cmd.extend( glob("bin/*so" ) )
	cmd.extend( glob("bin/dict*" ) )
	#cmd.extend( glob("bin/tag.txt" ) )
	cmd.extend( glob("dat/*dat" ) )
	cmd.extend( glob("dat/*txt" ) )
	cmd.extend( glob("aux/*" ) )
	cmd.extend( glob("python/*py") )
	cmd.extend( glob("test/*") )
	cmd.extend( glob("interface/*hpp" ) ) ## who is the genius that in ROOT6 need these at run time ? 
	tarCmdline = " ".join(cmd)
	print tarCmdline
	call(cmd)

	tarInfo = open("%s/tar.txt"%opts.dir,"w")
	print >> tarInfo, tarCmdline
	tarInfo.close()
	cmd="git rev-parse HEAD > %s/tag.txt" %opts.dir
	call(cmd,shell=True)
	cmd="git describe --tags >> %s/tag.txt" %opts.dir
	call(cmd,shell=True)
	cmd="git diff HEAD > %s/patch.txt" %opts.dir
	call(cmd,shell=True)

## expand *
if True:
	fileList=[]
	for f in config['Files']:
		list=[]
		if '/store/' in f:
			if opts.mount:
				list =  FindEOS(f, "%%MOUNTPOINT%%/eos")
			else:
				list =  FindEOS(f)
		else :
			list=glob(f)
			if list == []: ### maybe remote ?
				list=f
		fileList.extend(list)
	config['Files']=fileList
	splittedInput=chunkIt(config['Files'],opts.njobs )

for iJob in range(0,opts.njobs):
	sh=open("%s/sub%d.sh"%(opts.dir,iJob),"w")
	basedir=opts.dir
	if basedir[0] != '/': basedir=os.environ['PWD'] + "/" + opts.dir
	
	sh.write('#!/bin/bash\n')
	sh.write('[ "$WORKDIR" == "" ] && export WORKDIR="/tmp/%s/" \n'%(os.environ['USER']))
	sh.write('cd %s\n'%(os.getcwd() ) )
	sh.write('LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH\n'%os.getcwd())
	sh.write('eval `scramv1 runtime -sh`\n') # cmsenv

	if opts.tar:
		sh.write("mkdir -p $WORKDIR/%s_%d\n"%(opts.dir,iJob))
		sh.write("cd $WORKDIR/%s_%d\n"%(opts.dir,iJob))
		sh.write("tar -xzf %s/package.tar.gz\n"%(basedir ))
		sh.write("mkdir -p %s\n"%opts.dir)
		sh.write("cp %s/*dat %s/\n"%(basedir,opts.dir))

	touch = "touch " + basedir + "/sub%d.pend"%iJob
	call(touch,shell=True)
	cmd = "rm " + basedir + "/sub%d.run 2>&1 >/dev/null"%iJob
	call(cmd,shell=True)
	cmd = "rm " + basedir + "/sub%d.done 2>&1 >/dev/null"%iJob
	call(cmd,shell=True)
	cmd = "rm " + basedir + "/sub%d.fail 2>&1 >/dev/null"%iJob
	call(cmd,shell=True)

	sh.write('date > %s/sub%d.run\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.done\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.pend\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.fail\n'%(basedir,iJob))

	if opts.mount:
		#mountpoint = "~/eos"
		mountpoint = "$WORKDIR/%s_%d/eos"%(opts.dir,iJob)
		sh.write('%s -b fuse mount %s\n'% (EOS,mountpoint))

	if opts.cp:
		sh.write("mkdir cp\n")
		for idx,file in enumerate(splittedInput[iJob]):
			if 'root://eoscms//' in file: file = re.sub('root://eoscms//','', file)
			# -1 = filename , -2 = dirname
			filename = file.split('/')[-1] ## remove all directories
			dir = file.split('/')[-2]
			sh.write("mkdir ./cp/%s\n"%dir)
			sh.write('cmsStage %s ./cp/%s/\n'%(file,dir))
			splittedInput[iJob][idx] = "./cp/" + dir + "/" + filename


	if opts.debug>1:
		# after CD in the WORKDIR and mount
		sh.write('echo "-------- ENV -------"\n')
		sh.write("env \n") # print run time environment
		sh.write('echo "--------------------"\n')
		sh.write('echo "-------- LS -------"\n')
		sh.write("ls -lR\n")
		sh.write('echo "--------------------"\n')



	if opts.compress:
		compressString="2>&1 | gzip > %s/log%d.txt.gz"%(opts.dir,iJob)
	else: compressString =""

	sh.write('python python/Loop.py -v -d %s/input%d.dat %s\n'%(opts.dir,iJob,compressString))

	if opts.compress:
		sh.write('EXITCODE=${PIPESTATUS[0]}\n')
	else:
		sh.write('EXITCODE=$?\n')
	sh.write('rm %s/sub%d.run\n'%(basedir,iJob))
	sh.write('[ $EXITCODE == 0 ] && touch %s/sub%d.done\n'%(basedir,iJob))
	sh.write('[ $EXITCODE != 0 ] && echo $EXITCODE > %s/sub%d.fail\n'%(basedir,iJob))

	if opts.mount:
		sh.write('%s -b fuse umount %s\n'% (EOS,mountpoint))

	outname = re.sub('.root','_%d.root'%iJob,config['Output'])

	if opts.tar:
		if basedir != opts.dir : 
			sh.write("[ $EXITCODE == 0 ] && mv -v %s/%s %s/\n"%(opts.dir,outname,basedir))
		if opts.compress:
			sh.write("mv %s/log%d.txt.gz %s/log%d.txt.gz\n"%(opts.dir,iJob,basedir,iJob) )
	
	dat=open("%s/input%d.dat"%(opts.dir,iJob),"w")
	dat.write("include=%s\n"%opts.input)
	dat.write(re.sub('%%MOUNTPOINT%%',"./", 'Files=%s\n'%( ','.join(splittedInput[iJob]) ) ))
	dat.write('Output=%s/%s\n'%(opts.dir,outname) )

	## make the sh file executable	
	call(["chmod","u+x","%s/sub%d.sh"%(opts.dir,iJob)])

	## submit
	#sh.write('#$-N %s/Job_%d\n'%(opts.dir,iJob))
	cmdline = "bsub -q " + opts.queue + " -o %s/log%d.txt"%(basedir,iJob) + " -J " + "%s/Job_%d"%(opts.dir,iJob) + " %s/sub%d.sh"%(basedir,iJob)
	print cmdline
	cmdFile.write(cmdline+"\n")

	if len(splittedInput[iJob]) == 0 : 
		print "No file to run on for job "+ str(iJob)+", will not send it!"
		continue
	if not opts.dryrun: 
		call(cmdline,shell=True)

