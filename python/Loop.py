
import os,sys,re,time
import json
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

############# easier to parse json files in python
def getJson(fname):
    try:
        jstring = open(fname).read()
    except IOError:
        jstring = fname
    return json.loads( jstring )

def applyJson(obj,fname):
    if opts.verbose: print "-> Apply JSON '" + fname + "' to objet '"+ obj.name() + "'" 
    goodlumis = getJson( fname )
    
    for run in goodlumis.keys():
        for lumis in goodlumis[run]:
            obj.addGoodLumi(int(run), int(lumis[0]), int(lumis[1]) )

################ IMPORT ROOT  ################
if opts.verbose: print "-> Importing root",
sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()
if opts.verbose: print "DONE"
################ LOAD LIBRARY ###############
if opts.verbose: print "-> Load Bare library"
status=r.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
rpath=True
if status < 0:
	if opts.verbose: print "-> trying bin/bare"
	status = r.gSystem.Load("bin/bare/libBare.so")
	sys.path.insert(0,"bin/bare")
	rpath=False
	if status<0: print " Failed to load libBare.so"
if status >= 0 and opts.verbose: print "DONE"

if opts.verbose: print "-> Load ChargedHiggs library"
if rpath:
	r.gSystem.Load( "./bin/libChargedHiggs.so")
else: ## it's likely that this will work
	r.gSystem.Load("./bin/libChargedHiggs.0.so")
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

if opts.verbose: print "-> InitTree"
loop.InitTree();

## activate branches
if opts.verbose: print "-> Activate branches"
branches=[]
for file in cfg['Branches']:
	branches.extend( ReadBranches(file) )
for b in branches:
	if opts.verbose: print " * ",b
	loop.ActivateBranch(b)

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
		if label[-1] == '/':
			label = label[:-1]
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
			name2=""
			h = puFile.Get(name)
			if h == None and 'pileup' in label:
				name2= re.sub("pileupUp","target_Up",name)
				name2= re.sub("pileupDown","target_Down",name2)
				name2= re.sub("pileup","target",name2)
				h= puFile.Get( name2)

			if h == None :
				print "[Loop.py]::[ERROR]: unable to get PU histo '" + name +"' from file",cfg['pileup']
				if name2 != "" : print "[Loop.py]::[ERROR]: nor '"+name2+"'"

			if name == "PU-pileup" or name=="PU-target":
				loop.AddTarget(h)
			elif name == "PU-pileupUp" or name == "PU-target_Up":
				loop.AddTarget(h,'Up')
			elif name == "PU-pileupDown" or name == "PU-target_Down":
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
	if key['type'] == 'spline':
		if opts.verbose: print label,key['type'], key['pt'], key['sf'],key['err'] 
		loop.AddSplineSF(label,key['pt'],key['sf'],key['err'])
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

for corr in cfg['Correct']:
	if opts.verbose: print "-> constructing corrector",corr
	if corr=='NONE' : continue
	c = r.__getattr__(corr)() 
	if corr in cfg['config']:
		for key in cfg['config'][corr]:
			if opts.verbose:print '  - config keys', key
			check = key.split('=')[0].split('(')[0].split('.')[0]
			try:
				getattr(c, check)
			except AttributeError:
				print "Corrector",corr,"seems not to have attribute",check
			exec( "c."+key ) 
	c.Init()
	loop.AddCorrector(c)

## add analysis
for analysis in cfg['Analysis']:
	if opts.verbose: print '-> Adding analysis',analysis
	classname=analysis.split(':')[0]
	analyzer = r.__getattr__(classname)()
	if analysis in cfg['config']:
	   for key in cfg['config'][analysis]:
		if opts.verbose:print '  - config keys', key
		## CHECK ATTRIBUTE
		check = key.split('=')[0].split('(')[0]
		#if not hasattr( analyzer, check):
		if check.startswith('@'):
			## global function
			exe = key[1:]
			exe = re.sub('!',',',exe)
			exe = re.sub("\$OBJ","analyzer",exe)
			exec( exe ) 
		else:
			try: 
				getattr(analyzer, check)
			except AttributeError:
				try: 
					check= check.split('.')[0]
					getattr(analyzer, check)
				except AttributeError:
					print "WARNING Analyzer",analysis,"do not have attribute",check,"key=",key
			##
			exec('analyzer.'+key)
	loop.AddAnalysis(analyzer)

if opts.verbose: print "-> Init Analysis"
loop.InitAnalysis()
###

if opts.verbose: print "-> Loop"

loop.Loop()

if opts.verbose: print "-> DONE!!!"

