import ROOT
import os,sys
from glob import glob

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(1)
ROOT.gStyle.SetPaintTextFormat("4.2f");

cat="cat12"

from array import array
def GetRGB(color):
        r=array('f',[0.])
        g=array('f',[0.])
        b=array('f',[0.])
        ROOT.gROOT.GetColor(color).GetRGB(r,g,b);  
        return ( r[0],g[0],b[0])

def ChangePalette(num=0):
    white  = GetRGB(ROOT.kWhite);
    darkRed = GetRGB (ROOT.kRed+2);
    red  = GetRGB (ROOT.kRed);
    black = GetRGB  (ROOT.kBlack);
    orange = GetRGB (ROOT.kOrange);
    yellow = GetRGB (ROOT.kYellow);
    darkBlue = GetRGB(ROOT.kBlue+2);
    blue = GetRGB (ROOT.kBlue);
    cyan = GetRGB (ROOT.kCyan);
    if num==0:
        r = array('d', [darkRed[0], red[0], orange[0], yellow[0], white[0], cyan[0], blue[0], darkBlue[0], black[0] ] ) 
        g = array('d', [darkRed[1], red[1], orange[1], yellow[1], white[1], cyan[1], blue[1], darkBlue[1], black[1] ] ) 
        b = array('d', [darkRed[2], red[2], orange[2], yellow[2], white[2], cyan[2], blue[2], darkBlue[2], black[2] ] ) 
        s = array('d', [0.,0.05,0.10, .30, .5, .7,.9,.95, 1.0 ] )
        FI = ROOT.TColor.CreateGradientColorTable(len(s), s, r, g, b, 255);
        ROOT.gStyle.SetNumberContours(99);
    if num==1:
        r = array('d', reversed([darkRed[0], red[0], orange[0], yellow[0], white[0], cyan[0], blue[0], darkBlue[0], black[0] ]) ) 
        g = array('d', reversed([darkRed[1], red[1], orange[1], yellow[1], white[1], cyan[1], blue[1], darkBlue[1], black[1] ]) ) 
        b = array('d', reversed([darkRed[2], red[2], orange[2], yellow[2], white[2], cyan[2], blue[2], darkBlue[2], black[2] ]) ) 
        s = array('d', [0.,0.05,0.10, .30, .5, .7,.9,.95, 1.0 ] )
        FI = ROOT.TColor.CreateGradientColorTable(len(s), s, r, g, b, 255);
        ROOT.gStyle.SetNumberContours(99);
    if num==2: ##Gamma Gamma
        s = array('d',[0.00, 1.00])
        r = array('d',[1.00, 48./255.])
        g = array('d',[1.00, 48./255.])
        b = array('d',[1.00, 131./255.])
        FI = ROOT.TColor.CreateGradientColorTable(len(s), s, r, g, b, 255);
        ROOT.gStyle.SetNumberContours(99);
    if num==3:
        r = array('d', reversed([darkRed[0], red[0], orange[0], yellow[0], white[0], yellow[0], orange[0], red[0], darkRed[0] ]) ) 
        g = array('d', reversed([darkRed[1], red[1], orange[1], yellow[1], white[1], yellow[1], orange[1], red[1], darkRed[1] ]) ) 
        b = array('d', reversed([darkRed[2], red[2], orange[2], yellow[2], white[2], yellow[2], orange[2], red[2], darkRed[2] ]) ) 
        s = array('d', [0.,0.25,0.39, .40, .5, .6,.61,.75, 1.0 ] )
        FI = ROOT.TColor.CreateGradientColorTable(len(s), s, r, g, b, 255);
        ROOT.gStyle.SetNumberContours(99);


if len(sys.argv)>1:
   cat=sys.argv[1]

c=ROOT.TCanvas("c","c",600,600)
c.SetTopMargin(0.15)
c.SetRightMargin(0.15)
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
ChangePalette(3)

h=ROOT.TH2D("bias","bias " + cat,10,0,10,10,0,10)

labels= ["Bern.","BWZGamma","BWZRed","SExp","fewz_1j","fewz_2j","fewz_full","7","8","9","10"]
for b in range(0,7):
    label = labels[b]
    h.GetXaxis().SetBinLabel(b+1,label)
    h.GetYaxis().SetBinLabel(b+1,label)

h.GetXaxis().SetTitle("fit")
h.GetYaxis().SetTitle("gen")
h.GetXaxis().SetTitleOffset(1.5)
h.GetYaxis().SetTitleOffset(2.0)

for gen in range(0,7):
   for fit in range(0,7):
      t=ROOT.TChain("tree_fit_sb")
      #print "using scheme '"+"*/"+cat+"/mlfit_" + cat + "_mu_1_gen_%d_fit_%d.root"%(gen,fit)+"'"
      files = glob("*/"+cat+"/mlfit_" + cat + "_mu_1_gen_%d_fit_%d.root"%(gen,fit))
      nfiles=len(files)
      for f in files:
          t.Add(f)
      #print "* Adding ",nfiles,"trees for gen=",gen,"fit=",fit
      t.Draw("(mu-1)/muErr","","goff")
      n=t.GetSelectedRows()
      print "      nevents=",n
      l = []
      for i in range(0,n):
      	l.append(t.GetV1()[i])
      l=[ x for x in sorted(l) if x>-20 and x<20 ]
      #h.SetBinContent(gen+1,fit+1, l[n/2])
      n=len(l)
      h.SetBinContent(fit+1,gen+1, l[n/2])
      print "gen=",gen,"fit=",fit,"median=",l[n/2],"n=",n
      #t.Delete()
h.Draw("COLZ TEXT")
h.GetZaxis().SetRangeUser(-1,1)
h.GetXaxis().SetRangeUser(0,7)
h.GetYaxis().SetRangeUser(0,7)
h.Draw("AXIS X+ Y+ SAME")
c.Update()

raw_input("ok?")
c.SaveAs("bias_"+cat+".pdf")
c.SaveAs("bias_"+cat+".png")

