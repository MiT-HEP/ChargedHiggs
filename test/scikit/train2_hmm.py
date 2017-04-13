import numpy as np
import sys
import ROOT

from sklearn.datasets import make_blobs
from sklearn.ensemble import RandomForestClassifier
from sklearn.calibration import CalibratedClassifierCV
from sklearn.svm import SVC
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import log_loss


np.random.seed(0)


###################
## Generate data ##
###################




names=["RFC","SVC","MLP"]
classifiers= [  
        RandomForestClassifier(n_estimators=25), 
        SVC(gamma='auto', C=1,kernel='rbf',decision_function_shape='ovo'),  
        MLPClassifier(alpha=1),
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
       print "* doing mc",idx,":","pass_all && abs(eta1) <0.8 && abs(eta2)< .8 && mc >= %(mcmin)d && mc <= %(mcmax)d"%{"mcmin":mcmin,"mcmax":mcmax}
       t.Draw("mc:Hpt:Heta:Hphi:mjj_1:mjj_2:detajj_1:detajj_2:deltaphi:deltaeta","pass_all && abs(eta1) <0.8 && abs(eta2)< .8 && mc >= %(mcmin)d && mc <= %(mcmax)d "%{"mcmin":mcmin,"mcmax":mcmax},"goff")
       print " I selected",t.GetSelectedRows(), "I need 100"
       for ievt in range(0,100):
             x= [     t.GetVal(1)[ievt],
                      t.GetVal(2)[ievt],
                      t.GetVal(3)[ievt],
                      t.GetVal(4)[ievt],
                      t.GetVal(5)[ievt],
                      t.GetVal(6)[ievt],
                      t.GetVal(7)[ievt],
                      t.GetVal(8)[ievt],
                      t.GetVal(9)[ievt] 
                  ]
             ## fill -- target
             if t.GetVal(0)[ievt]< 0 : y.append(1)
             else : y.append(-1)
             X.append(x)
   print "-> End batch",ibatch
   return X,y

for ibatch in range(2,10):
   X,y= fillBatch(ibatch)
   for name,clf in zip(names,classifiers):
         print "  * ",name
         clf.fit(X,y)
X_valid,y_valid=fillBatch(0)
X_test,y_test= fillBatch(1)
print "-> End training"

print " --- VALID ---"
print X_valid,y_valid
print " --- TESt ---"
print X_test,y_test

for name,clf in zip(names,classifiers):
    sig_clf = CalibratedClassifierCV(clf, method="sigmoid", cv="prefit")
    sig_clf.fit(X_valid, y_valid)
    sig_clf_probs = sig_clf.predict_proba(X_test)
    sig_score = log_loss(y_test, sig_clf_probs)
    print "Classifier",name,"score=",sig_score


from sklearn.externals import joblib
for name,clf in zip(names,classifiers):
    joblib.dump(clf,name+'.pkl') ## load with clf = joblib.load('filename.pkl') 

