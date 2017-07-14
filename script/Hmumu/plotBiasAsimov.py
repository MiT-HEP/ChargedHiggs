import sys,os
from optparse import OptionParser, OptionGroup
usage = "usage: %prog [options]"
parser=OptionParser(usage=usage)
parser.add_option("-f","--file" ,dest='file',type='string',help="Input MLFIT file [Default=%default]",default="mlfit.root")
parser.add_option("-d","--dir" ,dest='dir',type='string',help="Directory [Default=%default]",default="shapes_fit_s/ch1_cat8")
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

fIn=ROOT.TFile.Open(opts.file)

r=fIn.Get(opts.dir+"/data").Clone("r")
BKG=fIn.Get(opts.dir+"/BKG").Clone("BKG")

for i in range(0,r.GetN()):
    r.GetY()[i] = r.GetY()[i]/BKG.GetBinContent(BKG.FindBin(r.GetX()[i])) ;
    r.GetEYhigh()[i] = r.GetEYhigh()[i]/BKG.GetBinContent(BKG.FindBin(r.GetX()[i])) ;
    r.GetEYlow()[i] = r.GetEYlow()[i]/BKG.GetBinContent(BKG.FindBin(r.GetX()[i])) ;

c=ROOT.TCanvas("c","c",800,800)
c.SetTopMargin(0.05)
c.SetBottomMargin(0.15)
c.SetRightMargin(0.05)
c.SetLeftMargin(0.15)

r.Draw("APE")
r.GetXaxis().SetRangeUser(110,150)
r.GetYaxis().SetRangeUser(.9,1.2)
r.GetXaxis().SetTitle("m_{#mu#mu} [GeV]")
r.GetXaxis().SetTitleOffset(1.4)
r.GetYaxis().SetTitle("Test function / fit function")
r.GetYaxis().SetTitleOffset(1.4)

rb=fIn.Get(opts.dir+"/BKG").Clone("rb")
for i in range(1,rb.GetNbinsX()+1):
    rb.SetBinContent(i, rb.GetBinContent(i)/BKG.GetBinContent(i));
    rb.SetBinError(i, rb.GetBinError(i)/BKG.GetBinContent(i));

rb.SetFillColor(ROOT.kRed)
rb.SetFillStyle(3001)
rb.Draw("E2 SAME")
r.Draw("PE SAME")

tree=fIn.Get("tree_fit_sb")
tree.Scan("mu")
tree.Draw("mu","","goff")

rs = fIn.Get(opts.dir+"/total_signal").Clone("rs")
for i in range(1,rs.GetNbinsX()+1):
    rs.SetBinContent(i, 1+rs.GetBinContent(i)/tree.GetV1()[0]/BKG.GetBinContent(i));
rs.Draw("HIST SAME")

ltx=ROOT.TLatex()
ltx.SetNDC()
ltx.SetTextFont(43)
ltx.SetTextSize(28)
ltx.SetTextAlign(13)
ltx.DrawLatex(.18,.93,"#bf{CMS} #it{#scale[0.8]{Simulation Preliminary}}")

leg=ROOT.TLegend(.7,.7,.9,.9)
leg.AddEntry(r,"Asimov Dataset","PE")
leg.AddEntry(rb,"Background fit","F")
leg.AddEntry(rs,"Signal (pre-fit)","L")

leg.Draw()

c.Modify()
c.Update()

raw_input("ok?")
