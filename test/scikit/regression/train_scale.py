from optparse import OptionParser
parser=OptionParser()
parser.add_option("","--mc",action='store_true',default=False,help="do MC")
parser.add_option("","--rocco",action='store_true',default=False,help="Use Rochester Corrections")
opts,args=parser.parse_args()
doMC=opts.mc


import numpy as np
np.random.seed(1234567)
print "numpy version=",np.__version__
import random
import math
import sys,os
import time
sys.setrecursionlimit(10000)
import ROOT
#import uproot

if opts.rocco:
    ROOT.gSystem.Load(os.environ['CMSSW_BASE']+"/src/ChargedHiggs/bin/libChargedHiggs.0.so")
    rocco = ROOT.RoccoR("aux/rochester/RoccoR2017v0.txt")

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

timer = ElapsedTimer()
################## KERAS ################

import keras
print "keras version=",keras.__version__

from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout, Flatten
from keras.layers import Convolution2D, MaxPooling2D
from keras.wrappers.scikit_learn import KerasRegressor
from keras.utils import np_utils

def build_model():
    model = Sequential()
    #model.add(Convolution2D(32, 3, 3, activation='relu', input_shape=(1,28,28)))
    #model.add(MaxPooling2D(pool_size=(2,2)))
    model.add(Dense(40, input_dim=5,activation='tanh') )
    model.add(Dropout(0.25))
    model.add(Dense(25,activation='tanh'))
    model.add(Dense(1,activation='tanh'))
    
    model.compile(
            loss='mean_squared_error',
            #loss='binary_crossentropy',
            optimizer='sgd',
            #optimizer='adam',
            metrics=['accuracy'])
    return model

###################
## Generate data ##
###################

classifier = build_model()

def fill(n=-1,model=None,modelEntry=9999):
   print "-> Start fill",n
   if model != None: print "-> Using model after entry",modelEntry

   X=[]
   y=[]
   t=ROOT.TChain("nero/events")

   if not doMC:
       for idx in range(0,100):t.Add("root://eoscms///store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/Charmonium/Charmonium-Run2017B/180525_094034/0000/NeroNtuples_%d.root"%idx)
        ## Add Z, for high pT

       for idx in range(0,100):t.Add("root://eoscms///store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/SingleMuon/SingleMuon-Run2017B/180420_115326/0000/NeroNtuples_%d.root"%idx)
        
   if doMC: 
       #Z MC
       for idx in range(0,100): t.Add("root://eoscms///eos/cms/store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/DYJetsToLL/180528_112630/0000/NeroNtuples_%d.root"%idx)

   print "TotEntries=",t.GetEntries()

   for ientry in range(0,t.GetEntries()):
        t.GetEntry(ientry)

        ## selection
        muons=[]

        for ilep in range(0,t.lepP4.GetSize()):
            ## any selection
            if abs(t.lepPdgId[ilep])==13:muons.append(ilep)
            if len(muons) > 2: break

        if len(muons)<2: continue
        if t.lepPdgId[muons[0]] * t.lepPdgId[muons[1]]> 0: continue ## same sign
        
        mass = ROOT.TLorentzVector()
        mass += t.lepP4[muons[0]]
        mass += t.lepP4[muons[1]]

        ## fill
        for i,m in enumerate(muons):
            o=muons[0] if i==1 else muons[1]

            correctionOther =1.

            if model!=None and ientry > modelEntry :
                x= [       t.lepP4[o].Pt(), ## 1 
                           t.lepP4[o].Eta(),## 2
                           t.lepP4[o].Phi(),## 3
                           t.npv,           ## 4
                           t.rho,           ## 5
                      ]
                correctionOther = 1. + model.predict(np.array([x]))[0]

            x= [       t.lepP4[m].Pt(), ## 1 
                       t.lepP4[m].Eta(),## 2
                       t.lepP4[m].Phi(),## 3
                       t.npv,           ## 4
                       t.rho,           ## 5
                  ]
            if opts.rocco :
               if doMC:
                   # find gen level mc
                   mcIdx_m=-1
                   mcIdx_o=-1
                   for i in range(0,t.genP4.GetSize()):
                       if not (t.genFlags[i] & 1 ) : continue
                       if not abs(t.genPdgId[i]) == 13: continue
                       if not t.lepP4[m].DeltaR(t.genP4[i])<0.1: mcIdx_m=i
                       if not t.lepP4[o].DeltaR(t.genP4[i])<0.1: mcIdx_o=i
                       if mcIdx_m >=0 and mcIdx_o >=0 : break
                   mcSF_m =1
                   mcSF_o =1
                   u1=ROOT.gRandom.Rndm()
                   u2=ROOT.gRandom.Rndm()
                   nl = 15
                   if mcIdx_m >0 :
                        mcSF_m=rocco->kScaleFromGenMC(lep->Charge(), lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta(),lep->GetP4Dirty().Phi(), nl, gen[mcIdx]->Pt(), u1, s, m);
               else:
                   print "DEBUG: Pt was",t.lepP4[m].Pt(),
                   ch=-1 if t.lepPdgId[m] >0 else 1
                   dataSFm= rocco.kScaleDT(ch,t.lepP4[m].Pt(),t.lepP4[m].Eta(),t.lepP4[m].Phi(),0,0)
                   print "DEBUG: Pt was",t.lepP4[m].Pt(),
                   t.lepP4[m]*=dataSFm
                   print " -> ",t.lepP4[m].Pt()

                   ch=-1 if t.lepPdgId[o] >0 else 1
                   dataSFo= rocco.kScaleDT(ch,t.lepP4[o].Pt(),t.lepP4[o].Eta(),t.lepP4[o].Phi(),0,0)
                   t.lepP4[o]*=dataSFo

            targetMass=-1
            if abs(mass.M()-3.09)<0.3: targetMass = 3.09 ## j/psi
            if abs(mass.M()-3.686)<0.3: targetMass = 3.686 ## psi'
            #if abs(mass.M()-9.460)<0.2: targetMass = 10.02 ## upsilon 1S
            #if abs(mass.M()-10.02)<0.1: targetMass = 10.02 ## upsilon 2S
            #if abs(mass.M()-10.355)<0.1: targetMass = 10.355 ## upsilon 3S
            if abs(mass.M()-91.1876)<20: targetMass = 91.1876 ## Z


            px1=t.lepP4[m].Px()
            px2=t.lepP4[o].Px() * correctionOther
            py1=t.lepP4[m].Py()
            py2=t.lepP4[o].Py() * correctionOther
            pz1=t.lepP4[m].Pz()
            pz2=t.lepP4[o].Pz() * correctionOther
            E1=t.lepP4[m].E()
            E2=t.lepP4[o].E() * correctionOther
            mmu = 0.1056
            A = E1*E2 - px1*px2 -py1*py2 -pz1*pz2
            sf=(2*A-math.sqrt(4*A**2 + 4*(mmu**2)*(targetMass**2-mmu**2))) /(-2*mmu**2)

            if sf <0.95 or sf >1.05: continue

            X.append(x)
            y.append(sf-1) ## easier to model with tanh

        ## inside the loop
        if n>0 and len(y)>=n: 
            yield np.array(X),np.array(y)
            X=[]
            y=[]
   yield np.array(X),np.array(y)
   print "-> End batch"
   return


#X,y= fill()
#classifier.fit(X,y)
## TODO: Use fit_generator
for ibatch, (X,y) in enumerate(fill(10000,classifier)):
    print "---- DEBUG",ibatch,"----------"
    print "X=",X
    print "y=",y
    print "------------------------------"
    if ibatch ==0:
        classifier.fit(X,y)
    classifier.train_on_batch(X,y)

print "-> End training"

# Store model to file
outname='scale_mc.hd' if doMC else 'scale_data.hd'
classifier.save(outname)
classifier.summary()
##keras.models.load_model(filepath)
timer.elapsed_time()


#from sklearn.externals import joblib
#joblib.dump(classifier,'KERAS.pkl') 

print "-> End !"
