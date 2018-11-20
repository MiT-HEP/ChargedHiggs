#!/usr/bin/env python
import os
import sys
import array 
import numpy as np
#import keras
#print "keras version=",keras.__version__

usage="Plot ROCs out of TMVA output"
from optparse import OptionParser
parser = OptionParser(usage=usage)

parser.add_option("-f","--file",dest="file",type="string",help="File:var file and main variable",default="file.root")
parser.add_option("-t","--tree",dest="tree",type="string",help="Tree [%default]",default="TestTree")
parser.add_option("-v","--var",dest="var",type="string",help="var2:nbins:xmin:xmax",default="var1")
#parser.add_option("-m","--multiclass",dest="multiclass",action="store_true",help="Switch on Multiclass mode [%default]",default=False)
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

ROOT.gStyle.SetOptStat(0)
objs=[]
model=None




colors=[ ROOT.kBlack,38,46,ROOT.kOrange, 8, ROOT.kBlue,ROOT.kGreen,ROOT.kCyan,ROOT.kMagenta]

leg = ROOT .TLegend(0.18,0.18,.4,.5)
leg.SetBorderSize(0)

allPlots=[]
allLeg=[]

var = opts.var.split(":")[0]
nbins2= int(opts.var.split(":")[1])
xmin2= float(opts.var.split(":")[2])
xmax2= float(opts.var.split(":")[3])

for idx,s in enumerate(opts.file.split(',')):
    f = s.split(":")[0]
    v = s.split(":")[1]

    fIn = ROOT.TFile.Open(f)
    if fIn==None: print "ERROR: Unable to open file",f
    t = fIn.Get(opts.tree)
    if t==None:
        #  try to find directory
        n= fIn.GetListOfKeys().GetSize() 
        for ikey in range(0,n):
            name=fIn.GetListOfKeys().At(ikey).GetName()
            obj=fIn.Get(name)
            if obj.InheritsFrom("TDirectory"):t = fIn.Get(name+"/"+opts.tree)
            if t != None: 
                print "[INFO]","using tree in directory",name
                break
    if t==None: 
        print "[ERROR]","Unable to grab tree",opts.tree,"from",f
        continue

    print "-> processing var",v, "from file",f

    xmin=-1
    xmax=1
    nbins=2000


    hSig = ROOT.TH1D("hSig","hSig",nbins,xmin,xmax)
    hBkg = ROOT.TH1D("hBkg","hBkg",nbins,xmin,xmax)
    hSig.Sumw2()
    hBkg.Sumw2()

    t.Draw(v + ">>hSig","weight * (classID==1)","goff") ## 0 = sig
    t.Draw(v + ">>hBkg","weight * (classID==0)","goff") ## 1 = bkg

    hSig.Scale(1./hSig.Integral())
    hBkg.Scale(1./hBkg.Integral())
    ROOT.gROOT.cd()

    quantiles = [0.05,0.10,0.20,0.50,1.00]
    boundaries=[hSig.GetNbinsX()]
    iQ=0 ## index on the quantiles
    for i in reversed(range(0, hSig . GetNbinsX() )):
        if hSig.Integral(i,hSig . GetNbinsX()) > quantiles[iQ]:
            boundaries.append(i)
            iQ+=1
        if iQ >=len(quantiles) : break

    
    hSig2D=ROOT.TH2D("hSig2D","hSig2D",nbins,xmin,xmax,nbins2,xmin2,xmax2)
    hBkg2D=ROOT.TH2D("hBkg2D","hBkg2D",nbins,xmin,xmax,nbins2,xmin2,xmax2)
    #t.Draw(v +":"+var+ ">>hSig2D","weight * (classID==1)","goff") # 1=Sig
    #t.Draw(v +":"+var+ ">>hBkg2D","weight * (classID==0)","goff") 
    t.Draw(var+":"+v+ ">>hSig2D","weight * (classID==1)","goff") # 1=Sig
    t.Draw(var+":"+v+ ">>hBkg2D","weight * (classID==0)","goff") 

    for iQ,q in enumerate(quantiles):
        col = colors[iQ]
        print "Projection on boundaries:",boundaries[iQ+1],boundaries[iQ]-1 if iQ >0 else hSig.GetNbinsX(), "Quantile",quantiles[iQ], "==",hSig.Integral(boundaries[iQ+1],hSig . GetNbinsX())
        hSig2= hSig2D.ProjectionY("sig_%d"%iQ,boundaries[iQ+1],boundaries[iQ]-1 if iQ>0 else hSig.GetNbinsX())
        hBkg2= hBkg2D.ProjectionY("bkg_%d"%iQ,boundaries[iQ+1],boundaries[iQ]-1 if iQ>0 else hSig.GetNbinsX())
        hSig2.Scale(1./hSig2.Integral())
        hBkg2.Scale(1./hBkg2.Integral())

        hSig2.SetLineColor(col)
        hBkg2.SetLineColor(col)
        hSig2.SetLineWidth(2)
        hBkg2.SetLineWidth(2)
        hBkg2.SetLineStyle(7)
        hSig2.SetLineStyle(1)

        allPlots.append(hSig2)
        leg.AddEntry(allPlots[-1],"Quantile %.0f%%"%(q*100),"L")
        allPlots.append(hBkg2)


c = ROOT.TCanvas("c","c",int(500*1.5),int(450*1.5))
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
c.SetRightMargin(0.05)
c.SetTopMargin(0.15)

dummy = ROOT .TH1D("dummy","ROC",nbins2,xmin2,xmax2)
dummy.Draw("AXIS")
dummy.Draw("AXIS X+ Y+ SAME")

ymax=0
for p in reversed(allPlots):
    ymax=max(ymax,p.GetMaximum())
    p.Draw("HIST SAME")

dummy.SetMaximum(ymax*1.2)
leg.Draw()
c.Modify()
c.Update()

raw_input("ok?")


   


