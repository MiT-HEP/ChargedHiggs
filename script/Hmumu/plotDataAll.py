import os,sys
from array import array
import re,math
from subprocess import call
from optparse import OptionParser, OptionGroup

parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Hmumu.root")
parser.add_option("-c","--cat",dest='cat',type="string",help="do cat xxx for bkg [%default]",default="all")
parser.add_option("-o","--outdir",dest='outdir',type="string",help="output directory [%default]",default="")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")

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

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. if >1000 variable bin [%default]", default=10)
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

############# take configuration #########
config= eval(opts.hmm)
config.Print()
categories=config.categories
if opts.cat != "all" and opts.cat != "":
    categories= [opts.cat]
base="HmumuAnalysis/Vars/"
##########################################
fIn=ROOT.TFile.Open(opts.input)
if fIn==None:
    print "-> Input file",opts.input,"doesn't exist"
    raise IOError

dataHists={}
colors=[ROOT.kBlue+2,ROOT.kBlue,ROOT.kCyan+2,ROOT.kGreen+2,ROOT.kGreen,ROOT.kRed+2,ROOT.kRed,ROOT.kOrange,ROOT.kYellow,ROOT.kGray+2,ROOT.kBlack,ROOT.kMagenta,ROOT.kViolet]
for idx,cat in enumerate(categories):
    ## fetch histograms
    data=fIn.Get(base+"Mmm_"+cat+"_Data")

    ## check
    if data==None:
        print "<*> No Hist",base+"Mmm_"+cat+"_Data", "in",opts.input
        raise IOError
    data.Rebin(opts.rebin)

    ## reset contents to display
    ## now histograms contains errors
    blind=True
    for i in range(0,data.GetNbinsX()):
        ibin=i+1
        x=data.GetBinCenter(ibin)
        if blind and x>120 and x<130:
            data.SetBinContent(ibin,0)

    #data.SetLineColor(ROOT.kBlack)
    data.SetLineColor(colors[idx])
    data.SetLineWidth(2)
    dataHists[cat]=data


c=ROOT.TCanvas("c_","c",800,800)
for idx,cat in enumerate(categories):
    xlow=dataHists[cat].FindBin(110)
    xhigh=dataHists[cat].FindBin(150)
    dataHists[cat].Scale(1.0/dataHists[cat].Integral(xlow,xhigh) )
    if idx==0: 
        dataHists[cat].Draw("HIST")
        dataHists[cat].GetXaxis().SetRangeUser(110,150)
    else: dataHists[cat].Draw("HIST SAME")


c.SetLeftMargin(0.15)
c.SetBottomMargin(.15)
c.SetTopMargin(0.05)
c.SetRightMargin(0.05)

x0=.65
y0=.75
leg = ROOT.TLegend(x0,y0,x0+.4,y0+.15)
leg.SetNColumns(2)
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.AddEntry(None,"Cat="+cat,"h")
for idx,cat in enumerate(categories):
    leg.AddEntry(dataHists[cat],cat,"L")
leg.Draw()

c.Update()

if opts.outdir == "":
    raw_input("ok?")
else:
    c.SaveAs(opts.outdir + "/dataAll.pdf")
    c.SaveAs(opts.outdir + "/dataAll.png")
print "-- DONE --"

