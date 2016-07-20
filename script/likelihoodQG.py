import re
import sys, os

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import *
from optparse import OptionParser,OptionGroup

parser=OptionParser()
parser.add_option("-v","--verbose" ,dest='verbose',action='store_true',help="Verbose [%default]",default=False)
opts,args=parser.parse_args()

import ROOT as r
r.gROOT.SetBatch()

t=r.TChain("nero/events")

base="/store/user/amarini/Nero/v1.4"

datasets=[
#	"QCD_Pt_1000to1400_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_120to170_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_1400to1800_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_1800to2400_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_2400to3200_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_300to470_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_3200toInf_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_470to600_TuneCUETP8M1_13TeV_pythia8",
	"QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_600to800_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_800to1000_TuneCUETP8M1_13TeV_pythia8",
#	"QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8",
	]
for d in datasets:
	list= FindEOS( base + "/" + d ) 
	for l in list:
		t.Add(l)

t.SetBranchStatus("*",0)
t.SetBranchStatus("jetMatchedPartonPdgId",1)
t.SetBranchStatus("jetP4",1)
t.SetBranchStatus("jetQGL",1)
t.SetBranchStatus("jetFlavour",1)


hQ=r.TH1D("hQ","q",100,0,1)
hG=r.TH1D("hG","g",100,0,1)
hL=r.TH1D("hL","l",100,0,1)
t.Draw("jetQGL>>hQ","abs(jetFlavour)<6 && jetFlavour !=0  && jetP4.Pt()>30 && jetP4.Pt()<100","goff",10000)
t.Draw("jetQGL>>hG","abs(jetFlavour)==21  && jetP4.Pt()>30 && jetP4.Pt()<100","goff",10000)


hQ.Scale(1./hQ.Integral());
hG.Scale(1./hG.Integral());

for i in range(1,hL.GetNbinsX()+1):
    q= hQ.GetBinContent(i);
    g= hG.GetBinContent(i);
    x= hL.GetBinCenter(i);
    if q==0 and g==0 and x<0.5 :
		    g=1; 
		    q=0; 
    if q==0 and g==0 and x>0.5 :
	    g=0; 
	    q=1;
    hL.SetBinContent(i, q/(q+g) ) 


f=r.TF1("myfunc","TMath::Erf([0]*x)",0,1);
hL.Fit("myfunc","QN");
hL.Fit("myfunc","QNM");

c=r.TCanvas("c","c")
c.Draw()
hL.Draw("H")
f.SetLineColor(r.kBlack)
f.SetLineWidth(2)
f.Draw("L SAME")

hQ.SetFillColor(34)
hG.SetFillColor(54)
hG.SetLineColor(r.kRed)
hQ.SetLineColor(r.kBlue)

hG.Draw("H SAME")
hQ.Draw("H SAME")

hL.Draw("H SAME")
hL.Draw("AXIS SAME")
hL.Draw("AXIS X+ Y+ SAME")

raw_input("ok?")
c.SaveAs("/tmp/amarini/c.pdf")
c.SaveAs("/tmp/amarini/c.root")



