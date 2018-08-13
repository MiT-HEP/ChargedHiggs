import numpy as np
import keras

import sys
sys.setrecursionlimit(10000)

import matplotlib.pyplot as plt

from utils import ElapsedTimer
timer=ElapsedTimer()

doMC=False

import ROOT

t=ROOT.TChain("nero/events")
if doMC:
    for idx in range(0,100): t.Add("root://eoscms///eos/cms/store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/DYJetsToLL/180528_112630/0000/NeroNtuples_%d.root"%idx)
else:
    for idx in range(0,100):t.Add("root://eoscms///store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/SingleMuon/SingleMuon-Run2017B/180420_115326/0000/NeroNtuples_%d.root"%idx)
    #for idx in range(0,100):t.Add("root://eoscms///store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/Charmonium/Charmonium-Run2017B/180525_094034/0000/NeroNtuples_%d.root"%idx)

X=[]
y=[]

model=keras.models.load_model('scale_data.hd')
model.summary()

maxEntries=100000

pt_eta = ROOT.TProfile2D("prof","prof %s;p_{T}^{#muon};#eta^{#muon};Residual"%("MC" if doMC else "data"),100,0,200,100,-2.5,2.5)
eta_phi = ROOT.TProfile2D("prof","prof %s;#eta^{#muon};#phi^{#muon};Residual"%("MC" if doMC else "data"),100,-2.5,2.5,100,-3.1415,3.1415)

for ientry in range(0,min(maxEntries,t.GetEntries())):
    t.GetEntry(ientry)
    muons=[]

    for ilep in range(0,t.lepP4.GetSize()):
        ## any selection
        if abs(t.lepPdgId[ilep])==13:muons.append(ilep)
        if len(muons) > 2: break

    if len(muons)<2: continue
    if t.lepPdgId[muons[0]] * t.lepPdgId[muons[1]]> 0: continue ## same sign

    for m in muons:
        x= [           t.lepP4[m].Pt(), ## 1 
                       t.lepP4[m].Eta(),## 2
                       t.lepP4[m].Phi(),## 3
                       t.npv,           ## 4
                       t.rho,           ## 5
                  ]
        sf = model.predict( np.array([x]) )[0]
        pt_eta . Fill(t.lepP4[m].Pt(),t.lepP4[m].Eta(),sf)
        eta_phi . Fill(t.lepP4[m].Eta(),t.lepP4[m].Phi(),sf)

def prepareCanvas(c):
    c.SetTopMargin(0.10)
    c.SetRightMargin(0.10)
    c.SetBottomMargin(0.15)
    c.SetLeftMargin(0.15)
    return

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPalette(ROOT.kTemperatureMap)

c1 = ROOT.TCanvas("c1","c",800,800)
prepareCanvas(c1)
pt_eta.Draw("COL Z")
pt_eta.GetZaxis().SetRangeUser(-0.1,0.1)

c2 = ROOT.TCanvas("c2","c",800,800)
prepareCanvas(c2)
eta_phi.Draw("COL Z")
eta_phi.GetZaxis().SetRangeUser(-0.1,0.1)

timer.elapsed_time()
raw_input("ok?")

