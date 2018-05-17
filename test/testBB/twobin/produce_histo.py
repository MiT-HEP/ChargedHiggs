import ROOT

fOut=ROOT.TFile.Open("template.root","RECREATE")
d=ROOT.TH1D("data_obs","data_obs",2,0,2)

s=ROOT.TH1D("sig","sig",2,0,2)
s.SetBinContent(1,0.1)
s.SetBinError(1,0)
s.SetBinContent(2,1)
s.SetBinError(2,0)

b1=ROOT.TH1D("bkg1","bkg1",2,0,2)
b1.SetBinContent(1,1.0)
b1.SetBinContent(2,0)
b1.SetBinError(1,0.5)
b1.SetBinError(2,0.5)

b2=ROOT.TH1D("bkg2","bkg2",2,0,2)
b2.SetBinContent(1,10)
#b2.SetBinContent(2,0)
b2.SetBinContent(2,0.)
b2.SetBinError(1,0.5)
b2.SetBinError(2,0.5)

#################################
### Manual stat uncertainties ###
#################################
#
b2_bin1StatUp = b2.Clone(b2.GetName()+"_"+b2.GetName() + "_bin1StatUp")
b2_bin1StatUp.SetBinContent(1, b2.GetBinContent(1) + b2.GetBinError(1))
#
b2_bin1StatDown = b2.Clone(b2.GetName() +"_"+b2.GetName()+ "_bin1StatDown")
b2_bin1StatDown.SetBinContent(1, max(b2.GetBinContent(1) - b2.GetBinError(1),0.))
#
b2_bin2StatUp = b2.Clone(b2.GetName() +"_"+b2.GetName()+ "_bin2StatUp")
b2_bin2StatUp.SetBinContent(2, b2.GetBinContent(2) + b2.GetBinError(2))
#
b2_bin2StatDown = b2.Clone(b2.GetName()+"_"+b2.GetName() + "_bin2StatDown")
b2_bin2StatDown.SetBinContent(2, max(b2.GetBinContent(2) - b2.GetBinError(2),0.))

#
b1_bin1StatUp = b1.Clone(b1.GetName()+"_"+b1.GetName() + "_bin1StatUp")
b1_bin1StatUp.SetBinContent(1, b1.GetBinContent(1) + b1.GetBinError(1))
#
b1_bin1StatDown = b1.Clone(b1.GetName()+"_"+b1.GetName() + "_bin1StatDown")
b1_bin1StatDown.SetBinContent(1, max(b1.GetBinContent(1) - b1.GetBinError(1),0.))
#
b1_bin2StatUp = b1.Clone(b1.GetName()+"_"+b1.GetName() + "_bin2StatUp")
b1_bin2StatUp.SetBinContent(2, b1.GetBinContent(2) + b1.GetBinError(2))
#
b1_bin2StatDown = b1.Clone(b1.GetName()+"_"+b1.GetName() + "_bin2StatDown")
b1_bin2StatDown.SetBinContent(2, max(b1.GetBinContent(2) - b1.GetBinError(2),0.))
#
b1_bin1StatUp.Write()
b1_bin1StatDown.Write()
b1_bin2StatUp.Write()
b1_bin2StatDown.Write()

b2_bin1StatUp.Write()
b2_bin1StatDown.Write()
b2_bin2StatUp.Write()
b2_bin2StatDown.Write()

#################################

d.Write()
s.Write()
b1.Write()
b2.Write()


fOut.Close()
