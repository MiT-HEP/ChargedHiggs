import os, sys
from subprocess import check_output,call
import ROOT

#dataset1="/JetHT/Run2016C-Nano1June2019-v1/NANOAOD"
dataset1=""
dataset2="/JetHT/Run2016C-UL2016_MiniAODv1_NanoAODv2-v1/NANOAOD"

#run=275920 -> 2016C
#run=275841 ->2016 C

# 2016D
#276327
#276352
run=276355
#276357
#276361

xrootd="root://xrootd-cms.infn.it/"

query="file dataset=%s run=%d"


#files1 = [
#        xrootd+x for x in check_output("dasgoclient -query 'file dataset="+dataset1+" run=%d'"%run,shell=True).split()
#        ]
files1=[]

files2 = [
        xrootd+x for x in check_output("dasgoclient -query 'file dataset="+dataset2+" run=%d'"%run,shell=True).split()
        ]
## overwrite
xcms="root://eoscms/"
#dataset2="/eos/cms/store/group/phys_higgs/cmshmm/amarini/nano/2016/JetHT//crab_JetHT_2016_Run2016C-"
dataset2="/eos/cms/store/group/phys_higgs/cmshmm/amarini/nano/2016/JetHT//crab_JetHT_2016_Run2016D-"
files2=[
        xcms+x for x in check_output("eos find -f %s"%dataset2,shell=True).split()
        ]

#files1=[]
#files2=[]
print "FILES1",','.join(files1)
print "FILES2",','.join(files2)

sum1=0.
sum2=0.
for fname in files1:
     print "Opening",fname
     f=ROOT.TFile.Open(fname)
     if f==None: raise RuntimeError(fname+": Unable to open file")
     t=f.Get("Events")
     sum1+=t.GetEntries("FatJet_pt[]>200 && (FatJet_jetId[] &2)&& FatJet_msoftdrop[] > 50 && FatJet_msoftdrop[] < 100 && run==%d"%run)

for fname in files2:
     print "Opening",fname
     f=ROOT.TFile.Open(fname)
     if f==None: raise RuntimeError(fname+": Unable to open file")
     t=f.Get("Events")
     sum2+=t.GetEntries("FatJet_pt[]>200 && (FatJet_jetId[]&2) && FatJet_msoftdrop[] > 50 && FatJet_msoftdrop[] < 100 && run==%d"%run)


xcms="root://eoscms/"
#dataset3="/eos/cms/store/group/phys_higgs/cmshmm/amarini/Nero/v3.7/year2016/JetHT/JetHT-Run2016C/"
#files3=[
#        xcms+x for x in check_output("eos find -f %s"%dataset3,shell=True).split()
#        ]
dataset3=""
files3=[]
print "FILES3",','.join(files3)
sum3=0.
for fname in files3:
     print "Opening",fname
     f=ROOT.TFile.Open(fname)
     if f==None: raise RuntimeError(fname+": Unable to open file")
     t=f.Get("nero/events")
     sum3+=t.GetEntries("fatjetAK8WithDeepTagP4[].Pt()>200 && fatjetAK8WithDeepTagSoftdropMass[] > 50 && fatjetAK8WithDeepTagSoftdropMass[] < 100 && runNum==%d"%run)

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()
# load FWlite python libraries
from DataFormats.FWLite import Handle, Events
dataset4="/JetHT/Run2016D-HIPM_UL2016_MiniAODv2-v1/MINIAOD"
files4 = [
        xrootd+x for x in check_output("dasgoclient -query 'file dataset="+dataset4+" run=%d'"%run,shell=True).split()
        ]
sum4=0.
for fname in files4:
    print "Opening",fname
    events = Events(fname)
    #if event.eventAuxiliary().event() % 100 ==1:
    #       print "\n-> Event %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
    handleJetReco  = Handle ("std::vector<pat::Jet>")
    labelJetReco = ("slimmedJetsAK8")

    for iev,event in enumerate(events):
        ##
        event.getByLabel(labelJetReco,handleJetReco)
        jetsReco = handleJetReco.product()
        for j in jetsReco:
            #jp4=ROOT.TLorentzVector()
            #jp4.SetPtEtaPhiM(j.pt(),j.eta(),j.phi(),j.mass())
            sdmass=j.groomedMass("SoftDropPuppi")
            if (j.pt() >200 and sdmass>50 and sdmass<100 and event.eventAuxiliary().run() == run):
                sum4+=1.

print "-----------------------------------"
print "RUN",run
print "DATASET",dataset1,"SUM1=",sum1
print "DATASET",dataset2,"SUM2=",sum2
print "DATASET",dataset3,"SUM3=",sum3
print "DATASET",dataset4,"SUM3=",sum4
print "-----------------------------------"

