import os,sys
from optparse import OptionParser
from array import array
import re
import math

print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")

from ParseDat import FindEOS


usage ='''Run on NeroNtuples in order to produce ~Z/like selection and plots vs R'''
parser= OptionParser(usage=usage)
parser.add_option("-b","--batch",dest="batch",action="store_true",help="Run in Batch mode",default=False);
parser.add_option("-e","--eos",dest="eos",type='string',help="Eos location. [%default]",default="/eos/user/a/amarini/Nero/v2.2.qg/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8");
parser.add_option("-o","--output",dest="output",type='string',help="OutputFile [%default]",default="outputQG.root");

opts, args = parser.parse_args()


sys.argv=[]
import ROOT
if opts.batch:
        ROOT.gROOT.SetBatch()

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
print "-> Opening output",opts.output
out=ROOT.TFile.Open(opts.output,"RECREATE")


def DeactivateBranches(t):
    t.SetBranchStatus("*",0)
    t.SetBranchStatus("jet*",1)
    t.SetBranchStatus("lep*",1)

def BookAndFill(hist,name,nBins,xmin,xmax,dR,value,weight=1.):
    ''' Save Hist and Profile '''
    if name not in hist:
        # vector<float> dRToProduce{0.01,0.02,0.03,0.04,0.05,0.10,0.15,0.20};
        #dR2=[0.005,0.015,0.025
        #deltaR = array('f',[0,0.01,0.02])
        print "Booking hist with name",name
        out.cd()
        hist[name] = ROOT.TH2D(name,name,25,0.005,0.255,nBins,xmin,xmax)
        hist[name+"_profX"] = ROOT.TProfile(name + "_profX",name,25,0.005,0.255)
    hist[name] . Fill(dR,value,weight)
    hist[name+"_profX"] . Fill(dR,value,weight)

fileList=FindEOS(opts.eos)
print "-> Found",len(fileList),"files"

hist={}

dRToProduce=[0.01,0.02,0.03,0.04,0.05,0.10,0.15,0.20]
n=len(fileList)
try:
  for ifile,f in enumerate(fileList):
    print "-> Opening file", f, ifile,"/",n
    fIn=ROOT.TFile.Open(f)
    t=fIn.Get("nero/events")
    DeactivateBranches(t)
    for ientry in range(0,t.GetEntries()):
        t.GetEntry(ientry)
        passLepSel=False

        if t.jetP4.GetEntries() == 0 : continue

        if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 20 and t.lepPdgId[0]* t.lepPdgId[1] == -13*13 :  ## OS SF muon, leading two
            ll = t.lepP4[0] + t.lepP4[1]
            if ll.M() <60 or ll.M() >120 : continue
            passLepSel=True

        if not passLepSel: continue
        
        nJets=0
        leadJet = -1
        subleadJet = -1
        if t.jetP4.GetEntries() == 0 : continue
        for ij in range(0,t.jetP4.GetEntries() ):
            if t.lepP4[0].DeltaR(t.jetP4[ij]) <0.4 : continue
            if t.lepP4[1].DeltaR(t.jetP4[ij]) <0.4 : continue

            if leadJet <0 :
                leadJet=ij
            elif subleadJet<0:
                subleadJet = ij

        ## jet selection
        if leadJet < 0 : continue 
        if ll.DeltaR(t.jetP4[leadJet] ) < 2.5 : continue  ## b2b
        if subleadJet >=0 and t.jetP4[subleadJet].Pt()/ll.Pt() > .3: continue ## jet veto

        pt= ll.Pt()
        eta=abs(t.jetP4[leadJet].Eta())

        ## fill
        for var in ["Axis1","Axis2","Mult","PtD"]:
          for gen in ["","Gen"]:
            for dR in dRToProduce:
                #jetQglGenAxis1_dR0p50 
                varName="jetQgl"+gen + var+"_dR0p%.0f"%(dR*1000)
                value = eval("t."+varName + "[leadJet]")

                if value <-990 : continue

                name = var 
                if gen != "": name+= "_" + gen
                nBins=1000
                xmin=0
                xmax=1.
                if var == "Mult": 
                    nBins=50
                    xmin=0
                    xmax=50
                if var == "Axis1" or var == "Axis2": 
                    nBins=1000
                    xmin=0
                    xmax=10
                    if value == 0 : continue;
                    value = -math.log(value)
                
                if pt> 0 and pt < 30:
                    name += "_pt_0_30"
                elif pt < 50:
                    name += "_pt_30_50"
                elif pt < 80:
                    name += "_pt_50_80"
                elif pt < 120:
                    name += "_pt_80_120"
                else: 
                    name += "_pt_120_Inf"

                if eta <2.0 :
                    name += "_eta_0_2"
                elif eta <3.0 :
                    name += "_eta_2_3"
                else:
                    name += "_eta_3_5"

                BookAndFill(hist,name,nBins,xmin,xmax,dR,value,1.)

except KeyboardInterrupt:
    print "Caught CTRL+C: Exiting. Please wait"
    pass

out.cd()
for h in hist:
    hist[h].Write()
out.Close()
print "->DONE"
