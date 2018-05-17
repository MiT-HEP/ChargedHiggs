import ROOT

#fOut=ROOT.TFile.Open("template.root","RECREATE")

def Import(w,o):
    getattr(w,'import')(o)
    return

w = ROOT.RooWorkspace("w","w")
x = ROOT.RooRealVar("x","x",0,10)
Import(w,x)

mu=ROOT.RooRealVar("mu", "average", 3,0,10);
mu.setConstant()
sigma=ROOT.RooRealVar("sigma", "sigma", 1);
sigma.setConstant()
sig=ROOT.RooGaussian("sig","sig",x,mu,sigma)
sig_norm = ROOT.RooRealVar("sig_n","sig_n",1.)
sig_norm.setConstant()
Import(w,sig)
Import(w,sig_norm)

l = ROOT.RooRealVar("lambda","lambda",-.5,-5,-0.0001)
bkg=ROOT.RooExponential("bkg","sig",x,l)
norm=1.e3
bkg_norm = ROOT.RooRealVar("bkg_norm","bkg_norm",norm,norm/2.,norm*2.)
bkg_norm.removeRange()

#bkg_norm.setConstant()
Import(w,bkg)
Import(w,bkg_norm)

## binning definition
h=ROOT.TH1D("h_data_template","h_data_template",100,0,10)
#ds = ROOT.RooDataSet("data_obs","data_obs",ROOT.RooArgSet(x) )
dh = ROOT.RooDataHist("data_obs","data_obs",ROOT.RooArgList(x) , h)
#for d in data:
#    x.setVal(d)
#    ds.add( ROOT.RooArgSet(x))
dh.Print()
Import(w,dh)

#epdf=ROOT.RooExtendPdf("bkg_e","bkg_e",bkg,bkg_norm)
x.setBins(100)
mc_norm=norm

to_save=[]
for itoy in range(0,10):
    n = ROOT.gRandom.Poisson(mc_norm)
    dh=bkg.generateBinned(ROOT.RooArgSet(x),n)
    dh.SetName("bkg_toy_%d"%itoy)
    Import(w,dh)
    h= dh.createHistogram("h_bkg_toy_%d"%itoy,x)
    h.SetName("h_bkg_toy_%d"%itoy)
    to_save.append(h)

w.writeToFile("template.root")

print "-> Saving histograms"
fOut=ROOT.TFile.Open("template.root","UPDATE")
dh_sig=sig.generateBinned(ROOT.RooArgSet(x),1,ROOT.RooFit.Asimov())
h_sig = dh_sig.createHistogram("h_sig",x)
h_sig.SetName("h_sig")
h_sig.Write()
for h in to_save: h.Write()
fOut.Close()

print "-- DONE --"
del w
del h

