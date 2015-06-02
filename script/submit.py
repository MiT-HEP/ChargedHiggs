#!env python

import os,sys
from glob import glob
from subprocess import call

from optparse import OptionParser

usage = "usage: %prog [options]"
parser=OptionParser(usage=usage)
parser.add_option("-i","--input" ,dest='input',type='string',help="Input Configuration file",default="")
parser.add_option("-d","--dir" ,dest='dir',type='string',help="Directory where to write the configuration",default="submit")
parser.add_option("-v","--debug" ,dest='debug',type='int',help="Debug Verbosity. From 0-3. Default=%default",default=0)
parser.add_option("-n","--njobs" ,dest='njobs',type='int',help="Number of Job to submit",default=50)
parser.add_option("-q","--queue" ,dest='queue',type='string',help="Queue",default="1nh")
parser.add_option("-t","--no-tar" ,dest='tar',action='store_false',help="Do not Make Tar",default=True)

parser.add_option("","--dryrun" ,dest='dryrun',action='store_true',help="Do not Submit",default=False)
parser.add_option("","--no-compress" ,dest='compress',action='store_false',help="Don't compress",default=True)
parser.add_option("","--compress"    ,dest='compress',action='store_true',help="Compress stdout/err")
parser.add_option("-m","--mount-eos" ,dest='mount',action='store_true',help="Mount eos file system. Do not use xrootd.",default=False)
(opts,args)=parser.parse_args()

EOS='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select'

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

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
	cmd.extend( glob("python/*py") )
	cmd.extend( glob("test/*") )
	cmd.extend( glob("interface/*hpp" ) ) ## who is the genius that in ROOT6 need these at run time ? 
	print " ".join(cmd)
	call(cmd)

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
	sh.write('touch %s/sub%d.run\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.done\n'%(basedir,iJob))
	sh.write('rm %s/sub%d.fail\n'%(basedir,iJob))

	if opts.mount:
		#mountpoint = "~/eos"
		mountpoint = "$WORKDIR/%s_%d/eos"%(opts.dir,iJob)
		sh.write('%s -b fuse mount %s\n'% (EOS,mountpoint))

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
	if not opts.dryrun: 
		call(cmdline,shell=True)

