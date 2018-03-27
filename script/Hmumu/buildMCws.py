import os,sys
from array import array
import re,math

from optparse import OptionParser, OptionGroup
parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Hmumu.root")
parser.add_option("","--binput",type='string',help="Backgorund Model Input ROOT file. If none skip this part. [%default]", default="")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="MC.root")
parser.add_option("-v","--var",dest='var',type="string",help="variable [%default]",default="Mmm")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")
opts,args= parser.parse_args()

print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")
from hmm import *

oldArgv=sys.argv[:]
sys.argv=[]
import ROOT

sys.argv=oldArgv[:]

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gROOT.SetBatch()

### general functions
def Import(w,obj):
    #getattr(w,'import')(obj,ROOT.RooCmdArg())
    getattr(w,'import')(obj,ROOT.RooFit.RecycleConflictNodes())

## Global variables
#MonteCarlo=["DYJetsToLL_M-105To160","TT",'TTTT','TTW','TTZ','TTG','TT','ST','WWW','WWZ','WZZ','ZZZ','WW','WZ','ZZ','EWK_LLJJ' ]
MonteCarlo=["DYJetsToLL_M-105To160","TT",'TTTT','TTW','TTZ','TTG','TT','ST','WWW','WWZ','WZZ','ZZZ','WW','WZ','ZZ' ]
Systs=["JES","PU","ScaleRF:DY","ScaleR:DY","ScaleF:DY","ScaleR:TT","ScaleF:TT","ScaleRF:TT"]
config= eval(opts.hmm)
config.Print()
w=ROOT.RooWorkspace("w","w")
fIn=ROOT.TFile.Open(opts.input)
if fIn==None: raise IOError("Input file '"+opts.input+"' does not exist")

x=ROOT.RooRealVar("mmm","mmm",config.xmin,config.xmax)
arglist_obs=ROOT.RooArgList(x)

#####
import re
def GetMC(cat,nuis="",match=""):
    print "->Getting MC:",cat,nuis,match
    h=None
    for idx,mc in enumerate(MonteCarlo):
        useNuis=True
        if match != "":
            useNuis=False
            if re.search(match,mc): useNuis = True
        nuis_str = ""
        if useNuis: nuis_str = nuis[:]
        hTmp=fIn.Get("HmumuAnalysis/Vars/Mmm_"+cat+"_"+mc+nuis_str)
        print "       Adding *","HmumuAnalysis/Vars/Mmm_"+cat+"_"+mc+nuis_str
        if hTmp == None:
            if idx>1: ## make sure that DY and TT are there
                print "[WARNING]: Unable to get mc",mc,"for cat",cat
            else:
                print "[ERROR]: Unable to get mc",mc,"for cat",cat
                raise IOError

        if h == None: h=hTmp.Clone("simulation_"+cat)
        else: h.Add(hTmp)
    h.Scale(config.lumi())
    bin0 = h.FindBin(config.xmin)
    bin1 = h.FindBin(config.xmax)
    for ib in range(0,bin0) + range(bin1+1,h.GetNbinsX()+2): ## excluded bin0, bin1
        h.SetBinContent(ib,0)
        h.SetBinError(ib,0)
    return h

histos=[]
for cat in config.categories:
    h=GetMC(cat)    
    bin0 = h.FindBin(config.xmin)
    bin1 = h.FindBin(config.xmax)
    #scale h to effective lumi
    devents_a=array('d',[.0])
    events=h.IntegralAndError(bin0,bin1,devents_a)
    devents=devents_a[0]
    ## devents S = sqrt( S * events) 
    ## I want that devents' = sqrt(events')
    S =  math.sqrt(events)/devents

    scale=ROOT.RooRealVar("S_"+cat,"stat scaling",S) ## this variable account for the statistics, should be constant. Statistical fluctuation will be put in the Likelihood
    scale.setConstant()
    Import(w,scale)
    roo_mc= ROOT.RooDataHist("mc_"+cat,"mc_"+cat,arglist_obs,h)
    Import(w,roo_mc)
    histos.append(h)

objs=[]
if opts.binput != "":
    print "->Loading",os.environ['CMSSW_BASE']+"/lib/"+os.environ['SCRAM_ARCH']+"/libHiggsAnalysisCombinedLimit.so"
    ROOT.gSystem.Load(os.environ['CMSSW_BASE']+"/lib/"+os.environ['SCRAM_ARCH']+"/libHiggsAnalysisCombinedLimit.so")
    print "->Load LibBare"
    ROOT.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
    print "->Loading ChargedHiggs"
    ROOT.gSystem.Load("bin/libChargedHiggs.so")

    bIn=ROOT.TFile.Open(opts.binput)
    if bIn == None: raise IOError("Unable to open file:" + opts.binput)
    wb=bIn.Get("w")

    for icat,cat in enumerate(config.categories):
        name= config.bkg_functions[icat]
        h=GetMC(cat)    
        bfunc = wb.pdf(name)
        if bfunc == None: raise IOError("Unable to get func" + name)
        name2="bkg_func_binned_"+cat
        b2= ROOT.RooBinnedUncertainties(name2,name,x,bfunc, bin1-bin0,config.xmin,config.xmax)

        ## uncertainties reflected in the rate
        nuisRateStr="1."
        nuisRateAL = ROOT.RooArgList()
        nuisRateCount=0

        for syst_str in Systs:
            if ':' in syst_str: 
                s=syst_str.split(':')[0]
                match=syst_str.split(':')[1]
            else: 
                s= syst_str
                match=""
            hup=GetMC(cat,"_"+s+"Up",match)    
            hdown=GetMC(cat,"_"+s+"Down",match)    
            hup.Add(h,-1)
            hdown.Add(h,-1)

            up=ROOT.std.vector('float')()
            dn=ROOT.std.vector('float')()
            for i in range (0,bin1-bin0):
                up.push_back(hup.GetBinContent( bin0+i) / h.GetBinContent(bin0+i)  )
                dn.push_back(hdown.GetBinContent( bin0+i) / h.GetBinContent(bin0+i) )
            
            nuisName = s
            if match != "": nuisName += "_" + match
            n = ROOT.RooRealVar(nuisName,"systematic: "+s +" " + match,-4.,4)
            objs.append(n)
            print "--> Adding uncertainty",s
            b2.addUncertainty(n, up, dn)
            print "--> Adding to the rate:" 
            nuisRateAL.add( n)

            I_nominal = h.Integral()
            DeltaIup=hup.Integral() / I_nominal
            DeltaIdown=hdown.Integral() / I_nominal
            nuisRateStr += " + ( %f*@%d*(@%d>=0) ) + ( %f*@%d*(@%d<0) )"%(DeltaIup,nuisRateCount,nuisRateCount,DeltaIdown,nuisRateCount,nuisRateCount)
            nuisRateCount +=1
    
        b2.info()
        Import(w,b2)
        objs.append(b2)

        nuisRate=ROOT.RooFormulaVar("bkg_func_binned_norm_nuis_"+cat,nuisRateStr,nuisRateAL)
        print "-> TotalNuisRate is:",nuisRateStr
        Import(w,nuisRate)
        objs.append(nuisRateAL)
        objs.append(nuisRate)

print "-> Writing"
w.writeToFile(opts.output)

print "->Done"
del w
