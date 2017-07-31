import os,sys
from array import array
import re,math
from subprocess import call
from optparse import OptionParser, OptionGroup

parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input Signal Model File. [%default]", default="SignalModel.root")
parser.add_option("-c","--cat",dest='cat',type="string",help="do cat xxx for sig [%default]",default="cat0")
parser.add_option("-o","--outdir",dest='outdir',type="string",help="output directory [%default]",default="")
### parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")

### print "-> Looking for basepath"
### basepath = ""
### mypath = os.path.abspath(os.getcwd())
### while mypath != "" and mypath != "/":
### 	if "ChargedHiggs" in os.path.basename(mypath):
### 		basepath = os.path.abspath(mypath)
### 	mypath = os.path.dirname(mypath)
### print "-> Base Path is " + basepath
### sys.path.insert(0,basepath)
### sys.path.insert(0,basepath +"/python")
### from hmm import *

opts,args= parser.parse_args()

########### IMPORT ROOT #############
oldArgv=sys.argv[:]
sys.argv=[]
import ROOT

sys.argv=oldArgv[:]

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
if opts.outdir != "":
    ROOT.gROOT.SetBatch()
    try:
        os.mkdir(opts.outdir) ## makedirs is like mkdir -p
    except OSError: ## directory already exist
        pass
####################################

#config= eval(opts.hmm)
#config.Print()
#categories=config.categories
#if opts.cat != "all" and opts.cat != "":
#    categories= [opts.cat]

print "-> loading combine"
ROOT.gSystem.Load("libHiggsAnalysisCombinedLimit.so")

fIn=ROOT.TFile.Open(opts.input)
w=fIn.Get("w")

x=w.var("mmm")
w.var("MH").setVal(125)
ggH=w.pdf("pdf_sigmodel_GluGlu_"+opts.cat)

polys = []
n=6

garbage=[]
zero=ROOT.RooRealVar("zero","zero",0)
uno=ROOT.RooRealVar("uno","uno",1)
#neg=ROOT.RooRealVar("neg","neg",-1)

plot=x.frame()
ggH.plotOn(plot,ROOT.RooFit.LineColor(ROOT.kBlack))

colors=[ROOT.kBlue,ROOT.kRed,ROOT.kGreen,ROOT.kMagenta,ROOT.kCyan,ROOT.kGray+2,ROOT.kOrange,ROOT.kYellow]
for idx in range(0,n+1):
    coeff=ROOT.RooArgList("mylist_%d"%idx)
    for cidx in range(0,n+1):
        if idx==cidx: 
            coeff.add(uno)
        else: 
            coeff.add(zero)
    ##
    p = ROOT.RooBernsteinFast(n)("poly%d_%d"%(idx,n),"poly%d_%d"%(idx,n),x,coeff)
    #p = ROOT.RooBernstein("poly%d_%d"%(idx,n),"poly%d_%d"%(idx,n),x,coeff)
    p.plotOn(plot,ROOT.RooFit.LineColor( colors[idx]))
    polys.append(p)
    garbage.append(coeff)

polys[2].Print("V")

c=ROOT.TCanvas("c","c")
plot.Draw()

raw_input("ok?")
