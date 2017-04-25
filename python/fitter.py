import os,sys,re,time
from glob import glob
from subprocess import call
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)

parser.add_option("-c","--class",dest="classname",type="string",help="Class [%default]",default="PurityFit")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")
parser.add_option("-f","--file",dest="file",type="string",help="InputFile [%default]",default="QCDPurity.root")
parser.add_option("-o","--outfile",dest="outfile",type="string",help="OutputFile [%default]",default="QCDFit.root")
parser.add_option("-t","--outtxt",dest="outtxt",type="string",help="OutputTxt [%default]",default="R.txt")
parser.add_option("-l","--qcdlumi",dest="qcdlumi",type="float",help="Prescaled luminosity to use [%default]",default=209.)
parser.add_option("-p","--plotdir",dest="plotdir",type="string",help="PlotDir for Fitter and Background Fitter [%default]",default="")

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
    from hmm import hmm,hmmAutoCat
    config = eval(opts.hmm)
    config.Print()
    fitter.xmin = config.xmin
    fitter.xmax = config.xmax
    if opts.plotdir != "":
        fitter.plotDir=opts.plotdir+"/"

if opts.classname== "Fitter":
    fitter.outname= opts.outfile
    fitter.inname =opts.file
    ## Hmumu
    for m in config.sig_mass_points:
        fitter.mIn.push_back(m)
    ## category definition
    fitter.processes.clear()
    for procStr in config.processes:
        fitter.processes.push_back(procStr)
    fitter.inputMasks.clear()
    for catStr in config.categories:
        fitter.inputMasks.push_back("HmumuAnalysis/Vars/Mmm_"+catStr+"_%s_HToMuMu_M%.0f")
        for procStr in [ "GluGlu","VBF","ZH","WPlusH","WMinusH","ttH"]:
            if procStr == "ttH":
                fitter.SetGaussians(fitter.inputMasks.size()-1,procStr, 2)
            else:
                fitter.SetGaussians(fitter.inputMasks.size()-1,procStr, 3)
            if (catStr,procStr) in  config.sigfit_gaussians:
                fitter.SetGaussians(fitter.inputMasks.size()-1,procStr,config.sigfit_gaussians[(catStr,procStr)])

if opts.classname== "BackgroundFitter":
    fitter.outname= opts.outfile
    fitter.inname =opts.file
    fitter.rebin=1
    ## Hmumu
    fitter.inputMasks.clear()
    for catStr in config.categories:
        fitter.inputMasks.push_back("HmumuAnalysis/Vars/Mmm_"+catStr+"_Data")

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

if opts.classname== "Fitter" or opts.classname== "BackgroundFitter":
    if opts.verbose: print "-> Write"
    fitter.write()

if opts.verbose: print "-> DONE <-"

