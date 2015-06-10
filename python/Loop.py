import os,sys,re,time
from glob import glob
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)
parser.add_option("-d","--dat",dest="dat",type='string',help="Configuration file",default='dat/config.dat')

(opts,args) = parser.parse_args()

#  find the base directory

if opts.verbose: print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)

if opts.verbose: print "-> Base Path is " + basepath


#os.environ['LD_LIBRARY_PATH'] =  basepath + "../NeroProducer/Core/bin/" + ":" + os.environ['LD_LIBRARY_PATH']
#sys.path.insert(0,basepath + "../NeroProducer/Core/bin/")
#sys.path.insert(0,os.getcwd())

################ IMPORT ROOT  ################
if opts.verbose: print "-> Importing root",
sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()
if opts.verbose: print "DONE"
################ LOAD LIBRARY ###############
if opts.verbose: print "-> Load Bare library"
r.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
if opts.verbose: print "-> Load ChargedHiggs library"
r.gSystem.Load( "./bin/libChargedHiggs.so")
if opts.verbose: print "DONE",
################ CREATING LOOPER ##########
from ROOT import Looper

loop = Looper()
################ LOAD CONFIGURATION ########
from ParseDat import *
cfg = ParseDat(opts.dat)

if opts.verbose:
	PrintDat(cfg)

### configurable

for file in cfg['Files']:
    for f in FindEOS(file):
	if opts.verbose: print "Adding file: '"+f+"'"
	loop.AddToChain(f)

loop.InitTree();

## init output
if opts.verbose: print "-> Opening output file '"+cfg['Output']+"'",
loop.InitOutput( cfg['Output'] )
if opts.verbose: print "DONE"
## add mc

if opts.verbose:print "######### MCDB ##############"
mcdb= ReadMCDB(cfg['MCDB'])
for label in mcdb:
	if opts.verbose:
		print label, " ".join(mcdb[label])
	### LABEL dir Entries xSec
	### loop.AddMC( label,dir,xSec,Entries)
	loop.AddMC(label, mcdb[label][0], float(mcdb[label][2]),float(mcdb[label][1] ) )
if opts.verbose:print "#############################"

## add PU
if 'pileup' in cfg and cfg['pileup'] != '':
	if opts.verbose: print "Loading PU file",cfg['pileup']
	RD=True
	if 'pileupRun' not in cfg: RD=False
	if RD and cfg['pileupRun'] == [] : RD=False
	if RD and cfg['pileupRun'][0] == -1 : RD=False
	if RD and cfg['pileupLumi'] == [] : RD=False
	if RD and cfg['pileupLumi'][0] == -1 : RD=False

	if opts.verbose:
		if RD: print "-> RD Pileup Reweighting"
		else : print "-> non RD Reweight"

	puFile = r.TFile.Open(cfg['pileup'])
	if puFile == None: print "ERROR: No PU File",cfg['pileup']
	labels= [ x for x in mcdb ]
	labels.extend( ['pileup','pileupUp','pileupDown'] )
	for label in labels:
		if RD:
			for idx in range(0,len(cfg['pileupRun'])-1):
				runMin = cfg['pileupRun'][idx]
				runMax = cfg['pileupRun'][idx+1]
				name = "RD-" + label + "_"+str(runMin)+"_" + str(runMax)
				lumi = cfg['pileupLumi'][idx]
				h = puFile.Get(name)
				if name.startswith("RD-pileup"):
					loop.AddTarget(h,runMin,runMax,lumi)
				elif name.startswith("RD-pileupUp"):
					loop.AddTarget(h,'Up',runMin,runMax,lumi)
				elif name.startswith("RD-pileupDown"):
					loop.AddTarget(h,'Down',runMin,runMax,lumi)
				else:
					loop.AddPuMC( label, h, runMin,runMax )
				
		else:
			name = "PU-"+ label
			h = puFile.Get(name)
			if h == None :
				print "[Loop.py]::[ERROR]: unable to get PU histo",name, "from file",cfg['pileup']
			if name == "PU-pileup":
				loop.AddTarget(h)
			elif name == "PU-pileupUp":
				loop.AddTarget(h,'Up')
			elif name == "PU-pileupDown":
				loop.AddTarget(h,'Down')
			else:
				loop.AddPuMC( label, h )
		

## add SF
if opts.verbose:print "######### SFDB ##############"
sfdb = ReadSFDB( cfg['SFDB'] )
for key in sfdb:
	label = key['label']
	if key['type'] == 'pteta':
		if opts.verbose: print label,key['type'],  key['pt1'] ,key['pt2'],key['eta1'],key['eta2'],key['sf'],key['err'] 
		loop.AddPtEtaSF(label, key['pt1'] ,key['pt2'],key['eta1'],key['eta2'],key['sf'],key['err'])
	if key['type'] == 'base':
		if opts.verbose: print label,key['type'], key['sf'],key['err'] 
		loop.AddSF(label, key['sf'], key['err'])
if opts.verbose:print "#############################"

for smear in cfg['Smear']:
	if smear[0]=='@': 
		smear=smear[1:]
		if opts.verbose: print '-> constructing smear',smear
		smearer = r.__getattr__(smear)()
		loop.AddSmear(smearer)
	else:
		if opts.verbose: print "-> Adding smear from name '"+smear+"'"
		loop.AddSmear(smear)

## add analysis
for analysis in cfg['Analysis']:
	if opts.verbose: print '-> Adding analysis',analysis
	analyzer = r.__getattr__(analysis)()
	if analysis in cfg['config']:
	   for key in cfg['config'][analysis]:
		if opts.verbose:print '  - config keys', key
		## CHECK ATTRIBUTE
		check = key.split('=')[0].split('(')[0]
		#if not hasattr( analyzer, check):
		try: 
			getattr(analyzer, check)
		except AttributeError:
			print "WARNING Analyzer",analysis,"do not have attribute",check
		##
		exec('analyzer.'+key)
	loop.AddAnalysis(analyzer)

if opts.verbose: print "-> Init Analysis"
loop.InitAnalysis()
###

if opts.verbose: print "-> Loop"

loop.Loop()

if opts.verbose: print "-> DONE!!!"

