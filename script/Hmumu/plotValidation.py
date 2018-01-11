#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-f","--file",dest="file",default="Test.root",type="string",help="Input File [%default]")
parser.add_option("-r","--rebin",dest="rebin",default=4,type="int",help="Rebin factor [%default]")
(opts,args)=parser.parse_args()

import ROOT
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

fIn=ROOT.TFile("Test.root")
c=ROOT.TCanvas("c","c",400,400)
c.SetTopMargin(0.05)
c.SetRightMargin(0.05)
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
histos=[]
for i in range(0,100):
    h=fIn.Get("HmumuAnalysis/Vars/Mmm_cat%d_Data"%i)
    if h==None: break
    h.Rebin(opts.rebin)
    histos.append(h)

## plot first, middle and last
n=len(histos)
if n>0:
    histos[0].SetLineColor(38)
    histos[n/2].SetLineColor(ROOT.kGreen+2)
    histos[-1].SetLineColor(46)

    histos[0].Draw("H")
    histos[n/2].Draw("H SAME")
    histos[-1].Draw("H SAME")

    histos[0].GetXaxis().SetRangeUser(70,110)

txt = ROOT.TLatex()
txt.SetNDC()
txt.SetTextAlign(33)
txt.SetTextSize(0.03)

y=.88
xmin,xmax=80,85

txt.SetTextFont(62)
txt.DrawLatex(.93,y,"Events in %d-%d"%(xmin,xmax))
txt.SetTextFont(42)

for idx,h in enumerate(histos):
    bmin=h.FindBin(xmin)
    bmax=h.FindBin(xmax)
    txt.DrawLatex(.93,y-.04*(idx+1),"cat%d=%d"%(idx,h.Integral(bmin,bmax)))

c.SaveAs("plot.png")

