from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call,check_output
from glob import glob

import sys,os,re

config = config()

config.General.requestName = 'ChargedHiggs_request_ZYX'
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

config.Data.outLFNDirBase = '/store/user/%s/ChargedHiggs/TauNuOneProngV5' % (getUsernameFromSiteDB())
config.Data.publication = False

config.Site.storageSite = 'T2_CH_CERN'
#config.Site.blacklist = [ 'T2_US_Florida','T2_US_Vanderbilt']
#config.Site.whitelist = [ 'T2_CH_CERN','T2_US_MIT','T3_US_MIT']
## white list the site where the ntuples are, or change settings to allow it to run slowly
config.Site.whitelist = [ 'T2_CH_CERN']

config.JobType.scriptExe =  "ChargedHiggsCrab.sh"
config.JobType.outputFiles= ["ChargedHiggsFwk.root","log.txt.gz"]

dat=glob("*.dat")
config.JobType.inputFiles=dat+["package.tar.gz"]


