import numpy as np
np.random.seed(1234567)
print "numpy version=",np.__version__
import random
import math
import sys
sys.setrecursionlimit(10000)
import ROOT
import time
import re

import keras
print "keras version=",keras.__version__

cidx=0
def makeROC(sig,bkg):
    global cidx
    roc =ROOT.TGraph();
    roc.SetName("roc_"+var)
    sig.Scale(1./sig.Integral());
    bkg.Scale(1./bkg.Integral());
    for i in range(0,sig.GetNbinsX()):
        roc.SetPoint(roc.GetN(),1.-sig.Integral(1,i+1),bkg.Integral(1,i+1))
    colors=[38,46,ROOT.kGray,ROOT.kBlack,ROOT.kGreen+2,ROOT.kOrange,ROOT.kMagenta]
    roc.SetLineColor(colors[cidx])
    cidx+=1
    return roc

class ElapsedTimer(object):
    def __init__(self):
        self.start_time = time.time()
    def elapsed(self,sec):
        if sec < 60:
            return str(sec) + " sec"
        elif sec < (60 * 60):
            return str(sec / 60) + " min"
        else:
            return str(sec / (60 * 60)) + " hr"
    def elapsed_time(self):
        print("Elapsed: %s " % self.elapsed(time.time() - self.start_time) )

classifier =keras.models.load_model("weights00000001.h5")
# '@' -> t. ; '~' w/o t.
features= [  "@pt1/@mass", #0
             "@pt2/@mass", #1
             "eta1", #eta1 #2
             "eta2", #eta2 #3
             "phi1", #phi1 #4
             "phi2", #phi1 #5
             #
             "Hpt", ## Hpt       #6
             "Heta", ## Heta     #7 
             "Hphi", ## Hphi     #8
             "mjj_1", ##mjj      #9
             "mjj_2",            #10
             "detajj_1", ##detajj#11
             "detajj_2",         #12
             #t.nbjets", ##nb      #13
             "maxDeepB",      #13
             "njets", ##nj       #14
             "~ptj1", ##jpt         #15
             "met", ##met         #16
             "softNjets1",   #17
             "softHt1",      #18
             "firstQGL",     #19
             "secondQGL",    #20
             "thirdQGL",     #21
             "costhetastar",     #22
        ]

def loop():
    histos={}
    for mc in ["S","B"]:
        histos["bdt_"+mc] = ROOT.TH1D("bdt_"+mc,"bdt_"+mc,200,-1,1);
        histos["dnn_"+mc] = ROOT.TH1D("dnn_"+mc,"dnn_"+mc,200,-1,1);

    for idx in range(0,300):
        if idx==1 or idx==56: continue
        f=ROOT.TFile.Open("root://eoscms///store/user/amarini/Hmumu/fwk/2018_08_10_SyncTree//ChHiggs_%d.root"%idx)
        t=f.Get("hmm")
        if t==None: print "ERROR File",f.GetName(),"does not exist"
        print "Entries in file",idx,"Entries=",t.GetEntries()
        for ientry in range(0,t.GetEntries()):
            t.GetEntry(ientry)
            if t.eventNum %2 ==0: continue
            if t.mass <110 or t.mass >150 : continue
            if not (t.pass_recomuons and t.pass_asymmcuts and t.pass_trigger)  : continue
            ptj1 = 0.
            if t.njets >0 : ptj1=t.jet_pt[0]
            x= [ eval(re.sub('@','t.',s)) if '@' in s else eval(re.sub('~','',s)) if '~' in s else eval("t."+s) for s in features ]

            y=-1
            if t.mc< 0 : y=1
            else : y=0

            xsec=1
            if t.mc <0 :  xsec*=2.176E-4
            if t.mc <-10 and t.mc >-15 : xsec*=48.58
            if t.mc <-20 and t.mc >-25 : xsec*=3.78
            w= t.mcWeight * xsec


            if (y==1):
                histos["bdt_S"].Fill(t.bdt,w)
                histos["dnn_S"].Fill(classifier.predict([x])[0],w)
            else:
                histos["bdt_B"].Fill(t.bdt,w)
                histos["dnn_B"].Fill(classifier.predict([x])[0],w)
    return histos

fOut=ROOT.TFile.Open("comparison.root")
histos=loop()

roc_bdt=makeROC( histos["bdt_S"],histos["bdt_B"])
roc_bdt.SetName("roc_bdt")
roc_dnn=makeROC( histos["dnn_S"],histos["dnn_B"])
roc_dnn.SetName("roc_dnn")
for hstr in histos: histos[hstr].Write()
roc_bdt.Write()
roc_dnn.Write()
