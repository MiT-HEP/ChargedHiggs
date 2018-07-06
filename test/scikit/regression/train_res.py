import numpy as np
import time

import matplotlib.pyplot as plt

# using to understand how to implement GAN
# https://github.com/roatienza/Deep-Learning-Experiments/blob/master/Experiments/Tensorflow/GAN/dcgan_mnist.py
# https://deeplearning4j.org/generative-adversarial-network

## probably multiply merge and add is the right way
#https://statcompute.wordpress.com/2017/01/08/an-example-of-merge-layer-in-keras/
# this is the resolution model
# https://statcompute.wordpress.com/2017/01/08/an-example-of-merge-layer-in-keras/

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


from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten, Reshape
from keras.layers import Conv2D, Conv2DTranspose, UpSampling2D
from keras.layers import LeakyReLU, Dropout
from keras.layers import BatchNormalization
from keras.layers import multiply
from keras.layers import Input
from keras.optimizers import Adam, RMSprop

from keras import backend as K

from utils import ElapsedTimer
timer = ElapsedTimer()

#def neg_categorical_crossentropy(y_true,y_pred):
#    ''' 
#    neg of https://github.com/keras-team/keras/blob/master/keras/losses.py
#    '''

from keras.losses import *
def negate(y_true,y_pred):
    return -categorical_crossentropy(y_true, y_pred)


def build_sigma():
    branch1 = Sequential()  ## input = reco_variables
    branch1.add(Dense(40, input_dim=4,activation='tanh') )
    branch1.add(Dropout(0.25))
    branch1.add(Dense(25,activation='tanh'))
    branch1.add(Dense(1,activation='tanh')) # [pt, eta, phi,npv]
    return branch1

def build_model1(b1):
    branch1=b1
    branch2=Input(input_dim=5) # pt, eta, phi, ptr/ptg, rnd

    model1 = Sequential()  ## input =  [ [ pt,eta,phi,npv ], [pt, eta ,phi,ptr/ptg,rnd] ]
    model1.add(Merge([branch1,branch2],mode='concat'))
    model1.add(Lambda(lambda x: ( x[1]*(1.+x[5]*x[0]*x[4]),x[2],x[3] ))) ##input = [ sigma, pt, eta ,phi, ptr/ptg,rnd, -> output = [pt', eta,phi,]
    
    return model1

def model_mc(): 
    ''' Smear the two muons and return the invariant mass'''
    smearer=build_sigma()
    muon1=build_model1(smearer)
    muon2=build_model1(smearer)
    model = Squential()
    model.add(Merge( [muon1,muon2],mode='concat')) # input [ [pt,eta,phi], [pt,eta,phi] ]
    #muonMass=0.105
    # massless approx
    model.add(Lambda(lambda x: (math.sqrt((2.*x[0]*x[3])*(math.cosh(x[1]-x[4])-math.cos(x[2]-x[5])))) ))

def model_data(): 
    muon1=Input(input_dim=3)
    muon2=Input(input_dim=3)
    model = Squential()
    model.add(Merge( [muon1,muon2],mode='concat')) # input [ [pt,eta,phi], [pt,eta,phi]
    #muonMass=0.105
    # massless approx
    model.add(Lambda(lambda x: (math.sqrt((2.*x[0]*x[3])*(math.cosh(x[1]-x[4])-math.cos(x[2]-x[5])))) ))

    return model

def discriminator():
    model=Sequential()
    model.add(input ?+???
    model.compile(loss=negate, 
            optimizer='sgd',metrics['accuracy'])

## TODO construct model to discriminate between data and MC

def data_generator():
   X=[]
   y=[]
   t=ROOT.TChain("nero/events")

   for idx in range(0,100):
       t.Add("root://eoscms///store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/Charmonium/Charmonium-Run2017B/180525_094034/0000/NeroNtuples_%d.root"%idx)

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
        x= [           t.lepP4[0].Pt(), ## 1 
                       t.lepP4[0].Eta(),## 2
                       t.lepP4[0].Phi(),## 3
                       t.npv,           ## 4
                       t.rho,           ## 5
                  ]
        sf1 = model.predict( np.array([x]) )[0]
        x= [           t.lepP4[1].Pt(), ## 1 
                       t.lepP4[1].Eta(),## 2
                       t.lepP4[1].Phi(),## 3
                       t.npv,           ## 4
                       t.rho,           ## 5
                  ]
        sf2 = model.predict( np.array([x]) )[0]
        
        mass = ROOT.TLorentzVector()
        mass += (t.lepP4[muons[0]]*(1.+sf1))
        mass += (t.lepP4[muons[1]]*(1.+sf2))

        

def mc_generator():
    yield 


model=keras.models.load_model('scale_data.hd')
model.summary()
