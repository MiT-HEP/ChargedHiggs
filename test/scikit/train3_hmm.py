import numpy as np
np.random.seed(1234567)
#np.random.seed(0)
import random
import math
import sys
sys.setrecursionlimit(10000)
import ROOT

#import tensorflow as tf
#from tensorflow.contrib.learn import *

################## KERAS ################
from keras.models import Sequential
from keras.layers import Dense, Activation

features=["Hpt","Heta","Hphi"]
#t.Draw("mc:Hpt:Heta:Hphi:mjj_1:mjj_2:detajj_1:detajj_2:deltaphi:deltaeta:nbjets:pt1:pt2:eta1:eta2:mass:mcWeight",
#"pass_all && mass >110 && mass <150 && mc >= %(mcmin)d && mc <= %(mcmax)d "%{"mcmin":mcmin,"mcmax":mcmax},"goff")
features=[
        ## muons variables
        "Hpt","Heta","Hphi",
        "deltaphi","deltaeta",
        "eta1","eta2",
        ## jets variables
        "ncentjets","njets","htCent",
        ## additional leptons: to expand
        "pass_leptonveto",
        ## vbf variables
        "mjj_1","mjj_2","detajj_1","detajj_2",
        "softNjets1","softHt1","softNjets5","softNjets10",
        "firstQGL","secondQGL","thirdQGL",
        ## bjets variables
        "nbjets","maxDeepB","leadDeepB","maxCSV",
        ## w/z variables
        "mt1","mt2","met",
        ]

print "-> building model"

model = Sequential()
model.add(Dense(60, input_dim=len(features),activation='tanh') )
model.add(Dense(60,activation='relu') )
model.add(Dense(20,activation='tanh'))
model.add(Dense(6,activation='sigmoid'))

print "-> compiling model"
model.compile(
        loss='categorical_crossentropy',
        #optimizer='adam',
        optimizer='sgd',
        metrics=['accuracy'])


###################
## Generate data ##
###################

mctypes= [[-15,-10],##ggH
          [-25,-20],##qqH
          [-55,-30],##ZH,WM
          [-65,-60],##ttH
          [10,19], ## DY
          [20,29], ## TT
        ]

print "FIX VBF in generator"
def generator2():
    files = []
    for n in range(0,100):
        if n in [53,55,59,73,93,94]: continue #exclude running jobs
        files . append("/eos/user/a/amarini/Hmumu/fwk/2018_01_25_ExclusiveCategoriesAndTree/ChHiggs_%d.root"%n)
    for ifile,f in enumerate(files):
        print "-> opening file",ifile+1,"of",len(files),":",f
        fIn=ROOT.TFile.Open(f)
        if fIn == None: print "<*> Error: no such file",f
        tIn=fIn.Get("hmm")
        if tIn == None: print "<*> Error: no such tree","hmm","in file",f
        nentries = tIn.GetEntries()
        print "  * tree has",nentries
        for ientry in range(0,nentries):
            #print "-> Getting ntry",ientry,"of",nentries
            if tIn == None: print "<*> Unexpected error: tIn become None"
            tIn.GetEntry(ientry)
            if not tIn.pass_all : continue
            if tIn.mass<110 or tIn.mass>150 : continue
            if tIn.eventNum %2 ==0 : continue
            y= np.zeros( (1,len(mctypes)))
            x= np.zeros( (1,len(features)))
            w= np.array([1.])
            for idx,mc in enumerate(mctypes):
                if tIn.mc >= mc[0] and tIn.mc<=mc[1]: y[0][idx] =1
            if tIn.mc == 0 and tIn.runNum == 1: y[0][1] =1 ## FIX VBF

            for idx,f in enumerate(features):
                x[0][idx]= eval("tIn."+ f) 
            w[0] = tIn.weight
            #print "   with values","x=",x,"y=",y,"w=",w
            #print "       shapes:",x.shape,y.shape,w.shape
            yield x,y,w
    raise StopIteration

print "-> try generator"
gen = generator2()
## debug , get the first
#print "VALUES=",next(gen)
#print "VALUES=",next(gen)
#print "VALUES=",next(gen)
model.fit_generator(gen,steps_per_epoch=6000,epochs=50)

print "-> End training"
model.save('model.hd')
model.summary()


print "-> End !"
