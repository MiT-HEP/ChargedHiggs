import ROOT

#fOut=ROOT.TFile.Open("template.root","RECREATE")

def Import(w,o):
    getattr(w,'import')(o)
    return

w = ROOT.RooWorkspace("w","w")
x = ROOT.RooRealVar("x","x",0,10)
Import(w,x)

#data=[3.29779,2.26382,0.271258,3.05073,4.2746,0.452908,1.16416,0.199286,6.14129,1.47887,2.30121,0.206421,0.91576,3.58029,0.043837,5.07826,0.773249,4.388,
#        1.57817,7.8598,4.27447,2.05627,3.9502,0.648981,2.68938,0.242014,0.947936,0.240518,6.07086,3.14022,2.59809,0.944341,0.0740509,0.00399546,4.87106,
#        1.70461,0.242629,7.41026,0.758612,0.0807072,3.14942,0.856279,1.9991,5.50521,0.0430989,0.207928,0.588551,1.24582,1.58905,6.13201,0.0362424,12.0109,
#        1.02065,0.0371316,0.706902,0.0636697,0.44787,4.45066,0.87164,0.529395,0.506368,0.635089,1.14078,0.76929,1.33174,0.151593,2.82427,0.454944,0.0154883,
#        2.58425,0.183701,0.596943,0.1578,1.8277,0.0123421,0.766656,1.5828,0.30329,3.2371,1.38899,1.34754,1.69661,4.29753,0.102483,0.525163,3.52272,0.849751,
#        1.21959,1.40408,0.687517,0.645178,0.314946,1.9885,0.874915,0.742486,4.2612,3.03483,0.0227588,2.44306,2.27362]

data=[3.29779,2.26382,0.271258,3.05073,4.2746,0.452908,1.16416,0.199286]

mu=ROOT.RooRealVar("mu", "average", 5,0,10);
sigma=ROOT.RooRealVar("sigma", "sigma", 1);
sigma.setConstant()
sig=ROOT.RooGaussian("sig","sig",x,mu,sigma)
sig_norm = ROOT.RooRealVar("sig_n","sig_n",1.)
sig_norm.setConstant()
Import(w,sig)
Import(w,sig_norm)

l = ROOT.RooRealVar("lambda","lambda",-.5,-5,-0.0001)
bkg=ROOT.RooExponential("bkg","sig",x,l)
bkg_norm = ROOT.RooRealVar("bkg_norm","bkg_norm",len(data),len(data)/2.,2*len(data))
bkg_norm.removeRange()

#bkg_norm.setConstant()
Import(w,bkg)
Import(w,bkg_norm)

ds = ROOT.RooDataSet("data_obs","data_obs",ROOT.RooArgSet(x) )
for d in data:
    x.setVal(d)
    ds.add( ROOT.RooArgSet(x))
ds.Print()
Import(w,ds)

w.writeToFile("template.root")


