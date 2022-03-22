from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import ROOT 
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
import os,sys
import math

print ("-> Looking for basepath")
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
print ("-> Base Path is " + basepath)
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")

from hmm import Stack

fname="fitDiagnostics.root"
#year=18
year=216
#n=36
n=100
paper=False
#labels=["QCD_HT","TT_TuneCP5"]



########
lumi=35920. if year==16 else 41530 if year==17 else 59740
if year == 116: lumi=19523.
if year == 216: lumi=16803.
colors = [ ROOT.kBlue, 8, ROOT.kCyan+2, ROOT.kAzure-6,ROOT.kOrange+7,ROOT.kMagenta,ROOT.kGreen-4];
##########
fIn=ROOT.TFile.Open(fname)
c=ROOT.TCanvas("c","canvas",800,800)
c.SetTopMargin(0.05)
c.SetLeftMargin(0.15)
c.SetRightMargin(0.05)
c.SetBottomMargin(0.15)
c.SetLogy()

hdata=ROOT.TH1D("data","data",n,0,n)
hdata.SetMarkerStyle(20)
hdata.SetMarkerColor(ROOT.kBlack)
hdata.SetLineColor(ROOT.kBlack)

htt=hdata.Clone("tt")
htt.SetFillColor(ROOT.kBlue)
htt.SetLineColor(ROOT.kBlack)
htt.SetLineWidth(2)

hqcd=hdata.Clone("qcd")
hqcd.SetFillColor(ROOT.kOrange)
hqcd.SetLineColor(ROOT.kBlack)
hqcd.SetLineWidth(2)

for i in xrange(0,n):
    gtmp  = fIn.Get("shapes_fit_s/bin%d/data"%i)
    h1tmp = fIn.Get("shapes_fit_s/bin%d/QCD_HT"%i)
    h2tmp = fIn.Get("shapes_fit_s/bin%d/TT_TuneCP5"%i)
    if gtmp == None: 
        print ("Ignoring bin %d"%i)
        continue
    #x=gtmp.GetPointX (0) 
    y=gtmp.GetY()[0]
    hdata.SetBinContent(i+1,y)
    hqcd.SetBinContent(i+1, h1tmp.GetBinContent(1))
    htt.SetBinContent(i+1, h2tmp.GetBinContent(1))

hdata.GetYaxis().SetRangeUser(1,1000000)
hdata.Draw("AXIS")
#hdata.Draw("PE SAME")
if n== 16:
    hdata.GetXaxis().SetBinLabel(1,"pT_{<600}-|#eta|_{<1.3} #minus pT_{<600}-|#eta|_{<1.3}");
    hdata.GetXaxis().SetBinLabel(2,"pT_{>600}-|#eta|_{<1.3} #minus ");
    hdata.GetXaxis().SetBinLabel(3,"pT_{<600}-|#eta|_{>1.3} #minus ");
    hdata.GetXaxis().SetBinLabel(4,"pT_{>600}-|#eta|_{>1.3} #minus ");
    hdata.GetXaxis().SetBinLabel(5,"pT_{<600}-|#eta|_{<1.3} #minus pT_{>600}-|#eta|_{<1.3}");
    hdata.GetXaxis().SetBinLabel(6," ");
    hdata.GetXaxis().SetBinLabel(7," ");
    hdata.GetXaxis().SetBinLabel(8," ");
    hdata.GetXaxis().SetBinLabel(9,"pT_{<600}-|#eta|_{<1.3} #minus pT_{<600}-|#eta|_{>1.3}");
    hdata.GetXaxis().SetBinLabel(10," ");
    hdata.GetXaxis().SetBinLabel(11," ");
    hdata.GetXaxis().SetBinLabel(12," ");
    hdata.GetXaxis().SetBinLabel(13,"pT_{<600}-|#eta|_{<1.3} #minus pT_{>600}-|#eta|_{>1.3}");
    hdata.GetXaxis().SetBinLabel(14," ");
    hdata.GetXaxis().SetBinLabel(15," ");
    hdata.GetXaxis().SetBinLabel(16," ");
else:
    for i in xrange(0,int(math.sqrt(n))):
        for j in xrange(0,int(math.sqrt(n))):
            hdata.GetXaxis().SetBinLabel(i+int(math.sqrt(n))*j+1,"w%dw%d"%(i,j))
            hdata.LabelsOption("v")

bkg=Stack()
bkg.SetName("bkgmc")
bkg.Add(hqcd,True)
bkg.Add(htt,True)
bkg.Draw("HIST SAME")

hdata.SetBinErrorOption(ROOT.TH1.kPoisson)
hdata.Draw("PE SAME") # redraw
hdata.Draw("AXIS SAME") # redraw
hdata.Draw("AXIS X+ Y+ SAME")

if True:
    txt=ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextFont(43)
    txt.SetTextSize(20)
    txt.SetTextAlign(31)
    txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(lumi/1000.)))
    txt.SetTextSize(30)
    txt.SetTextAlign(13)
    if paper:
        txt.DrawLatex(.18,.92,"#bf{CMS}")
    else:
        txt.DrawLatex(.16,.92,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")

    #if what=='prefit':
    #    txt.SetTextSize(28)
    #    txt.DrawLatex(.16,88,"PREFIT")

c.Modify()
c.Update()

raw_input("ok?")
