import sys, os
import re
from optparse import OptionParser,OptionGroup

parser=OptionParser()
parser.add_option("-e","--eos" ,dest='eos',type='string',help="Eos input directory [%default]",default="/store/user/amarini/Nero/v1.1/TauNoId")
parser.add_option("-v","--verbose" ,dest='verbose',action='store_true',help="Verbose [%default]",default=False)
parser.add_option("-l","--limit" ,dest='limit',type='int',help="Limit on running entries [%default]",default=-1)

opts,args=parser.parse_args()

EOS='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select'

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import *

import ROOT as r
r.gROOT.SetBatch()

if opts.verbose: print "-> Load Bare library"
status=r.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")

t=r.TChain("nero/events")
list =  FindEOS(opts.eos)

for l in list:
	print "Adding: ",l
	t.Add(l)

t.SetBranchStatus("*",0)
t.SetBranchStatus("genPdgId",1)
t.SetBranchStatus("genFlags",1)
t.SetBranchStatus("mcWeight",1)

fracs=[0.,0.,0.,0.,0.,0.,0.,0.,0.,0.]


for iEntry in range(0,t.GetEntries()):
	t.GetEntry(iEntry)
	if iEntry%1000 == 0 :
		print "\r Doing entry:",iEntry,"/",t.GetEntries(), ":", "%.1f %%"%(float(iEntry)*100./t.GetEntries()),
		sys.stdout.flush()

	if opts.limit >=0 and iEntry>opts.limit:
		print "LIMIT REACHED"
		break
	np=0;
	for ig in range(0,t.genPdgId.size()):
		isJet=abs(t.genPdgId[ig])<6 or t.genPdgId[ig] == 21
		if not isJet : continue
		if not (t.genFlags[ig] & r.BareMonteCarlo.HardProcessBeforeFSR ) :continue
		np+=1
	fracs[np]  += t.mcWeight

print " ----------------- "
Sum=0.0
print "Total xSec=",
for f in fracs:
	print f,
	Sum+=f
print

print "Fraction xSec=",
for f in fracs:
	print f/Sum,
print
print " ----------------- "
