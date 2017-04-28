import numpy as np
import random
import math
import sys
sys.setrecursionlimit(10000)
import ROOT

############### SCIKIT LEARN ###############
from sklearn.datasets import make_blobs
from sklearn.ensemble import RandomForestClassifier
from sklearn.calibration import CalibratedClassifierCV
from sklearn.svm import SVC
from sklearn.svm import SVR
from sklearn.neural_network import MLPClassifier
from sklearn.neural_network import MLPRegressor
from sklearn.linear_model import SGDClassifier
from sklearn.linear_model import SGDRegressor
from sklearn.linear_model import PassiveAggressiveClassifier
from sklearn.linear_model import PassiveAggressiveRegressor
from sklearn.linear_model import Perceptron
from sklearn.linear_model import Ridge
from sklearn.linear_model import ElasticNet
from sklearn.metrics import log_loss
from sklearn.ensemble import GradientBoostingRegressor

#import tensorflow as tf
#from tensorflow.contrib.learn import *

################## KERAS ################
from keras.models import Sequential
from keras.layers import Dense, Activation
from keras.wrappers.scikit_learn import KerasRegressor
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline

def build_model():
    model = Sequential()
    model.add(Dense(10, input_dim=10,activation='softmax') )
    model.add(Dense(20,activation='tanh'))
    model.add(Dense(1,activation='tanh'))
    
    model.compile(loss='mean_squared_error',
            optimizer='sgd',
            metrics=['accuracy'])
    return model


################# ROOT ###############
ROOT.gSystem.Load("MuonResolution/MyRes_cc.so")
res = ROOT.myRes();

np.random.seed(0)

###################
## Generate data ##
###################


names=["SGD","BDTG","KERAS","MLP","SVR"]
classifiers= [  
        SGDRegressor(loss="epsilon_insensitive"),
        GradientBoostingRegressor(n_estimators=100, learning_rate=0.02, max_depth=4, random_state=0),
        KerasRegressor(build_fn=build_model, verbose=1),
        MLPRegressor(alpha=0.1),
        SVR(gamma='auto', C=1,kernel='rbf'),  
        ]

unbatch=[]
names2=[]
#names2=["SVR"]
#unbatch = [
#        SVR(gamma='auto', C=1,kernel='rbf'),  
#        ]

### batch loop
mctypes= [[-15,-10],##ggH
          [-25,-20],##qqH
          [10,19], ## DY
          [20,29], ## TT
        ]
ientry= [0,0,0,0] ## one for each type


def fillBatch(ibatch):
   print "-> Start batch",ibatch
   X=[]
   y=[]
   w=[]
   t=ROOT.TChain("hmm")
   GluGlu=[18,19,20,21,22,23,31,32,33,34,35,36,37,38,39,40,54,55,56,57,58,59]
   VBF=[24,25,41,42,43,44,45,46,47,48,49,50,51,52,53,60,61]
   nbatches=1
   All = range(0,100)
   random.shuffle(All)

   for idx in range(0,100):
       t.Add("../../test/Hmumu/Hmumu_2017_04_26_Sync/ChHiggs_%d.root"%All[ibatch+idx*nbatches])

   print "TotEntries=",t.GetEntries()

   for idx,(mcmin,mcmax) in enumerate(mctypes):
       ievt=0
       #print "* doing mc",idx,"(",mcmin,",",mcmax,"):","pass_all && abs(eta1) <0.8 && abs(eta2)< .8 && mc >= %(mcmin)d && mc <= %(mcmax)d"%{"mcmin":mcmin,"mcmax":mcmax}
       #       0   1   2    3    4    5       6       7       8        9        10     11  12   13  14  15    16
       t.Draw("mc:Hpt:Heta:Hphi:mjj_1:mjj_2:detajj_1:detajj_2:deltaphi:deltaeta:nbjets:pt1:pt2:eta1:eta2:mass:mcWeight","pass_all && mass >110 && mass <150 && mc >= %(mcmin)d && mc <= %(mcmax)d "%{"mcmin":mcmin,"mcmax":mcmax},"goff")
       print " I selected",t.GetSelectedRows()
       #for ievt in range(0,min(4000,t.GetSelectedRows())):
       for ievt in range(0,t.GetSelectedRows()):
             x= [     t.GetVal(1)[ievt],
                      t.GetVal(2)[ievt],
                      t.GetVal(3)[ievt],
                      t.GetVal(4)[ievt],
                      t.GetVal(5)[ievt],
                      t.GetVal(6)[ievt],
                      t.GetVal(7)[ievt],
                      t.GetVal(8)[ievt],
                      t.GetVal(9)[ievt],
                      t.GetVal(10)[ievt] 
                  ]
             dpOp = (res.resolution(t.GetVal(11)[ievt],t.GetVal(13)[ievt])) **2 + (res.resolution(t.GetVal(12)[ievt],t.GetVal(14)[ievt])**2)
             dmOm = math.sqrt(dpOp)
             dm = dmOm * t.GetVal(15)[ievt]
             w.append(1./dm * t.GetVal(16)[ievt])
             #print "dm=",dm,"w=",1./dm, "dpOp1=",res.resolution(t.GetVal(11)[ievt],t.GetVal(13)[ievt]),"dpOp2=",(res.resolution(t.GetVal(12)[ievt],t.GetVal(14)[ievt]))
             ## fill -- target
             if t.GetVal(0)[ievt]< 0 : y.append(1)
             else : y.append(-1)
             X.append(x)
   print "-> End batch",ibatch
   return np.array(X),np.array(y),np.array(w)

#X_un = []
#y_un = []
#w_un = []

for ibatch in range(0,1):
   X,y,w= fillBatch(ibatch)
   print "Sample is:"
   print "----"
   print X
   print "----"
   print y
   print "----"
   print w
   print "===="
   #if len(unbatch) >0 :
   #    X_un.extend(X)
   #    y_un.extend(y)
   #    w_un.extend(w)
   #X_tmp=[] ## artificially reweighted with w_un*10
   #y_tmp=[]
   #for x_i,y_i,w_i in zip(X,y,w): 
   #    for i in range(0, int(w_i*10)): 
   #        X_tmp.append(x_i)
   #        y_tmp.append(y_i)
   for name,clf in zip(names,classifiers):
         print "  * ",name
         print "Fitting with:",len(X),len(y),len(w)
         try:
            if ibatch==0 :
                if name=='MLP':
                   clf.fit(X,y)
                else:
                   clf.fit(X,y,sample_weight=w)
            else:
                   clf.partial_fit(X,y,sample_weight=w)
         except ZeroDivisionError as e:
             print "Encountered ZeroDivision error. Trying to ignore batch",ibatch,name
             pass

## for name,clf in zip(names2,unbatch):
##     print "* Fitting (unbatch) ",name
##     if name =='MLP':
##         X_tmp=[] ## artificially reweighted with w_un*10
##         y_tmp=[]
##         for x_i,y_i,w_i in zip(X_un,y_un,w_un): 
##             for i in range(0, int(w_i*10)): 
##                 X_tmp.append(x_i)
##                 y_tmp.append(y_i)
##         clf.fit(X_tmp,y_tmp)
##     else:
##         clf.fit(X_un,y_un,sample_weight=w_un)

print "-> End training"


from sklearn.externals import joblib
for name,clf in zip(names,classifiers)+zip(names2,unbatch):
    joblib.dump(clf,name+'.pkl') ## load with clf = joblib.load('filename.pkl') 

print "-> End !"
