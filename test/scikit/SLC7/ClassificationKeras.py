#!/usr/bin/env python

from ROOT import TMVA, TFile, TTree, TCut
from subprocess import call
from os.path import isfile

from keras.models import Sequential
from keras.layers.core import Dense, Activation
from keras.regularizers import l2
from keras import initializations
from keras.optimizers import SGD

# Setup TMVA
TMVA.Tools.Instance()
TMVA.PyMethodBase.PyInitialize()

output = TFile.Open('TMVA.root', 'RECREATE')
factory = TMVA.Factory('TMVAClassification', output,
        '!V:!Silent:Color:DrawProgressBar:Transformations=D,G:AnalysisType=Classification')

# Load data
if not isfile('tmva_class_example.root'):
    call(['curl', '-O', 'http://root.cern.ch/files/tmva_class_example.root'])

data = TFile.Open('tmva_class_example.root')
signal = data.Get('TreeS')
background = data.Get('TreeB')

dataloader = TMVA.DataLoader('dataset')
for branch in signal.GetListOfBranches():
    dataloader.AddVariable(branch.GetName())

dataloader.AddSignalTree(signal, 1.0)
dataloader.AddBackgroundTree(background, 1.0)
dataloader.PrepareTrainingAndTestTree(TCut(''),
        'nTrain_Signal=4000:nTrain_Background=4000:SplitMode=Random:NormMode=NumEvents:!V')

# Generate model

# Define initialization
def normal(shape, name=None):
    return initializations.normal(shape, scale=0.05, name=name)

# Define model
model = Sequential()
model.add(Dense(64, init=normal, activation='relu', W_regularizer=l2(1e-5), input_dim=4))
#model.add(Dense(32, init=normal, activation='relu', W_regularizer=l2(1e-5)))
model.add(Dense(2, init=normal, activation='softmax'))

# Set loss and optimizer
model.compile(loss='categorical_crossentropy', optimizer=SGD(lr=0.01), metrics=['accuracy',])

# Store model to file
model.save('model.h5')
model.summary()

# Book methods
factory.BookMethod(dataloader, TMVA.Types.kFisher, 'Fisher',
        '!H:!V:Fisher:VarTransform=D,G')
factory.BookMethod(dataloader, TMVA.Types.kPyKeras, 'PyKeras',
        'H:!V:VarTransform=D,G:FilenameModel=model.h5:NumEpochs=20:BatchSize=32')

# Run training, test and evaluation
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()
