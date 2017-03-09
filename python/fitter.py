import os,sys,re,time
from glob import glob
from subprocess import call
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)

parser.add_option("-c","--class",dest="classname",type="string",help="Class [%default]",default="PurityFit")
parser.add_option("-f","--file",dest="file",type="string",help="InputFile [%default]",default="QCDPurity.root")
parser.add_option("-o","--outfile",dest="outfile",type="string",help="OutputFile [%default]",default="QCDFit.root")
parser.add_option("-t","--outtxt",dest="outtxt",type="string",help="OutputTxt [%default]",default="R.txt")
parser.add_option("-l","--qcdlumi",dest="qcdlumi",type="float",help="Prescaled luminosity to use [%default]",default=209.)

(opts,args) = parser.parse_args()

#  find the base directory

if opts.verbose: print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
################ IMPORT ROOT  ################
if opts.verbose: print "-> Importing root",
sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()
if opts.verbose: print "DONE"

################ LOAD LIBRARY ###############
if opts.verbose: print "-> Load Bare library" ## probably not needed
r.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
if opts.verbose: print "-> Load ChargedHiggs library"
r.gSystem.Load( "./bin/libChargedHiggs.so")
if opts.verbose: print "DONE",

################ CREATING FITTER ##########
###line = "from ROOT import " + opts.classname
###exec(line)
###line= "fitter= "+ opts.classname+ "()"
###exec(line)
fitter = r.__getattr__(opts.classname)()
if opts.classname== "PurityFit" or opts.classname=="PurityFitAnalytic":
	fitter.outname= opts.outfile
	fitter.inname =opts.file
	#fitter.lumi=2308.
	fitter.lumi=opts.qcdlumi
	if opts.verbose: fitter.verbose_ = 1

	print "-> Setting lumi to PRESCALED LUMI:",fitter.lumi
	fitter.txtoutname=opts.outtxt

	################ CONFIGURING FITTER ##########
	#fitter.PtBins.push_back(0)
	#print "COARSE BINNINGS"
	#fitter.PtBins.push_back(50)
	#fitter.PtBins.push_back(70)
	#fitter.PtBins.push_back(100)
	#fitter.PtBins.push_back(150)
	#fitter.PtBins.push_back(8000)
	print "FINE BINNINGS"
	fitter.PtBins.push_back(50)
	fitter.PtBins.push_back(55)
	fitter.PtBins.push_back(60)
	fitter.PtBins.push_back(65)
	fitter.PtBins.push_back(70)
	fitter.PtBins.push_back(80)
	fitter.PtBins.push_back(90)
	fitter.PtBins.push_back(100)
	fitter.PtBins.push_back(125)
	fitter.PtBins.push_back(150)
	fitter.PtBins.push_back(175)
	fitter.PtBins.push_back(200)
	fitter.PtBins.push_back(250)
	fitter.PtBins.push_back(500)
	fitter.PtBins.push_back(750)
	fitter.PtBins.push_back(1000)
	fitter.PtBins.push_back(8000)

if opts.classname== "BackgroundFitter" or opts.classname == "Fitter":
	fitter.xmin=105
	fitter.xmax=150

if opts.classname== "Fitter":
	fitter.outname= opts.outfile
	fitter.inname =opts.file
	## Hmumu
	fitter.mIn.push_back(120)
    	fitter.mIn.push_back(125)
    	fitter.mIn.push_back(130)
	## category definition
	fitter.processes.clear()
	for procStr in [ "GluGlu","VBF","ZH","WPlusH","WMinusH","ttH"]:
	#for procStr in [ "GluGlu"]:
		fitter.processes.push_back(procStr)
	fitter.inputMasks.clear()
	for muStr in ["BB","BO","BE","OO","OE","EE"]:
	#for muStr in ["BB"]:
	  for catStr in [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]:
		fitter.inputMasks.push_back("HmumuAnalysis/Vars/Mmm_"+catStr+"_"+muStr+"_%s_HToMuMu_M%.0f")
		for procStr in [ "GluGlu","VBF","ZH","WPlusH","WMinusH","ttH"]:
			if procStr == "ttH":
				fitter.SetGaussians(fitter.inputMasks.size()-1,procStr, 2)
			else:
				fitter.SetGaussians(fitter.inputMasks.size()-1,procStr, 3)
	## overwrite some category
	## for cat in [0,4,22,25,26,27]:
	## 	fitter.SetGaussians(cat,"GluGlu", 2)
	## for cat in [17,25,26,27,28]:
	## 	fitter.SetGaussians(cat,"VBF", 2)
	## for cat in [17,20,22,25,26,27,28]:
	## 	fitter.SetGaussians(cat,"WMinusH", 2)
	## for cat in [12,20,26,27,29,4]:
	## 	fitter.SetGaussians(cat,"WPlusH", 2)
	## for cat in [22]:
	## 	fitter.SetGaussians(cat,"ZH", 2)
	## for cat in [14,19,29]:
	## 	fitter.SetGaussians(cat,"ttH", 1)

doSyst=False
if opts.classname== "Fitter" and doSyst:
	## only in normalization, no shape morphing
	fitter.systIn.push_back("JES");
	fitter.systIn.push_back("TAU");
	fitter.systIn.push_back("TAUHIGHPT");
	fitter.systIn.push_back("TAUSCALE");
	fitter.systIn.push_back("BTAG");
	fitter.systIn.push_back("TRIG");
	fitter.systIn.push_back("TRIGMET");

if opts.classname== "BackgroundFitter":
	fitter.outname= opts.outfile
	fitter.inname =opts.file
	fitter.rebin=1
	## Hmumu
	fitter.inputMasks.clear()
	for muStr in ["BB","BO","BE","OO","OE","EE"]:
	#for muStr in ["BB"]:
	  for catStr in [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]:
		fitter.inputMasks.push_back("HmumuAnalysis/Vars/Mmm_"+catStr+"_"+muStr+"_Data")

## else:
## 	call( "mkdir -p plot/sigfit", shell=True)
## 	fitter.plotDir = "plot/sigfit/"
## 	fitter.nGaussians = 2
## 	fitter.nBernstein = 4
## 	fitter.xmin = 150
## 	fitter.mIn.push_back(200)
## 	fitter.mIn.push_back(250)
## 	fitter.mIn.push_back(500)
## 	fitter.mIn.push_back(900)
## 	fitter.outputFileName = "sigfit.root"
## 	fitter.inputFileName = "ChHiggs.root"

################ INIT FITTER ##########
if opts.verbose: print "-> Init"

fitter.init()
if opts.verbose: print "-> Fit"
fitter.fit()

if opts.classname== "PurityFit":
	fitter.extra="_1p"
	fitter.labelbin="tauinviso_1p"
	fitter.labelspline="tauinvisospline_1p"
	if opts.verbose: print "-> Fit2"
	fitter.init()
	fitter.fit()

	fitter.extra="_3p"
	fitter.labelbin="tauinviso_3p"
	fitter.labelspline="tauinvisospline_3p"

	if opts.verbose: print "-> Fit3"
	fitter.init()
	fitter.fit()



if opts.verbose: print "-> End"
fitter.end()

#doSyst=True
#if opts.classname== "Fitter" and doSyst:
#	for syst in ["JES","TAU","BTAG","TRIG","TRIGMET"]:
#	   for shift in ["Up","Down"]:
#	      fitter.inputMasks.clear()
#	      fitter.inputMasks.push_back("ChargedHiggsTauNu/Vars/Mt_HplusToTauNu_M-%.0f_13TeV_amcatnlo_"+syst+shift)
#	      fitter.datasetMask_ = "hist_Mt_M%.0f_"+syst+shift
#	      fitter.eaMask_="ea_sigmodel_cat%d_"+syst+shift
#	      fitter.normMask_="pdf_sigmodel_cat%d_"+syst+shift+"_norm"
#	      fitter.modelMask_="pdf_sigmodel_cat%d_"+syst+shift
#	      fitter.writeDatasets_= False;
#	      fitter.systLabel_="_"+syst+shift;
#	      fitter.init()
#	      fitter.fit()
#	      fitter.end()

if opts.classname== "Fitter" or opts.classname== "BackgroundFitter":
	if opts.verbose: print "-> Write"
	fitter.write()

if opts.verbose: print "-> DONE <-"

