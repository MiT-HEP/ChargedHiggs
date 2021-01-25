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
print "My path is",mypath, "basepath is",basepath
################ IMPORT ROOT  ################
if opts.verbose: print "-> Importing root",
sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()
if opts.verbose: print "DONE"

################ LOAD LIBRARY ###############
if opts.verbose: print "-> Load Bare library" ## probably not needed
status=r.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
#r.gSystem.Load( mypath+"/../NeroProducer/Core/bin/libBare.so")

rpath=True
if status < 0:
	if opts.verbose: print "-> trying bin/bare"
	status = r.gSystem.Load("bin/bare/libBare.so")
	sys.path.insert(0,"bin/bare")
	rpath=False
	if status<0: print " Failed to load libBare.so"
if status >= 0 and opts.verbose: print "DONE"

if opts.verbose: print "-> Load ChargedHiggs library"
#r.gSystem.Load( "./bin/libChargedHiggs.so")
#r.gSystem.Load( mypath+"/bin/libChargedHiggs.so")
if rpath:
    r.gSystem.Load( "./bin/libChargedHiggs.so")
else: ## it's likely that this will work
    r.gSystem.Load("./bin/libChargedHiggs.0.so")

if opts.verbose: print "DONE",

if opts.classname== "BackgroundFitter" or opts.classname == "Fitter":
    from hmm import *
    if os.path.isfile("hmm2.py") :
        from hmm2 import *
    if os.path.isfile("python/vbshadr.py") :
        print "loading vbshadr.py"
        from vbshadr import *
    config = eval(opts.hmm)
    config.Print()

################ CREATING FITTER ##########
###line = "from ROOT import " + opts.classname
###exec(line)
###line= "fitter= "+ opts.classname+ "()"
###exec(line)
if opts.classname== "Fitter":
    print "Constructing Fitter",opts.classname
    fitter = r.__getattr__(opts.classname)("%d"%config.year)
else:
    print "Constructing Fitter",opts.classname
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

    fitter.EtaBins.push_back(0.)
    fitter.EtaBins.push_back(0.5)
    fitter.EtaBins.push_back(0.9)
    fitter.EtaBins.push_back(1.44)
    fitter.EtaBins.push_back(1.7)
    fitter.EtaBins.push_back(2.0)
    fitter.EtaBins.push_back(2.1)

if opts.classname== "BackgroundFitter" or opts.classname == "Fitter":
    fitter.xmin = config.xmin
    fitter.xmax = config.xmax
    fitter.xname = config.xname
    if opts.plotdir != "":
        fitter.plotDir=opts.plotdir+"/"

if opts.classname== "Fitter":
    fitter.outname= opts.outfile
    fitter.inname =opts.file
    if config.background_fitstrategy==1: 
        fitter.mhmin=1000
        fitter.mhmax=2000
    if config.dcb:
        fitter.useDCB = True
    ## Hmumu
    for m in config.sig_mass_points:
        fitter.mIn.push_back(m)
    ## category definition
    fitter.processes.clear()
    for procStr in config.processes:
        fitter.processes.push_back(procStr)
    fitter.inputMasks.clear()
    for catStr in config.categories:

        if catStr=="": 
            fitter.inputMasks. push_back( config.dirname+config.varname + "_" + config.sigspec)
        else:
            fitter.inputMasks.push_back(config.dirname+config.varname+"_"+catStr+"_"+config.sigspec)

        for procStr in config.processes:
            if procStr == "ttH":
                fitter.SetGaussians(fitter.inputMasks.size()-1,procStr, 2)
            else:
                fitter.SetGaussians(fitter.inputMasks.size()-1,procStr, 3)
            if (catStr,procStr) in  config.sigfit_gaussians:
                fitter.SetGaussians(fitter.inputMasks.size()-1,procStr,config.sigfit_gaussians[(catStr,procStr)])
    #fitter.inputMasks.clear()

    for catStr,proc in config.sigfit_replace:
        cat = config.categories.index(catStr)
        cat2Str,proc2=config.sigfit_replace[(catStr,proc)]
        cat2 = config.categories.index(cat2Str)
        fitter.SetReplace(cat,proc,cat2,proc2)
        if (cat2Str,proc2) in  config.sigfit_gaussians:
            fitter.SetGaussians(cat, proc, config.sigfit_gaussians[(cat2Str,proc2)] )
        elif proc2 =="ttH":
            fitter.SetGaussians(cat, proc, 2 )
        else:
            fitter.SetGaussians(cat, proc, 3 )

    # nuisances and correlations
    for cat,proc in config.sigfit_scale_unc:
        scale=config.sigfit_scale_unc[(cat,proc)]
        try: 
            value=float(scale)
            print "-> Setting Scale Uncertainty for ",cat,proc,"to value",value
            fitter.SetScaleUnc(cat,proc,value)
        except TypeError as a:
            try:
                cat2,proc2 = scale
                fitter.SetScaleUncCorr(cat,proc,cat2,proc2)
                print "-> Setting Scale Uncertainty for ",cat,proc,"correlated to",cat2,proc2
            except TypeError as e:
                print "-> Unable to understand scale error type. Two exceptions follows"
                print "---------"
                print a
                print "---------"
                print e
                print "---------"
                raise e
    for cat,proc in config.sigfit_smear_unc:
        smear=config.sigfit_smear_unc[(cat,proc)]
        try: 
            value=float(smear)
            fitter.SetSmearUnc(cat,proc,value)
        except TypeError as a:
            try:
                cat2,proc2 = smear
                fitter.SetSmearUncCorr(cat,proc,cat2,proc2)
            except TypeError as e:
                print "-> Unable to understand smear error type. Two exceptions follows"
                print "---------"
                print a
                print "---------"
                print e
                print "---------"
                raise e


if opts.classname== "BackgroundFitter":
    fitter.outname= opts.outfile
    fitter.inname =opts.file
    fitter.rebin=1
    fitter.fitStrategy=config.background_fitstrategy
    if config.background_fitstrategy==1: fitter.blind=0
    ## Hmumu
    fitter.inputMasks.clear()
    if not config.background_input_masks:
        for catStr in config.categories:
            if catStr =="":
                fitter.inputMasks.push_back(config.dirname + config.varname+"_Data")
            else:
                fitter.inputMasks.push_back(config.dirname + config.varname+"_"+catStr+"_Data")
    else:
        for s in config.background_input_masks: 
            fitter.inputMasks.push_back(s)


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

