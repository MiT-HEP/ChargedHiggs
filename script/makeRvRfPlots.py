#!/usr/bin/env python

# Usual reasons to love python imports
import os
import sys
import array 

from optparse import OptionParser
parser = OptionParser()
#parser.add_option("-f","--file",dest="files",default=[],action="append",help="Add a file")
parser.add_option("-f","--file",dest="file",default="",type="string",help="Input file")
#parser.add_option("-l","--label",dest="label",default="",type="string",help="Labels, or none or one for input file")
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C")
#parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true")
#parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
#parser.add_option("-u","--unblind",dest="unblind",default=False,action="store_true",help="Draw observation")
##
parser.add_option("","--addSM",dest="addSM",default=False,action="store_true",help="Add SM Diamond to 2D plot")
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

def set_palette(ncontours=999):
    style=4
    if (style==1):
     # default palette, looks cool
     stops = [0.00, 0.34, 0.61, 0.84, 1.00]
     red   = [0.00, 0.00, 0.77, 0.85, 0.90]
     green = [0.00, 0.81, 1.00, 0.20, 0.00]
     blue  = [0.51, 1.00, 0.12, 0.00, 0.00]

     st = array.array('d', stops)
     re = array.array('d', red)
     gr = array.array('d', green)
     bl = array.array('d', blue)

    elif (style==3):
     
     red   = [ 0.00, 0.90, 1.00] 
     blue  = [ 1.00, 0.50, 0.00] 
     green = [ 0.00, 0.00, 0.00] 
     stops = [ 0.00, 0.50, 1.00] 
     st = array.array('d', stops)
     re = array.array('d', red)
     gr = array.array('d', green)
     bl = array.array('d', blue)

    elif (style==2):
     # blue palette, looks cool
     stops = [0.00, 0.14, 0.34, 0.61, 0.84, 1.00]
     red   = [0.00, 0.00, 0.00, 0.05, 0.30, 1.00]
     green = [0.00, 0.00, 0.00, 0.00, 0.00, 0.00]
     blue  = [1.00, 0.80, 0.6, 0.4, 0.2, 0.0]
    elif (style==4):
     stops = [0.00, 1.00]
     red   = [1.00, 48./255.]
     green = [1.00, 48./255.]
     blue = [1.00, 131./255.]

     st = array.array('d', stops)
     re = array.array('d', red)
     gr = array.array('d', green)
     bl = array.array('d', blue)

    npoints = len(st)
    ROOT.TColor.CreateGradientColorTable(npoints, st, re, gr, bl, ncontours)
    ROOT.gStyle.SetNumberContours(ncontours)


objs=[]
graphs=[]

c=ROOT.TCanvas()
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)
xmin=-1
xmax=6
dummy = ROOT.TH2D("dummy","dummy",1000, xmin,xmax,1000,xmin,xmax)
dummy.GetXaxis().SetRangeUser(xmin,xmax)
#dummy.GetYaxis().SetRangeUser(1.e-10,1)
dummy.GetYaxis().SetRangeUser(xmin,xmax)

dummy.GetXaxis().SetTitle("rV")
dummy.GetYaxis().SetTitle("rF")
dummy.GetZaxis().SetTitle("-2 #Delta Ln L")

dummy.Draw("AXIS")


fIn=ROOT.TFile.Open(opts.file)
tree = fIn.Get('limit')
res=[]
for i in range(tree.GetEntries()):
  tree.GetEntry(i)
  rf = tree.RF
  rv = tree.RV
  if tree.deltaNLL<0 : print "Warning, found -ve deltaNLL = ",  tree.deltaNLL, " at ", rv,rf
  if 2*tree.deltaNLL < 100:
    res.append([rv,rf,2*tree.deltaNLL])
  else:
    res.append([rv,rf,100.])
res.sort()
## clean spikes
#res = cleanSpikes1D(res)
minNLL = min([re[2] for re in res])

obs=ROOT.TGraph2D()
for rv,rf, nll in res: 
    if nll>=0. and nll<10:
        obs.SetPoint(obs.GetN(),rv,rf,nll)
#if True: smoothNLL(obs,res)
graphs.append(obs)
th2=dummy.Clone("obs2")
for i in range(0,th2.GetNbinsX()):
 for j in range(0,th2.GetNbinsY()):
    x= th2.GetXaxis().GetBinCenter(i+1)
    y= th2.GetYaxis().GetBinCenter(j+1)
    #th2.SetBinContent(i+1,j+1,obs.Eval(x,y))
    th2.SetBinContent(i+1,j+1,obs.Interpolate(x,y))

th2.SetTitle("")
th2.SetMinimum(-0.0001)
th2.SetMaximum(10.)

cont_1sig = th2.Clone('cont_1_sig')
cont_1sig.SetContour(2)
cont_1sig.SetContourLevel(1,2.3)
cont_1sig.SetLineColor(ROOT.kBlack)
cont_1sig.SetLineWidth(3)
cont_1sig.SetLineStyle(1)
cont_2sig = th2.Clone('cont_2_sig')
cont_2sig.SetContour(2)
cont_2sig.SetContourLevel(1,6.18)
cont_2sig.SetLineColor(ROOT.kBlack)
cont_2sig.SetLineWidth(3)
cont_2sig.SetLineStyle(2)
cont_2sig.SetLineWidth(2)

gBF = ROOT.TGraph()
printedOK=False
for ev in range(tree.GetEntries()):
    tree.GetEntry(ev)
    if tree.deltaNLL==0:
      if not printedOK : 
        print "Best Fit (RV,RF) : ","=%.4f"%tree.RV,",","=%.4f"%tree.RF
        printedOK=True
      gBF.SetPoint(0,tree.RV,tree.RF)
    if tree.deltaNLL<0: continue
gBF.SetMarkerStyle(34)
gBF.SetMarkerSize(2.0)
gBF.SetMarkerColor(ROOT.kBlack)
gBF.SetLineColor(ROOT.kBlack)

## leg = ROOT.TLegend(0.75,.15,.89,.45)
## leg.AddEntry()
##   #float(options.legend.split(',')[0]),float(options.legend.split(',')[1]),float(options.legend.split(',')[2]),float(options.legend.split(',')[3]))
##   #leg.SetFillColor(10)

smmarker = ROOT.TMarker(1,1,33)
smmarker.SetMarkerColor(97)
smmarker.SetMarkerSize(2.5)
smmarker2 = ROOT.TMarker(1,1,33)
smmarker2.SetMarkerColor(89)
smmarker2.SetMarkerSize(1.4)
smmarker_leg = smmarker.Clone("smmarker_leg")
#smmarker_leg.SetMarkerStyle(27)
smmarker_leg.SetMarkerSize(2.5)

### DRAW
set_palette(ncontours=255);
th2.Draw("colz9")
gBF_underlay = gBF.Clone()
gBF_underlay.SetMarkerColor(ROOT.kWhite)
gBF_underlay.SetMarkerSize(2.6)
gBF_underlay.Draw("Psame")
gBF.Draw("Psame")
cont_1sig.SetLineColor(1);
cont_2sig.SetLineColor(1);
cont_1sig.Draw("cont3same9")
cont_2sig.Draw("cont3same9")
smmarker.Draw()
#leg.Draw()

# draw fit value
l = ROOT.TLatex()
l.SetNDC()
#l.SetTextFont(42)
#l.SetTextSize(0.045)
#l.SetTextAlign(33)
#l.DrawLatex(0.89,0.88,"#hat{#mu}_{SM} = %4.1f ^{#font[122]{+}%4.1f}_{#font[122]{-}%4.1f}"%(xmin,eplus,eminus))

l.SetTextFont(42)
l.SetTextSize(0.055)
l.SetTextAlign(13)
l.DrawLatex(0.13,.88,"#bf{CMS} #scale[0.75]{#it{Preliminary}}")
l.SetTextSize(0.035)
l.SetTextAlign(31)
l.DrawLatex(0.90,.91,"35.9 fb^{-1} (13 TeV)")

l.SetTextAlign(13)
l.SetTextSize(0.035)
l.DrawLatex(0.13+0.5,0.88-.05,"H#rightarrow#mu#mu")

c.Update()

raw_input("ok?")

c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")

