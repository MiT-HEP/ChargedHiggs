#!/usr/bin/env python

import sys, os
import re

from optparse import OptionParser
usage="%prog file snapshot"
parser=OptionParser()
parser.add_option("-i","--include",default='',help="keep regexp")
parser.add_option("-x","--exclude",default='',help="exclusion regexp")
opts,args=parser.parse_args()

fname = args[0]
snapshotName = args[1]

import ROOT
ROOT.gROOT.SetBatch(True)
fin = ROOT.TFile.Open(fname)
if not fin.IsOpen():
    print >> sys.stderr,"Unable to open " + fname
    sys.exit(1)

for key in fin.GetListOfKeys():
    obj = fin.Get(key.GetName())
    if isinstance(obj,ROOT.RooWorkspace): w= obj
#w= fin.Get("w")
snapshot = w.getSnapshot(snapshotName)
if snapshot == None:
    print >> sys.stderr,"could not find snapshot %s in file %s" % (snapshotName, fname)
    sys.exit(1)

w.loadSnapshot(snapshotName) 

vars=w.allVars()
it= vars.iterator()

values=[]
while True:
    var=it.Next()
    if var == None:
        break
    name=var.GetName()

    #print>>sys.stderr, "DEBUG: name is",name,"exclude",opts.exclude,"RE",re.search(opts.exclude,name)
    #print>>sys.stderr, "DEBUG: name is",name,"include",opts.include,"RE",re.search(opts.include,name)
    if opts.include !='' and not re.search(opts.include,name): continue
    if opts.exclude !='' and re.search(opts.exclude,name): continue

    values.append(var.GetName()+"="+str(var.getVal()))

print ','.join(values)
