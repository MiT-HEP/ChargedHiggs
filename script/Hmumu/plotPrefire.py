from optparse import OptionParser
parser=OptionParser()
parser.add_option("-i","--input",help="input file for prefiring [%default]",default="/eos/cms/store/user/amarini/Hmumu/fwk/2018_07_02_MITBdt_ExC_Rocco_UnprefirableData/2018_07_02_MITBdt_ExC_Rocco_UnprefirableData.root")
parser.add_option("-r","--rebin",type='int',help="Rebin [%default]",default=10)
parser.add_option("","--data",action='store_true',help="Data [%default]",default=True)
parser.add_option("","--mc",dest='data',action='store_false',help="MC")
parser.add_option("","--mcname",help="MC histogram [%default]",default="VBF_HToMuMu_M125")
opts,args=parser.parse_args()

import ROOT
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

fIn=ROOT.TFile.Open(opts.input)
if opts.data:
    num=fIn.Get("HmumuAnalysis/Vars/BdtOnH_Prefire_Data")
    den=fIn.Get("HmumuAnalysis/Vars/BdtOnH_Data")
else:
    num=fIn.Get("HmumuAnalysis/Vars/BdtOnH_"+opts.mcname)
    den=fIn.Get("HmumuAnalysis/Vars/BdtOnH_NoPrefire_"+opts.mcname)


c=ROOT.TCanvas("c","c",800,1000)
p1=ROOT.TPad("p1","p1",0,0.4,1,1)
p2=ROOT.TPad("p2","p2",0,0,1,.4)


p1.Draw()
p2.Draw()

num.Rebin(opts.rebin)
den.Rebin(opts.rebin)

p1.SetTopMargin(0.05)
p1.SetBottomMargin(0.0)
p2.SetTopMargin(0.0)
p2.SetBottomMargin(0.3)

p1.SetLeftMargin(0.15)
p1.SetRightMargin(0.05)
p2.SetLeftMargin(0.15)
p2.SetRightMargin(0.05)
## Upper Pad
p1.cd()
num.SetFillColor(38)
num.SetLineColor(ROOT.kBlack)
num.SetLineWidth(2)

den.SetFillColor(46)
den.SetLineColor(ROOT.kBlack)
den.SetLineWidth(2)

den.GetXaxis().SetTitle("Bdt")
den.GetYaxis().SetTitle("Events")

den.Draw("HIST ")
num.Draw("HIST SAME")

p2.cd()
dummy=den.Clone()
dummy.Reset("ACE")

CL1=1.-2.*ROOT.RooStats.SignificanceToPValue(1)
CL2=1.-2.*ROOT.RooStats.SignificanceToPValue(2)
g1=ROOT.TGraphAsymmErrors()
g2=ROOT.TGraphAsymmErrors()

for i in range(0,num.GetNbinsX()):
    t=num.GetBinContent(i+1)
    s=den.GetBinContent(i+1)
    midp1=ROOT.TEfficiency.MidPInterval(s,t, CL1,True)
    midp2=ROOT.TEfficiency.MidPInterval(s,t, CL1,False)
    h_MIDP = midp1/(1.-midp1)
    l_MIDP = midp2/(1.-midp2)
    
    x=num.GetBinCenter(i+1)
    dx=num.GetBinWidth(i+1)
    y= 0 if s==0 else t/s
    g1.SetPoint(i, x, y )
    g2.SetPoint(i, x, y )
    
    if i<10: print "->Setting error for bin",i,"to:","s=",s,"t=",t,"midp=",h_MIDP,l_MIDP, "x=",x,"y=",y,"eyl=",y-l_MIDP,"eyh=",h_MIDP-y 
    g1.SetPointError(i, dx/2.,dx/2., y-l_MIDP,h_MIDP-y )
    midp1=ROOT.TEfficiency.MidPInterval(s,t, CL2,True)
    midp2=ROOT.TEfficiency.MidPInterval(s,t, CL2,False)
    h_MIDP = midp1/(1.-midp1)
    l_MIDP = midp2/(1.-midp2)
    g2.SetPointError(i, dx/2.,dx/2., y-l_MIDP,h_MIDP-y )


g1.SetMarkerStyle(20)
g1.SetMarkerSize(0.8)
g1.SetFillColor(ROOT.kGreen+1)

g2.SetFillColor(ROOT.kOrange)

dummy.Draw("AXIS")
dummy.GetYaxis().SetRangeUser(-0.01,0.10)
if opts.data:
    g2.Draw("E 2 SAME")
    g1.Draw("P E 2 SAME")
else:
    g1.Draw("P SAME")
dummy.Draw("AXIS SAME")

p1.cd()
x_txt=1.-p1.GetRightMargin()-0.02
y_txt=1.-p1.GetTopMargin()-0.02
ltx=ROOT.TLatex()
ltx.SetNDC()
ltx.SetTextSize(0.04)
ltx.SetTextAlign(33)
if opts.data:
    ltx.DrawLatex(x_txt,y_txt,"Run2017-F")
    ltx.DrawLatex(x_txt,y_txt-0.05,"Un-prefirable data")
else:
    dummy.GetYaxis().SetRangeUser(1.0-0.05,1.0+0.05)
    ltx.DrawLatex(x_txt,y_txt,"2017-Prefire Map")
    ltx.DrawLatex(x_txt,y_txt-0.05,"mc:"+opts.mcname)

p1.Modified()
p1.Update()
p2.Modified()
p2.Update()
c.RedrawAxis()
c.Modified()
c.Update()
raw_input("ok?")
