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
    getattr(w,'import')(obj,ROOT.RooCmdArg())

## Global variables
MonteCarlo=["DYJetsToLL_M-105To160","TT",'TTTT','TTW','TTZ','TTG','TT','ST','WWW','WWZ','WZZ','ZZZ','WW','WZ','ZZ','EWK_LLJJ' ]
Systs=["JES","PU","ScaleRF","ScaleR","ScaleF"]
config= eval(opts.hmm)
config.Print()
w=ROOT.RooWorkspace("w","w")
fIn=ROOT.TFile.Open(opts.input)
if fIn==None: raise IOError("Input file '"+opts.input+"' does not exist")

x=ROOT.RooRealVar("mmm","mmm",config.xmin,config.xmax)
arglist_obs=ROOT.RooArgList(x)

#####

def GetMC(cat,nuis=""):
    h=None
    for idx,mc in enumerate(MonteCarlo):
        hTmp=fIn.Get("HmumuAnalysis/Vars/Mmm_"+cat+"_"+mc+nuis)
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
        bfunc = wb.pdf(name)
        if bfunc == None: raise IOError("Unable to get func" + name)
        ## import pdf from Background Model
        ## move to the binned version -> systematics attached to MC
        ##  name2="bkg_func_binned_"+cat
        ##  pars=ROOT.RooArgList()
        ##  if 'zmod_' in name: 
        ##      #name="zmod_cat%d_ord1"%cat
        ##      pars.add(wb.var(name+"_a"))
        ##      pars.add(wb.var(name+"_b"))
        ##      pars.add(wb.var(name+"_c"))
        ##  elif 'exp' in name: 
        ##      #name="exp_cat%d_ord3"%cat
        ##      pars.add(wb.var(name+"_p1"))
        ##      pars.add(wb.var(name+"_p2"))
        ##      pars.add(wb.var(name+"_f1"))
        ##  elif 'zmod2' in name:
        ##      #name="zmod2_cat%d_ord5"%cat
        ##      pars.add(wb.var(name+"_a"))
        ##      pars.add(wb.var(name+"_b"))
        ##      pars.add(wb.var(name+"_c"))
        ##      pars.add(wb.var(name+"_e"))
        ##      pars.add(wb.var(name+"_f"))
        ##      pars.add(wb.var(name+"_g"))
        ##      pars.add(wb.var(name+"_h"))
        ##  else:
        ##      raise ValueError('Unsupported function:'+name)
        #b2 = ROOT. RooParametricShapeBinPdf( name2,name,bfunc, x,pars,histos[icat])
        name2="bkg_func_binned_"+cat
        b2= ROOT.RooBinnedUncertainties(name2,name,x,bfunc, bin1-bin0,config.xmin,config.xmax)
        for s in Systs:
            hup=GetMC(cat,"_"+s+"Up")    
            hdown=GetMC(cat,"_"+s+"Down")    
            hup.Add(h,-1)
            hdown.Add(h,-1)

            up=ROOT.std.vector('float')()
            dn=ROOT.std.vector('float')()
            for i in range (0,bin1-bin0):
                up.push_back(hup.GetBinContent( bin0+i) )
                dn.push_back(hdown.GetBinContent( bin0+i) )
            
            n = ROOT.RooRealVar(s,"systematic: "+s,-4.,4)
            objs.append(n)
            print "--> Adding uncertainty",s
            b2.addUncertainty(n, up, dn)
    
        b2.info()
        Import(w,b2)
        objs.append(b2)

print "-> Writing"
w.writeToFile(opts.output)

print "->Done"
del w
