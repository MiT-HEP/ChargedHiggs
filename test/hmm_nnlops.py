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

## das query. do voms_proxy_init -voms cms
from subprocess import check_output
import re
xrootd="root://xrootd-cms.infn.it/"
dataset="/GluGluHToMuMu_M-125_13TeV_powheg_MINLO_NNLOPS_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM"
cmd="dasgoclient -query='file dataset=%s'"%dataset
output = check_output(cmd,shell=True)
files = [ xrootd+x for x in output.split('\n') if '/store' in x ] 

## DEBUG
#files = ["file:/tmp/amarini/78B22988-930A-E811-AA8C-001E67396ACC.root"]

onMiniAOD=True

onlyEvent=None
verbose=False

h={}

h["pt-njets"]=ROOT.TH2D("pt-njets","pt-njets",1000,0,1000,10,0-.5,10-.5)
h["all"] = ROOT.TH1D("all","all",10,-0.5,10-.5)

## counters events
try:
   for f in files:
     try:
        # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
        print "->Opening file",f.split()[0]
        if 'file:' or 'root:' in f.split()[0] :
            events = Events(f.split()[0])
        else:
            events = Events("root://eoscms//"+f.split()[0])

        #lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
        lhe,lheLabel = Handle("LHEEventProduct"),"source"
        handlePruned  = Handle ("std::vector<reco::GenParticle>")
        handleJets  = Handle ("std::vector<reco::GenJet>")

        if onMiniAOD:
            labelPruned = ("prunedGenParticles")
            labelJets = ("slimmedGenJets")
        else:
            labelPruned = ("genParticles")
            labelJets = ("ak4GenJets")

        if events==None: 
            print "[ERROR] Events is none.Try to continue"
            continue

        for iev,event in enumerate(events):

            if onlyEvent != None and event.eventAuxiliary().event() != onlyEvent: continue

            if verbose:
                print "\n-> Event %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())

            try:
                event.getByLabel(lheLabel, lhe)
                hepeup = lhe.product().hepeup()
                #if verbose:
                #  for i in range(0,hepeup.IDUP.size() ):
                #    x=ROOT.TLorentzVector()
                #    x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) 
                #    if hepeup.ISTUP[i] != 1: continue;
                #    print " *) pdgid=",hepeup.IDUP[i],"pt=",x.Pt(),"eta=",x.Eta(),"phi=",x.Phi()

                w=lhe.product().weights()[0].wgt

                ### GEN PARTICLES
                if verbose:
                    print " ------------ "
                event.getByLabel (labelPruned, handlePruned)
                pruned = handlePruned.product()
            except RuntimeError:
                print "-> RuntimeERROR trying to continue"
                continue

            h["all"].Fill(1,w)
            h["all"].Fill(2,w*w)
            h["all"].Fill(3,1)

            higgs=ROOT.TLorentzVector()
            muPlus=ROOT.TLorentzVector()
            muMinus=ROOT.TLorentzVector()

            for p in pruned:
                mother=p.mother(0)
                mpdg=0
                if mother: mpdg=mother.pdgId()

                if verbose:
                    print " *) PdgId : %s   pt : %s  eta : %s   phi : %s mother : %s" %(p.pdgId(),p.pt(),p.eta(),p.phi(),mpdg) 
                
                lastHiggs=False
                for idau in range(0,p.numberOfDaughters()):
                    if abs(p.daughter(idau).pdgId() ) == 25 :
                        lastHiggs=True

                fromHiggs=False
                tmp=mother
                while True:
                    if tmp==None or fromHiggs: break
                    if tmp.pdgId() == 25: fromHiggs=True
                    tmp=tmp.mother() ## next

                    
                if abs(p.pdgId())==25 and lastHiggs: 
                    higgs.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),p.mass())

                if p.status() ==1 and p.pdgId()==13 and fromHiggs: 
                    muMinus.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),0.105)
                if p.status() ==1 and p.pdgId()==-13 and fromHiggs: 
                    muPlus.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),0.105)


            event.getByLabel(labelJets, handleJets)
            njets=0
            jet0=ROOT.TLorentzVector()
            for j in handleJets.product():
                if verbose:
                    print " *) GenJet :   pt : %s  eta : %s   phi : %s " %(j.pt(),j.eta(),j.phi()) 
                if j.pt()<30: continue
                if abs(j.eta())>4.7: continue
                jet=ROOT.TLorentzVector()
                jet.SetPtEtaPhiM(j.pt(),j.eta(),j.phi(),j.mass())

                if jet0.Pt()< jet.Pt(): jet0=jet

                if muMinus and jet.DeltaR(muMinus) <0.1 :
                    continue
                if muPlus and jet.DeltaR(muPlus) <0.1 :
                    continue
                njets+=1
                

            #njets
            print "-> ", "HPT",higgs.Pt(),"HETA",higgs.Eta(),"HPHI",higgs.Phi(),"NJ",njets,"W",w,"J0.Pt",jet0.Pt(),"MU+",muPlus.Pt(),"MU-",muMinus.Pt()
            #h["mass"].Fill(higgs.M())
            h["pt-njets"] . Fill(higgs.Pt(),njets,w)

     except TypeError:
         # eos sucks
         pass

#
except KeyboardInterrupt:
    pass

fOut=ROOT.TFile("hmm_nnlops_gen.root","RECREATE")
fOut.cd()
for hstr in h:
    h[hstr].Write()
print "DONE"
