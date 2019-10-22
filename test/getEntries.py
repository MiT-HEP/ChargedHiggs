import ROOT
import os,sys
from subprocess import call,check_output

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import *


#EOS='/usr/bin/eos'
print "->looking in path",sys.argv[1]
list =  FindEOS(sys.argv[1])

counts=[0,0]
for f in list:
    fIn=ROOT.TFile.Open(f)
    t1=fIn.Get("nero/events")
    t2=fIn.Get("nero/all")
    counts[0] += t1.GetEntries()
    counts[1] += t2.GetEntries()

print "nero/events has",counts[0],"entries"
print "nero/all has",counts[1],"entries"

