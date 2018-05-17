#!/usr/bin/env python

from ROOT import TMVA, TFile, TString
from array import array
from subprocess import call
from os.path import isfile

# Setup TMVA
TMVA.Tools.Instance()
TMVA.PyMethodBase.PyInitialize()
reader = TMVA.Reader("Color:!Silent")

# Load data
if not isfile('tmva_reg_example.root'):
    call(['curl', '-O', 'http://root.cern.ch/files/tmva_reg_example.root'])

data = TFile.Open('tmva_reg_example.root')
tree = data.Get('TreeR')

branches = {}
for branch in tree.GetListOfBranches():
    branchName = branch.GetName()
    branches[branchName] = array('f', [-999])
    tree.SetBranchAddress(branchName, branches[branchName])
    if branchName != 'fvalue':
        reader.AddVariable(branchName, branches[branchName])

# Book methods
reader.BookMVA('PyKeras', TString('dataset/weights/TMVAClassification_PyKeras.weights.xml'))

# Print some example regressions
print('Some example regressions:')
for i in range(20):
    tree.GetEntry(i)
    print('True/MVA value: {}/{}'.format(branches['fvalue'][0],reader.EvaluateMVA('PyKeras')))
