import ROOT
import os,sys
## runs over the sync files


dir="sync/"
fIn=ROOT.TFile.Open("HmmSync.root")
t=fIn.Get("hmm")

vbf0 = open(dir+"vbf0.txt","w")
oneb = open(dir+"oneb.txt","w")
gf = open(dir+"gf.txt","w")
vbf1 = open(dir+"vbf1.txt","w")
untag0 = open(dir+"untag0.txt","w")
untag1 = open(dir+"untag1.txt","w")
All = open(dir+"all.txt","w")

for ientry in range(0,t.GetEntries()):
	t.GetEntry(ientry)
	if not t.pass_all: continue
	print >>All,t.eventNum
	if t.cat %6==0:
		print >> vbf0 , t.eventNum
	if t.cat %6==1:
		print >> oneb , t.eventNum
	if t.cat %6==2:
		print >> gf , t.eventNum
	if t.cat %6==3:
		print >> vbf1 , t.eventNum
	if t.cat %6==4:
		print >> untag0 , t.eventNum
	if t.cat %6==5:
		print >> untag1 , t.eventNum

##$( root HmmSync.root > all_roc.txt)<<EOF
##hmm->Scan("eventNum","pass_all")
##.q
##EOF

