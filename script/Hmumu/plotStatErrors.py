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

for cat in categories:
    ## fetch histograms
    data=fIn.Get(base+"Mmm_"+cat+"_Data")
    bkg=None
    for b in ['DY','TT' ] :
        h=fIn.Get(base+"Mmm_"+cat+"_"+b) 
        if h == None:
            print "<*> Ignoring",base+"Mmm_"+cat+"_"+b
            continue

        if bkg == None :
            bkg = h.Clone("bkg")
        else: 
            bkg.Add(h)

    bkg.Scale(config.lumi())
    sig=None
    for proc in config.processes:
        if sig == None:
            sig=fIn.Get(base+"Mmm_"+cat+"_"+proc+"_HToMuMu_M125").Clone("sig")
            if sig == None: 
                print "Ignoring",base+"Mmm_"+cat+"_"+proc+"_HToMuMu_M125"
            else:
                sig.Scale(config.xsec(proc,125))
        else:
            tmp=fIn.Get(base+"Mmm_"+cat+"_"+proc+"_HToMuMu_M125")
            if tmp == None: 
                print "Ignoring",base+"Mmm_"+cat+"_"+proc+"_HToMuMu_M125"
            else:
                tmp.Scale(config.xsec(proc,125))
                sig.Add(tmp)
    sig.Scale( config.br(125.)* config.lumi() )

    ## check
    if data==None:
        print "<*> No Hist",base+"Mmm_"+cat+"_Data", "in",opts.input
        raise IOError
    if bkg==None:
        print "<*> No Hist",base+"Mmm_"+cat+"_DY", "in",opts.input
        raise IOError
    if sig==None:
        print "<*> No Sig Hist"
        raise IOError

    data.Rebin(opts.rebin)
    bkg.Rebin(opts.rebin)
    sig.Rebin(opts.rebin)

    ## reset contents to display
    ## now histograms contains errors
    blind=True
    for i in range(0,bkg.GetNbinsX()):
        ibin=i+1
        data.SetBinContent(ibin,data.GetBinError(ibin))
        bkg.SetBinContent(ibin,bkg.GetBinError(ibin))
        x=data.GetBinCenter(ibin)
        if blind and x>120 and x<130:
            data.SetBinContent(ibin,0)

    data.SetLineColor(ROOT.kBlack)
    data.SetLineWidth(2)

    bkg.SetLineColor(38)
    bkg.SetLineWidth(2)

    sig.SetLineColor(46)
    sig.SetLineWidth(2)

    c=ROOT.TCanvas("c_"+cat,"c",800,800)

    c.SetLeftMargin(0.15)
    c.SetBottomMargin(.15)
    c.SetTopMargin(0.05)
    c.SetRightMargin(0.05)


    data.Draw("HIST")
    bkg.Draw("HIST SAME")
    sig.Draw("HIST SAME")
    
    x0=.55
    y0=.75
    leg = ROOT.TLegend(x0,y0,x0+.4,y0+.15)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.AddEntry(None,"Cat="+cat,"h")
    leg.AddEntry(data,"Stat. Error on data","L")
    leg.AddEntry(bkg,"Stat. Error on bkg MC","L")
    leg.AddEntry(sig,"Signal","L")
    leg.Draw()

    c.Update()

    if opts.outdir == "":
        raw_input("ok?")
    else:
        c.SaveAs(opts.outdir + "/" + cat+ "_stat_errors.pdf")
        c.SaveAs(opts.outdir + "/" + cat+ "_stat_errors.png")
print "-- DONE --"

