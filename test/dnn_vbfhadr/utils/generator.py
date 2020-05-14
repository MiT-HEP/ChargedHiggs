import threading
import random
import ROOT
import os
import re
import numpy as np

lock=threading.Lock()

def get_tree(fname,tname):
    print "-> called get_tree('"+fname+"','"+tname+"')"
    fIn=ROOT.TFile.Open(fname)
    if fIn==None: 
        print "ERROR File",fname,"does not exist."
        raise RuntimeError()

    t=fIn.Get(tname)
    if t==None: 
        print "ERROR Tree:","'"+tname+"'","does not exist in ",fname, "."
        raise RuntimeError()

    print "Entries=",t.GetEntries(), fIn.GetName()
    return fIn,t

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
def fill(t,batchsize=8000,selection="",targets=["",""],weight="",features=["",""]):
    print "-> Start fill"
    X=[]
    Y=[]
    W=[]

    while True:

        shuffle=None

        for ientry in range(0,t.GetEntries()):
            if shuffle==None: 
                t.GetEntry(ientry)
            else:
                t.GetEntry(shuffle[ientry])

            sel = [ eval(re.sub('@','t.',s)) if '@' in s else eval(re.sub('~','',s)) if '~' in s else eval("t."+s) for s in [selection] ][0]
            ## nominally [1,0] for sig and [0,1] for bkg
            y = [ eval(re.sub('@','t.',s)) if '@' in s else eval(re.sub('~','',s)) if '~' in s else eval("t."+s) for s in targets ]
            ## fill

            #'@' -> 't.', '~' -> "" , else t.name
            x= [ eval(re.sub('@','t.',s)) if '@' in s else eval(re.sub('~','',s)) if '~' in s else eval("t."+s) for s in features ]

            if np.sum(np.array(y)) !=1 : continue


            Y.append(y)
            X.append(x)
            
            w= [ eval(re.sub('@','t.',s)) if '@' in s else eval(re.sub('~','',s)) if '~' in s else eval("t."+s) for s in [weight] ][0]
            W.append(w)
            
            if batchsize>0 and len(Y)>=batchsize: 
                yield np.array(X),np.array(Y)
                X=[]
                Y=[]
                W=[]
        #cycling
        shuffle=range(0,t.GetEntries())
        shuffle=random.shuffle(shuffle)
        print "cycling"

    print "-> End batch"
    return
