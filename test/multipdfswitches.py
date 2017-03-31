import os, sys
from optparse import OptionParser

## generate a toy and fit it, keeping r fixed. make sure there is a small nuisance to activate multipdfs
#combine -M GenerateOnly -t 10 --expectSignal=1 --expectSignalMass=125 -m 125  test/cms_datacard_hmumu_syst.txt  --freezeNuisances=pdfindex_cat0,pdfindex_cat1,pdfindex_cat2,pdfindex_cat3,pdfindex_cat4,pdfindex_cat5,pdfindex_cat6,pdfindex_cat7,pdfindex_cat8,pdfindex_cat9,pdfindex_cat10,pdfindex_cat11,pdfindex_cat12,pdfindex_cat13,pdfindex_cat14,pdfindex_cat15,pdfindex_cat16,pdfindex_cat17,pdfindex_cat18,pdfindex_cat19,pdfindex_cat20,pdfindex_cat21,pdfindex_cat22,pdfindex_cat23,pdfindex_cat24,pdfindex_cat25,pdfindex_cat26,pdfindex_cat27,pdfindex_cat28,pdfindex_cat29  --toysFrequentist --saveToys
#for r in -2 -1 0 1 2 3 4 5 8 10 20 ;
#do
#	combine -M MultiDimFit -t 1 --toysFile higgsCombineTest.GenerateOnly.mH125.123456.root -m 125 --cminDefaultMinimizerType=Minuit2 --algo=fixed --redefineSignalPOI MH --fixedPointPOIs MH=125 --setPhysicsModelParameterRanges r=-2,20:MH=124.99,125.01 --freezeNuisances=r --setPhysicsModelParameters r=$r -n "_r${r}" --saveWorkspace cms_datacard_hmumu.txt
#done

usage = "usage: %prog [options] files"
parser=OptionParser(usage=usage)
#parser.add_option("-q","--queue" ,dest='queue',type='string',help="Queue [%default]",default="1nh")
parser.add_option("-n","--ncats" ,dest='ncats',type='int',help="Number of categories [%default]",default=30)
parser.add_option("-v","--verbose" ,dest='verbose',action='store_true',help="Verbose [default]",default=False)
parser.add_option("","--plots" ,dest='plots',action='store_true',help="do plots (no batch) [default]",default=True)
parser.add_option("-b","--batch" ,dest='plots',action='store_false',help="Batch")
(opts,args)=parser.parse_args()


if opts.verbose: print "-> Load ROOT"
sys.argv=[]
import ROOT 
if not opts.plots:
    ROOT.gROOT.SetBatch()

from array import array
def getRGB(color):
    r=array('f', [0.0])
    g=array('f', [0.0])
    b=array('f', [0.0])
    ROOT.gROOT.GetColor(color).GetRGB( r,g,b )
    return (r[0],g[0],b[0])

if opts.verbose: print "-> Load Combine"
ROOT.gSystem.Load("libHiggsAnalysisCombinedLimit.so")

################# READ ######################
# (cat,r) = idx
values={}
r_values=set([])

for f in args:
    fIn=ROOT.TFile.Open(f)
    w=fIn.Get("w")
    w.loadSnapshot("MultiDimFit")
    r=w.var("r").getVal()
    r_values.add( r)  
    mh=w.var("MH").getVal()
    print "MH=",mh
    for cat in range(0,opts.ncats):
        idx=w.cat("pdfindex_cat%d"%cat).getIndex()
        values[(cat,r)] = idx

## define plots
if opts.plots:
    ## PALETTE
    rL = [getRGB(ROOT.kWhite)[0],getRGB(ROOT.kGreen-9)[0],getRGB(ROOT.kRed-9)[0],getRGB(ROOT.kBlue-9)[0],getRGB(ROOT.kGray)[0],getRGB(ROOT.kOrange)[0],getRGB(ROOT.kMagenta-4)[0],getRGB(ROOT.kBlue)[0],getRGB(ROOT.kRed)[0]]
    gL = [getRGB(ROOT.kWhite)[1],getRGB(ROOT.kGreen-9)[1],getRGB(ROOT.kRed-9)[1],getRGB(ROOT.kBlue-9)[1],getRGB(ROOT.kGray)[1],getRGB(ROOT.kOrange)[1],getRGB(ROOT.kMagenta-4)[1],getRGB(ROOT.kBlue)[1],getRGB(ROOT.kRed)[1]]
    bL = [getRGB(ROOT.kWhite)[2],getRGB(ROOT.kGreen-9)[2],getRGB(ROOT.kRed-9)[2],getRGB(ROOT.kBlue-9)[2],getRGB(ROOT.kGray)[2],getRGB(ROOT.kOrange)[2],getRGB(ROOT.kMagenta-4)[2],getRGB(ROOT.kBlue)[2],getRGB(ROOT.kRed)[2]]
    sL = [0.0,.1,.2,.3,.4,.5,.6,.7,1.0]

    r=array('d',rL)
    g=array('d',gL)
    b=array('d',bL)
    s=array('d',sL)
    FI=ROOT.TColor.CreateGradientColorTable(len(s), s, r, g, b, 10);
    ROOT.gStyle.SetNumberContours(10);
    
    ## change palette
    ROOT.gStyle.SetOptStat(0)
    h=ROOT.TH2D("h","multipdf",len(r_values),-.5,len(r_values)-.5,opts.ncats,-.5,opts.ncats-.5)

###########print
print "----------- R ---------"
print "    ",
for r in sorted(r_values):
    print r,
print
for cat in range(0,opts.ncats):
    print "----------- CAT %d ---------"%cat
    print "idx=",
    for idx,r in enumerate(sorted(r_values)):
        if opts.plots: 
            h.SetBinContent( idx+1,cat+1,values[(cat,r)]+0.001 )
            h.SetBinError( idx+1,cat+1,0.001 )
        print values[(cat,r)],
    print

###########plot
if opts.plots:
    ROOT.gStyle.SetPaintTextFormat(".0f")
    c=ROOT.TCanvas("c","c",800,800)
    c.SetLeftMargin(0.15)
    c.SetBottomMargin(0.15)
    c.SetRightMargin(0.10)
    c.SetTopMargin(0.10)
    h.Draw("AXIS b")
    h.Draw("COLZ SAME")
    for idx,r in enumerate(sorted(r_values)):
        h.GetXaxis().SetBinLabel(idx+1," %.0f"%r)
    for icat in range(0,opts.ncats):
        h.GetYaxis().SetBinLabel(icat+1," cat%d"%icat)
    h.GetXaxis().SetNdivisions(-len(r_values))
    h.GetYaxis().SetNdivisions(-opts.ncats)
    h.GetXaxis().SetTitle("r")
    h.GetYaxis().SetTitle("channel")
    h.GetYaxis().SetTitleSize(0.04)
    h.GetXaxis().SetTitleSize(0.04)
    h.GetYaxis().SetTitleOffset(1.8)
    h.GetXaxis().SetTitleOffset(1.1)
    h.GetZaxis().SetRangeUser(-.5,9.5)
    h.GetZaxis().SetTickLength(0)
    h.Draw("AXIS b SAME")
    h.Draw("AXIS b X+ Y+ SAME")
    h.Draw("TEXT0 SAME")
    raw_input("ok?")

