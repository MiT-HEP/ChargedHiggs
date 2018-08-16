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

################## KERAS ################

import keras
print "keras version=",keras.__version__

from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout, Flatten
from keras.layers import Convolution2D, MaxPooling2D
import keras.regularizers as regularizers
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

def build_model():
    model = Sequential()
    #model.add(Convolution2D(32, 3, 3, activation='relu', input_shape=(1,28,28)))
    #model.add(MaxPooling2D(pool_size=(2,2)))
    model.add(Dense(40, input_dim=len(features),activation='relu',use_bias=True,kernel_initializer='glorot_normal',bias_initializer='zeros',kernel_regularizer=regularizers.l2(0.01),bias_regularizer=regularizers.l2(0.01)) )
    model.add(Dropout(0.25))
    model.add(Dense(25,activation='tanh',use_bias=True,kernel_regularizer=regularizers.l2(0.01),kernel_initializer='glorot_normal',bias_initializer='zeros'))
    model.add(Dense(10,activation='relu',use_bias=True,kernel_initializer='glorot_normal',bias_initializer='zeros'))
    model.add(Dense(1,activation='sigmoid'))
    
    model.compile(
            #loss='mean_squared_error',
            loss='binary_crossentropy',
            #optimizer='sgd',
            optimizer='adam',
            metrics=['accuracy'])
    return model


################# ROOT ###############
#ROOT.gSystem.Load("MuonResolution/MyRes_cc.so")
#res = ROOT.myRes();


###################
## Generate data ##
###################


classifier = build_model()


### batch loop
mctypes= [[-15,-10],##ggH
          [-25,-20],##qqH
          [10,19], ## DY
          [20,29], ## TT
        ]

import threading
lock=threading.Lock()
#def threadsafe(f):
#    def wrapper(*a,**kw):
#        with lock:
#            return f(*a,**kw)
#    return wrapper

class threadsafe_iter:
    """Takes an iterator/generator and makes it thread-safe by
    serializing call to the `next` method of given iterator/generator.
    """
    def __init__(self, it):
        self.it = it
        self.lock = threading.Lock()

    def __iter__(self):
        return self

    def next(self):
        with self.lock:
            return self.it.next()

def threadsafe_generator(f):
    """A decorator that takes a generator function and makes it thread-safe.
    """
    def g(*a, **kw):
        return threadsafe_iter(f(*a, **kw))
    return g

@threadsafe_generator
def fill(batchsize=8000):
    print "-> Start fill"
    X=[]
    Y=[]
    W=[]

    #t=ROOT.TChain("hmm")
    #for idx in range(0,300):
    #    #t.Add("/eos/user/k/klute/Nero/2017_09_18_HmmTreeMoreVars_v4/ChHiggs_%d.root"%idx)
    #    if idx==1 or idx==56: continue
    #    if idx>10: break
    #    t.Add("/eos/cms/store/user/amarini/Hmumu/fwk/2018_08_10_SyncTree//ChHiggs_%d.root"%idx)
    # /eos/cms/store/user/amarini/Hmumu/fwk/2018_08_15_SyncTreeHH
    files=["root://eoscms///store/user/amarini/Hmumu/fwk/2018_08_10_SyncTree//ChHiggs_%d.root"%idx for idx in range(0,300) if idx !=1 and idx !=56]
    files.extend(["root://eoscms///store/user/amarini/Hmumu/fwk/2018_08_15_SyncTreeHH/ChHiggs_%d.root"%idx for idx in range(0,10)])
    random.shuffle(files) 
    while True:
        for idx in range(0,len(files)):
            #if idx==1 or idx==56: continue
            f=ROOT.TFile.Open(files[idx])

            t=f.Get("hmm")
            if t==None: print "ERROR File",f.GetName(),"does not exist"
            print "Entries in file",idx,"Entries=",t.GetEntries(), f.GetName()
            for ientry in range(0,t.GetEntries()):
                 t.GetEntry(ientry)

                 if t.eventNum %2 ==0: continue
                 ## selection
                 if t.mass <110 or t.mass >150 : continue
                 if not (t.pass_recomuons and t.pass_asymmcuts and t.pass_trigger)  : continue
                 #dpOp = (res.resolution(t.pt1,t.eta1)) **2 + (res.resolution(t.pt2,t.eta2)**2)
                 #dmOm = math.sqrt(dpOp)
                 #dm = dmOm * t.mass

                 ### define weight
                 #w.append(1./dm * t.mcWeight * t.puWeight)
                 #w.append(1./dm )

                 ptj1 = 0.
                 if t.njets >0 : ptj1=t.jet_pt[0]

                 ## fill

                 #'@' -> 't.', '~'
                 x= [ eval(re.sub('@','t.',s)) if '@' in s else eval(re.sub('~','',s)) if '~' in s else eval("t."+s) for s in features ]
                 ## fill -- target

                 if t.mc< 0 : y=1
                 else : y=0

                 Y.append(y)
                 X.append(x)
                 
                 xsec=1
                 if t.mc <0 :  xsec*=2.176E-4
                 if t.mc <-10 and t.mc >-15 : xsec*=48.58
                 if t.mc <-20 and t.mc >-25 : xsec*=3.78
                 if t.mc <-25 and t.mc >-30 : xsec*=3.78
                 w= t.mcWeight * xsec
                 W.append(w )
                 
                 #yield np.array([x]),np.array([y]),np.array([w])

                 if batchsize>0 and len(Y)>=batchsize: 
                     yield np.array(X),np.array(Y),np.array(W)
                     X=[]
                     Y=[]
                     W=[]
        #cycling
        print "cycling"

    print "-> End batch"
    return


timer = ElapsedTimer()

#for ibatch, (X,y,w) in enumerate(fill(10000)):
#    print "---- DEBUG",ibatch,"----------"
#    print "X=",X
#    print "y=",y
#    print "w=",w
#    print "------------------------------"
#    if ibatch ==0:
#        classifier.fit(X,y,sample_weight=w)
#    classifier.train_on_batch(X,y,sample_weight=w)

from keras.callbacks import Callback

class WeightsSaver(Callback):
    def __init__(self):
        self.N = 0

    def on_batch_end(self, batch, logs={}):
        # batch restart from 0 at each epoch
        if batch == 0:
            print "Saving benchmark",self.N
            name = '/tmp/amarini/weights%08d.h5' % self.N
            self.model.save_weights(name)
            self.N +=1


classifier.fit_generator(fill(8000),steps_per_epoch=100, epochs=10,callbacks=[WeightsSaver()],workers=4)

print "-> End training"

# Store model to file
classifier.save('model.hd')
classifier.summary()

timer.elapsed_time()

print "-> End !"
