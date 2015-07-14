import os,sys,re,time
from glob import glob
from subprocess import call
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)

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
from ROOT import Fitter
fitter = Fitter()

################ CONFIGURING FITTER ##########

call( "mkdir -p plot/sigfit", shell=True)
fitter.plotDir = "plot/sigfit/"
fitter.nGaussians = 1
fitter.nBernstein = 3
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
if opts.verbose: print "-> Fit"
fitter.fitSignal()
if opts.verbose: print "-> FinalModel"
fitter.finalModel()
if opts.verbose: print "-> Write"
fitter.write()
