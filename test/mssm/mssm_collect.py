import os,sys
import ROOT

fIn=ROOT.TFile.Open("MSSM_1.root")
mA = 1000

def fit(h,save=""):
    '''return mean and variance'''
    x=ROOT.RooRealVar("x","x",0,10000)
    arglist_obs = ROOT.RooArgList(x)
    data= ROOT.RooDataHist("data","mmm",arglist_obs,h)
    m=ROOT.RooRealVar("m","m",h.GetMean(),0,10000)
    s=ROOT.RooRealVar("s","s",h.GetRMS()*.9,0,10000)
    w=ROOT.RooRealVar("w","w",0.004,0.001,5)
    pdf=ROOT.RooVoigtian("pdf","pdf",x,m,w,s,True)
    #n=ROOT.RooRealVar("norm","norm",h.Integral(),h.Integral()*0.5, h.Integral()*2.0)
    #epdf=ROOT.RooExtendPdf("epdf","epdf",pdf,n)

    pdf.fitTo(data,
            ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
            )
    if save!="":
        c=ROOT.TCanvas()
        p=x.frame()
        data.plotOn(p,ROOT.RooFit.Binning(50))
        pdf.plotOn(p)
        p.Draw()
        raw_input("ok?")
        c.SaveAs(save)
    return (m.getVal(),s.getVal())


h_n=fIn.Get("nominal")
h_m=fIn.Get("mean")

m_n,s_n= fit(h_n,"nominal.pdf")
m_m,s_m= fit(h_m,"mean.pdf")

ntoys=2000
ht=ROOT.TH1D("ht","ht",1000,-20,20)
for itoy in range(0,ntoys):
    h_toy=fIn.Get("toy%d"%itoy)
    if h_toy==None: 
        print "Ignoring toy",itoy
        continue
    if itoy==1: m,s= fit(h_toy,"toy1.pdf")
    else: m,s= fit(h_toy)
    print "mean is ",m
    #ht.Fill(m-m_n)
    ht.Fill(m-mA)

ht.Draw()
print "NOMINAL",m_n,s_n
print "MEAN",m_m,s_m
print "TOY (wrt mA)",ht.GetMean(),"RMS of mean",ht.GetRMS()

raw_input("ok?")
