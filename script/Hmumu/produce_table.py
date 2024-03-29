import ROOT 
import os,sys
from array import array
#import re,math


print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")

from optparse import OptionParser, OptionGroup
parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Input.root")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmmWithTTH")
parser.add_option("","--base",type='string',help="Base Directory [%default]", default="HmumuAnalysis/Vars")
parser.add_option("","--mass",type='float',help="Higgs mass to fetch [%default]", default=125.0)
opts,args= parser.parse_args()


fIn = ROOT.TFile.Open(opts.input)
if fIn == None:
    print "<*> Error,",opts.input,"no such file or directory"

from hmm import hmm,hmmAutoCat,hmmWithTTH,hmmExCat
config= eval(opts.hmm)
config.Print()

#S=0.0; for (int i=0;i<=15;++i){double acc=((TH1D*)gDirectory->Get(Form("Mmm_cat%d_GluGlu_HToMuMu_M125",i)))->IntegralAndError(1001,1800,err); ;cout<<"cat"<<i<<": acc="<<  acc <<"err="<<err<<endl;S+=acc; } ;cout <<"TOT="<<S<<endl;

## sum values
NsAll={}
NsSum=0.0
NdataSum=0
eaSum={}
for icat,cat in enumerate(config.categories):
    fwhm = "-" ## not this one, the one from the Signal model
    ea={}
    Ns=0.
    Ndata=0
    err= array('d',[.0])
    for proc in config.processes:
        h= fIn.Get(opts.base + "/" + "Mmm_%s_%s_HToMuMu_M%.0f"%(cat,proc,opts.mass))
        if h== None:
            print>>sys.stderr, "<*> Hist", opts.base + "/" + "Mmm_%s_%s_HToMuMu_M%.0f"%(cat,proc,opts.mass),"doesn't exist"
            h=ROOT.TH1D("tmp","tmp",10000,0,200)
        bin0 = h.FindBin(110)
        bin1 = h.FindBin(150)
        ea[proc] = h. IntegralAndError(bin0,bin1,err)
        Ns +=  ea[proc] * config.lumi() * config.xsec(proc) * config.br()
        NsAll[(proc,cat)] = ea[proc]*config.lumi()*config.xsec(proc)*config.br()

        ea[proc] *=100 ## save it in %
        if proc not in eaSum: eaSum[proc] = 0.0
        eaSum[proc] += ea[proc]

    
    hData=fIn.Get(opts.base + "/" + "Mmm_%s_Data"%(cat))
    bin0 = hData.FindBin(110)
    bin1 = hData.FindBin(150)
    Ndata = hData . IntegralAndError(bin0,bin1,err)
    NsSum += Ns
    NdataSum += Ndata
    print ' & '.join( [cat,fwhm, "%.1f"%ea['GluGlu'],"%.1f"%ea['VBF'],"%.1f"%ea['WPlusH'],"%.1f"%ea['WMinusH'],"%.1f"%ea['ZH'],"%.1f"%ea['ttH'],"%.1f"%Ns,"%d"%Ndata]),"\\\\"

### overall line

print '\\hline '
print ' & '.join( ["overall","-","%.1f"%eaSum['GluGlu'],"%.1f"%eaSum['VBF'],"%.1f"%eaSum['WPlusH'],"%.1f"%eaSum['WMinusH'],"%.1f"%eaSum['ZH'],"%.1f"%eaSum['ttH'],"%.1f"%NsSum,"%d"%NdataSum]),"\\\\"

print 
print 

print "CAT   "[0:6],
for proc in config.processes:
    print ("  "+ proc + "                ")[0:9],
print ("  DY             ")[0:9],
print ("  TT             ")[0:9],
print

for icat,cat in enumerate(config.categories):
  print (cat+"   ")[0:6],
  for proc in config.processes:
    print "%8.3f"%NsAll[(proc,cat)],
  hDY=fIn.Get(opts.base + "/" + "Mmm_%s_DY"%(cat))
  if hDY != None:
      bin0 = hDY.FindBin(110)
      bin1 = hDY.FindBin(150)
      print "%10.1f"%  ( hDY. Integral(bin0,bin1)*config.lumi()),
  else: 
      print "%10.1f"%(0.0),
  hTT=fIn.Get(opts.base + "/" + "Mmm_%s_TT"%(cat))
  if hTT != None:
      bin0 = hTT.FindBin(110)
      bin1 = hTT.FindBin(150)
      print "%10.1f"%  ( hTT. Integral(bin0,bin1)*config.lumi()),
  else: 
      print "%10.1f"%(0.0),
  print

print 
print 




