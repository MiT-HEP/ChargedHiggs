from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call,check_output
from glob import glob

import sys,os,re

config = config()

config.General.requestName = 'ChargedHiggs_request_XXX'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

## JobType
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'pset.py' ## fake pset, not used, don't remove

config.Data.ignoreLocality = True

config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = 10

config.Data.outLFNDirBase = '/store/user/%s/ChargedHiggs/' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.publishDataName =''

config.Site.storageSite = 'T2_CH_CERN'
#config.Site.blacklist = [ 'T2_US_Florida','T2_US_Vanderbilt']
#config.Site.whitelist = [ 'T2_CH_CERN']



if __name__ == '__main__':

    from CRABAPI.RawCommand import crabCommand
    from CRABClient.ClientExceptions import ClientException
    from httplib import HTTPException

    # We want to put all the CRAB project directories from the tasks we submit here into one common directory.
    # That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).
    config.General.workArea = 'ChargedHiggsSubmission'

    def submit(config):
	### for some reason only the first dataset is submitted correctly, work around
	### if len(sys.argv) ==1:
	### 	## book the command and run python
	### 	cmd = "python " + sys.argv[0] + " '" + config.General.requestName + "'"
	### 	print "calling: "+cmd
	### 	call(cmd,shell=True)
	### 	return
	### if len(sys.argv) > 1:
	### 	## if it is not in the request try the next
	### 	if sys.argv[1] !=  config.General.requestName: return
	###
        try:
            crabCommand('submit', config = config)
        except HTTPException as hte:
            print "Failed submitting task: %s" % (hte.headers)
        except ClientException as cle:
            print "Failed submitting task: %s" % (cle)
    def setInputFiles(myconfigs):
    	    config.Data.userInputFiles = []
	    for myconfig in myconfigs:
    		try: 
    			cfg = ParseDat(myconfig)
    		except:
    			sys.path.insert(0,"../python") ## To import
    			sys.path.insert(0,"../") ## to open dat/
    			from ParseDat import ParseDat, FindEOS
    			cfg = ParseDat(myconfig)
    		for file in cfg['Files'] :
    		   for f in FindEOS( file ):
    			   config.Data.userInputFiles.append(f)
	    print "-> user files=",config.Data.userInputFiles
    
    def setOutputFiles(myconfigs):
    	    config.JobType.outputFiles=[ ] 
	    for myconfig in myconfigs:
    		try: 
    			cfg = ParseDat(myconfig)
    		except:
    			sys.path.insert(0,"../python")
    			sys.path.insert(0,"../")
    			from ParseDat import ParseDat, FindEOS
    			cfg = ParseDat(myconfig)
    		config.JobType.outputFiles.append( cfg['Output'] )
	    print "-> Output files=",config.JobType.outputFiles

    def submitDir(dir=""):
	    	## TODO: 
		## * the sh need just to untar
		## * and run Loop.py -v -d input.dat
		## added to the send box
		config.JobType.inputFiles = [
				# use the tar created from sendBatch
				dir + '/package.tar.gz',
				]
		list = glob(dir + '/*dat')
		## load ParseDat
		setInputFiles(list)
		###
		config.JobType.inputFiles.extend(list)
		config.Data.totalUnits = len(list)  ## SET the N. Of jobs
		from ParseDat import ParseDat, FindEOS
		cfg = ParseDat( list[0] ) ## get the first one
		output= cfg['Output']
		outdir = '/'.join(output.split('/')[0:-1])
		outfile = output.split('/')[-1]
		outfile= re.sub( '_[0-9]*\.root','.root', outfile)
		#config.JobType.outputFiles=["QCDPurity.root"] ## TODO automatic --> read from one  input.dat
		config.JobType.outputFiles=[outfile] #
		shFile=dir + "/grid.sh"
		sh = open (shFile,"w")
		sh.write("#!/bin/bash\n")
		sh.write("echo '------------- INFO ------------'\n")
		sh.write(" ls -l \n") ## DEBUG
		sh.write('echo "ARGS=$@" \n') ## DEBUG
		sh.write('NUM=$[$1-1] \n') ## offset by one
		sh.write('env\n' ) ## DEBUG
		sh.write("echo '------------- ---- ------------'\n")
		sh.write("tar -xzf package.tar.gz\n")
		#sh.write("mkdir test/mysub/QCDPurity/\n")  ### TODO automatic 
		sh.write("mkdir -p %s\n"%outdir)  ###
		sh.write("LD_LIBRARY_PATH=$PWD/bin/bare:$LD_LIBRARY_PATH\n")
		sh.write("LD_LIBRARY_PATH=$PWD/bin:$LD_LIBRARY_PATH\n")
		sh.write("LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH\n")
		sh.write("python python/Loop.py -v -d input${NUM}.dat \n")
		sh.write("EXITCODE=$?\n")
		#sh.write("mv t/mysub/QCDPurity/QCDPurity_${NUM}.root QCDPurity.root\n") ### TODO automatic, crab will append the _num.root +1
		sh.write("mv " + outdir + "/" + re.sub('.root','',outfile) + "_${NUM}.root " +outfile+"\n") ### 
		## FJR
		sh.write('FJR="FrameworkJobReport.xml"\n')
		sh.write('[ "${EXITCODE}" == "0" ] && echo \'<FrameworkJobReport Status="Success">\' > ${FJR}\n')
		sh.write('[ "${EXITCODE}" == "0" ] || echo \'<FrameworkJobReport Status="Failed"> \' >${FJR}\n')
		sh.write('[ "${EXITCODE}" == "0" ] || echo "<FrameworkError ExitStatus=\\"${EXITCODE}\\" Type=\\"ExeExitCode\\"/> " >> ${FJR}\n')
		sh.write('echo "<PerformanceReport>">>${FJR}\n')
		sh.write('echo \'   <PerformanceSummary Metric="StorageStatistics">\' >>${FJR}\n')
		sh.write('echo \'       <Metric Name="Parameter-untracked-bool-enabled" Value="true"/>\' >> ${FJR}\n')
		sh.write('echo \'       <Metric Name="Parameter-untracked-bool-stats" Value="true"/>\' >> ${FJR}\n')
		sh.write('echo \'       <Metric Name="Parameter-untracked-string-cacheHint" Value="application-only"/>\' >> ${FJR}\n')
		sh.write('echo \'       <Metric Name="Parameter-untracked-string-readHint" Value="auto-detect"/>\' >> ${FJR}\n')
		sh.write('echo \'       <Metric Name="ROOT-tfile-read-totalMegabytes" Value="0"/>\' >> ${FJR}\n')
		sh.write('echo \'       <Metric Name="ROOT-tfile-write-totalMegabytes" Value="0"/>\' >> ${FJR}\n')
		sh.write('echo \'   </PerformanceSummary>\' >>${FJR}\n')
		sh.write('echo "</PerformanceReport>">>${FJR}\n')
		sh.write('echo "<ReadBranches/>" >>${FJR}\n')
		sh.write('echo "<GeneratorInfo/>">> ${FJR}\n')
		sh.write('echo "</FrameworkJobReport>" >> ${FJR}\n')
		## end FJR
		sh.close()
		call("chmod u+x " + shFile, shell=True )
    		config.General.requestName = dir.replace("/","_")
		config.JobType.scriptExe = shFile
			#config.JobType.scriptArgs = ''
		## call submission function
		submit(config)
    #############################################################################################
    ## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
    #############################################################################################

    ###################################################
    ## todo: set output.root to be transfered, set split, 
    ###################################################

    #submitDir("mysub/QCDPurity")
    submitDir("mysub/QCDPurity3p0")
