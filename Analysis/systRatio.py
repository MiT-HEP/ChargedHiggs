
import os,sys,math

import ROOT
ROOT.gROOT.SetBatch()
ROOT.gStyle.SetOptStat(0)
#ROOT.gStyle.SetOptTitle(0)


def plotSysts(hNom, hUp, hDown, name):

    c = ROOT.TCanvas("c", "c", 800, 600)
    c.SetGrid();

    c.SetTopMargin(.06)
    c.SetRightMargin(0.08)
    #c.SetBottomMargin(0.0)
    c.SetLeftMargin(0.12)

    h = hNom.Clone("base")
    for i in range(1, h.GetNbinsX()+1): h.SetBinContent(i, 1)

    h.SetLineWidth(2)
    h.SetLineColor(ROOT.kBlack)
    h.SetXTitle('H_{T} (GeV)')
    h.SetYTitle('syst/nom')
    h.SetTitle(name)

    h.GetXaxis().SetTitleFont(43)
    h.GetXaxis().SetTitleSize(22)
    h.GetXaxis().SetLabelFont(43)
    h.GetXaxis().SetLabelSize(22)

    h.GetYaxis().SetTitleFont(43)
    h.GetYaxis().SetTitleSize(22)
    h.GetYaxis().SetLabelFont(43)
    h.GetYaxis().SetLabelSize(22)

    h.GetXaxis().SetTitleOffset(1.2)
    h.GetYaxis().SetTitleOffset(1.3)

    h.GetXaxis().SetLabelOffset(0.01)
    h.GetYaxis().SetLabelOffset(0.01)

    h.GetYaxis().SetRangeUser(0.7, 1.3)
    h.GetXaxis().SetRangeUser(0, 8000)

    h.Draw("HIST")

    hUp.Divide(hNom)
    hUp.SetLineWidth(2)
    hUp.SetLineColor(ROOT.kRed)

    hDown.Divide(hNom)
    hDown.SetLineWidth(2)
    hDown.SetLineColor(ROOT.kBlue)

    for i in range(1, hUp.GetNbinsX()+1):
        if hUp.GetBinContent(i) < 10e-10: hUp.SetBinContent(i,1)
        if hDown.GetBinContent(i) < 10e-10: hDown.SetBinContent(i,1)

    hUp.Draw("HIST SAME")
    hDown.Draw("HIST SAME")

    c.SaveAs("%s/%s.png" % (outDir, name))



if __name__ == "__main__":

    cats = ['BB']
    procs = ["QCD",
"VVEWK",
"EWKV",
"ttbar",
"VVOther",
"VQQ",
]
    systs = ["CMS_pileUp","CMS_scale_AK8j",
"CMS_jes_RelativeSample_2016",
"CMS_jes_BBEC1_2017",
"CMS_jes_BBEC1_2018",
"CMS_jes_EC2_2018",
"CMS_jes_RelativeSample_2018",
"CMS_jes_EC2",
"CMS_jes_Absolute",
"CMS_jes_BBEC1",
"CMS_jes_HF_2016",
"CMS_jes_EC2_2016",
"CMS_jes_EC2_2017",
"CMS_jes_HF_2018",
"CMS_jes_HF_2017",
"CMS_jes_HF",
"CMS_jes_RelativeSample_2017",
"CMS_jes_BBEC1_2016",
"CMS_jes_RelativeBal",
"CMS_jes_Absolute_2017",
"CMS_jes_Absolute_2018",
"CMS_jes_FlavorQCD",
"CMS_jes_Absolute_2016",
"CMS_eff_Xcc",
"CMS_eff_Xbb",
"CMS_eff_Xqq",
"CMS_btag_HF",
"CMS_btag_LF",
"CMS_btag_LFSTAT1",
"CMS_btag_LFSTAT2",
"CMS_btag_HFSTAT1",
"CMS_btag_HFSTAT2",
]

    baseNameNom = "{cat}_{proc}"
    baseNameSyst = "{cat}_{proc}_{syst}"
    
    outDir = "/afs/cern.ch/user/d/dalfonso/www/VBS/JAN20fits/SYST/"
    
    fIn = ROOT.TFile("DATACARDMVA_bin0p05_stat0p5/AUG4/cms_vbshad_2021_2_BDTnoBnoMET_BB_SR.inputs.root", "READ")

    for cat in cats:
        for proc in procs:
            for syst in systs:
            
                filename = baseNameNom.format(cat=cat, proc=proc)
                print filename
                hNom = fIn.Get(baseNameNom.format(cat=cat, proc=proc))
                hUp = fIn.Get(baseNameSyst.format(cat=cat, proc=proc, syst=syst) + "Up")
                hDown = fIn.Get(baseNameSyst.format(cat=cat, proc=proc, syst=syst) + "Down")
                plotSysts(hNom, hUp, hDown, "%s_%s_%s"%(proc, cat, syst))



exit()

'''
KEY: TH1DBB_QCD;1DNN noBnoMET 
KEY: TH1DBB_QCD_CMS_pileUpUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_pileUpUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_pileUpDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_pileUpDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2016Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2016Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2016Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2016Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2017Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2017Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2017Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2017Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_scale_AK8jUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_scale_AK8jUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_scale_AK8jDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_scale_AK8jDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2018Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2018Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2018Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2018Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_scale_unclusterUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_scale_unclusterUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_scale_unclusterDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_scale_unclusterDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2018Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2018Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2018Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2018Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2018Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2018Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2018Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2018Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_eff_XccUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_eff_XccUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_eff_XccDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_eff_XccDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_AbsoluteUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_AbsoluteUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_AbsoluteDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_AbsoluteDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2016Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2016Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2016Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2016Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2016Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2016Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2016Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2016Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_HFUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_HFUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_HFDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_HFDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2017Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2017Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2017Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_EC2_2017Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2018Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2018Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2018Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2018Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_L1PrefireUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_L1PrefireUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_L1PrefireDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_L1PrefireDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2017Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2017Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2017Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HF_2017Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HFUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HFUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_HFDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_HFDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_LFUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_LFUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_LFDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_LFDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2017Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2017Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2017Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeSample_2017Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2016Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2016Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2016Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_BBEC1_2016Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT1Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT1Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT1Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT1Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT2Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT2Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT2Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_LFSTAT2Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeBalUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeBalUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeBalDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_RelativeBalDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_eff_XbbUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_eff_XbbUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_eff_XbbDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_eff_XbbDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2017Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2017Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2017Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2017Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT1Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT1Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT1Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT1Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_QCDnonclosure_s_BBUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_QCDnonclosure_s_BBUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_QCDnonclosure_s_BBDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_QCDnonclosure_s_BBDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT2Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT2Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT2Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_btag_HFSTAT2Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2018Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2018Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2018Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2018Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_FlavorQCDUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_FlavorQCDUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_FlavorQCDDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_FlavorQCDDown;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2016Up;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2016Up;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2016Down;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_jes_Absolute_2016Down;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_eff_XqqUp;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_eff_XqqUp;1DNN noBnoMET  [backup cycle]
KEY: TH1DBB_QCD_CMS_eff_XqqDown;2DNN noBnoMET  [current cycle]
KEY: TH1DBB_QCD_CMS_eff_XqqDown;1DNN noBnoMET  [backup cycle]
'''    
