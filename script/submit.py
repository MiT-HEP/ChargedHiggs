#!env python

import os,sys
from glob import glob
from subprocess import call, check_output
import re
import tempfile

from optparse import OptionParser, OptionGroup

usage = "usage: %prog [options]"
parser=OptionParser(usage=usage)
parser.add_option("-i","--input" ,dest='input',type='string',help="Input Configuration file",default="")
parser.add_option("-d","--dir" ,dest='dir',type='string',help="Directory where to write the configuration",default="submit")
parser.add_option("-v","--debug" ,dest='debug',type='int',help="Debug Verbosity. From 0-3. Default=%default",default=0)
parser.add_option("-n","--njobs" ,dest='njobs',type='int',help="Number of Job to submit",default=50)
parser.add_option("-q","--queue" ,dest='queue',type='string',help="Queue",default="1nh")
parser.add_option("","--aaa" ,dest='aaa',action='store_true',help="Always use AAA for datasets",default=False)
parser.add_option("","--full-check" ,dest='fullcheck',action='store_true',help="Check every file for aaa vs eos",default=False)

job_opts= OptionGroup(parser,"Job options:","these options modify the job specific")
job_opts.add_option("-t","--no-tar" ,dest='tar',action='store_false',help="Do not Make Tar",default=True)
job_opts.add_option("-p","--proxy" ,dest='proxy',action='store_true',help="Transfer Proxy",default=False)
job_opts.add_option("","--dryrun" ,dest='dryrun',action='store_true',help="Do not Submit",default=False)
job_opts.add_option("","--no-compress" ,dest='compress',action='store_false',help="Don't compress",default=True)
job_opts.add_option("","--compress"    ,dest='compress',action='store_true',help="Compress stdout/err")
job_opts.add_option("","--local"    ,dest='local',action='store_true',help="Use local directory for output and later cp",default=False)
job_opts.add_option("-m","--mount-eos" ,dest='mount',action='store_true',help="Mount eos file system.",default=False)
job_opts.add_option("","--hadoop" ,dest='hadoop',action='store_true',help="Use Hadhoop and MIT-T3",default=False)
job_opts.add_option("","--condor" ,dest='condor',action='store_true',help="Use Condor Batch (LXPLUS) -- default",default=True)
job_opts.add_option("","--lsf" ,dest='condor',action='store_false',help="Use LSF (deprecated)")
job_opts.add_option("-c","--cp" ,dest='cp',action='store_true',help="cp Eos file locally. Do not use xrootd. ",default=False)
job_opts.add_option("","--nosyst" ,dest='nosyst',action='store_true',help="Do not Run Systematics",default=False)
job_opts.add_option("","--config" ,dest='config',action='append',type="string",help="add line in configuration",default=[])
job_opts.add_option("","--venv" ,dest='venv',action='store_true',help="Use virtual environment in ChHiggs/venv",default=False)

hadd_opts=OptionGroup(parser,"Hadd options","these options modify the behaviour of hadd")
hadd_opts.add_option("","--hadd" ,dest='hadd',action='store_true',help="Hadd Directory.",default=False)
hadd_opts.add_option("","--clear" ,dest='clear',action='store_true',help="Clear Directory (used with hadd).",default=False)
hadd_opts.add_option("","--nocheck" ,dest='nocheck',action='store_true',help="Skip checking of log files in hadd. 'zcat log*.txt.gz| grei -i error' ",default=False)

summary= OptionGroup(parser,"Summary","these options are used in case of summary is wanted")
summary.add_option("-s","--status",dest="status", action='store_true', help = "Display status information for a submission. (Can be use with hadoop option)", default=False)
summary.add_option("","--resubmit",dest="resubmit", action='store_true', help = "Resubmit failed jobs. (no hadoop)", default=False)
summary.add_option("-j","--joblist",dest="joblist", type='string', help = "Resubmit this job list. '' or 'fail' will submit the failed jobs. 'run' will submit the running jobs. Job list = 3,5,6-10", default="")

parser.add_option_group(job_opts)
parser.add_option_group(hadd_opts)
parser.add_option_group(summary)

(opts,args)=parser.parse_args()

#EOS='/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select'
EOS='/usr/bin/eos'
instance="root://eoscms/"
if '/eos/user' in opts.dir: instance="root://eosuser/"
EOS2=EOS+" " + instance + " "

if 'CMSSW_BASE' not in os.environ:
	print "-> Use a CMSSW environment: cmsenv"
	exit(0)

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

## bash color string
red="\033[01;31m"
green = "\033[01;32m"
yellow = "\033[01;33m"
cyan = "\033[01;36m"
white = "\033[00m"

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

def Print_ (done, run, fail, pend):
	'''Internal'''
	tot = len(run) + len(fail) + len(done) + len(pend)

	color = red
	if len(run) > len(fail) and len(run) >len(pend)  : color= yellow
	if len(pend) > len(run) and len(pend) >len(fail): color= cyan
	if len(done) == tot and tot >0 : color = green
	
	print " ----  Directory "+ color+opts.dir+white+" --------"
	print " Pend: " + cyan   + "%3d"%len(pend) + " / " + str(tot) + white + " : " + PrintLine(pend)  ###
	print " Run : " + yellow + "%3d"%len(run) + " / "  + str(tot) + white + " : " + PrintLine(run)  ### + ",".join(run)  + "|" 
	print " Fail: " + red    + "%3d"%len(fail) + " / " + str(tot) + white + " : " + PrintLine(fail) ### + ",".join(fail) + "|" 
	print " Done: " + green  + "%3d"%len(done) + " / " + str(tot) + white + " : " + PrintLine(done) ### + ",".join(done) + "|" 
	print " -------------------------------------"

def PrintSummary(dir, doPrint=True):
	''' Print summary informations for dir'''
	run  = glob(dir + "/*run")
	fail = glob(dir + "/*fail")
	done = glob(dir + "/*done")
	pend = glob(dir + "/*pend")

	run = [ re.sub('\.run','' , re.sub('.*/sub','', r) ) for r in run ] 	
	fail = [ re.sub('\.fail','' , re.sub('.*/sub','', r) ) for r in fail ] 	
	done = [ re.sub('\.done','' , re.sub('.*/sub','', r) ) for r in done ] 	
	pend = [ re.sub('\.pend','' , re.sub('.*/sub','', r) ) for r in pend ] 	

	if doPrint:
		Print_(done,run,fail,pend)

	return ( done, run, fail, pend)

def PrintHadhoop(dir, doPrint = True):
	log = glob ( dir + "/test.*.log" )
	done=[]
	run =[]
	fail=[]
	pend=[]
	for l in log:
		iJob = re.sub('.*/test\.','', re.sub("\.log","" , l ) )
		status = call( "cat %s | grep 'Job executing on host' >& /dev/null"%(l) , shell=True)
		if status != 0 :
			pend.append(iJob)
			continue
		status = call( "cat %s | grep 'Job terminated.' >& /dev/null"%(l) , shell=True)
		if status != 0:
			run.append(iJob)
			continue
		status = call( "cat %s | grep 'return value 0' >& /dev/null"%(l) , shell=True)
		if status != 0:
			fail.append(iJob)
			continue
		outlist = glob(dir +"/*_%s.root"%iJob)
		if len(outlist) != 1:
			print "unable to find outfile in:",outlist
			fail.append(iJob)
			continue
		outname=outlist[0]
		if os.stat(outname).st_size == 0:
			fail.append(iJob)
			continue
		done.append(iJob)
	
	if doPrint:
		Print_(done,run,fail,pend)
	return ( done, run, fail)

def write_condor_jdl(filename="condor.jdl"):
    ## write condor.jdl for batch submission and resubmission
    if opts.queue == "1nd" or opts.queue == "tomorrow": queue = "tomorrow"
    elif opts.queue == "1nh" or opts.queue == "microcentury": queue = "microcentury"
    elif opts.queue == "2nh" or opts.queue == "longlunch": queue = "longlunch"
    elif opts.queue == "8nh" or opts.queue == "workday": queue = "workday"
    elif opts.queue == "2nd" or opts.queue == "testmatch": queue = "testmatch" ## actually 3nd
    elif opts.queue == "1nw" or opts.queue == "nextweek": queue = "nextweek"
    else:  raise ValueError("Unknown queue:"+opts.queue)
    
    jdl = open(opts.dir +"/"+filename,"w")
    jdl.write('requirements = (OpSysAndVer =?= "CentOS7")\n')
    #jdl.write('requirements = (OpSysAndVer =?= "SLC6")\n')
    jdl.write("log = $(filename).log\n")
    jdl.write("output = $(filename).out\n")
    jdl.write("error = $(filename).err\n")
    jdl.write("executable = $(filename)\n")
    jdl.write("+JobFlavour = \"%s\"\n"%queue)
    if os.environ['USER']=='amarini' : jdl.write('+AccountingGroup = "group_u_CMST3.all"\n')

    #jdl.write("transfer_input_files = %(dir)s/package.tar.gz,%(input)s\n"%{"dir":subdir,"input": ",".join(inputLs)})
    #jdl.write("queue filename matching (%s/sub*sh)\n"%opts.dir)
    #jdl.close()
    return jdl

if opts.status:
	if opts.hadoop:
		PrintHadhoop(opts.dir,doPrint=True)
	else:
		( done, run, fail, pend) = PrintSummary(opts.dir,doPrint=True)
		cpu=[]
		termcpu=re.compile("TERM_CPULIMIT")
		for iJob in fail + run:
		    try:
		        log = open(opts.dir + "/log"+iJob+".txt")
		    except IOError: 
		        continue
		    if re.search("TERM_CPULIMIT",log.read() ) != None : cpu.append(iJob)
		    log.close()
		if len(cpu) >0 :
		    print " CPU LIMIT",PrintLine(cpu)
		    print " -------------------------------------"
		#if opts.fullstatus:
		notRunning=[]
		#bjobs = re.sub('//','/',check_output("bjobs -w",shell=True))
        if not opts.nocheck:
            condor_q= check_output("condor_q",shell=True) 
        else:
            condor_q=""
        if opts.dir not in condor_q and not opts.nocheck: 
		    print " NOT RUNNING! (condor) ", opts.dir
		    print " -------------------------------------"
        if not opts.nocheck: ## check size
            ls = check_output("ls -l %s/*root | tr -s ' ' | cut -d ' ' -f 5,9 "%opts.dir, shell=True)
            allfiles={}
            maxsize=0
            import ROOT
            for line in ls.split('\n'):
                if len(line.split() )< 2: continue
                try:
                    size = float(line.split()[0])
                    f = line.split()[1]
                    allfiles[f] = size
                    maxsize= max(size,maxsize)
                    fTry = ROOT.TFile.Open(f)
                    if fTry == None or fTry.IsZombie() : print "file:", f ,"is corrupted"
                    elif fTry.TestBit(ROOT.TFile.kRecovered): print "file:",f,"is recovered"
                    fTry.Close()
                except Exception as e:
                    print e
                    pass
            #print "Max size is",maxsize
            #for f in allfiles:
            #    if allfiles[f] < 0.75*maxsize: print "file:",f,"is suspiciously small"
	exit(0)

if opts.resubmit:
    ( done, run, fail, pend) = PrintSummary(opts.dir,False)
    
    if opts.joblist == '' or opts.joblist.lower() == 'fail':
        joblist = fail
    elif opts.joblist.lower() =='run':
        joblist = run
    elif opts.joblist.lower() == 'pend' :
        joblist = pend
    elif opts.joblist.lower() == 'done' :
        joblist = done
    elif opts.joblist.lower() == 'all' :
        joblist = pend + run + fail + done
    else:
        joblist = opts.joblist.split(',')

    if opts.proxy and opts.tar: ##update proxy
        if 'X509_USER_PROXY' in os.environ and '/afs/' in os.environ['X509_USER_PROXY']:
            pass
        else:
            print "-> Updating proxy in tar file","%s/package.tar.gz"%opts.dir, "Don't stop me now"
            cmd=["gunzip", "%s/package.tar.gz"%opts.dir]
            print "  * Uncopressing",' ' .join(cmd)
            call(cmd)

            cmd=["tar","-rf","%s/package.tar"%opts.dir]
            cmd.extend( glob("/tmp/x509up_u%d"%os.getuid())) 
            print "  * extending",' ' .join(cmd)
            call(cmd)

            cmd=["gzip", "%s/package.tar"%opts.dir]
            print "  * compressing",' ' .join(cmd)
            call(cmd)

            print "-> Proxy updated"

    jdl=write_condor_jdl("condor_resubmit.jdl") ##  write preample without queueing
    #jdl.write("queue filename matching (%s/sub*sh)\n"%opts.dir)
    for job in joblist:
        if '-' in job: 
           iBegin= int(job.split('-')[0])
           iEnd = int(job.split('-')[1])
        else: 
           iBegin= int(job)
           iEnd = int(job)
        for iJob in range(iBegin,iEnd+1):
            #iJob= int(job)
            if opts.dir[0] == '/': basedir = opts.dir
            else: basedir = os.environ['PWD'] + "/" + opts.dir
            touch = "touch " + basedir + "/sub%d.pend"%iJob
            call(touch,shell=True)
            cmd = "rm " + basedir + "/sub%d.fail"%iJob + " 2>&1 >/dev/null"
            call(cmd,shell=True)
            cmd = "rm " + basedir + "/sub%d.run"%iJob + " 2>&1 >/dev/null"
            call(cmd,shell=True)
            cmd = "rm " + basedir + "/log%d.txt"%iJob + " 2>&1 >/dev/null"
            call(cmd,shell=True)
            ## try to remove the ChHiggs_%d.root / QCDPurity
            cmd = "rm " + basedir + "/ChHiggs_%d.root"%iJob + " 2>&1 >/dev/null"
            call(cmd,shell=True)
            cmd = "rm " + basedir + "/QCDPurity_%d.root"%iJob + " 2>&1 >/dev/null"
            call(cmd,shell=True)
            cmd = "rm " + basedir + "/VBShad*_%d.root"%iJob + " 2>&1 >/dev/null"
            call(cmd,shell=True)

            if opts.condor:
                jdl.write("filename=%s/sub%d.sh\n"%(opts.dir,iJob))
                jdl.write("queue\n\n")
            else:
                cmdline = "bsub -q " + opts.queue + " -o %s/log%d.txt"%(basedir,iJob) + " -J " + "%s/Job_%d"%(opts.dir,iJob) + " %s/sub%d.sh"%(basedir,iJob)
                print cmdline
                call (cmdline,shell=True)
    if opts.condor:
        jdl.close()
        ## submit condor
        print "-> Submitting","%s/condor_resubmit.jdl"%opts.dir
        cmd = "condor_submit -batch-name %s %s/condor_resubmit.jdl"%(opts.dir,opts.dir)
        if not opts.dryrun and '/eos/' in opts.dir: 
                #'/eos/user/a/amarini'
                user=os.environ['USER']
                l = opts.dir.split('/') 
                l2=l[l.index(user)+1:]
                mydir='/'.join(l2)
                print "->eos workaround: using local dir",mydir
                call("mkdir -p %s"%mydir,shell=True)
                call("cp  %s/*{sh,jdl} %s/"%(opts.dir,mydir),shell=True)
                call("sed -i'' 's:/eos/user/"+user[0]+"/"+user+"/::g' %s/condor_resubmit.jdl"%mydir,shell=True)
                call("sed -i'' 's:/eos/cms/store/user/"+user+"/::g' %s/condor_resubmit.jdl"%mydir,shell=True)
                cmd ="condor_submit -batch-name %s %s/condor_resubmit.jdl"%(opts.dir,mydir)
                print "   cmd=",cmd
                status = call(cmd,shell=True)
                if status !=0:
                    print "unable to submit,",cmd
                else:
                    print cmd
        elif not opts.dryrun: 
            print "   cmd=",cmd
            call(cmd, shell=True)
        else:
            print "   cmd=",cmd
    exit(0)

if opts.hadd:
	filelist = glob(opts.dir + "/*.root")
	if opts.dir[-1] == '/':dir = opts.dir[:-1]
	else: dir  = opts.dir[:]

	if not opts.nocheck:
		cmd = "zcat %s/log*.txt.gz | grep -i error | sort | uniq -c "%dir
		st=call(cmd,shell=True)

		cmd = "zcat %s/log*.txt.gz | grep -i '\[error\]' > /dev/null "%dir
		st=call(cmd,shell=True)
		if st == 0 and opts.clear:
			print "-> Errors have been found. Refusing to clear"
			opts.clear = False
		cmd = "zcat %s/log*.txt.gz | grep -i warning | sort | uniq -c "%dir
		call(cmd,shell=True)

	name = re.sub('.*/','',dir)
	cmd = "[ -f %s%s.root ] && rm -v %s/%s.root"%(opts.dir,name,opts.dir,name) ## remove the file in oredr not to double count
	call(cmd,shell=True)
	cmd = "hadd -f %s/%s.root "%(opts.dir, name ) + " ".join(filelist)
	st=call(cmd,shell=True)

	if st !=0 :
		print "-> Unsuccessfull hadd. (in case refuse to clear). Removing created file."
		cmd="rm -v %s/%s.root "%(opts.dir, name )
		call(cmd,shell=True)
		opts.clear = False

	if opts.clear: 
		filelist = glob(opts.dir + "/*")
		rmlist = [ f for f in filelist if  re.sub('^.*/','',f) != name + ".root"]
		keeplist = [ f for f in filelist if re.sub('^.*/','',f) == name + ".root"]
		rmcmd = "rm " + " ".join(rmlist)
		#for i in rmlist:
		#	print i, "'"+re.sub('^.*/','',f) + "'", "'" + name + ".root'"
		print " I will keep = '",keeplist
		call(rmcmd,shell=True)
	exit(0)

# import Parser
from ParseDat import *
config=ParseDat(opts.input)

ds=DirectoryStore()
ds.init()
for f in config['Files']: ds.add(f) ## w/o * expansion
ds.end()

call("[ -d %s ] && rm -r %s"%(opts.dir,opts.dir),shell=True)

if '/eos/' in opts.dir or '/store/' in opts.dir:
    print "->> calling eos mkdir"
    call("[ -d %s ] && %s rm -r %s"%(opts.dir,EOS2,opts.dir),shell=True)
    call(EOS2+" mkdir %s"%opts.dir,shell=True)

call("mkdir -p %s"%opts.dir,shell=True) ## try again
cmdFile=open("%s/submit_cmd.txt"%opts.dir,"w")
cmdFile.write("##Commands used to submit on batch. Automatic written by python/submit.py script\n")
cmdFile.write("##"+' '.join(sys.argv)+"\n")

if opts.tar:
	cmd=["tar","-czf","%s/package.tar.gz"%opts.dir]
	if True: ## copy also the bare library in the tar. for grid submission
		cmdBare = "mkdir -p ./bin/bare"
		call(cmdBare,shell=True)
		cmdBare = "cp " +os.environ["CMSSW_BASE"] + "/src/NeroProducer/Core/bin/libBare.so ./bin/bare/"
		call(cmdBare,shell=True)
		cmdBare = "cp " +os.environ["CMSSW_BASE"] + "/src/NeroProducer/Core/bin/dict_rdict.pcm ./bin/bare/"
		call(cmdBare,shell=True)
		## run time libraries needs also the .h files :(
		cmdBare = "mkdir -p ./bin/interface"
		call(cmdBare,shell=True)
		cmdBare = "cp " + os.environ["CMSSW_BASE"] + "/src/NeroProducer/Core/interface/*hpp ./bin/interface/"
		call(cmdBare,shell=True);
		### this file is produced by make
		#cmdBare = "cp bin/libChargedHiggs.so bin/libChargedHiggs.0.so"
		#call(cmdBare,shell=True)
		#cmdBare = "/afs/cern.ch/user/a/amarini/public/patchelf --set-rpath '' bin/libChargedHiggs.0.so"
		#call(cmdBare,shell=True)
	cmd.extend( glob("bin/bare/*" ) )
	cmd.extend( glob("bin/interface/*" ) )
	cmd.extend( glob("bin/*so" ) )
	cmd.extend( glob("bin/dict*" ) )
	#cmd.extend( glob("bin/tag.txt" ) )
	#cmd.extend( glob("dat/*dat" ) )
	#cmd.extend( glob("dat/*txt" ) )
	cmd.extend( glob("dat/*" ) )
	cmd.extend( glob("aux/*" ) )
	cmd.extend( glob("python/*py") )
	#cmd.extend( glob("test/*") )
	cmd.extend( glob("test/*py") )
	cmd.extend( glob("test/*C") )
	cmd.extend( glob("test/*.hpp") )
	cmd.extend( glob("test/*.cpp") )
	cmd.extend( glob("test/*.o") )
	cmd.extend( glob("test/*.so") )
	cmd.extend( glob("test/*.exe") )
	cmd.extend( glob("interface/*hpp" ) ) ## who is the genius that in ROOT6 need these at run time ? 
	#/tmp/x509up_u45059
	if opts.proxy: 
		if 'X509_USER_PROXY' in os.environ and '/afs/' in os.environ['X509_USER_PROXY']:
			pass
		else:
			cmd.extend( glob("/tmp/x509up_u%d"%os.getuid())) ## export X509_USER_PROXY=x509up_u$(id -u)
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
        if f== "": continue
        list=[]
        if '/store/' in f or '/eos/user' in f:
            if opts.hadoop:
                list = FindHadoop( f ) 
            elif opts.mount:
                list =  FindEOS(f, "%%MOUNTPOINT%%/eos")
            else:
                list =  FindEOS(f)
        elif f.split('~')[0].split('/')[-1] in ['NANOAODSIM','NANOAOD','USER']:
                print "DEBUG-submit,Using dataset for",f, "forcing aaa" if opts.aaa else ""
                if opts.aaa:
                    list =  FindDataset(f,"aaa" )
                elif opts.fullcheck:
                    list =  FindDataset(f,"full-check")
                else:
                    list =  FindDataset(f)
        else :
            list=glob(f)
            if list == []: ### maybe remote ?
                list=f
        if len(list)==0:
            print "<*> Error in File list from",f,"-- No File Found"
            raise IOError
        fileList.extend(list)
    config['Files']=fileList
    splittedInput=chunkIt(config['Files'],opts.njobs )

if opts.hadoop: ### T3 MIT
   if len(fileList) ==0:
        print "filelist has 0 len: Nothing to be done"
        exit(0)
   redirect = ">&2"
   run= open("%s/run.sh"%opts.dir,"w")
   run.write("#!/bin/bash\n")
   run.write("JOBID=$1\n")
   run.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
   # if x509 proxy is copied to the workspace
   # export X509_USER_PROXY=x509up_u$(id -u)
   #use_x509userproxy = true
   run.write("scram p CMSSW %s\n"%os.environ['CMSSW_VERSION'])
   run.write("cd %s/src\n"%os.environ['CMSSW_VERSION'])
   run.write("eval `scram runtime -sh`\n")
   run.write("tar xzf ../../package.tar.gz\n")
   run.write("mkdir -p ../NeroProducer/Core/bin\n")
   run.write("cp -v bin/bare/* ../NeroProducer/Core/bin\n") 
   run.write("mkdir -p ../NeroProducer/Core/interface\n")
   run.write("cp -v bin/interface/* ../NeroProducer/Core/interface\n") 
   run.write('LD_LIBRARY_PATH=./:./bin/:$LD_LIBRARY_PATH\n')
   run.write('LD_LIBRARY_PATH=$LD_LIBRARY_PATH:'+os.environ['PWD']+':'+os.environ['PWD']+"/bin"+'\n')
   run.write("echo --- ENV ---- %s\n"%redirect)
   run.write("env | sed 's/^/ENV ---/' %s \n"%redirect)
   run.write("echo --- LS ---- %s\n"%redirect)
   run.write("ls -l | sed 's/^/LS ---/' %s\n"%redirect)
   #cmsRun cfg.py jobId=$JOBID
   run.write("python python/Loop.py -v -d ../../input${JOBID}.dat %s\n"%redirect)
   run.write('EXITCODE=$?\n')
   run.write("echo Finished At: %s\n"%redirect)
   run.write("date %s\n"%redirect)
   run.write("echo EXITCODE is $EXITCODE %s\n"%redirect)
   run.write("exit $EXITCODE\n")
   run.close()
   
   inputLs =[]
   subdir="."
   for iJob in range(0,opts.njobs):
     ntry=0
     while ntry<3:
        if len(splittedInput[iJob]) == 0 : 
             print "No file to run on for job "+ str(iJob)+"," + red + " will not send it!" + white
             continue
        outname = re.sub('.root','_%d.root'%iJob,config['Output'])
        dat=open("%s/input%d.dat"%(opts.dir,iJob),"w")
        inputLs.append("%s/input%d.dat"%(subdir,iJob))
        dat.write("include=%s\n"%opts.input)
        dat.write('Files=%s\n'%( ','.join(splittedInput[iJob]) ) )
        dat.write('Output=%s\n'%(outname) )
        if opts.nosyst:
            dat.write("Smear=NONE\n")
        for l in opts.config:
            dat.write(l+"\n")
        dat.close()
        # check that created dat file is non-empty
        ntry+=1
        if os.stat("%s/input%d.dat"%(opts.dir,iJob)).st_size >20: break
        print "-> Dat file not written correctly at trial",ntry
        if ntry==3: 
            sys.exit(0)
   # create condor.jdl
   outname = re.sub('.root','_$(Process).root',config['Output'])
   condor=open("%s/condor.jdl"%opts.dir,"w")
   condor.write("executable = %s/run.sh\n"%subdir)
   condor.write("should_transfer_files = YES\n")
   condor.write("when_to_transfer_output = ON_EXIT\n")
   condor.write("transfer_input_files = %(dir)s/package.tar.gz,%(input)s\n"%{"dir":subdir,"input": ",".join(inputLs)})
   condor.write("universe = vanilla\n")
   condor.write("log = test.$(Process).log\n")
   condor.write("output = %s\n"%outname)
   condor.write("error = test.$(Process).err\n")
   condor.write("requirements = Arch == \"X86_64\" && OpSysAndVer == \"SL6\" && HasFileTransfer\n")
   condor.write("arguments = $(Process)\n")
   condor.write("use_x509userproxy = true\n") ## X509 should work
   condor.write("queue %d\n"%(len(inputLs)))
   condor.close()
   cmdFile.write("cd %s\n"%opts.dir)
   cmdFile.write("condor_submit condor.jdl\n")
   cmd ="cd %s && condor_submit condor.jdl"%opts.dir
   if not opts.dryrun: 
        status = call(cmd,shell=True)
        if status !=0:
            print "unable to submit,",cmd
        else:
            print cmd


##################### WRITE BATCH ###########################
if not opts.hadoop:
    #if opts.condor:
    if True: ##always do it
        jdl=write_condor_jdl() ##  write preample without queueing
        jdl.write("queue filename matching (%s/sub*sh)\n"%opts.dir)
        jdl.close()

    for iJob in range(0,opts.njobs):
        sh=open("%s/sub%d.sh"%(opts.dir,iJob),"w")
        basedir=opts.dir
        if basedir[0] != '/': basedir=os.environ['PWD'] + "/" + opts.dir
        
        sh.write('#!/bin/bash\n')
        #sh.write('[ "$WORKDIR" == "" ] && export WORKDIR="/tmp/%s/" \n'%(os.environ['USER']))
        sh.write('[ "$WORKDIR" == "" ] && export WORKDIR=`mktemp -d` \n')
        sh.write('cd %s\n'%(os.getcwd() ) )
        sh.write('eval `scramv1 runtime -sh`\n') # cmsenv
        sh.write('LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH\n'%os.getcwd())
    
        if opts.venv:
            sh.write('PYTHONPATH=%s/venv/lib/python2.7/site-packages/:$PYTHONPATH\n'%os.getcwd())
            sh.write('PATH=%s/venv/bin:$PATH\n'%os.getcwd())
    
        if opts.tar:
            sh.write("mkdir -p $WORKDIR/%s_%d\n"%(opts.dir,iJob))
            sh.write("cd $WORKDIR\n")
            #sh.write("ln -s %s_%d/interface ./\n"%(opts.dir,iJob)) ## not including it was ok
            #sh.write("cd $WORKDIR/%s_%d\n"%(opts.dir,iJob)) ## not including it was ok
            #sh.write('LD_LIBRARY_PATH=${PWD}:${PWD}/bin:$LD_LIBRARY_PATH\n') ## TODO: test
            sh.write("tar -xzf %s/package.tar.gz\n"%(basedir ))
            sh.write("mkdir -p %s\n"%opts.dir)
            sh.write("cp %s/*dat %s/\n"%(basedir,opts.dir))
            if opts.proxy: 
                if 'X509_USER_PROXY' in os.environ and '/afs/' in os.environ['X509_USER_PROXY']:
                    sh.write("export X509_USER_PROXY=%s\n"%os.environ['X509_USER_PROXY'])
                else:
                    sh.write("export X509_USER_PROXY=/tmp/x509up_u%d\n"%os.getuid())
                    sh.write("cp -v tmp/x509up_u%d /tmp/\n"%os.getuid())
    
        touch = "touch " + basedir + "/sub%d.pend"%iJob
        call(touch,shell=True)
        cmd = "rm " + basedir + "/sub%d.run 2>&1 >/dev/null"%iJob + " 2>&1 >/dev/null"
        call(cmd,shell=True)
        cmd = "rm " + basedir + "/sub%d.done 2>&1 >/dev/null"%iJob + " 2>&1 >/dev/null"
        call(cmd,shell=True)
        cmd = "rm " + basedir + "/sub%d.fail 2>&1 >/dev/null"%iJob + " 2>&1 >/dev/null"
        call(cmd,shell=True)
    
        sh.write('date > %s/sub%d.run\n'%(basedir,iJob))
        sh.write('rm %s/sub%d.done 2>&1 >/dev/null\n'%(basedir,iJob))
        sh.write('rm %s/sub%d.pend 2>&1 >/dev/null\n'%(basedir,iJob))
        sh.write('rm %s/sub%d.fail 2>&1 >/dev/null\n'%(basedir,iJob))
    
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
            if opts.local:
                compressString="2>&1 | gzip > log%d.txt.gz"%(iJob)
            else:
                compressString="2>&1 | gzip > %s/log%d.txt.gz"%(opts.dir,iJob)
        else: compressString =""
    
        if opts.tar:
            ## trigger mountpoint for eos?
            sh.write("cd %s\n"%opts.dir)
            sh.write("touch trigger.mountpoint.tmp\n")
            sh.write("cd $WORKDIR\n")

        sh.write('python python/Loop.py -v -d %s/input%d.dat %s\n'%(opts.dir,iJob,compressString))
    
        if opts.compress:
            sh.write('EXITCODE=${PIPESTATUS[0]}\n')
        else:
            sh.write('EXITCODE=$?\n')

        sh.write('rm %s/sub%d.run 2>&1 >/dev/null\n'%(basedir,iJob))
        sh.write('[ $EXITCODE == 0 ] && touch %s/sub%d.done\n'%(basedir,iJob))
        sh.write('[ $EXITCODE != 0 ] && echo $EXITCODE > %s/sub%d.fail\n'%(basedir,iJob))
        sh.write('echo "Exit code is ${EXITCODE}"\n')
    
        if opts.mount:
            sh.write('%s -b fuse umount %s\n'% (EOS,mountpoint))
    
        outname = re.sub('.root','_%d.root'%iJob,config['Output'])
    
        if opts.tar:
            cp='mv -v'
            dest=''
            if '/store/' or '/eos' in basedir: 
                cp=EOS + ' cp  '
                dest=instance
            if opts.local:
                #sh.write("[ $EXITCODE == 0 ] && mv -v %s %s/ || { echo TRANSFER > %s/sub%d.fail; rm %s/sub%d.done; }  \n"%(outname,basedir,basedir,iJob,basedir,iJob))
                sh.write("[ $EXITCODE == 0 ] && "+cp+" %s %s%s/ || { echo TRANSFER > %s/sub%d.fail; rm %s/sub%d.done; }  \n"%(outname,dest,basedir,basedir,iJob,basedir,iJob))
            elif basedir != opts.dir: 
                sh.write("[ $EXITCODE == 0 ] && "+cp + " %s/%s %s%s/ || { echo TRANSFER > %s/sub%d.fail; rm %s/sub%d.done; }  \n"%(opts.dir,outname,dest,basedir,basedir,iJob,basedir,iJob))
            if opts.compress:
                if opts.local:
                    sh.write(cp +" log%d.txt.gz %s%s/log%d.txt.gz\n"%(iJob,dest,basedir,iJob) )
                else:
                    sh.write(cp +" %s/log%d.txt.gz %s%s/log%d.txt.gz\n"%(opts.dir,iJob,dest,basedir,iJob) )


        sh.write('echo "Finished At:"\n')
        sh.write("date\n")
        sh.close()
        
        dat=open("%s/input%d.dat"%(opts.dir,iJob),"w")
        dat.write("include=%s\n"%opts.input)
        dat.write(re.sub('%%MOUNTPOINT%%',"./", 'Files=%s\n'%( ','.join(splittedInput[iJob]) ) ))

        if opts.local:
            dat.write('Output=%s\n'%(outname) )
        else:
            dat.write('Output=%s/%s\n'%(opts.dir,outname) )

        if opts.nosyst:
            dat.write("Smear=NONE\n")
        for l in opts.config:
            dat.write(l+"\n")

        dat.close()
        ## make the sh file executable
        call(["chmod","u+x","%s/sub%d.sh"%(opts.dir,iJob)])
    
        #sh.write('#$-N %s/Job_%d\n'%(opts.dir,iJob))
        if len(splittedInput[iJob]) == 0 : 
            print "No file to run on for job "+ str(iJob)+"," + red + " will not send it!" + white
            continue
        ## submit
    if opts.condor:
            ## submit condor
            print "-> Submitting","%s/condor.jdl"%opts.dir
            cmd = "condor_submit -batch-name %s %s/condor.jdl"%(opts.dir,opts.dir)
            print "   cmd=",cmd
            if not opts.dryrun and '/eos/' in opts.dir: 
                #'/eos/user/a/amarini'
                user=os.environ['USER']
                l = opts.dir.split('/') 
                l2=l[l.index(user)+1:]
                mydir='/'.join(l2)
                print "->eos workaround: using local dir",mydir
                call("mkdir -p %s"%mydir,shell=True)
                call("cp  %s/*{sh,jdl} %s/"%(opts.dir,mydir),shell=True)
                call("sed -i'' 's:/eos/user/"+user[0]+"/"+user+"/::g' %s/condor.jdl"%mydir,shell=True)
                call("sed -i'' 's:/eos/cms/store/user/"+user+"/::g' %s/condor.jdl"%mydir,shell=True)
                cmd ="condor_submit -batch-name %s %s/condor.jdl"%(opts.dir,mydir)
                status = call(cmd,shell=True)
                if status !=0:
                    print "unable to submit,",cmd
                else:
                    print cmd
            elif not opts.dryrun: 
                call(cmd, shell=True)
## END
