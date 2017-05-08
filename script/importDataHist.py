import ROOT
import sys

w = ROOT.RooWorkspace("w","workspace") ;
x = ROOT.RooRealVar("mmm","mmm",110,150);

getattr(w,'import')(x)

fIn=ROOT.TFile.Open("test/Hmumu//Hmumu_2017_04_21_Bdt_ttH2/Hmumu_2017_04_21_Bdt_ttH2.root")
#fIn=ROOT.TFile.Open("test/Hmumu/Hmumu_2017_04_28_Bdt_amcatnlo/Hmumu_2017_04_28_Bdt_amcatnlo.root")
cat='cat12'
if len(sys.argv) > 1:
    cat =sys.argv[1] 
    print "Processing cat",cat
g=[]
g.extend([x,w])
for n in ['DY','TT']:
    h=fIn.Get("HmumuAnalysis/Vars/Mmm_"+cat+"_"+n)
    h.Scale(35867)
    h.Rebin(5)
    dh = ROOT.RooDataHist( n+"_"+cat,n+"_"+cat,ROOT.RooArgList(x),h  )
    getattr(w,'import')(dh)
    g.extend([h,dh])

w.writeToFile(cat+"_mc.root")

print "DONE"

