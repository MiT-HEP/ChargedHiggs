import os,sys,re,time
from glob import glob
from subprocess import call
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)
parser.add_option("-p","--purity",dest="purity",action='store_true',help="Run Purity Fit",default=False)

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
if opts.verbose: print "-> Load Bare library"
r.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
if opts.verbose: print "-> Load ChargedHiggs library"
r.gSystem.Load( "./bin/libChargedHiggs.so")
if opts.verbose: print "DONE",

################ CREATING FITTER ##########
if opts.purity:
	from ROOT import PurityFit
	fitter = PurityFit()
else:
	from ROOT import Fitter
	fitter = Fitter()

################ CONFIGURING FITTER ##########
if opts.purity:
	fitter.inname = "QCDPurity.root"
	fitter.outname = "QCDFit.root"
	if opts.verbose: fitter.verbose_ = 1
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

else:
	call( "mkdir -p plot/sigfit", shell=True)
	fitter.plotDir = "plot/sigfit/"
	fitter.nGaussians = 2
	fitter.nBernstein = 4
	fitter.xmin = 150
	fitter.mIn.push_back(200)
	fitter.mIn.push_back(250)
	fitter.mIn.push_back(500)
	fitter.mIn.push_back(900)
	fitter.outputFileName = "sigfit.root"
	fitter.inputFileName = "ChHiggs.root"

################ INIT FITTER ##########
if opts.verbose: print "-> Init"
fitter.init()

if opts.purity:
	if opts.verbose: print "-> Fit"
	fitter.fit()
else:
	if opts.verbose: print "-> Fit"
	fitter.fitSignal()
	if opts.verbose: print "-> FinalModel"
	fitter.finalModel()
	if opts.verbose: print "-> Write"
	fitter.write()
