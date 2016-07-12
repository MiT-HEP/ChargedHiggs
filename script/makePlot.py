import ROOT

fileQCD="QCDPurity.root"
fileEWK="EWKPurity.root"
fileTau="ChHiggs.root"

dirQCD="ChargedHiggsQCDPurity"
dirEWK="ChargedHiggsEWKPurity"
dirTau="ChargedHiggsTauNu"

dir="Vars"
hist="HT_Full"
rebin=10

c=ROOT.TCanvas("c","c",800,1000)
p1=ROOT.TPad("p1","p1",0,0,1,0.25)
p2=ROOT.TPad("p2","p2",0,0.25,1,1)

fEWK=ROOT.TFile.Open(fileEWK)
fQCD=ROOT.TFile.Open(fileQCD)
fTau=ROOT.TFile.Open(fileTau)
data=fEWK.Get(dirEWK+"/"+dir +"/" +hist+"_Data");
data.Rebin(10)
data.SetMarkerStyle(20);
data.SetLineColor(ROOT.kBlack)
data.SetMarkerColor(ROOT.kBlack)

mc=ROOT.THStack()
totMC=None
gar=[]
for bkg in ["WJets","DY","TT","ST","WW","WZ","ZZ"]:
	h=fEWK.Get(dirEWK+"/"+dir +"/" +hist+"_"+bkg)
	h.Scale(2318)
	h.SetLineColor(ROOT.kBlack)
	h.SetLineWidth(2)
	h.Rebin(10)
	color=ROOT.kGray
	if bkg=="WJets": color=ROOT.kGreen+2
	elif bkg=="ST": color=ROOT.kRed-4
	elif bkg =="TT": color=ROOT.kMagenta+2
	elif bkg=="WW": color=ROOT.kGreen
	elif bkg=="WZ": color=ROOT.kGreen
	elif bkg=="ZZ": color=ROOT.kGreen

	h.SetFillColor(color)

	gar.append(h)
	mc.Add(h)
	if totMC==None: totMC=h.Clone("totMC")
	else: totMC.Add(h)
p1.Draw()
p2.Draw()
p2.cd()	
mc.Draw("H")
data.Draw("PE SAME")

p1.cd()
dataR=data.Clone("dataR")
dataR.Divide(totMC)
dataR.Draw("PE")

raw_input("ok?")
