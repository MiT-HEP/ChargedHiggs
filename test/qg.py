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

#files=['/store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/BC262FB4-6FCD-E611-B788-90B11C2CB121.root']
files=['/store/mc/RunIISummer16MiniAODv2/DYToLL_2J_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/100000/0055B4A5-F9D2-E611-9ECA-008CFA1979A0.root']
files=['file:/tmp/amarini/80EC30B8-851A-EA11-BFB3-1CB72C1B649A.root']

#onlyEvent=12345
onlyEvent=None
## counters events
try:
   for f in files:
    # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
    print "->Opening file",f.split()[0]
    events = Events(("root://eoscms//" if ':' not in f.split()[0] else '' )+f.split()[0])
    lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
    #vector<reco::GenParticle>             "prunedGenParticles"  
    handlePruned  = Handle ("std::vector<reco::GenParticle>")
    labelPruned = ("prunedGenParticles")
    for iev,event in enumerate(events):

        if onlyEvent != None and event.eventAuxiliary().event() != onlyEvent: continue

        print "\n-> Event %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
        event.getByLabel(lheLabel, lhe)
        hepeup = lhe.product().hepeup()
        for i in range(0,hepeup.IDUP.size() ):
            x=ROOT.TLorentzVector()
            x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) 

            if hepeup.ISTUP[i] != 1: continue;

            print " *) pdgid=",hepeup.IDUP[i],"pt=",x.Pt(),"eta=",x.Eta(),"phi=",x.Phi()

            #if ((abs(hepeup.IDUP[i]) <=6)  or hepeup.IDUP[i] ==21): ht+= x.Pt();
            #if ((abs(hepeup.IDUP[i]) >=11) and (abs(hepeup.IDUP[i]) <=16)): Q += x;
        #w=lhe.product().weights()[0].wgt

        ### GEN PARTICLES
        print " ------------ "
        event.getByLabel (labelPruned, handlePruned)
        pruned = handlePruned.product()
        for p in pruned:
            mother=p.mother(0)
            mpdg=0
            if mother: mpdg=mother.pdgId()
            print " *) PdgId : %s   pt : %s  eta : %s   phi : %s mother : %s : status %d" %(p.pdgId(),p.pt(),p.eta(),p.phi(),mpdg,p.status()) 

except KeyboardInterrupt:
    pass

print "DONE"
