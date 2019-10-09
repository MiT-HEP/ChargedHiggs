import os,sys,re
import numpy as np
import math

from optparse import OptionParser
parser=OptionParser()
parser.add_option("-i","--input",help="Input File [%default]",default="/eos/user/a/amarini/DY/fwk/2019_06_28_DY2017_SecondTrial/2019_06_28_DY2017_SecondTrial.root")
parser.add_option("-v","--variable",dest="var",help="Variable [%default]",default="mult")
parser.add_option("-r","--rebin",type='int',help="Rebin [%default]",default=1)
parser.add_option("-j","--jets",help="Jets string [%default]",default="jet500_1000")
opts,args=parser.parse_args()

sys.argv=[]
import ROOT

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

fIn=ROOT.TFile.Open(opts.input)
if fIn==None: raise IOError("<*> Unable to open file: "+opts.input)

#35867 41860 59710
lumi=41860

dir="DYAnalysis/Vars"
#jet="jet500_1000"
jet=opts.jets[:]

data=fIn.Get(dir+"/"+opts.var+"_"+jet+"_Data")
if data!=None:
    data.Rebin(opts.rebin)

mc=fIn.Get(dir+"/"+opts.var+"_"+jet+"_DY")
q=fIn.Get(dir+"/"+opts.var+"_Q_"+jet+"_DY")
g=fIn.Get(dir+"/"+opts.var+"_G_"+jet+"_DY")
o=fIn.Get(dir+"/"+opts.var+"_O_"+jet+"_DY")
#u=fIn.Get(dir+"/"+opts.var+"_U_"+jet+"_DY")

if mc==None:print "mc None"
if q==None:print "q None"
if g==None:print "g None"
if o==None:
    o=mc.Clone("other")
    o.Reset("ACE")
    print "o None"

for x in [mc,q,g,o]:
    x.Scale(lumi)
    x.Rebin(opts.rebin)

# KEY: TH1D      qgl_Q_jet1500_2000_DY_Pdf9;1    Npvmm
# KEY: TH1D      qgl_Q_jet1500_2000_DY_ScaleFDown;1      Npvmm
# KEY: TH1D      qgl_Q_jet1500_2000_DY_ScaleFUp;1        Npvmm
# KEY: TH1D      qgl_Q_jet1500_2000_DY_ScaleRDown;1      Npvmm
# KEY: TH1D      qgl_Q_jet1500_2000_DY_ScaleRFDown;1     Npvmm
# KEY: TH1D      qgl_Q_jet1500_2000_DY_ScaleRFUp;1       Npvmm
# KEY: TH1D      qgl_Q_jet1500_2000_DY_ScaleRUp;1        Npvmm
#alpha=1.-ROOT.TMath.Prob(1,1) ## 68%
mc_pdf=[]
q_pdf=[]
g_pdf=[]
for pdf in range(0,30):
    tmp=fIn.Get(dir+"/"+opts.var+"_"+jet+"_DY_Pdf%d"%pdf)
    tmp_q=fIn.Get(dir+"/"+opts.var+"_Q_"+jet+"_DY_Pdf%d"%pdf)
    tmp_g=fIn.Get(dir+"/"+opts.var+"_G_"+jet+"_DY_Pdf%d"%pdf)
    if tmp==None:
        print "Unable to fetch pdf",pdf
        continue
    tmp.Scale(lumi)
    tmp.Rebin(opts.rebin)
    mc_pdf.append(tmp)

    if tmp_q!=None:
        tmp_q.Scale(lumi)
        tmp_q.Rebin(opts.rebin)
        q_pdf.append(tmp_q)
    if tmp_g!=None:
        tmp_g.Scale(lumi)
        tmp_g.Rebin(opts.rebin)
        g_pdf.append(tmp_g)

mc2=mc.Clone()
q2=mc.Clone()
g2=mc.Clone()
for i in range(0,mc2.GetNbinsX()):
    ibin=i+1
    c= mc.GetBinContent(ibin)
    replicas=[]
    replicas_q=[]
    replicas_g=[]
    for pdf in range(0,30):    
        replicas.append( mc_pdf[pdf].GetBinContent(ibin))
        replicas_q.append( q_pdf[pdf].GetBinContent(ibin))
        replicas_g.append( g_pdf[pdf].GetBinContent(ibin))
    #m=np.mean(replicas)
    #e=np.std(replicas)

    #mc2.SetBinError(ibin,e)
    #if c<=0: continue
    #if (c-m)**2/e**2 > 0.1: print "ERROR?"
    #print "c=",c,"m=",m,"e=",e
    if False: #envelope: max-min
        up=np.max(replicas)
        down=np.min(replicas)
        mc2.SetBinContent(ibin,(up+down)/2.)
        mc2.SetBinError(ibin,(up-down)/2.)
        up=np.max(replicas_q)
        down=np.min(replicas_q)
        q2.SetBinContent(ibin,(up+down)/2.)
        q2.SetBinError(ibin,(up-down)/2.)
        up=np.max(replicas_g)
        down=np.min(replicas_g)
        g2.SetBinContent(ibin,(up+down)/2.)
        g2.SetBinError(ibin,(up-down)/2.)
    if True: #sqrt sum
        r_tot = np.array(replicas)  - c
        e = math.sqrt(np.sum( np.square( r_tot) ))
        mc2.SetBinError(ibin,e)

        c_q= q.GetBinContent(ibin)
        r_q = np.array(replicas_q)  - c_q
        e = math.sqrt(np.sum( np.square( r_q) ))
        q2.SetBinContent(ibin,c_q)
        q2.SetBinError(ibin,e)

        c_g= g.GetBinContent(ibin)
        r_g = np.array(replicas_g)  - c_g
        e = math.sqrt(np.sum( np.square( r_g) ))
        g2.SetBinContent(ibin,c_g)
        g2.SetBinError(ibin,e)


q.SetLineColor(38)
g.SetLineColor(46)
o.SetLineColor(8)
#u.SetLineColor(ROOT.kGray)
mc.SetLineColor(ROOT.kBlack)
data.SetLineColor(ROOT.kBlack)
data.SetMarkerColor(ROOT.kBlack)
data.SetMarkerStyle(20)

mc2.SetMarkerColor(ROOT.kGray)
mc2.SetMarkerStyle(0)
mc2.SetFillColor(ROOT.kGray)
mc2.SetLineWidth(0)
q2.SetMarkerColor(ROOT.kBlue-10)
q2.SetMarkerStyle(0)
q2.SetFillColor(ROOT.kBlue-10)
q2.SetLineWidth(0)
g2.SetMarkerColor(ROOT.kRed-10)
g2.SetMarkerStyle(0)
g2.SetFillColor(ROOT.kRed-10)
g2.SetLineWidth(0)

for x in [q,g,o,mc]: x.SetLineWidth(2)

data.SetMaximum( max (mc.GetMaximum(),data.GetMaximum())*1.2)

last=0
for i in range(0,data.GetNbinsX()):
    if data.GetBinContent(i+1) >0 or mc.GetBinContent(i+1)> 0: last=i+1
data.GetXaxis().SetRange(0,last+2)
data.GetXaxis().SetTitle(opts.var)

c=ROOT.TCanvas("c","c",800,800)

data.Draw("PE")
mc2.Draw("E2 SAME")
q2.Draw("E2 SAME")
g2.Draw("E2 SAME")
q.Draw("HIST SAME")
g.Draw("HIST SAME")
o.Draw("HIST SAME")
#u.Draw("HIST SAME")
mc.Draw("HIST SAME")
data.Draw("PE SAME")

leg=ROOT.TLegend(.5,.5,.89,.89)
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.AddEntry(None,re.sub('_',' to ',re.sub("jet",'',jet)) + "GeV","H")
leg.AddEntry(data,"data","PE")
leg.AddEntry(mc,"MC","L")
leg.AddEntry(q,"Quark","L")
leg.AddEntry(g,"Gluon","L")
leg.AddEntry(o,"other","L")
leg.Draw()

c.Update()

raw_input("ok?")


