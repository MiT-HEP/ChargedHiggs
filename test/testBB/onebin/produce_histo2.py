import ROOT

fOut=ROOT.TFile.Open("template.root","RECREATE")
d=ROOT.TH1D("data_obs","data_obs",1,0,1)

s=ROOT.TH1D("sig","sig",1,0,1)
s.SetBinContent(1,1)
s.SetBinError(1,0.9)

b1=ROOT.TH1D("bkg1","bkg1",1,0,1)
b1.SetBinContent(1,1)
b1.SetBinError(1,0.5)

### OLD NUISANCES
b1Up=ROOT.TH1D("bkg1StatUp","bkg1",1,0,1)
b1Up.SetBinContent(1,1+0.1)

b1Down=ROOT.TH1D("bkg1StatDown","bkg1",1,0,1)
b1Down.SetBinContent(1,1-0.1)

d.Write()
s.Write()
b1.Write()
b1Up.Write()
b1Down.Write()

fOut.Close()
