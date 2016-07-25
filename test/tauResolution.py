import ROOT

f=ROOT.TFile.Open("root://eoscms///store/user/amarini/Nero/v1.4.nopujet/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/DY-amcatnlo/160701_205500/0000/NeroNtuples_70.root")
t=f.Get("nero/events")

h=ROOT.TH1D("h","res",400,0,2);

#reco efficiency th-> AntiE + DecayMode (NO ISO)
Pass=0
Tot=0


for i in range(0,t.GetEntries()):
	t.GetEntry(i)
	myTau=-1
	for j in range(0,t.tauIsoDeltaBetaCorr.size()):
		if t.tauIsoDeltaBetaCorr[0]<1.5:
			myTau=j
			break
	if myTau<0: continue;

	myJet=-1
	for j in range(0,t.genjetP4.GetEntries()):
		if t.genjetP4[j].DeltaR( t.tauP4[myTau])<.3:
			myJet=j
			break
	if myJet<0: continue
	h.Fill( t.tauP4[myTau].Pt() / t.genjetP4[myJet].Pt() )
h.Draw()
raw_input("ok?")
	
