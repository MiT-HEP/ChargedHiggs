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
#cmd="das_client.py --limit=1000 --query='file dataset=/ChargedHiggs_HplusTB_HplusToTauNu_M-200_13TeV_amcatnlo_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'"
#output = check_output(cmd,shell=True)
#files = [ x for x in output.split('\n') if '/store' in x ] 

#madspin="madspin"
madspin="nomadspin"
indir="/store/group/phys_higgs/cmshtt/amarini/ChargedHToTauNu_M500_amcatnlo_"+madspin+"_13TeV_pythia8/GEN"
cmd="eos find -f "+ indir
output=check_output(cmd,shell=True)
#files = [ "root://eoscms/"+re.sub('/eos/cms','',x) for x in output.split('\n') if '/store' in x ] 
#blacklist=["/eos/cms/store/group/phys_higgs/cmshtt/amarini/ChargedHToTauNu_M500_amcatnlo_madspin_13TeV_pythia8/GEN/180821_121144/0000/step1_10.root"]
blacklist=[]
files = [ "file:"+x for x in output.split('\n') if '/store' in x and x not in blacklist] 
files = ["file:/afs/cern.ch/user/a/amarini/public/forMiao/step5.root"]
onMiniAOD=False

#onlyEvent=12345
onlyEvent=None
verbose=True

h={}

#h["mt"] = ROOT.TH1D("mt","mt",1500,500,1500)
h["all"] = ROOT.TH1D("all","mt",10,0.5,10.5)
h["mt-had"] = ROOT.TH1D("mt-had","mt",1000,0,1000)
h["mt-lep"] = ROOT.TH1D("mt-lep","mt",1000,0,1000)
h["mass"] = ROOT.TH1D("mass","mass",1000,0,1000)
h["njets"] = ROOT.TH1D("njets","njets",10,0,10)
h["taupt"] = ROOT.TH1D("taupt","taupt",1000,0,1000)
h["tauhpt"] = ROOT.TH1D("tauhpt","tauhpt",1000,0,1000)
h["leppt"] = ROOT.TH1D("leppt","leppt",1000,0,1000)
h["met"] = ROOT.TH1D("met","met",1000,0,1000)
h["lep-met-dphi"] = ROOT.TH1D("lep-met-dphi","lep-met-dphi",1000,0,3.1416)
h["leadjetpt"] = ROOT.TH1D("leadjetpt","leadjetpt",1000,0,1000)


## counters events
try:
   for f in files:
     try:
        # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
        print "->Opening file",f.split()[0]
        if 'file:' in f.split()[0] :
            events = Events(f.split()[0])
        else:
            events = Events("root://eoscms//"+f.split()[0])
            #events = Events("root://xrootd-cms.infn.it//"+f.split()[0]) #AAA
        lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
        handlePruned  = Handle ("std::vector<reco::GenParticle>")
        handleJets  = Handle ("std::vector<reco::GenJet>")

        if onMiniAOD:
            labelPruned = ("prunedGenParticles")
            labelJets = ("slimmedGenJets")
        else:
            labelPruned = ("genParticles")
            labelJets = ("ak4GenJets")

        if events==None: 
            print "Events is none.Try to continue"
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

            tau=ROOT.TLorentzVector()
            nu=ROOT.TLorentzVector()
            met=ROOT.TLorentzVector()
            #lep=ROOT.TLorentzVector()
            lep=None

            for p in pruned:
                mother=p.mother(0)
                mpdg=0
                if mother: mpdg=mother.pdgId()
                if verbose:
                    print " *) PdgId : %s   pt : %s  eta : %s   phi : %s mother : %s" %(p.pdgId(),p.pt(),p.eta(),p.phi(),mpdg) 

                if p.status() ==1 and abs(p.eta())<4.7 and abs(p.pdgId()) not in [12,14,16]:
                    tmp=ROOT.TLorentzVector()
                    tmp.SetPtEtaPhiM( p.pt(),p.eta(),p.phi(),0.0)
                    met-=tmp

                if p.status() ==1 and (abs(p.pdgId())==11 or abs(p.pdgId())==13 ) and abs(mpdg)==15:
                    lep=ROOT.TLorentzVector()
                    lep.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),0.0 if abs(p.pdgId())==11 else 0.105)

                if abs(p.pdgId()) == 15 and abs(mpdg)==37:
                    tau.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),1.7)

                if abs(p.pdgId()) == 16 and abs(mpdg)==37:
                    nu.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),0.0)


            event.getByLabel(labelJets, handleJets)
            njets=0
            taujet=None
            leadjetpt=0
            for j in handleJets.product():
                if verbose:
                    print " *) GenJet :   pt : %s  eta : %s   phi : %s " %(j.pt(),j.eta(),j.phi()) 
                if j.pt()<30: continue
                if abs(j.eta())>4.7: continue
                jet=ROOT.TLorentzVector()
                jet.SetPtEtaPhiM(j.pt(),j.eta(),j.phi(),j.mass())
                if lep != None and lep.DeltaR(jet)<0.1: continue ## exclude lep-jets
                if jet.DeltaR(tau) <0.1 :
                    taujet=jet
                    continue
                njets+=1
                leadjetpt=max(leadjetpt,j.pt())
                

            #njets
            hp=tau+nu ## true

            h["mass"].Fill(hp.M())

            #return TMath::Sqrt( 2.* fabs(pt1) * fabs(pt2) * fabs( 1.-TMath::Cos(ChargedHiggs::deltaPhi(phi1,phi2)) ) );
            if taujet : h["mt-had"] . Fill(math.sqrt(2*taujet.Pt()*met.Pt()*(1.-math.cos(met.DeltaPhi(taujet)))),w)
            if lep    : h["mt-lep"] . Fill(math.sqrt(2*lep.Pt()*met.Pt()*(1.-math.cos(met.DeltaPhi(lep)))),w)
            h["njets"] . Fill(njets,w)
            h["taupt"] . Fill(tau.Pt(),w)
            if taujet : h["tauhpt"] . Fill(taujet.Pt(),w)
            if lep    : h["leppt"] . Fill(lep.Pt(),w)
            h["met"] . Fill(met.Pt(),w)
            if lep: h["lep-met-dphi"] . Fill(abs(lep.DeltaPhi(met)),w)
            h["leadjetpt"] . Fill(leadjetpt,w)

     except TypeError:
         # eos sucks
         pass

#
except KeyboardInterrupt:
    pass

fOut=ROOT.TFile("ch_"+madspin+"_gen.root","RECREATE")
fOut.cd()
for hstr in h:
    h[hstr].Write()
print "DONE"
