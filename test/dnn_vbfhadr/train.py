
#### CONFIGURATION

import json
with open("config.json") as f:
    config=json.load(f)


import numpy as np
np.random.seed(1234567)
import sys
sys.setrecursionlimit(10000)
import ROOT
import re

from utils.timer import ElapsedTimer
from utils.generator import fill, get_tree
from utils.model import BuildModel
from utils.saver import WeightsSaver
from utils.plot import *

### KERAS
from sklearn.metrics import roc_auc_score
import keras
print "keras version=",keras.__version__

for key in config:
    print "-> Training ",key['name']
    timer = ElapsedTimer()
    classifier = BuildModel(key['features'],len(key['targets']))
    classifier.summary()
    #fIn,tree =get_tree('/eos/user/d/dalfonso/VBS_BDT/MARCH9/MET.root','tree_vbs')
    fIn,tree=get_tree(key['file'].encode('ascii'),key['tree'].encode('ascii'))
    history=classifier.fit_generator(
            fill(tree,1000,key['sel'],key['targets'],key['weight'],key['features']),
            steps_per_epoch=10, 
            epochs=10,
            #callbacks=[WeightsSaver()],
            workers=4
            )
    print "trained model",key['name']
    classifier.save('model_'+key['name']+'.hd')
    classifier.summary()
    fIn.Close()
    timer.elapsed_time()
    plot(history, key['name']+"_plots.pdf")
print "-> End training"
