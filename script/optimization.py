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
parser.add_option("","--double",dest="double",action='store_true',help="Do Double Boundary optimisation",default=False)

#Iso_Lead_HplusToTauNu-M

(opts,args) = parser.parse_args()

import ROOT

fInput = ROOT.TFile.Open(opts.file)
fOutput = ROOT.TFile.Open(opts.out,"RECREATE")

def RemoveSpikes(h):
	for i in range(2, h.GetNbinsX() ): 
		c = h.GetBinContent(i)
		e = h.GetBinError(i)
		cm = (h.GetBinContent(i-1) + h.GetBinContent(i+1)) /2
		em = math.sqrt( h.GetBinError(i-1)**2 + h.GetBinError(i+1)**2 ) /2
		e1 = h.GetBinError(i-1)
		e2 = h.GetBinError(i+1)

		if c == 0 : continue

		if e > e1*1.5  and e > e2 * 1.5  :
			h.SetBinContent(i,cm)
			h.SetBinError(i,em)

def GetHistoFromFile( nameString ):
	''' Collect from the inputFiles the histograms and Add them'''
	sig = fInput.FindObjectAny( nameString )

	if sig == None:
		if opts.verbose: print "try to add multiple histo:" 
		for s in nameString.split(','):
			if sig == None: 
				sig = fInput.FindObjectAny(s)
				if sig == None: print "Error: no histo",s
			else:
				h_s = fInput.FindObjectAny(s)
				if h_s == None: print "Error: no histo",s
				sig.Add(h_s)
	if sig == None:
		print "ERROR: sig histogram not define: '"+nameString+"'"
	RemoveSpikes(sig)
	return sig


def errorAoAB(a,ea,c,ec):
	''' compute error on a/c assuming c = a+b '''
	if a==0: return 1;

	b = c-a
	eb = math.sqrt( ec*ec - ea * ea)

	e = ( -b/(a**2) * ea )**2 + ( -a* eb) **2
	return math.sqrt(e)

## SIG/BKG PLOT

def ComputeEff(h, norm,i, great=False):
	''' Compute the effeciency of a bin-cut '''
	if norm == 0 : return 0
	eff =h.Integral(1,i) /norm
	if opts.great: eff = h.Integral(i,h.GetNbinsX() ) / norm
	return eff

def CreateRoC(sig,bkg):
	'''Create a RoC plot from the Sig and Bkg'''
	g = ROOT.TGraphAsymmErrors()
	g.SetName("RoC")
	g.SetTitle("RoC")
	sig_norm= sig.Integral()
	bkg_norm= bkg.Integral()
	for i in range(1, sig.GetNbinsX() +1 ):
		sig_eff = ComputeEff(sig,sig_norm,i, opts.great)
		bkg_eff = ComputeEff(bkg,bkg_norm,i, opts.great)
	
		g.SetPoint(g.GetN() , sig_eff, bkg_eff)

	return g
## SIG/BKG PLOT

def CreateSoB(sig,bkg):
	''' Create SoB plot'''
	h= sig.Clone("SoB")
	h.Reset("ICESM")
	sig_norm = sig.Integral()	
	bkg_norm = bkg.Integral()	
	for i in range(1, sig.GetNbinsX() +1 ):
		sig_eff = ComputeEff(sig,sig_norm,i, opts.great)
		bkg_eff = ComputeEff(bkg,bkg_norm,i, opts.great)
	
		if bkg_eff !=0:
			h.SetBinContent(i, sig_eff/bkg_eff)
		else:
			h.SetBinContent(i, 0)
	return h	
## SIG/sBKG PLOT

def CreateSosB(sig,bkg):
	''' Create S/sqrt(B) plots'''
	h2= sig.Clone("SosB")
	h2.Reset("ICESM")
	
	for i in range(1, sig.GetNbinsX() +1 ):
		sig_int = ComputeEff(sig,1,i, opts.great)
		bkg_int = ComputeEff(bkg,1,i, opts.great)

		sig_int *= opts.lumi
		bkg_int *= opts.lumi
	
		if bkg_int !=0:
			h2.SetBinContent(i, sig_int/math.sqrt(bkg_int))
		else:
			h2.SetBinContent(i, 0)
	return h2

## SIG/BKG EFF
def CreateEffPlot(h,name):
	h3= sig.Clone(name)
	h3.Reset("ICESM")

	norm = h.Integral()
	for i in range(1, sig.GetNbinsX() +1 ):
		eff = ComputeEff(h,norm,i, opts.great)
	
		h3.SetBinContent(i, eff)
	
	return h3

def CombineSig(s1,b1,s2,b2,type=2):
	''' Combine significances using 
	type=1 :Fisher discriminant
	type=2 :Profile Likelihood
	type = 3: Sum
	'''
	Z1 = 0
	Z2 = 0

	if b1>0:Z1= s1/math.sqrt(b1)
	if b2>0:Z2= s2/math.sqrt(b2)

	if type == 1:
		p1 = ROOT.RooStats.SignificanceToPValue(Z1)
		p2 = ROOT.RooStats.SignificanceToPValue(Z2)
		chi2 = -2 *( math.log(p1) + math.log(p2) )
		p= ROOT.TMath.Prob(chi2, 4)

		Z = ROOT.RooStats.PValueToSignificance(p)
		return Z
	elif type == 2:
		#gauss function is  N* e^(x-S)^2/sigma^2
		Z = ROOT.TMath.Sqrt( Z1**2 + Z2**2 )
		return Z
	else:
		return Z1+Z2

def CreateCatOpt(sig,bkg,name,start=-1,end=-1):
	''' Create S/sqrt(B) plots'''
	h= sig.Clone("OptCat_"+name)
	h.Reset("ICESM")
	
	for i in range(start, end + 1 ):

		sig_int_cat0 = sig.Integral(start,i) * opts.lumi
		bkg_int_cat0 = bkg.Integral(start,i) * opts.lumi

		sig_int_cat1 = sig.Integral(i+1,end) * opts.lumi
		bkg_int_cat1 = bkg.Integral(i+1,end) * opts.lumi

		Z1= 0	
		Z2= 0
		if bkg_int_cat0 >0 : Z1 = sig_int_cat0 /  math.sqrt( bkg_int_cat0)
		if bkg_int_cat1 >0 : Z2 = sig_int_cat1 /  math.sqrt( bkg_int_cat1)
		Z = CombineSig(sig_int_cat0,bkg_int_cat0,sig_int_cat1,bkg_int_cat1)
		h.SetBinContent(i, Z)
	return h

def FindMaximum(h):
	max = 0
	iMax = -1
	for i in range(1,h.GetNbinsX()+1):
		c = h.GetBinContent(i)
		if max <= c :
			max = c
			iMax = i
	return (iMax,max)

def TwoBoundariesOpt(sig,bkg):
	''' Assume opts.great '''
	if not opts.great: 
		print "Two Boundaries optimization not implemented for less"

	end = sig.GetNbinsX()
	start = 1
	bound = (end -start) /2
	## Optimize the Rejection
	h2 = ROOT.TH2D("TwoBound","Two Bound Opt BinWidth=%.3f"%sig.GetBinWidth(1),end, 1-.5, end-.5, end, 1-.5, end-.5 )
	
	maxStart = -1
	maxBound = -1
	maxZ=-1
	for start in range(1,end):
		if start % 10 == 0 : print " doing %d / %d"%(start,end)
		h = CreateCatOpt(sig,bkg,"start_%i"%start,start,end)
		for bound in range(start,end):
			h2.SetBinContent(start,bound,h.GetBinContent(bound))
		(imax,z) = FindMaximum(h)
		if z > maxZ :
			maxZ = z
			maxStart=start
			maxBound=imax
		h.Write() ############### WRITE###########
	print "--- Cat Opt ---"
	print "maxZ = ",maxZ
	print "maxStart = ",maxStart," -> ",sig.GetBinCenter(maxStart)
	print "maxBound = ",maxBound," -> ",sig.GetBinCenter(maxBound)
	print "---------------"
	return h2


if __name__ == "__main__":
	sig= GetHistoFromFile(opts.sig)
	bkg= GetHistoFromFile(opts.bkg)

	sig.Write("sig_IN")
	bkg.Write("bkg_IN")
	
	CreateEffPlot(sig,"S").Write()
	CreateEffPlot(bkg,"B").Write()
	SosB=CreateSosB(sig,bkg)
	SosB.Write()
	CreateSoB(sig,bkg).Write()
	CreateRoC(sig,bkg).Write()
	
	print " ->  One Boundary"
	(iMax,z) = FindMaximum(SosB)
	print "max one boundary is",iMax,"->",sig.GetBinCenter(iMax)," Z = ",z

	if opts.double:
		print " ->  Starting Two Boundaries optimization"
		fOutput.mkdir("opt")
		fOutput.cd("opt")
		TwoBoundariesOpt(sig,bkg).Write()
	
	## Two Boundaries Optimization
