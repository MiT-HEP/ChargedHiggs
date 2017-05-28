# import ROOT in batch mode
import sys
import math
oldargv = sys.argv[:]
#sys.argv = [ '-b-' ]
import ROOT
#ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/007A3F80-9DB8-E611-9096-0CC47A78A4B8.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/0444930E-ACB8-E611-8A01-0CC47A7452DA.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/1E8E78D8-93B8-E611-853F-0025905A60BC.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/2E4D09C4-C0B8-E611-B02B-0CC47A78A4A6.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/36D91C56-96B8-E611-AC28-0CC47A74525A.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/40FD6185-D0B8-E611-9D2F-0025905A60EE.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/6CF85F7B-B0B8-E611-93BB-0CC47A78A4A6.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/9C6F82AC-B0B8-E611-9DF9-0CC47A78A30E.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/9E923785-D0B8-E611-A410-0CC47A7452DA.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/A27C2A60-B5B8-E611-8511-0CC47A7C3404.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/AAF768F9-AAB8-E611-8EAF-003048FFD79E.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/DC014062-B6B8-E611-A9BD-0CC47A78A2EC.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/E4F8418C-B8B8-E611-9EC7-0CC47A78A4A6.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/E686B2F0-A3B8-E611-8727-0025905B85C6.root
#/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/F0C900F2-A2B8-E611-B998-0CC47A7452DA.root

##PUMoriond17
#files=['/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/FlatPU28to62HcalNZSRAW_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/F0C900F2-A2B8-E611-B998-0CC47A7452DA.root']

## Prev
#files=['/store/mc/RunIISpring16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1/80000/D0A9D6B0-7758-E611-B5E5-0025905B85C6.root']
files=['file:/afs/cern.ch/user/a/amarini/work/GenProduction/CMSSW_7_1_25_patch4/src/ChHiggs_M1000.root']
#files=['file:/afs/cern.ch/user/a/amarini/work/GenProduction/CMSSW_7_6_3_patch2/src/HIG-RunIISummer15wmLHEGS-00276.root']
#files=['/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-300_13TeV_amcatnlo_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/B86AC1AC-01CB-E611-907E-0025905B85CC.root']


## TOP BOTTOM
#files=['/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTB_M-2000_13TeV_amcatnlo_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/90000/EC624DD7-70F1-E611-B4C8-02163E0128AF.root']
#files=['/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTB_M-1000_13TeV_amcatnlo_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/F656816E-9DDD-E611-9B69-FA163EBAF02E.root']
#files=['/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTB_M-800_13TeV_amcatnlo_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/E45B6228-73E3-E611-9EC6-001E67A400F0.root']

onMiniAOD=False
TopBottom=False

#onlyEvent=12345
onlyEvent=None
verbose=False
h = ROOT.TH1D("hp","hp",1000,500,1500)
h2 = ROOT.TH1D("hp2","hp2",1000,500,1500)
#h = ROOT.TH1D("hp","hp",1000,0,1000)
#h = ROOT.TH1D("hp","hp",1000,1500,2500)
## counters events
try:
   for f in files:
    # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
    print "->Opening file",f.split()[0]
    if 'file:' in f.split()[0]:
        events = Events(f.split()[0])
    else:
        events = Events("root://eoscms//"+f.split()[0])
    lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
    #vector<reco::GenParticle>             "prunedGenParticles"  
    handlePruned  = Handle ("std::vector<reco::GenParticle>")
    if onMiniAOD:
        labelPruned = ("prunedGenParticles")
    else:
        labelPruned = ("genParticles")
        
    for iev,event in enumerate(events):

        if onlyEvent != None and event.eventAuxiliary().event() != onlyEvent: continue

        if verbose:
            print "\n-> Event %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
        event.getByLabel(lheLabel, lhe)
        hepeup = lhe.product().hepeup()
        if verbose:
          for i in range(0,hepeup.IDUP.size() ):
            x=ROOT.TLorentzVector()
            x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) 

            if hepeup.ISTUP[i] != 1: continue;

            print " *) pdgid=",hepeup.IDUP[i],"pt=",x.Pt(),"eta=",x.Eta(),"phi=",x.Phi()

        w=lhe.product().weights()[0].wgt

        ### GEN PARTICLES
        if verbose:
            print " ------------ "
        event.getByLabel (labelPruned, handlePruned)
        pruned = handlePruned.product()
        tau=ROOT.TLorentzVector()
        nu=ROOT.TLorentzVector()

        #last copy
        tauL=ROOT.TLorentzVector()
        nuL=ROOT.TLorentzVector()

        for p in pruned:
            mother=p.mother(0)
            mpdg=0
            if mother: mpdg=mother.pdgId()
            if verbose:
                print " *) PdgId : %s   pt : %s  eta : %s   phi : %s mother : %s" %(p.pdgId(),p.pt(),p.eta(),p.phi(),mpdg) 

            if TopBottom:
                if abs(p.pdgId()) == 5 and abs(mpdg)==37:
                    tau.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),p.mass())
                if abs(p.pdgId()) == 6 and abs(mpdg)==37:
                    nu.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),p.mass())
            else: #Tau Nu
                if abs(p.pdgId()) == 15 and abs(mpdg)==37:
                    tau.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),1.7)
                if abs(p.pdgId()) == 16 and abs(mpdg)==37:
                    nu.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),0.0)

                ### LAST COPY
                if abs(p.pdgId()) == 15:
                    tauL.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),1.7)
                    fromHiggs=False

                if abs(p.pdgId()) == 16:
                    fromHiggs=False
                    fromTau=False

                    m=p.mother(0)
                    while m!=None:
                        if abs(m.pdgId()) == 37: fromHiggs=True
                        if abs(m.pdgId()) == 15: fromTau = True
                        m=m.mother(0)
                    if fromHiggs and not fromTau:
                        nuL.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),0.0)

        hp=tau+nu
        h.Fill(hp.M())
        hp=tauL+nuL
        h2.Fill(hp.M())

#
except KeyboardInterrupt:
    pass

c=ROOT.TCanvas("c","c")
h.Draw()
c2=ROOT.TCanvas("c2","c2")
h2.Draw()
raw_input("ok?")
print "DONE"
