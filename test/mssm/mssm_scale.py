import os,sys

from subprocess import call, check_output

dataset='/MSSMggH-HiggsToMuMu_MA-1000_Tanb-*_13TeV_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'
mA = 1000
wA = 1 # impose width of gen particles
ntoys=2000

ds=check_output("dasgoclient -query='dataset=%s'"%dataset,shell=True).split()

files=[]
for d in ds:
    files.extend(check_output("dasgoclient -query='file dataset=%s'"%dataset,shell=True).split())

import ROOT
# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events, Runs

output=ROOT.TFile("MSSM.root","RECREATE")

histos={'nominal':ROOT.TH1D("nominal","nominal",20000,0,2000)}
histos['mean']=histos["nominal"].Clone("mean")
for itoy in range(0,ntoys): histos["toy%d"%itoy]=histos["nominal"].Clone("toy%d"%itoy)

def GetGE(phi,eta,pt):
    ''' q/pt bias for each muon. I guess this numbers are in %.'''
    if pt <200: return (0,0)
    if abs(eta) <1.2: return (0,0)

    if phi > ROOT.TMath.Pi() /3.: #> 60
        if eta >2.1: return (0.092,0.075)
        elif eta > 1.2: return (0.07,0.035)
        elif eta >0: return (0.0039,0.024)
        elif eta >-1.2: return (-0.035,0.023)
        elif eta > -2.1: return (-0.11,0.029)
        else: return (-0.15,0.063)
    elif phi > -ROOT.TMath.Pi() /3.: #-60,60
        if eta >2.1: return (-0.12,0.061)
        elif eta > 1.2: return (0.036,0.039)
        elif eta >0: return (-0.017,0.022)
        elif eta >-1.2: return (0.023,0.023)
        elif eta > -2.1: return (0.041,0.03)
        else: return (0.38,0.09)
    else:
        if eta >2.1: return (-0.24,0.078)
        elif eta > 1.2: return (0.0051,0.033)
        elif eta >0: return (-0.012,0.022)
        elif eta >-1.2: return (-0.0041,0.025)
        elif eta > -2.1: return (-0.039,0.032)
        else: return (-0.39,0.046)

rstore={}
def GetRN(phi,eta,itoy):
    ''' This insure consistency across the events'''
    global rstore
    mybin=0
    if phi > ROOT.TMath.Pi() /3.: #> 60
        if eta >2.1:        mybin=1
        elif eta > 1.2:     mybin=2
        elif eta >0:        mybin=3
        elif eta >-1.2:     mybin=4
        elif eta > -2.1:    mybin=5
        else:               mybin=6
    elif phi > -ROOT.TMath.Pi() /3.: #-60,60
        if eta >2.1:        mybin=7
        elif eta > 1.2:     mybin=8
        elif eta >0:        mybin=9
        elif eta >-1.2:     mybin=10
        elif eta > -2.1:    mybin=11
        else:               mybin=12
    else:
        if eta >2.1:        mybin=13
        elif eta > 1.2:     mybin=14
        elif eta >0:        mybin=15
        elif eta >-1.2:     mybin=16
        elif eta > -2.1:    mybin=17
        else:               mybin=18

    if (mybin,itoy) not in rstore:
        rstore[(mybin,itoy)]=ROOT.gRandom.Gaus(0,1)
    return rstore[(mybin,itoy)]
    

for ifile,f in enumerate(files):
    try:
        print "->Opening file",f.split()[0], "with XROOTD",ifile,"/",len(files)
        events = Events("root://xrootd-cms.infn.it//"+f.split()[0]) #AAA
        for iev,event in enumerate(events):

            labelPruned = ("prunedGenParticles")
            handlePruned  = Handle ("std::vector<reco::GenParticle>")

            labelMuons = ("slimmedMuons")
            handleMuons  = Handle ("std::vector<pat::Muon>")

            event.getByLabel (labelPruned, handlePruned)
            event.getByLabel (labelMuons, handleMuons)

            pruned = handlePruned.product()
            muons = handleMuons.product()

            ## find Gen Muons
            genmuons=[]
            for p in pruned:
                if p.status() ==1 and abs(p.pdgId())==13  :
                    lep=ROOT.TLorentzVector()
                    lep.SetPtEtaPhiM(p.pt(),p.eta(),p.phi(),0.105)
                    genmuons.append(lep)
                if len(genmuons) >=2: break
            if len(genmuons) <2 : continue
            genA=genmuons[0]+genmuons[1] 
            if abs(genA.M()-mA) > wA : continue

            recomuons=[]
            q=[]
            for m in muons:
                if abs(m.eta()) > 2.4 : continue
                if m.pt() < 20 : continue

                chiso  = m.pfIsolationR04().sumChargedHadronPt;
                niso   = m.pfIsolationR04().sumNeutralHadronEt;
                phoiso = m.pfIsolationR04().sumPhotonEt;
                puiso = m.pfIsolationR04().sumPUPt;
                totiso = chiso +ROOT.TMath.Max( niso + phoiso - .5*puiso, 0. ) ;

                if  totiso/m.pt() > 0.25 : continue;
                if not m.isMediumMuon(): continue

                lep=ROOT.TLorentzVector()
                lep.SetPtEtaPhiM(m.pt(),m.eta(),m.phi(),0.105)
                recomuons.append(lep)
                q.append(m.charge())
                if len(recomuons) >=2 : break
            # at least two recomuons
            if len(recomuons)<2: continue
            if q[0]*q[1] >0 : continue # opposite sign
            
            pt0,eta0,phi0 = recomuons[0].Pt(),recomuons[0].Eta(),recomuons[0].Phi()
            pt1,eta1,phi1 = recomuons[1].Pt(),recomuons[1].Eta(),recomuons[1].Phi()

            b0,s0 = GetGE(phi0,eta0,pt0)
            b1,s1 = GetGE(phi1,eta1,pt1)

            recoA_n = recomuons[0]+recomuons[1]
            histos['nominal'].Fill( recoA_n.M())
            
            recoA_m = recomuons[0]*(1.+b0/100.*pt0*q[0]) + (recomuons[1]*(1.+b1/100.*pt1*q[1]))
            histos['mean'] . Fill( recoA_m.M())
            for itoy in range(0,ntoys):
                #ROOT.gRandom.Gaus(b0,s0) /100. *recomuons[0].Pt()*q[0] if recomuons[0].Pt() >200 else 0.
                sf0 = (GetRN(phi0,eta0,itoy)*s0+b0)/100. *pt0*q[0] 
                #sf1 = ROOT.gRandom.Gaus(b1,s1) /100. *recomuons[1].Pt()*q[1] if recomuons[1].Pt() >200 else 0.
                sf1 = (GetRN(phi1,eta1,itoy)*s1+b1)/100. *pt1*q[1] 
                recoA_t = recomuons[0]*(1.+sf0) + (recomuons[1]*(1.+sf1))
                histos['toy%d'%itoy].Fill(recoA_t.M())
        print "-> Saving checkpoint"
        output.cd()
        for s in histos: 
            histos[s].Write("",ROOT.TObject.kOverwrite)
        output.Write()
    except KeyboardInterrupt: 
        print "-> Exiting ..."
        break
    except: pass 


output.cd()
for s in histos: 
    print "-> Writing",s
    histos[s].Write("",ROOT.TObject.kOverwrite)
output.Write()
output.Close()

print "--------------------------"
print "Used the folling random store:", rstore
print "DONE"

