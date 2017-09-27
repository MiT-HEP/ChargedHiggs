import numpy as np
np.random.seed(1234567)
print "numpy version=",np.__version__
import random
import math
import sys
sys.setrecursionlimit(10000)
import ROOT

################## KERAS ################

import keras
print "keras version=",keras.__version__

from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout, Flatten
from keras.layers import Convolution2D, MaxPooling2D
from keras.wrappers.scikit_learn import KerasRegressor
from keras.utils import np_utils

from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline


import keras.backend as K
def s_over_sqrt_sb(y_true, y_pred, weights=[1],dim=1):
    ''' return max( s/s+b) ''' 
    ## -1,0 -> bkg 
    ## 1  -> sig
    ## 2  -> sig ..
    l = zip(y_pred,y_true)
    sort(l, reverse=True)
    s=[0.0 for x in range(0,dim) ]
    sob=[0.0 for x in range(0,dim) ]
    A=0.0
    loss=0.0
    for p,t in l:
        A += 1.
        s[t-1] +=1.
        sob[t-1] = max(sob[t-1],pow(s[t-1],2)/A)
    # sum them all
    S=0.0
    for k in sob: S+= k
    return S

def build_model():
    model = Sequential()
    #model.add(Convolution2D(32, 3, 3, activation='relu', input_shape=(1,28,28)))
    #model.add(MaxPooling2D(pool_size=(2,2)))
    model.add(Dense(40, input_dim=17,activation='tanh') )
    model.add(Dropout(0.25))
    model.add(Dense(25,activation='tanh'))
    model.add(Dense(1,activation='sigmoid'))
    
    model.compile(
            #loss='mean_squared_error',
            loss='binary_crossentropy',
            #optimizer='sgd',
            optimizer='adam',
            metrics=['accuracy'])
    return model


################# ROOT ###############
ROOT.gSystem.Load("MuonResolution/MyRes_cc.so")
res = ROOT.myRes();


###################
## Generate data ##
###################


names=["KERAS"]
#classifier = KerasRegressor(build_fn=build_model, verbose=1)
classifier = build_model()


### batch loop
mctypes= [[-15,-10],##ggH
          [-25,-20],##qqH
          [10,19], ## DY
          [20,29], ## TT
        ]


def fill():
   debugSig=0
   debugBkg=0
   print "-> Start fill"
   X=[]
   y=[]
   w=[]
   t=ROOT.TChain("hmm")

   for idx in range(0,100):
       #t.Add("/eos/user/k/klute/Nero/2017_09_14_HmmTree/ChHiggs_%d.root"%idx)
       t.Add("/eos/user/k/klute/Nero/2017_09_18_HmmTreeMoreVars_v4/ChHiggs_%d.root"%idx)

   print "TotEntries=",t.GetEntries()

   for ientry in range(0,t.GetEntries()):
        t.GetEntry(ientry)

        ## selection
        if t.mass <110 or t.mass >150 : continue
        if not t.pass_all : continue
        dpOp = (res.resolution(t.pt1,t.eta1)) **2 + (res.resolution(t.pt2,t.eta2)**2)
        dmOm = math.sqrt(dpOp)
        dm = dmOm * t.mass

        ### define weight
        #w.append(1./dm * t.mcWeight * t.puWeight)
        w.append(1./dm )

        ptj1 = 0.
        if t.njets >0 : ptj1=t.jet_pt[0]

        ## fill
        x= [          t.pt1/t.mass, #0
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
                      ptj1, ##jpt         #15
                      t.met ##met         #16
                  ]
        #res (pt,eta)
        ## fill -- target
        if t.mc< 0 : y.append(1)
        else : y.append(-1)
        X.append(x)

        if (debugSig <3 and t.mc<0) or (debugBkg <3 and t.mc>0) :
            print "---- DEBUG",ientry,"----------"
            print " NO XSEC WEIGHT NO XSEC WEIGHT NO XSEC WEIGHT"
            print "X=",X
            print "y=",y
            print "w=",w, "mcW",t.mcWeight,"pu",t.puWeight,"1./dm",1./dm,"dm=",dm
            print "------------------------------"
            if t.mc <0 : debugSig +=1
            if t.mc >0 : debugBkg +=1
   print "-> End batch"
   return np.array(X),np.array(y),np.array(w)


X,y,w= fill()

classifier.fit(X,y,sample_weight=w)

print "-> End training"

# Store model to file
classifier.save('model.hd')
classifier.summary()
##keras.models.load_model(filepath)


from sklearn.externals import joblib
joblib.dump(classifier,'KERAS.pkl') 

print "-> End !"
