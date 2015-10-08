#!/usr/bin/env python

import ROOT
import os,sys

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import FindEOS


#files = FindEOS("/store/user/amarini/Nero/v1.0/PromptReco-v1/SingleMuon")

chain = ROOT.TChain("nero/events")


h = ROOT.TH1D("h","h",400,0,200)
h_base = ROOT.TH1D("h_base","h_base",400,0,200)

BaseHeader = ""

###    TauDecayModeFindingNewDMs = 1UL <<0,
###    TauDecayModeFinding = 1UL <<1,
###    AgainstEleLoose = 1UL <<8,
###    AgainstEleMedium = 1UL<<9,
###    AgainstMuLoose = 1UL<<10,
###    AgainstMuTight = 1UL<<11

tauId="(tauId[0] & 1) && (tauId[0] & (1<<8)) && (tauId[0] & (1<<10))"

### selection for the tau Pt
if False:
	BaseTrigger=""
	#ProbeTrigger="HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120"
	ProbeTrigger="HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80"
	sel = "tauIsoDeltaBetaCorr[0]<1.5 &&"+ tauId+ " && abs(tauP4[0].Eta()) < 2.1 && metP4[0].Pt()>90"
	var = "tauP4[0].Pt()"
	BaseHeader = "MET Dataset"
	files = FindEOS("/store/user/amarini/Nero/v1.1//MET")


### selection for the met cut
if True:
	BaseTrigger="HLT_LooseIsoPFTau50_Trk30_eta2p1_v"
	#BaseTrigger=""
	ProbeTrigger="HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80"
	#ProbeTrigger="HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120"
	var = "metPuppi.Pt()"
	sel = "tauIsoDeltaBetaCorr[]<1.5 && "+ tauId +" && abs(tauP4[].Eta()) < 2.1 && tauP4[].Pt() > 51"
	h_base.GetXaxis().SetTitle("E_{T}^{miss} (Puppi)")
	files = FindEOS("/store/user/amarini/Nero/v1.1/Tau/")

## eta
if False:
	h = ROOT.TH1D("h","h",200,0,4)
	h_base = ROOT.TH1D("h_base","h_base",200,0,4)
	#BaseTrigger="HLT_LooseIsoPFTau50_Trk30_v" ## NO ETA # NO MET
	BaseTrigger="" ## NO ETA # NO MET
	BaseHeader = "MET Dataset"
	ProbeTrigger="HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80"
	var = "tauP4[].Eta()"
	h_base.GetXaxis().SetTitle("#eta_{#tau} ")
	sel = "tauIsoDeltaBetaCorr[]<1.5 && " + tauId+" && tauP4[].Pt() > 51 && metP4[0].Pt()>90"
	files = FindEOS("/store/user/amarini/Nero/v1.1//MET")


##HLT_PFMET120_NoiseCleaned_BtagCSV0p72,
## check it vs BTagDiscriminator
##h = ROOT.TH1D("h","h",200,0,1)
##h_base = ROOT.TH1D("h_base","h_base",200,0,1)
###BaseTrigger="HLT_LooseIsoPFTau50_Trk30_v" ## NO ETA # NO MET
##BaseTrigger="" ## NO ETA # NO MET
##ProbeTrigger="HLT_PFMET120_NoiseCleaned_BtagCSV0p72"
##var = "jetBdiscr"
##sel = "tauIsoDeltaBetaCorr[]<1.5 && tauId[]>0.5 && tauP4[].Pt() > 40 && metP4[0].Pt()>130"
##files = FindEOS("/store/user/amarini/Nero/v1.0.2/17Jul2015-v1/MET")

######################

## look for base/probe trigger
fT= ROOT.TFile.Open(files[0])
tN = fT.Get("nero/triggerNames")
BaseNumber = tN.GetTitle().split(',').index(BaseTrigger)
ProbeNumber = tN.GetTitle().split(',').index(ProbeTrigger)
fT.Close()
if BaseTrigger != "" : sel += "&& triggerFired[%d]"%BaseNumber

print "Triggers: "
if BaseTrigger != "" : print "\t* Base", BaseTrigger,BaseNumber
print "\t* Probe", ProbeTrigger,ProbeNumber
## 

for f in files:
	chain.Add(f)

chain.Draw(var + ">>h",sel +" && triggerFired[%d]"%(ProbeNumber),"goff")
chain.Draw(var + ">>h_base",sel,"goff")

h_base.SetLineColor(ROOT.kRed)

ROOT.gStyle.SetOptStat(0)

c= ROOT.TCanvas()

p1= ROOT.TPad("p2","p2",0,0.4,1,1)
p2= ROOT.TPad("p1","p1",0,0,1,0.4)

p1.Draw()
p2.Draw()

p1.cd()
h_base.Draw()
h.Draw("SAME")
leg = ROOT.TLegend(0.7,.7,.9,.9,BaseHeader)
leg.AddEntry(h_base,BaseTrigger)
leg.AddEntry(h,ProbeTrigger)
leg.Draw()

p2.cd()
r=h.Clone("r")
r.Divide(h_base)
r.Draw()


dict= {"var":"var", "sel":""}
def FindValue(s, look):
	if look not in s : return -1
	num= s.find(look)
	s1=s[num:]
	last= s1.find('&&')
	if last >=0 : s1 =s1[:last]
	medium= s1.find('=')
	if medium < 0 : medium = s1.find('>')
	if medium < 0 : medium = s1.find('<')

	s2 = s1[medium:].replace(">","").replace('=','').replace('<','')
	return float(s2)

if 'met' in var : dict[ "var" ] = "met"
if 'tau' in var and 'Pt' in var : dict[ "var" ] = "tauPt"
if 'tau' in var and 'Eta' in var: dict[ "var" ] = "tauEta"
if 'jet' in var and 'jetBdiscr' in var : dict[ "var" ] = "jetBdiscr"

if 'tauP4[].Pt()' in sel:
	dict ["sel"] += "_tauPt%.0f"%FindValue( sel, 'tauP4[].Pt()' ) 
#tauEta2p1
if 'tauP4[].Eta()' in sel:
	dict ["sel"] += "_tauEta" + ("%.1f"%FindValue( sel, 'tauP4[].Eta()' ) ).replace('.','p') 
#MET90
if 'metP4[0].Pt()' in sel: 
	dict ["sel"] += "_MET%.0f"%FindValue( sel, 'metP4[0].Pt()' ) 

name = "plot/trigger_%(var)s%(sel)s"%dict
print "Save name is '" + name + "'"

raw_input("Looks ok ? ")
c.SaveAs(name + ".pdf")
c.SaveAs(name + ".png")
