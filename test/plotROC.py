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

parser.add_option("-f","--file",dest="file",type="string",help="File:var[:legend][:flip] list comma separated",default="weights/output.root:DNN:dnn")
parser.add_option("-t","--tree",dest="tree",type="string",help="Tree [%default]",default="TestTree")
#parser.add_option("-m","--multiclass",dest="multiclass",action="store_true",help="Switch on Multiclass mode [%default]",default=False)
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

ROOT.gStyle.SetOptStat(0)
objs=[]
model=None

def keras_from_minitree_hmm(t,fname='scikit/model.hd'):
    global model 
    if model== None:
        model = keras.models.load_model(fname)
        #float keras= py->Eval("kmodel.predict(np.array([ [ x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15],x[16] ] ] ))[0][0]");
    x= [ t.pt1/t.mass, #0
         t.pt2/t.mass, #1
         t.eta1, #eta1 #2
         t.eta2, #eta2 #3
         t.phi1, #phi1 #4
         t.phi2, #phi1 #5
         #
         t.Hpt, ## Hpt       #6
         t.Heta, ## Heta     #7 
         t.Hphi, ## Hphi     #8
         t.mjj_1, ##mjj      #9
         t.mjj_2,            #10
         t.detajj_1, ##detajj#11
         t.detajj_2,         #12
         t.nbjets, ##nb      #13
         t.njets, ##nj       #14
         t.ptj1, ##jpt         #15 ###  TO FIX ?
         t.met ##met         #16
        ]
    return model.predict( np.array ( [ x ] )) [0][0]

def keras_from_minitree_tb(t,fname='scikit/KERAS_tb.hd'):
    global model 
    if model== None:
        model = keras.models.load_model(fname)
    x= [          
            t.bjet_pt_0_,
            t.ht,
            t.DRl1b1,
            t.DEtaMaxBB,
            t.DRbbmin,
            t.MassMinlb,
            t.AvCSVPt,
            t.FW2,
            t.MJJJmaxPt,
            t.Cen,
            t.DRlbmin,
            t.AvDRBB,
            t.DRlbmaxPT,
            ]
    return model.predict( np.array ( [ x ] )) [0][0]

c = ROOT.TCanvas("c","c",int(500*1.5),int(450*1.5))
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
c.SetRightMargin(0.05)
c.SetTopMargin(0.15)

dummy = ROOT .TH2D("dummy","ROC",100,0,1,100,0,1)
dummy.Draw("AXIS")
dummy.Draw("AXIS X+ Y+ SAME")

dummy.GetXaxis().SetTitle("signal efficiency")
dummy.GetYaxis().SetTitle("background rejection")
dummy.GetXaxis().SetTitleOffset(1.2)
dummy.GetYaxis().SetTitleOffset(1.2)

one = ROOT.TGraph()
one.SetName("identity")
one.SetPoint(0,0,1)
one.SetPoint(1,1,0)
one.SetLineColor(ROOT.kGray+1)
one.Draw("L SAME")

colors=[ ROOT.kBlack,38,46,ROOT.kOrange, 8, ROOT.kBlue,ROOT.kGreen,ROOT.kCyan,ROOT.kMagenta]
styles=[ 1          ,1 ,1 , 7, 		7, 7,		3	, 3,3]

leg = ROOT .TLegend(0.18,0.18,.4,.5)
leg.SetBorderSize(0)

allSig=[]
allBkg=[]
allLeg=[]

for idx,s in enumerate(opts.file.split(',')):
    f = s.split(":")[0]
    v = s.split(":")[1]
    if len(s.split(':')) >2: l = s.split(":")[2]
    else: l = f+"_"+v

    if len(s.split(':')) >3: flip = bool(s.split(":")[3])
    else: flip = False

    col = colors[idx]
    sty = styles[idx]
    fIn = ROOT.TFile.Open(f)
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
    doLike=False
    if v[0] =="~": 
        v=v[1:]
        doLike=True
        print "-> Changing var to likelihood:",v

    xmin=-1
    xmax=1
    nbins=2000
    if v=="ht":
        nbins=200
        xmin=0
        xmax=4000

    hSig = ROOT.TH1D("hSig","hSig",nbins,xmin,xmax)
    hBkg = ROOT.TH1D("hBkg","hBkg",nbins,xmin,xmax)
    hSig.Sumw2()
    hBkg.Sumw2()

    if v == "hmm_keras":
        for ientry in range(0,t.GetEntries()):
            t.GetEntry(ientry)
            val = keras_from_minitree_hmm(t,'scikit/model.hd')
            if t.classID == 0 : hSig.Fill(val, t.weight)
            else : hBkg.Fill(val,t.weight)
    if v == "htb_keras":
        for ientry in range(0,t.GetEntries()):
            t.GetEntry(ientry)
            val = keras_from_minitree_tb(t)
            if t.classID == 0 : hSig.Fill(val, t.weight)
            else : hBkg.Fill(val,t.weight)
    else:
        t.Draw(v + ">>hSig","weight * (classID==1)","goff") ## 0 = sig
        t.Draw(v + ">>hBkg","weight * (classID==0)","goff") ## 1 = bkg
        if doLike:
            t0=fIn.Get("dataset/TrainTree")
            hSig0 = hSig.Clone("hSig0")
            hBkg0 = hBkg.Clone("hBkg0")
            t0.Draw(v + ">>hSig0","weight * (classID==1)","goff") ## 0 = sig
            t0.Draw(v + ">>hBkg0","weight * (classID==0)","goff") ## 1 = bkg
            ## one Tree -> hSig0 = hSig
            ## rmap to a likelihood ratio s/s+b
            hSig0.Scale(1./hSig0.Integral())
            hBkg0.Scale(1./hBkg0.Integral())
            hSig.Scale(1./hSig.Integral())
            hBkg.Scale(1./hBkg.Integral())
            hL0 = hSig.Clone("hL0") ##DEBUG
            hL = hSig.Clone("hL") ##DEBUG
            lSig=ROOT.TH1D("lSig","lSig",100,0,1.)
            lBkg=ROOT.TH1D("lBkg","lBkg",100,0,1.)
            for i in range(0,hSig.GetNbinsX()):
                s0 = hSig0.GetBinContent(i+1)
                b0 = hBkg0.GetBinContent(i+1)
                if s0+b0 > 0 and s0>0 : lr  = s0/(s0+b0)
                else: lr =0.
                if lr >.90: print "HT",hSig.GetBinCenter(i+1),"s=",s,"b=",b,"ds=",hSig0.GetBinError(i+1),"db=",hBkg0.GetBinError(i+1)
                hL0.SetBinContent(i+1,lr)
                s = hSig.GetBinContent(i+1)
                b = hBkg.GetBinContent(i+1)

                if s+b > 0 and s>0 : lrTest  = s/(s+b)
                else: lrTest =0.
                hL.SetBinContent(i+1,lrTest)

                lSig.Fill(lr,s)
                lBkg.Fill(lr,b)
            if True: ## DEBUG
                c0=ROOT.TCanvas("c0","c0")
                hSig.Draw("HIST")
                hBkg.Draw("HIST SAME")
                hL0.SetLineColor(ROOT.kRed)
                hL0.SetLineWidth(2)
                hL0.Scale(0.05)
                hL0.Draw("HIST SAME")
                hL.SetLineColor(8)
                hL.SetLineWidth(2)
                hL.Scale(0.05)
                hL.Draw("HIST SAME")
                raw_input("debug")
                c.cd()

            hSig = lSig
            hBkg = lBkg

    hSig.Scale(1./hSig.Integral())
    hBkg.Scale(1./hBkg.Integral())

    hSig.SetName("sig_%s"%l)
    hBkg.SetName("bkg_%s"%l)
    ROOT.gROOT.cd()
    allSig.append(hSig.Clone())
    allBkg.append(hBkg.Clone())
    allLeg.append(l)

    ### c0.cd(idx+1)
    ### hSig.DrawNormalized("HIST")
    ### hBkg.DrawNormalized("HIST SAME")
    ### hSig.SetLineColor(ROOT.kRed)
    ### hBkg.SetLineColor(ROOT.kBlue)

    g = ROOT.TGraph()
    g.SetName(l)
    g.SetLineColor(col)
    g.SetLineStyle(sty)
    g.SetLineWidth(2)
    ## sig -- rej
    #g.SetPoint(0,0,1)

    for i in range (0, hSig .GetNbinsX() ):
        rsig = hSig.Integral(1, i+1) ## sig efficiency is 1.-sig
        sig = 1.-rsig if not flip else rsig
        rbkg = hBkg.Integral(1, i+1) ## bkg rej
        rej = rbkg if not flip else 1.-rbkg
        #rej = 1.-bkg
        n = g.GetN()
        if n> 0 and sig == g.GetX()[n-1] and rej == g.GetY()[n-1] : continue ## remove duplicates
        g.SetPoint(n,sig, rej) 

    c.cd()
    g.Draw("L SAME")
    objs.append(g)
    leg.AddEntry(g,l,"L")

    #hSig.Delete()
    #hBkg.Delete()

leg.Draw("SAME")
c.Modify()
c.Update()

c2=ROOT.TCanvas("c2","c2",800,600)
c2.SetBottomMargin(0.15)
c2.SetLeftMargin(0.15)
c2.SetRightMargin(0.05)
c2.SetTopMargin(0.07)
leg2 = ROOT .TLegend(0.18,0.65,.4,.94)
leg2.SetBorderSize(0)
leg2.SetFillStyle(0)
m=0
for idx in range(0,len(allSig)):
    hSig=allSig[idx]
    hBkg=allBkg[idx]
    hSig.Rebin(20)
    hBkg.Rebin(20)
    hSig.SetLineColor(38)
    hSig.SetLineWidth(2)
    hBkg.SetLineColor(46)
    hBkg.SetLineWidth(2)
    hBkg.SetLineStyle(idx+1)
    hSig.SetLineStyle(idx+1)
    m=max(m,hSig.GetMaximum())
    m=max(m,hBkg.GetMaximum())
    if idx==0:
        v = opts.file.split(',')[0].split(":")[1]
        hSig.SetTitle("Signal and Background Distributions")
        hSig.Draw("HIST")
        hSig.GetXaxis().SetTitle(v)
    else: hSig.Draw("HIST SAME")
    hBkg.Draw("HIST SAME")
    if idx==0:
        leg2.AddEntry(hSig,"Sig","F")
        leg2.AddEntry(hBkg,"Bkg","F")
    hDummy=ROOT.TH1D("h","h",1,0,1)
    hDummy.SetLineColor(ROOT.kGray+2)
    hDummy.SetLineWidth(2)
    hDummy.SetLineStyle(idx+1)
    leg2.AddEntry(hDummy,allLeg[idx],"L")
    objs.append(hDummy)

allSig[0].SetMaximum(m*1.2)
leg2.Draw()

c2.Modify()
c2.Update()

raw_input("ok?")


   


