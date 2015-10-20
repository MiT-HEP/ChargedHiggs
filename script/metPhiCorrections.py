import sys,os
import re
import array
from optparse import OptionParser, OptionGroup
usage = "usage: %prog [options]"
parser=OptionParser(usage=usage)
parser.add_option("-e","--eos" ,dest='eos',type='string',help="Eos input directory [Default=%default]",default="/store/user/amarini/Nero/v1.1/TauNoId")
parser.add_option("","--mc" ,dest='mc',action='store_true',help="is MC? [Default=%default]",default=False)
parser.add_option("","--data" ,dest='data',action='store_true',help="is Data [Default=%default]",default=False)
parser.add_option("-a","--append" ,dest='append',action='store_true',help="is Data [Default=%default]",default=False)
parser.add_option("-o","--output",dest="out",type='string',help="OutputFile. [Default=%default]",default='aux/MetPhi.root')
parser.add_option("-n","--order",dest="order",type='int',help="Polynomial order. [Default=%default]",default=1)
(opts,args) = parser.parse_args()

if opts.mc and opts.data:
	print "is it both mc and data?"
	exit(0)

EOS='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select'

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import *

import ROOT
print "->Loading"

if opts.append:
	fOut = ROOT.TFile.Open(opts.out,"UPDATE")
else:
	fOut = ROOT.TFile.Open(opts.out,"RECREATE")

t=ROOT.TChain("nero/events")

if opts.eos != "":
	list =  FindEOS(opts.eos)
	for l in list:
		print "Adding: ",l
		t.Add(l)
else:
	print "-> TEST <-"
	t.Add("root://eoscms///store/user/amarini/Nero/v1.1.1/SingleMuon/NeroNtuples/151013_092356/0000/NeroNtuples_6.root")

t.SetBranchStatus("*",0)
t.SetBranchStatus("npv",1)
t.SetBranchStatus("met*",1)
#t.SetBranchStatus("mcWeight",1)
t.SetBranchStatus("lepP4*",1)
t.SetBranchStatus("lepIso*",1)
t.SetBranchStatus("lepPdgId*",1)
t.SetBranchStatus("lepSelBits*",1)

extra=""
if opts.mc: extra = "_mc"
if opts.data: extra = "_data"

metPx = ROOT.TH2D("th2%s_metpx"%extra,"metpx",100,0,100,2000,-1000,1000)
metPxProf = ROOT.TProfile("tpr%s_metpx"%extra,"metpx",100,0,100)
metPy = ROOT.TH2D("th2%s_metpy"%extra,"metpy",100,0,100,2000,-1000,1000)
metPyProf = ROOT.TProfile("tpr%s_metpy"%extra,"metpy",100,0,100)

print "->Looping"
for i in range(0,t.GetEntries()):
	t.GetEntry(i)
	## 2 muon selection

	l1 = -1 
	l2 = -1
	#LepLoose  = 1UL<<3
	if t.lepP4.GetEntries() <2 : continue
	for i in range(0,t.lepP4.GetEntries() ) :
		if t.lepSelBits[i] & (1<<3): 
			l1 = i
			break
	for i in range(i+1,t.lepP4.GetEntries() ) :
		if t.lepSelBits[i] & (1<<3): 
			l2 = i
			break
	#print "DEBUG l1=",l1,"l2=",l2
	if l1< 0 or l2 <0 : continue
	if t.lepP4[l2].Pt()< 20 : continue
	##OS-SF, muon
	t.lepPdgId[0]* t.lepPdgId[1] == -13*13
	#print "DEBUG OS SF"

	ll = t.lepP4[0] + t.lepP4[1]
	#print "DEBUG M=",ll.M()
	if ll.M() <91-20 or ll.M()> 91+20: continue

	#print "DEBUG: Event pass selection"

	metPx.Fill( t.npv , t.metPuppi.Px() ) 
	metPy.Fill( t.npv , t.metPuppi.Py() ) 

	metPxProf.Fill( t.npv , t.metPuppi.Px() ) 
	metPyProf.Fill( t.npv , t.metPuppi.Py() ) 

print "->Fit"

f1 = ROOT.TF1("fx%s"%extra,"[0]",0,100)
f2 = ROOT.TF1("fy%s"%extra,"[0]",0,100)
if opts.order == 1:
	f1 = ROOT.TF1("fx%s"%extra,"[0] + [1]*x",0,100)
	f2 = ROOT.TF1("fy%s"%extra,"[0] + [1]*x",0,100)
if opts.order == 2:
	f1 = ROOT.TF1("fx%s"%extra,"[0] + [1]*x + [2]*x*x",0,100)
	f2 = ROOT.TF1("fy%s"%extra,"[0] + [1]*x + [2]*x*x",0,100)

fit1 = metPxProf.Fit(f1,"QNRS")
fit1.Get().SetName("fitresults_X%s"%extra)
fit2=metPyProf.Fit(f2,"QNRS")
fit2.Get().SetName("fitresults_Y%s"%extra)

print "->Save"
f1.Write()
f2.Write()
metPxProf.Write();
metPyProf.Write()
metPx.Write();
metPy.Write()
fit1.Get().Write()
fit2.Get().Write()

print "->Draw"

def set_palette():
	ncontours=999

	stops = [0.00, 1.00]
	#red   = [1.00, 48./255.]
	#green = [1.00, 48./255.]
	#blue  = [1.00, 131./255.]	
	red   = [1.00, 117./255.]
	green = [1.00, 117./255.]
	blue  = [1.00, 172./255.]	

	st = array.array('d', stops)
	re = array.array('d', red)
	gr = array.array('d', green)
	bl = array.array('d', blue)
	
	npoints = len(st)
	ROOT.TColor.CreateGradientColorTable(npoints, st, re, gr, bl, ncontours)
	ROOT.gStyle.SetNumberContours(ncontours)

if True:
	set_palette()

cx=ROOT.TCanvas("cx_%s"%extra,"cx")
metPx.Draw("COLZ")
metPxProf.SetLineWidth(2)
metPxProf.SetLineColor(ROOT.kBlack)
metPxProf.SetMarkerStyle(20)
metPxProf.Draw("PSAME")
f1.Draw("LSAME")

cy=ROOT.TCanvas("cy_%s"%extra,"cy")
metPy.Draw("COLZ")
metPyProf.SetLineWidth(2)
metPyProf.SetLineColor(ROOT.kBlack)
metPyProf.SetMarkerStyle(20)
metPyProf.Draw("P SAME")
f2.Draw("LSAME")

raw_input("ok?")
cx.Write()
cy.Write()

fOut.Close()
