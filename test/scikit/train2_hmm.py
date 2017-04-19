import numpy as np
import math
import sys
import ROOT

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

#import tensorflow as tf
#from tensorflow.contrib.learn import *

ROOT.gSystem.Load("MuonResolution/MyRes_cc.so")
res = ROOT.myRes();

np.random.seed(0)

##def exp_decay(global_step):
##    return tf.train.exponential_decay(
##        learning_rate=0.1, global_step=global_step,
##        decay_steps=100, decay_rate=0.001)
##optimizer = tf.train.AdagradOptimizer(learning_rate= exp_decay)

###################
## Generate data ##
###################


#names=["RFC","SVC","MLP"]
names=["SGD"]
classifiers= [  
        #RandomForestClassifier(n_estimators=25), 
        #MLPClassifier(alpha=1),
        #PassiveAggressiveRegressor(),
        #SGDClassifier(),
        SGDRegressor(loss="epsilon_insensitive"),
        #Perceptron(),
        #tf.contrib.learn.SKCompat(DNNClassifier(hidden_units=[10, 20, 10],n_classes=2,  optimizer=optimizer, feature_columns=[tf.contrib.layers.real_valued_column("input",dimension=10)], model_dir="/tmp/amarini/") )
        ]
#names2=["DNN","SVC"]
names2=["SVR","MLP","Ridge","EN"]
unbatch = [
        #DNNClassifier(hidden_units=[10, 20, 10],n_classes=2,  optimizer=optimizer, feature_columns=[tf.contrib.layers.real_valued_column("",dimension=10)], model_dir="/tmp/amarini/") ,
        SVR(gamma='auto', C=1,kernel='rbf'),  
        MLPRegressor(alpha=0.1),
        Ridge(alpha=.5),
        ElasticNet(),
        ]

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
   nbatches=10
   for idx in range(0,2):
       if len(GluGlu) >  ibatch+idx*nbatches:t.Add("../../test/Hmumu/Hmumu_2017_04_11_Sync/ChHiggs_%d.root"%GluGlu[ibatch+idx*nbatches])
       if len(VBF) > ibatch+idx*nbatches:t.Add("../../test/Hmumu/Hmumu_2017_04_11_Sync/ChHiggs_%d.root"%VBF[ibatch+idx*nbatches])
   print "TotEntries=",t.GetEntries()

   for idx,(mcmin,mcmax) in enumerate(mctypes):
       ievt=0
       print "* doing mc",idx,"(",mcmin,",",mcmax,"):","pass_all && abs(eta1) <0.8 && abs(eta2)< .8 && mc >= %(mcmin)d && mc <= %(mcmax)d"%{"mcmin":mcmin,"mcmax":mcmax}
       t.Draw("mc:Hpt:Heta:Hphi:mjj_1:mjj_2:detajj_1:detajj_2:deltaphi:deltaeta:nbjets:pt1:pt2:eta1:eta2:mass","pass_all && abs(eta1) <0.8 && abs(eta2)< .8 && mass >110 && mass <150 && mc >= %(mcmin)d && mc <= %(mcmax)d "%{"mcmin":mcmin,"mcmax":mcmax},"goff")
       print " I selected",t.GetSelectedRows(), "I need 4000"
       for ievt in range(0,min(10000,t.GetSelectedRows())):
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
             w.append(1./dm)
             #print "dm=",dm,"w=",1./dm, "dpOp1=",res.resolution(t.GetVal(11)[ievt],t.GetVal(13)[ievt]),"dpOp2=",(res.resolution(t.GetVal(12)[ievt],t.GetVal(14)[ievt]))
             ## fill -- target
             if t.GetVal(0)[ievt]< 0 : y.append(1)
             else : y.append(-1)
             X.append(x)
   print "-> End batch",ibatch
   return X,y,w

X_un = []
y_un = []
w_un = []

############## Defin input fn for  DNN ###########Y
#i=0
#def input_fn():
#    global i
#    if len(X_un) > i  : 
#        x=tf.constant(np.float32(X_un[i]))
#        y=tf.constant(np.int_([y_un[i]]))
#        i+=1
#        print "-- UNPUT FN Returning key=",i
#        print "x=",x,"|",X_un[i]
#        print "y=",y,"|",y_un[i]
#        print "------------------"
#        return x,y
#    else: 
#        raise ValueError
#################################################

for ibatch in range(2,10):
   X,y,w= fillBatch(ibatch)
   if len(unbatch) >0 :
       X_un.extend(X)
       y_un.extend(y)
       w_un.extend(w)
   X_tmp=[] ## artificially reweighted with w_un*10
   y_tmp=[]
   for x_i,y_i,w_i in zip(X,y,w): 
       for i in range(0, int(w_i*10)): 
           X_tmp.append(x_i)
           y_tmp.append(y_i)
   for name,clf in zip(names,classifiers):
         print "  * ",name
         print "Fitting with:",len(X),len(y),len(w)
         if ibatch==2 :
            if name != "EN":
                clf.fit(X,y,sample_weight=w)
            else:
                clf.fit(X_tmp,y_tmp)
         else:
            if name != "EN":
                clf.partial_fit(X,y,sample_weight=w)
            else:
                clf.partial_fit(X_tmp,y_tmp)

for name,clf in zip(names2,unbatch):
    print "* Fitting (unbatch) ",name
    if name == 'DNN':
        clf.fit(input_fn=input_fn,steps=16000)
    elif name =='MLP' or name=='EN':
        X_tmp=[] ## artificially reweighted with w_un*10
        y_tmp=[]
        for x_i,y_i,w_i in zip(X_un,y_un,w_un): 
            for i in range(0, int(w_i*10)): 
                X_tmp.append(x_i)
                y_tmp.append(y_i)
        clf.fit(X_tmp,y_tmp)
    else:
        clf.fit(X_un,y_un,sample_weight=w_un)

X_valid,y_valid,w_valid=fillBatch(0)
X_test,y_test,w_test= fillBatch(1)
print "-> End training"

print " --- VALID ---"
#print X_valid,y_valid
print " --- TESt ---"
#print X_test,y_test

#for name,clf in zip(names,classifiers) + zip(names2,unbatch):
#    sig_clf = CalibratedClassifierCV(clf, method="sigmoid", cv="prefit")
#    sig_clf.fit(X_valid, y_valid)
#    sig_clf_probs = sig_clf.predict_proba(X_test)
#    sig_score = log_loss(y_test, sig_clf_probs)
#    print "Classifier",name,"score=",sig_score


from sklearn.externals import joblib
for name,clf in zip(names,classifiers)+zip(names2,unbatch):
    joblib.dump(clf,name+'.pkl') ## load with clf = joblib.load('filename.pkl') 

