# import ROOT in batch mode
import sys,os
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
##PUMoriond17
#/eos/cms//store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-200_13TeV_amcatnlo_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1

## Prev
#files=['file:/afs/cern.ch/user/a/amarini/work/GenProduction/CMSSW_7_1_25_patch4/src/ChHiggs_M1000.root']

## Recursive search on eos
#indir="/eos/cms/store/mc/RunIISummer16MiniAODv2/ChargedHiggs_HplusTB_HplusToTauNu_M-200_13TeV_amcatnlo_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1"
##files = [indir + filename for filename in os.listdir(indir)[:]]
#files=[]
#for root, dirs, fs in os.walk(indir):
#    files = [ root + "/" + x for x in fs[:] ] 

## das query. do voms_proxy_init -voms cms
from subprocess import check_output
cmd="das_client.py --limit=1000 --query='file dataset=/ChargedHiggs_HplusTB_HplusToTauNu_M-200_13TeV_amcatnlo_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'"
output = check_output(cmd,shell=True)
files = [ x for x in output.split('\n') if '/store' in x ] 

onMiniAOD=False
TopBottom=False

#onlyEvent=12345
onlyEvent=None
verbose=False
h = ROOT.TH1D("hp","hp",1000,500,1500)
h2 = ROOT.TH1D("hp2","hp2",1000,500,1500)
## counters events
try:
   for f in files:
    # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
    print "->Opening file",f.split()[0]
    if 'file:' in f.split()[0]:
        events = Events(f.split()[0])
    else:
        #events = Events("root://eoscms//"+f.split()[0])
        events = Events("root://xrootd-cms.infn.it//"+f.split()[0]) #AAA
    lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
    #vector<reco::GenParticle>             "prunedGenParticles"  
    handlePruned  = Handle ("std::vector<reco::GenParticle>")
    handleJets  = Handle ("std::vector<reco::GenJet>")

    if onMiniAOD:
        labelPruned = ("prunedGenParticles")
        labelJets = ("slimmedGenJets")
    else:
        labelPruned = ("genParticles")
        labelJets = ("genJets")
        
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

        event.getByLabel(labelJets, handleJets)
        for j in handleJets.product():
            if verbose:
                print " *) GenJet :   pt : %s  eta : %s   phi : %s " %(j.pt(),j.eta(),j.phi()) 

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
