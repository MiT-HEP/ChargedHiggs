import ROOT



#shapes BKG                  ch1_hmm_13TeV_cat14  Hmumu/2017_08_21_NewCat5_NoTTH/BackgroundModel.root w:zmod_cat14_ord1
#shapes data_obs             ch1_hmm_13TeV_cat14  Hmumu/2017_08_21_NewCat5_NoTTH/BackgroundModel.root w:dataObs_cat14


fIn=ROOT.TFile.Open("Hmumu/2017_08_21_NewCat5_NoTTH/BackgroundModel.root")

w=fIn.Get("w")
#data=w.data("dataObs_cat14")
#pdf=w.pdf("zmod_cat14_ord1")
data=w.data("dataObs_cat0")
pdf=w.pdf("zmod2_cat0_ord5")
x= w.var("mmm")

nevt=ROOT.RooRealVar("nevt","nevt",data.sumEntries(),0,2*data.sumEntries());
epdf=ROOT.RooExtendPdf("epdf","epdf",pdf,nevt) ;

res=epdf.fitTo(data,ROOT.RooFit.Extended(True),ROOT.RooFit.Save(True))

res.Print("V")

print "-------------------------------------------------------------------------"
print "-------------------------------------------------------------------------"

x.setRange("low",110,122)
x.setRange("high",128,150)

#pdf.Print("V")

res=epdf.fitTo(data,ROOT.RooFit.Extended(True),ROOT.RooFit.Range("low,high"),ROOT.RooFit.Save(True))

#pdf.Print("V")
res.Print("V")
