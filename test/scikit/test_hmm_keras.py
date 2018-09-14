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
def makeROC(sig,bkg,flip=False):
    global cidx
    roc =ROOT.TGraph();
    roc.SetName("roc_"+sig.GetName())
    if sig.Integral() >0:sig.Scale(1./sig.Integral());
    if bkg.Integral() >0:bkg.Scale(1./bkg.Integral());
    for i in range(0,sig.GetNbinsX()):
        x=1.-sig.Integral(1,i+1)
        y=bkg.Integral(1,i+1)
        if flip:
            x=1.-x
            y=1.-y
        roc.SetPoint(roc.GetN(),x,y)
    colors=[38,46,ROOT.kGray,ROOT.kBlack,ROOT.kGreen+2,ROOT.kOrange,ROOT.kMagenta]
    roc.SetLineColor(colors[cidx%len(colors)])
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


# '@' -> t. ; '~' w/o t.
features= [  "@pt1/@mass", #0
             "@pt2/@mass", #1
             "eta1", #eta1 #2
             "eta2", #eta2 #3
             #"phi1", #phi1 #4
             #"phi2", #phi1 #5
             #
             "Hpt", ## Hpt       #6
             "Heta", ## Heta     #7 
             #"Hphi", ## Hphi     #8
             "mjj_1", ##mjj      #9
             #"mjj_2",            #10
             "detajj_1", ##detajj#11
             #"detajj_2",         #12
             #t.nbjets", ##nb      #13
             "maxDeepB",      #13
             #"njets", ##nj       #14
             "~ptj1", ##jpt         #15
             "~ptj2", ##jpt         #15
             "met", ##met         #16
             #"softNjets1",   #17
             "softHt1",      #18
             "firstQGL",     #19
             #"secondQGL",    #20
             #"thirdQGL",     #21
             "costhetastar",     #22
        ]

classifier =keras.models.load_model("model7.hd")
multiclass=True
do2D=True
import threading
import time
lock=threading.Lock()

histos={}
for mc in ["S","B"]:
    histos["bdt_"+mc] = ROOT.TH1D("bdt_"+mc,"bdt_"+mc,200,-1,1);
    histos["dnn_"+mc] = ROOT.TH1D("dnn_"+mc,"dnn_"+mc,10000,-1,1);
    histos["dnn1_"+mc] = ROOT.TH1D("dnn1_"+mc,"dnn_"+mc,10000,-1,6);
    histos["dnn2_"+mc] = ROOT.TH1D("dnn2_"+mc,"dnn_"+mc,10000,-1,6);
    histos["dnn3_"+mc] = ROOT.TH1D("dnn3_"+mc,"dnn_"+mc,10000,-1,6);
    for multi in ['DY','TT','ggH','qqH']:
        histos["dnn_"+multi+"_"+mc] = ROOT.TH1D("dnn_"+multi+"_"+mc,"dnn_"+mc,10000,-1,1);

if do2D:
    for mc in ["S","B"]:
        for var in features:
            v=re.sub('~','',re.sub('@','',re.sub('!','',var)))
            xmin=-1
            xmax=1
            nbins=200
            if 'pt' in v or 'mjj' in v or 'Ht' in v or 'met' in v:
                xmin=0
                xmax=1000
            if 'njets' in v or 'Njets' in v : 
                nbins=10
                xmin=0
                xmax=10
            if 'eta' in v:
                xmin=-5
                xmax=5
            if 'costhetastar' in v:
                xmin=-1
                xmax=1
            if 'QGL' in v:
                xmin=0
                xmax=1
            if 'phi' in v:
                xmin=-3.1416
                xmax=3.1416
            histos[v+"_"+mc] = ROOT.TH2D(v+"_"+mc,v,10000,-1,6,nbins,xmin,xmax)

def loop(idx_num):
    global histos
    global lock
    global classifier
    printDebug=True
    #for idx in range(0,300):
    for idx in [idx_num]: ## the for is a residual
        if idx==1 or idx==56: continue
        f=ROOT.TFile.Open("root://eoscms///store/user/amarini/Hmumu/fwk/2018_08_10_SyncTree//ChHiggs_%d.root"%idx)
        if f==None: 
            print "ERROR File",idx,"does not exist"
            continue
        t=f.Get("hmm")
        if t==None: 
            print "ERROR File",f.GetName(),"does not exist"
            continue
        print "Entries in file",idx,"Entries=",t.GetEntries()
        for ientry in range(0,t.GetEntries()):
            t.GetEntry(ientry)
            #if t.eventNum %2 ==0: continue
            if t.mass <110 or t.mass >150 : continue
            if not (t.pass_recomuons and t.pass_asymmcuts and t.pass_trigger)  : continue
            ptj1 = 29.
            if t.njets >0 : ptj1=t.jet_pt[0]
            ptj2 = 29.
            if t.njets >1 : ptj2=t.jet_pt[1]
            x= [ eval(re.sub('@','t.',s)) if '@' in s else eval(re.sub('~','',s)) if '~' in s else eval("t."+s) for s in features ]
            
            if t.mc==0: continue

            if multiclass:
                if t.mc< 0 : y=[0,0,1,0]
                else : y=[1,0,0,0]

                if t.mc <-20 and t.mc >-30 : y=[0,0,0,1]

                if t.mc >20: y=[0,1,0,0]
            else:
                if t.mc< 0 : y=1
                else : y=0

            xsec=1
            if t.mc <0 :  
                xsec*=2.176E-4
                # 120 -1, 125 -2 ,130 -3
                if t.mc <-10 and t.mc >-15 : xsec*=48.58 #ggH
                elif t.mc <-20 and t.mc >-25 : xsec*=3.78  #VBF
                # powheg -25
                elif t.mc <-25 and t.mc >-30 : xsec*=0.  #VBF-powheg
                elif t.mc <-30 and t.mc >-35 : xsec*=0.8839 #ZH
                elif t.mc <-40 and t.mc >-45 : xsec*=0.5328 #W-
                elif t.mc <-50 and t.mc >-55 : xsec*=0.84   #W+
                elif t.mc <-60 and t.mc >-65 : xsec*=0.5071 #ttH
                else: 
                    print "ERROR", "unable to identify signal mc",t.mc,"Trying to ignore it"
                    xsec=0. ## unknown signal
            w= t.mcWeight * xsec
            
            lock.acquire()

            if multiclass: #multiclass
                dnn0=classifier.predict(np.array([x]))[0]
                #[dy,tt,ggH,vbf]
                dy=60000. ## expected events in fwhm
                tt=dy*6/50.
                ggh=230.
                vbf=18.

                sigma=np.array([dy,tt,ggh,vbf])

                S = np.sum(dnn0)
                p=dnn0/S 


                l=p*sigma ## copy l

                dnnSM= np.exp(dnn0)/np.sum(np.exp(dnn0))

                pmax=p.argmax()
                lmax=l.argmax()

                p[0]=1.-p[0]
                p[1]=1.-p[1]

                selp=np.array([0,0,0,0])
                sell=np.array([0,0,0,0])
                selp[pmax]=1
                sell[lmax]=1
                dnn=dnn0[3] ##VBF
                dnn1 =pmax+ np.sum(selp*p) 
                dnn2 =pmax+ np.sum(sell*p)  ## I do it on p, not on l

                dnn3 = 0.25*dnnSM[2]+0.75*dnnSM[3]
            else:
                dnn=classifier.predict(np.array([x]))[0][0]

            
            if printDebug:
                printDebug=False
                print " -------------- DEBUG ---------------"
                print "x=",x
                print "y=",y
                print "w=",w
                print "dnn=",dnn
                print "bdt=",t.bdt
                if multiclass:
                    print "DNN=",dnn0
                    print "P=",p
                    print "l=",l
                    print "pmax=",pmax
                    print "lmax=",lmax
                    print "dnn1=",dnn1
                    print "dnn2=",dnn2
                    print "dnn3=",dnn3
                print "-------------------------------------"
            mc='S' if t.mc<0 else 'B'

            histos["bdt_"+mc].Fill(t.bdt,w)
            histos["dnn_"+mc].Fill(dnn,w)

            if multiclass:
                histos["dnn1_"+mc].Fill(dnn1,w)
                histos["dnn2_"+mc].Fill(dnn2,w)
                histos["dnn3_"+mc].Fill(dnn3,w)

                histos["dnn_DY_"+mc].Fill(dnn0[0],w)
                histos["dnn_TT_"+mc].Fill(dnn0[1],w)
                histos["dnn_ggH_"+mc].Fill(dnn0[2],w)
                histos["dnn_qqH_"+mc].Fill(dnn0[3],w)

            if do2D:
                for iv,var in enumerate(features): 
                    v=re.sub('~','',re.sub('@','',re.sub('!','',var)))
                    value= x[iv]
                    histos[v+"_"+mc].Fill(dnn,value,w) 

            lock.release()
    return 

threads=[]
#histos=loop()
for idx in range(0,300):
    while threading.activeCount() >= 6: 
        time.sleep(3)
    t = threading.Thread(target=loop, args=(idx,)  )
    t.start()
    threads.append( t )

for t in threads:
    t.join()
print " ************* Done ***************"

fOut=ROOT.TFile.Open("comparison.root","RECREATE")
fOut.cd()
fOut.cd()
for hstr in histos: histos[hstr].Write()

roc_bdt=makeROC( histos["bdt_S"],histos["bdt_B"])
roc_bdt.SetName("roc_bdt")
roc_dnn=makeROC( histos["dnn_S"],histos["dnn_B"])
roc_dnn.SetName("roc_dnn")
roc_bdt.Write()
roc_dnn.Write()
if multiclass:
    roc_dnn1=makeROC( histos["dnn1_S"],histos["dnn1_B"])
    roc_dnn1.SetName("roc_dnn1")
    roc_dnn1.Write()

    roc_dnn2=makeROC( histos["dnn2_S"],histos["dnn2_B"])
    roc_dnn2.SetName("roc_dnn2")
    roc_dnn2.Write()

    roc_dnn3=makeROC( histos["dnn3_S"],histos["dnn3_B"])
    roc_dnn3.SetName("roc_dnn3")
    roc_dnn3.Write()

    roc_dnn_DY=makeROC( histos["dnn_DY_S"],histos["dnn_DY_B"],True)
    roc_dnn_DY.SetName("roc_dnn_DY")
    roc_dnn_DY.Write()

    roc_dnn_TT=makeROC( histos["dnn_TT_S"],histos["dnn_TT_B"],True)
    roc_dnn_TT.SetName("roc_dnn_TT")
    roc_dnn_TT.Write()

    roc_dnn_ggH=makeROC( histos["dnn_ggH_S"],histos["dnn_ggH_B"])
    roc_dnn_ggH.SetName("roc_dnn_ggH")
    roc_dnn_ggH.Write()

    roc_dnn_qqH=makeROC( histos["dnn_qqH_S"],histos["dnn_qqH_B"])
    roc_dnn_qqH.SetName("roc_dnn_qqH")
    roc_dnn_qqH.Write()
