import ROOT
import math

t=ROOT.TChain("nero/events")
t.Add("root://eoscms///store/group/phys_higgs/cmshmm/amarini/Nero/v3.2/Charmonium/Charmonium-Run2017B/180525_094034/0000/NeroNtuples_88.root")


hmm = ROOT.TH1D("hmm","hmm",1000,0,100)

for ientry in range(0,t.GetEntries()):
    if ientry & 1023 ==0: print "executing entry",ientry,"of",t.GetEntries()

    t.GetEntry(ientry)
    muons=[]

    for ilep in range(0,t.lepP4.GetSize()):
        ## any selection
        if abs(t.lepPdgId[ilep])==13:muons.append(ilep)
        if len(muons) > 2: break

    if len(muons)<2: continue
    if t.lepPdgId[muons[0]] * t.lepPdgId[muons[1]]> 0: continue ## same sign

    mass = ROOT.TLorentzVector()
    mass += t.lepP4[muons[0]]
    mass += t.lepP4[muons[1]]
    hmm.Fill(mass.M())

    print "---------------------"
    print "--> DiMuon ",mass.M()
    print "Px", t.lepP4[muons[0]].Px() ,":",t.lepP4[muons[1]].Px()
    print "Py", t.lepP4[muons[0]].Py() ,":",t.lepP4[muons[1]].Py()
    print "Pz", t.lepP4[muons[0]].Pz() ,":",t.lepP4[muons[1]].Pz()
    print "E ", t.lepP4[muons[0]].E() ,":",t.lepP4[muons[1]].E()
    print "M ", t.lepP4[muons[0]].M() ,":",t.lepP4[muons[1]].M()
    A=t.lepP4[muons[0]].E()*t.lepP4[muons[1]].E() - t.lepP4[muons[0]].Px()*t.lepP4[muons[1]].Px()-t.lepP4[muons[0]].Py()*t.lepP4[muons[1]].Py() - t.lepP4[muons[0]].Pz()*t.lepP4[muons[1]].Pz()
    print "A",A
    mmu=0.1056
    #print "T",A/mmu**2 * (1. + math.sqrt(1+mmu**2*(3.1**2-mmu**2)/A**2))
    print "sf1", (2*A+math.sqrt(4*A**2 + 4*(mmu**2)*(3.1**2-mmu**2))) /(-2*mmu**2)
    print "sf2", (2*A-math.sqrt(4*A**2 + 4*(mmu**2)*(3.1**2-mmu**2))) /(-2*mmu**2)
    

hmm.Draw("H")

raw_input("ok?")

