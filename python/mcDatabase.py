#!env python

import re
import os,sys
import math
from optparse import OptionParser
from subprocess import call, check_output

parser = OptionParser(usage = "usage");
parser.add_option("-e","--eos",dest="eos",type="string",help="eos directory to scout, will not read the files in the pSet",default="");
parser.add_option("-i","--dat",dest="dat",type="string",help="Input dat to scout. Either this is '' or eos is ''",default="");
parser.add_option("-x","--xsec",dest="xsec",type="float",help="Use external cross-section",default=-1);
parser.add_option("-l","--label",dest="label",type="string",help="MC label",default="DYamcatnlo");
parser.add_option("-f","--file",dest="file",type="string",help="mc_database file name",default="dat/mc_database.txt");
parser.add_option("-r","--recursive", dest='rec', action= 'store_true', help="do same for each subdir", default =False);
parser.add_option("-p","--pileup",dest="pu",type="string",help="TODO FIXME")

(opts,args)=parser.parse_args()

#EOS = "/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select"
EOS = "/usr/bin/eos"
if '/eos/user' in opts.eos: EOS += " root://eosuser"

if opts.rec:
	cmd = EOS +" find -d "
	#if '/eos/user' in opts.eos: 
	cmd += " --childcount "
	cmd+=opts.eos
	cmd += " || true" ##FIXME
	list = check_output(cmd,shell=True);
	for line in list.split('\n'):
		if line =='': continue
		print "Line is '"+line+"'"
		dir = line.split() [0]
		ndir = line.split() [1]
		nfiles = line.split() [2]
		#
		nd = float(re.sub('ndir=','',ndir) )
		nf = float(re.sub('nfiles=','',nfiles) )
		dir = re.sub('/eos/cms','',dir)
		if dir[-1] == '/' : dir = dir[:-1] # remove trailing slash

		## find label: directory not containing only numbers, hyphens, underscore, or empty
		dirs=dir.split('/')
		idx=len(dirs) -1 
		while idx >=0 :
			label = dirs[idx]
			if re.match( '^[0-9_\-]*$',label): 
				idx -= 1
			elif re.match( 'PUMoriond17',label) and '_' not in label:  ## avoid also  PU submissions
				idx -= 1
			else:
				break
		if idx <0: label = re.sub('.*/','',dir)
		## 
		if nf >0 and not re.match("Run2015",dir) and not re.match("Run2016",dir) and not re.match('Run2017',dir) \
                and not re.match('SingleMuon',dir) and not re.match('DoubleMuon',dir)  and "/failed" not in dir:
			print "Found one directory:",dir
			cmd = "python %s -e %s -x %f -l '%s' -f %s"%(sys.argv[0],dir,opts.xsec,label,opts.file)
			print "going to execute",cmd
			call(cmd,shell=True)
	exit(0)

if opts.dat != "":
	if opts.eos != "": 
		print "IGNORING eos option! don't put it with dat (-i) option"
	from ParseDat import *
	cfg = ParseDat(opts.dat)

	print "* reading already parsed mcdb"
	try:
		mcdb= ReadMCDB(cfg['MCDB'])
	except KeyError:
		mcdb={}

	for f in cfg['Files']:
		#this are the datasets
		if '.root' in f : continue

		## find label: directory not containing only numbers, hyphens, underscore, or empty
		dirs=f.split('/')
		idx=len(dirs) -1 
		while idx >=0 :
			label = dirs[idx]
			if re.match( '^[0-9_\-]*$',label): 
				idx -= 1
			else:
				break
		if idx <0: label = re.sub('.*/','',f)
		
		if label == 'Tau': continue # exclude data
		if label == 'SingleMuon': continue # exclude data
		if label == 'SingleElectron': continue # exclude data
		if label == 'DoubleMuon': continue # exclude data
		if label == 'DoubleElectron': continue # exclude data
		if re.match('Run201[5,6,7]',label): continue

		cmd = "python %s -e %s -x %f -l '%s' -f %s"%(sys.argv[0],f,opts.xsec,label,opts.file)
		if label in mcdb:
			print "* label", label, "already parsed. Cmd was:"
			print "  ",cmd
			continue
		elif label != '':
		    print "going to execute",cmd
		    call(cmd,shell=True)
	exit(0)


cmd = EOS+ " find -f " + opts.eos

outputList = check_output(cmd,shell=True)
fileList0 = outputList.split() ## change lines into list
if '/eos/user' in opts.eos:
	fileList = [ re.sub("/eos/user","root://eosuser///eos/user",f) for f in fileList0 if '/failed/' not in f ]
else:
	fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]

import ROOT as r
r.gROOT.SetBatch()

## TODO count mcWeights, nEntries
n=0
myTmp= r.TFile.Open("/tmp/" + os.environ["USER"] + "/mytmp.root","RECREATE")
myTmp.cd()
sum=r.TH1D("SumWeights","Sum of mcWeights",1,0,2)

for idx,fName in enumerate(fileList):
	print "processing file:",idx,"/",len(fileList)," : ", fName
	fROOT = r.TFile.Open( fName )
	if idx == 0:
		myTmp.cd()
		h_xSec = fROOT.Get("nero/xSec").Clone("myxSec")
		hScales = fROOT.Get("nero/scaleReweightSums").Clone("myScales")
		hPdfs = fROOT.Get("nero/pdfReweightSums").Clone("myPdfs")
	else:
		h = fROOT.Get("nero/xSec")
		hScalesTmp = fROOT.Get("nero/scaleReweightSums")
		hPdfsTmp = fROOT.Get("nero/pdfReweightSums")
		if h.GetBinContent(2) == 0 : 
			print "Error is 0, xsec is",h.GetBinContent(1),"try to continue"
		else:
			print "\txSec in file is ",h.GetBinContent(1)/h.GetBinContent(2),"+/-",math.sqrt(1./h.GetBinContent(2))
		h_xSec.Add( h )
		hScales.Add( hScalesTmp )
		hPdfs.Add( hPdfsTmp )

	t = fROOT.Get("nero/all")
	mysum=r.TH1D("mysum","Sum of mcWeights",1,0,2)
	t.Draw("1>>mysum","mcWeight") ##>>+ doesn't work
	sum.Add(mysum)
	n += t.GetEntries()
	fROOT.Close()


print "---------------------------------------------"
try:
    print "internal xSec = ",h_xSec.GetBinContent(1)/h_xSec.GetBinContent(2), "+/-", math.sqrt(1./h_xSec.GetBinContent(2))
except ZeroDivisionError:
    print "internal xSec Error: Sum is 0"
print "SumWeights = ", sum.GetBinContent(1)
print "Tot Entries = ", n
print "---------------------------------------------"
### LABEL dir Entries xSec
f = open (opts.file,"a")
print>>f, opts.label,opts.eos, sum.GetBinContent(1),

if opts.xsec >0 : 
	print>>f, opts.xsec,
else: 
	## AUTOMAGIC CROSS SECTIONS -- INTERNAL if not written here
	try:
		xsec= h_xSec.GetBinContent(1)/h_xSec.GetBinContent(2)
	except: 
		xsec=0
	## DY
	#Bin=0,100      f=0.959886
	#Bin=100,200    f=0.0302262
	#Bin=200,400    f=0.00833685
	#Bin=400,600    f=0.00113651
	#Bin=600,800    f=0.000271553
	#Bin=800,1200   f=0.000118071
	#Bin=1200,13000 f=2.52766e-05
	#elif 'DYJetsToLL_M-50_HT-100to200'  in opts.label: xsec=174.266133
	#elif 'DYJetsToLL_M-50_HT-200to400'  in opts.label: xsec=48.065275
	#elif 'DYJetsToLL_M-50_HT-400to600'  in opts.label: xsec=6.552435
	#elif 'DYJetsToLL_M-50_HT-600to800'  in opts.label: xsec=1.565612
	#elif 'DYJetsToLL_M-50_HT-800to1200' in opts.label: xsec=0.680727
	#elif 'DYJetsToLL_M-50_HT-1200toInf' in opts.label: xsec=0.14573
	## GenXsecAnalyzer
	if 'DYJetsToLL_M-50_HT-0to70'       in opts.label: xsec=5317.864951
	elif 'DYJetsToLL_M-50_HT-70to100'   in opts.label: xsec=206.184
	elif 'DYJetsToLL_M-50_HT-100to200'  in opts.label: xsec=181.302
	elif 'DYJetsToLL_M-50_HT-200to400'  in opts.label: xsec=50.4177
	elif 'DYJetsToLL_M-50_HT-400to600'  in opts.label: xsec=6.98394
	elif 'DYJetsToLL_M-50_HT-600to800'  in opts.label: xsec=1.68141
	elif 'DYJetsToLL_M-50_HT-800to1200' in opts.label: xsec=0.775392
	elif 'DYJetsToLL_M-50_HT-1200to2500' in opts.label: xsec=0.186222
	elif 'DYJetsToLL_M-50_HT-2500toInf' in opts.label: xsec=0.004385
	elif 'DYJetsToLL_M-10to50' in opts.label:xsec=18610
	## amc@nlo sum=5991.7
	elif 'DYToLL_0J_13TeV' in opts.label: xsec=4754
	elif 'DYToLL_1J_13TeV' in opts.label: xsec=888.9 
	elif 'DYToLL_2J_13TeV' in opts.label: xsec=348.8 
	elif 'DYJetsToLL' in opts.label or 'DY' in opts.label: xsec=5765.4 # nnpdf 3.0
	#elif 'DYJets' in opts.label or 'DY' in opts.label: xsec=6025.
	## SIG
	elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-200' in opts.label: xsec=1
	elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-300' in opts.label: xsec=1
	elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-350' in opts.label: xsec=1
	elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-400' in opts.label: xsec=1
	elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-500' in opts.label: xsec=1
	elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-220' in opts.label: xsec=1
	elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-250' in opts.label: xsec=1
	### H-> mumu
	elif 'HToMuMu' in opts.label: xsec=1
	##TTX
	elif 'TTZToQQ' in opts.label: xsec=0.5297
	elif 'TTZToLLNuNu' in opts.label: xsec=0.2529
	elif 'TTWJetsToQQ' in opts.label:xsec=0.40620
	elif 'TTWJetsToLNu' in opts.label:xsec=0.2043
	elif 'TTGJets' in opts.label:xsec=3.697
	elif 'TTTT' in opts.label:xsec=0.009103
	### TT
	elif 'TTTo2L2Nu' in opts.label: xsec=88.20
	elif 'TT' in opts.label: xsec=831
	## ST
	elif 'ST_s-channel_4f_InclusiveDecays' in opts.label: xsec=10.32
	elif 'ST_s-channel_4f_leptonDecays' in opts.label: xsec=3.365
	elif 'ST_t-channel_antitop_4f' in opts.label: xsec=80.95
	elif 'ST_t-channel_top_4f' in opts.label: xsec=136.02
	elif 'ST_tW_antitop_5f' in opts.label: xsec=30.09
	elif 'ST_tW_top_5f' in opts.label: xsec=30.11
	### WJETS 20508.9 * 3 
	#Bin=0,70 f=0.936856
	#Bin=70,100 f=0.0275586
	#Bin=100,200 f=0.0270849
	#Bin=200,400 f=0.00719491
	#Bin=400,600 f=0.000958862
	#Bin=600,800 f=0.000230162
	#Bin=800,1200 f=9.69229e-05
	#Bin=1200,2500 f=1.99723e-05
	#Bin=2500,13000 f=1.76188e-07
	elif 'WJetsToLNu_HT-0To70'      in opts.label: xsec=57641.658055
	elif 'WJetsToLNu_HT-70To100'    in opts.label: xsec=1695.589715
	#elif 'WJetsToLNu_HT-100To200'   in opts.label: xsec=1666.444517
	#elif 'WJetsToLNu_HT-200To400'   in opts.label: xsec=442.679069
	#elif 'WJetsToLNu_HT-400To600'   in opts.label: xsec=58.995615
	#elif 'WJetsToLNu_HT-600To800'   in opts.label: xsec=14.161108
	#elif 'WJetsToLNu_HT-800To1200'  in opts.label: xsec=5.963346
	#elif 'WJetsToLNu_HT-1200To2500' in opts.label: xsec=1.22883
	#elif 'WJetsToLNu_HT-2500ToInf'  in opts.label: xsec=0.01084
	## genXS Analyzer
	elif 'WJetsToLNu_HT-100To200'   in opts.label: xsec=1627.45
	elif 'WJetsToLNu_HT-200To400'   in opts.label: xsec=435.237
	elif 'WJetsToLNu_HT-400To600'   in opts.label: xsec=59.1811
	elif 'WJetsToLNu_HT-600To800'   in opts.label: xsec=14.5805
	elif 'WJetsToLNu_HT-800To1200'  in opts.label: xsec=6.65621
	elif 'WJetsToLNu_HT-1200To2500' in opts.label: xsec=1.60809
	elif 'WJetsToLNu_HT-2500ToInf'  in opts.label: xsec=0.038914
	elif 'WJets' in opts.label: xsec=61526.7
	elif 'W0' in opts.label: xsec=34273.632815
	elif 'W1' in opts.label: xsec=18455.979619
	elif 'W2' in opts.label: xsec=6035.904629
	elif 'W3' in opts.label: xsec=1821.46719
	elif 'W4' in opts.label: xsec=939.684984
	##VV
	elif 'WWTo2L2Nu' in opts.label: xsec=12.46
	elif 'WZTo1L1Nu2Q' in opts.label: xsec=10.71
	elif 'WZTo1L3Nu' in opts.label: xsec=3.033
	elif 'WZTo2L2Q' in opts.label: xsec=5.595
	elif 'WZTo3LNu' in opts.label: xsec=4.42965
	elif 'ZZTo2L2Nu' in opts.label: xsec=0.5644
	elif 'ZZTo2L2Q' in opts.label: xsec=3.22
	elif 'ZZTo4L' in opts.label: xsec=1.256
	elif 'WWToLNuQQ' in opts.label: xsec=45.68
	elif 'WWTo1L1NuQQ' in opts.label: xsec=45.68
	#VVV
	elif 'WWW' in opts.label: xsec=0.2086
	elif 'WWZ' in opts.label: xsec=0.1651
	elif 'WZZ' in opts.label: xsec=0.05565
	elif 'ZZZ' in opts.label: xsec=0.01398
	### EWK EWK_LLJJ_MLL-50_MJJ-120
	elif 'EWK_LLJJ' in opts.label: xsec=1.630
	##
	#elif '' in opts.label: xsec=
	## INTERNAL
	print>>f,  xsec,

if hScales.GetBinContent(1) > 0:
	print>>f, "SCALES",
	for i in range(0,6):
		print>>f, sum.GetBinContent(1)/hScales.GetBinContent(i+1),

if hPdfs.GetBinContent(1) > 0:
	print>>f, "PDFS",
	for i in range(0,100):
		print>>f, sum.GetBinContent(1)/hPdfs.GetBinContent(i+1), 
print >>f,"" ##new line
print "---------------------------------------------"

