import os,sys
import math

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)
parser.add_option("-f","--file"	,dest="file"	,type='string',help="InputFile. [Default=%default]",default='TauOpt.root')
parser.add_option("-o","--output",dest="out"	,type='string',help="OutputFile. [Default=%default]",default='TauOptHisto.root')
parser.add_option("-s","--sig"	,dest="sig"	,type='string',help="Sig Histo. [Default=%default]",default='Iso_Match_HplusToTauNu-M200')
parser.add_option("-b","--bkg"	,dest="bkg"	,type='string',help="Bkg Histo. [Default=%default]",default='Iso_Lead_QCD')
parser.add_option("-l","--lumi"	,dest="lumi"	,type='float' ,help="InputFile. [Default=%default]",default=5000)
parser.add_option("-g","--great",dest="great",action='store_true',help="Sig Cut is greater than",default=False)

#Iso_Lead_HplusToTauNu-M

(opts,args) = parser.parse_args()

import ROOT

fInput = ROOT.TFile.Open(opts.file)
fOutput = ROOT.TFile.Open(opts.out,"RECREATE")

sig = fInput.FindObjectAny(opts.sig)
bkg = fInput.FindObjectAny(opts.bkg)

if sig == None:
	print "ERROR: sig histogram not define: '"+opts.sig+"'"
if bkg == None:
	print "ERROR: bkg histogram not define: '"+opts.bkg+"'"

## SIG/BKG PLOT

g = ROOT.TGraphAsymmErrors()
g.SetName("RoC")
g.SetTitle("RoC")

sig_norm = sig.Integral(1,sig.GetNbinsX())
bkg_norm = bkg.Integral(1,bkg.GetNbinsX())

if sig_norm == 0 :
	print "ERROR: check sig normalization"
if bkg_norm == 0 :
	print "ERROR: check bkg normalization"

def errorAoAB(a,ea,c,ec):
	''' compute error on a/c assuming c = a+b '''
	if a==0: return 1;

	b = c-a
	eb = math.sqrt( ec*ec - ea * ea)

	e = ( -b/(a**2) * ea )**2 + ( -a* eb) **2
	return math.sqrt(e)

def computeIntegralENorm(h,i,great=False):
	if great: 
		a = h.Integral(i, h.GetNbinsX() ) 
		ea = 0
		for j in range(i,h.GetNbinsX()+1):
			ea += h.GetBinError(i)**2
	else:
		a = h.Integral(1,i)
		ea = 0
		for j in range(1,i+1):
			ea += h.GetBinError(i)**2
	c = h.Integral()
	ec=0
	for j in range(1,h.GetNbinsX()+1):
		ec += h.GetBinError(i)**2

	ea = math.sqrt(ea)
	ec = math.sqrt(ec)
	
	return errorAoAB(a,ea,c,ec)

def computeIntegralE(h,i,great=False):
	if great: 
		a = h.Integral(i, h.GetNbinsX() ) 
		ea = 0
		for j in range(i,h.GetNbinsX()+1):
			ea += h.GetBinError(i)**2
	else:
		a = h.Integral(1,i)
		ea = 0
		for j in range(1,i+1):
			ea += h.GetBinError(i)**2
	ea = math.sqrt(ea)
	
	return ea

for i in range(1, sig.GetNbinsX() +1 ):
	sig_eff = sig.Integral(1,i) /sig_norm
	bkg_eff = bkg.Integral(1,i) /bkg_norm
	if opts.great: 
		sig_eff = sig.Integral(i,sig.GetNbinsX() ) / sig_norm
		bkg_eff = bkg.Integral(i,bkg.GetNbinsX() ) / bkg_norm

	## 
	#es= computeIntegralENorm(sig,i,opts.great)
	#eb= computeIntegralENorm(bkg,i,opts.great)
	##
	g.SetPoint(g.GetN() , sig_eff, bkg_eff)
	#g.SetPointError(g.GetN() -1,  es,es,eb,eb)

g.Write()
## SIG/BKG PLOT
h= sig.Clone("SoB")
h.Reset("all")

for i in range(1, sig.GetNbinsX() +1 ):
	sig_eff = sig.Integral(1,i) /sig_norm
	bkg_eff = bkg.Integral(1,i) /bkg_norm
	if opts.great: 
		sig_eff = sig.Integral(i,sig.GetNbinsX() ) / sig_norm
		bkg_eff = bkg.Integral(i,bkg.GetNbinsX() ) / bkg_norm

	es= computeIntegralENorm(sig,i,opts.great)
	eb= computeIntegralENorm(bkg,i,opts.great)

	if bkg_eff !=0:
		h.SetBinContent(i, sig_eff/bkg_eff)
		#e  = math.sqrt((es/sig_eff)**2 + (eb/bkg_eff)**2) * sig_eff/bkg_eff
		#h.SetBinError(i, e)
	else:
		h.SetBinContent(i, 0)
		#h.SetBinError(i, 1)

h.Write()
## SIG/sBKG PLOT
h2= sig.Clone("SosB")
h2.Reset("all")

for i in range(1, sig.GetNbinsX() +1 ):
	sig_int = sig.Integral(1,i) 
	bkg_int = bkg.Integral(1,i) 
	if opts.great: 
		sig_int = sig.Integral(i,sig.GetNbinsX() ) 
		bkg_int = bkg.Integral(i,bkg.GetNbinsX() )
	sig_int *= opts.lumi
	bkg_int *= opts.lumi
	es = computeIntegralE(sig,i,opts.great)
	eb = computeIntegralE(bkg,i,opts.great)

	if bkg_int !=0:
		#e = (1/math.sqrt(bkg_int) * es)**2 + ( -1./2. * sig_int * bkg_int**(-3./2.) *eb) **2
		#e = math.sqrt(e)
		h2.SetBinContent(i, sig_int/math.sqrt(bkg_int))
		#h2.SetBinError(i,e)
	else:
		h2.SetBinContent(i, 0)
		#h2.SetBinError(i, 1)
h2.Write()


