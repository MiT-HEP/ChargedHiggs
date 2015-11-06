import os,sys
import math
from subprocess import call,check_output

#### Threading
import threading
import time

from optparse import OptionParser


parser = OptionParser()

parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)
parser.add_option("-f","--file"	,dest="file"	,type='string',help="InputFile. [Default=%default]",default='TauOpt.root')
parser.add_option("-o","--output",dest="out"	,type='string',help="OutputFile. [Default=%default]",default='TauOptHisto.root')
parser.add_option("-s","--sig"	,dest="sig"	,type='string',help="Sig Histo. [Default=%default]",default='Iso_Match_HplusToTauNu-M200')
parser.add_option("-b","--bkg"	,dest="bkg"	,type='string',help="Bkg Histo. [Default=%default]",default='Iso_Lead_QCD')
parser.add_option("-l","--lumi"	,dest="lumi"	,type='float' ,help="InputFile. [Default=%default]",default=5000)
parser.add_option("-g","--great",dest="great",action='store_true',help="Sig Cut is greater than, instead of less than",default=False)

parser.add_option("","--double",dest="double",action='store_true',help="Do Double Boundary optimisation. (single variable, two cat)",default=False)

parser.add_option("","--2D",dest="twoD",action='store_true',help="Do a two2D optimization simultaneously vs Mt. (eg RbbMinVsMt)",default=False)

parser.add_option("","--twoBound2D",dest="doubleTwoD",action='store_true',help="Do Double Boundary optimisation. (with two cat, eg Bdt0 vs Bdt1)",default=False)

#Iso_Lead_HplusToTauNu-M

(opts,args) = parser.parse_args()

import ROOT

fInput = ROOT.TFile.Open(opts.file)
fOutput = ROOT.TFile.Open(opts.out,"RECREATE")

if fInput == None:
	print "'"+opts.file+"': No such file or directory"

def RemoveSpikes(h, eonly = True,n = 1):
	for i in range(1+n, h.GetNbinsX() + 1 -n ): 
		c = h.GetBinContent(i)
		e = h.GetBinError(i)
		cm = (h.GetBinContent(i-n) + h.GetBinContent(i+n)) /2
		em = math.sqrt( h.GetBinError(i-n)**2 + h.GetBinError(i+n)**2 ) /2
		e1 = h.GetBinError(i-n)
		e2 = h.GetBinError(i+n)

		if c == 0 : continue

		if e > e1*1.5  and e > e2 * 1.5  :
			h.SetBinContent(i,cm)
			h.SetBinError(i,em)

		if not eonly and  cm > 1.5*c:
			h.SetBinContent(i,cm)

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

def ComputeEff2D(h, norm, x, y, great=False):
	''' Compute the effeciency of a bin-cut '''
	if norm == 0 : return 0
	#Integral (Int_t binx1, Int_t binx2, Int_t biny1, Int_t biny2, Option_t *option="") const 

	if opts.great:
		eff =  h.Integral(x, h.GetNbinsX(), y,h.GetNbinsY())/ norm
	else:
		eff =h.Integral(1,x,1,y) /norm
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
	
		if bkg_int >0:
			h2.SetBinContent(i, sig_int/math.sqrt(bkg_int))
		else:
			h2.SetBinContent(i, 0)
	return h2

def CreateSosB2D(sig,bkg):
	''' Create a 2D S/sqrt(B) plots'''
	h2= sig.Clone("SosB")
	h2.Reset("ICESM")
	
	tot=float(sig.GetNbinsX() *sig.GetNbinsY())
	for x in range(1,sig.GetNbinsX() + 1):
	   for y in range(1,sig.GetNbinsY() + 1):
		idx= (x-1)*sig.GetNbinsX()  + (y-1)
		if idx%100==0:
			print "\r** Doing Entry",idx,"of",tot,": ",idx/tot*100,"%",
			sys.stdout.flush()
		sig_int = ComputeEff2D(sig,1,x,y, opts.great)
		bkg_int = ComputeEff2D(bkg,1,x,y, opts.great)

		sig_int *= opts.lumi
		bkg_int *= opts.lumi
	
		if bkg_int >0:
			h2.SetBinContent(x,y, sig_int/math.sqrt(bkg_int))
		else:
			h2.SetBinContent(x,y, 0)
	print
	return h2


## SIG/BKG EFF
def CreateEffPlot(h,name):
	h3= h.Clone(name)
	h3.Reset("ICESM")

	norm = h.Integral()
	for i in range(1, sig.GetNbinsX() +1 ):
		eff = ComputeEff(h,norm,i, opts.great)
	
		h3.SetBinContent(i, eff)
	
	return h3

def CreateEffPlot2D(h,name=""):
	h2 = h.Clone(name)
	h2.Reset("ICESM")
	norm = h.Integral()
	tot=h.GetNbinsX()*h.GetNbinsY()
	for x in range(1,h.GetNbinsX() + 1):
	   for y in range(1,h.GetNbinsY() + 1):
	       idx=(x-1)*h.GetNbinsX()+(y-1)
	       if idx%100 ==0 : 
		       print "\r** Doing entry",idx,"of",tot,": ",float(idx)/tot*100,"%",
		       sys.stdout.flush
	       eff =  ComputeEff2D(h,norm,x,y,opts.great)
	       h2.SetBinContent(x,y,eff);
	print ## newline
	return h2


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

def FindMaximum2D(h):
	max = 0
	iMax = (-1,-1)
	for x in range(1,h.GetNbinsX()+1):
	  for y in range(1,h.GetNbinsY()+1):
		c = h.GetBinContent(x,y)
		if max <= c :
			max = c
			iMax = (x,y)
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

def TwoBoundariesOpt2D(sig,bkg):
	''' Two boundaries (cat0,cat1) optimization in S/SqrtB like for Bdt0,Bdt1 pair '''
	if not opts.great: 
		print "Two Boundaries optimization not implemented for less"
	print "->Starting 2D optimization for ",sig.GetName(),"and",bkg.GetName()

	startX=1	
	endX=sig.GetNbinsX()
	startY=1	
	endY=sig.GetNbinsX()

	boundA=(startX+1,startY+1)
	boundB=(startX,startY)

	##   -------
	##   |   A |
	##   | B   |
	##   -------

	Z=-1
	boundaries = (boundA,boundB)

	### startX +1 -> remove the 1 boundary case
	### endX +1 -> include the endX in the loop 
	#bAlist = [ (x,y) for x in range(startX+1,endX+1) for y in range(startY+1,endY+1) ]
	
	hTemplate = ROOT.TH2D("hTemplate","hTempl", endX-startX+1, startX-0.5 ,endX+0.5, endY-startY+1, startY-0.5,endY-.5)

	#optimize boundA
	#optimize boundB
	# reinterate 3 times

	for nIter in range(0,3):
		print "->Doing iteration",nIter
		boundB=boundaries[1]
		startX=boundB[0]
		startY=boundB[1]
		#update the value of valid A boundaries
		bAlist = [ (x,y) for x in range(startX+1,endX+1) for y in range(startY+1,endY+1) ]
		sigAB = ComputeEff2D(sig,1, boundB[0],boundB[1],True)
		bkgAB = ComputeEff2D(bkg,1, boundB[0],boundB[1],True)
		for idx,boundA in enumerate(bAlist):
			## end -> <boundA, excluding boundA
			boundB=boundaries[1]
			if True:
				print "\r **Doing element",idx,"of",len(bAlist),". ",float(idx)/len(bAlist)*100,"%",
				sys.stdout.flush()
			sigA = ComputeEff2D(sig,1, boundA[0],boundA[1],True)
			sigB=sigAB-sigA
			bkgA = ComputeEff2D(bkg,1, boundA[0],boundA[1],True)
			bkgB=bkgAB-bkgA

			Ztmp = CombineSig(sigA,bkgA,sigB,bkgB)
			isSpike=False
			n=2
			if not isSpike:
			   for perm in [(0,n),(0,-n),(n,0),(-n,0)]:
				sigA = ComputeEff2D(sig,1, boundA[0]+perm[0],boundA[1]+perm[1],True)
				bkgA = ComputeEff2D(bkg,1, boundA[0]+perm[0],boundA[1]+perm[1],True)
				sigB=sigAB-sigA
				bkgB=sigAB-sigA
				Ztmp2=CombineSig(sigA,bkgA,sigB,bkgB)
				if Ztmp >0 and abs(Ztmp2-Ztmp)/Ztmp>0.05 : isSpike=True
				if isSpike:break

			if Ztmp > Z and not isSpike: 
				boundaries = (boundA,boundB)
				Z=Ztmp

		boundA=boundaries[0]
		hCurrent=hTemplate.Clone("hComb_A_%d_%d"%(boundA[0],boundA[1]))
		#update the Blist on the A boundary
		bBlist= [ (x,y) for x in range( startX,boundA[0] ) for y in range(startY,boundA[1] )]
		sigA = ComputeEff2D(sig,1, boundA[0],boundA[1],True)
		bkgA = ComputeEff2D(bkg,1, boundA[0],boundA[1],True)
		for boundB in bBlist:	
			boundA=boundaries[0]
			sigAB = ComputeEff2D(sig,1, boundB[0],boundB[1],True)
			sigB=sigAB-sigA
			bkgAB = ComputeEff2D(bkg,1, boundB[0],boundB[1],True)
			bkgB=bkgAB-bkgA

			Ztmp = CombineSig(sigA,bkgA,sigB,bkgB)
			mybin= hCurrent.FindBin(boundB[0],boundB[1])
			hCurrent.SetBinContent(mybin,Ztmp)

			isSpike=False
			n=2
			if not isSpike:
			   for perm in [(0,n),(0,-n),(n,0),(-n,0)]:
				sigAB = ComputeEff2D(sig,1, boundB[0]+perm[0],boundB[1]+perm[1],True)
				bkgAB = ComputeEff2D(bkg,1, boundB[0]+perm[0],boundB[1]+perm[1],True)
				sigB=sigAB-sigA
				bkgB=sigAB-sigA
				Ztmp2=CombineSig(sigA,bkgA,sigB,bkgB)
				if Ztmp >0 and abs(Ztmp2-Ztmp)/Ztmp>0.05 : isSpike=True
				if isSpike:break

			if Ztmp > Z and not isSpike : 
				boundaries = (boundA,boundB)
				Z=Ztmp
			## save hCurrent
		hCurrent.Write()

	outstr="--- Cat opt 2D --- \n"
	outstr += "maximum found at bin (" + str(boundA[0]) + " , " + str(boundA[1]) + ") + (" + str(boundB[0]) + " , " + str(boundB[1]) + ")\n"
	outstr += "maximum found at cut values = (" + str(sig.GetXaxis().GetBinLowEdge(boundA[0])) + " , " + str(sig.GetXaxis().GetBinLowEdge(boundA[1]) ) + ") + (";
	outstr += str(sig.GetYaxis().GetBinLowEdge(boundB[0])) + " , "+ str(sig.GetYaxis().GetBinLowEdge(boundB[1]) ) + ")\n"
	outstr += "Z= " + str(Z)  +"\n"
	outstr += "-----------------"

	print
	print
	print outstr
	n=ROOT.TNamed("outstr",outstr);
	n.Write()

def DiscrPower(h1,h2):
	''' Return the discrimination power'''
	n1 = h1.Integral()
	n2 = h2.Integral()
	
	if n1 == 0 : return 0;
	if n2 == 0 : return 1;

	h1.Scale(1./n1 ) 
	h2.Scale(1./n2 ) 

	D=0
	for i in range(1,h1.GetNbinsX() + 1):
		#D+= math.fabs(  h1.GetBinContent(i) - h2.GetBinContent(i) )
		diff = math.fabs(  h1.GetBinContent(i) - h2.GetBinContent(i) )
		sum  =  h1.GetBinContent(i)  +  h2.GetBinContent(i)
		if sum == 0 : continue
		D+= math.fabs(  diff*diff/sum )
	D/=2.0
	return D
	

def CombinedSign(h1,h2):
	''' Return the combined significance in all the bins'''
	Z=0
	for i in range(1,h1.GetNbinsX() + 1):
		if h2.GetBinContent(i) >0 :
			Zi = h1.GetBinContent(i) / math.sqrt( h2.GetBinContent(i) )
			Z += Zi*Zi
	Z= math.sqrt(Z)
	return Z

def Combine(h1,h2,postfix="",dir="/tmp/%(USER)s"):
	''' Run combine '''
	wdir = dir%os.environ + "/"

	print "running combination on hists: ",h1.GetName(),h2.GetName()," in wdir =",wdir

	## combine will not work
	if h1.Integral() == 0 : return 1e16
	if h2.Integral() == 0 : return 1e16

	f = ROOT.TFile.Open(wdir + "combination%s.root"%postfix,"RECREATE")
	h1.Clone("sig").Write()
	h2.Clone("bkg").Write()
	data= h1.Clone("data") ## no data, but needed to convert datacard into ws, I put an asimov instead
	data.Add(h2)
	data.Write()
	f.Close()
	print "writing datacard in ",wdir+"datacard%s.txt"%postfix 
	dat = open(wdir+"datacard%s.txt"%postfix ,"w")

	dat.write("-------------------------------------\n")
	dat.write("imax *                               \n")
	dat.write("jmax *                               \n")
	dat.write("kmax *                               \n")
	dat.write("-------------------------------------\n")
	dat.write("shapes data_obs cat0  combination%s.root  data \n"%postfix)
	dat.write("shapes sig      cat0  combination%s.root  sig\n"%postfix)
	dat.write("shapes bkg      cat0  combination%s.root  bkg\n"%postfix)
	dat.write("-------------------------------------\n")
	dat.write("bin cat0                             \n")
	dat.write("observation -1                       \n")
	dat.write("-------------------------------------\n")
	dat.write("bin     cat0    cat0                 \n")
	dat.write("process sig     bkg                  \n")
	dat.write("process 0       1                    \n")
	dat.write("rate    -1      -1                   \n")
	dat.write("-------------------------------------\n")

	dat.close()

	cmd = "cd " + wdir + " && " 
	cmd += "combine -M Asymptotic -m 200 -S 0  --cminDefaultMinimizerType=Minuit2 --run=expected --expectSignal=1  --rMax=1000000  datacard%s.txt"%postfix  ## mass is meaningless

	o = check_output(cmd, shell=True)

	median = -1
	print "----------------------------"
	for line in o.split('\n'):
		#Expected 50.0%: r < 1211.5479
		print line
		if line.startswith('Expected 50.0%:'):
			str=line.split('<')[1].replace(' ' ,'')
			median = float(str)
	print "----------------------------"
	print "   ---> Expected r < ",median
	return median

def CleanForCombine(px_s,px_b):
	''' The spike removal can be slow ... put it in the thread '''

	RemoveSpikes(px_s,False) ## look also at the content
	RemoveSpikes(px_b,False)

	RemoveSpikes(px_s,False,2)
	RemoveSpikes(px_b,False,2)

	RemoveSpikes(px_s,False,3)
	RemoveSpikes(px_b,False,3)

	#px_s.Smooth(20)
	#px_b.Smooth(20)
	return 


class CombineThread(threading.Thread):
	def __init__(self, sig, bkg, postfix):
		threading.Thread.__init__(self)
		self.sig = sig
		self.bkg = bkg
		self.postfix=postfix
		self.result = -1
	def run(self):
		print "Starting " + self.postfix
		CleanForCombine( self.sig,self.bkg)
		r = Combine( self.sig, self.bkg, self.postfix ) 
		self.result = r
		print "Exiting " + self.postfix

class DiscrPowerThread(threading.Thread):
	def __init__(self, sig, bkg):
		threading.Thread.__init__(self)
		self.sig = sig
		self.bkg = bkg
		self.result= -1
	def run(self):
		CleanForCombine(self.sig, self.bkg)
		self.result = DiscrPower(self.sig,self.bkg)

class CombinedSignThread(threading.Thread):
	def __init__(self, sig, bkg):
		threading.Thread.__init__(self)
		self.sig = sig
		self.bkg = bkg
		self.result= -1
	def run(self):
		CleanForCombine(self.sig, self.bkg)
		self.result=CombinedSign( self.sig,self.bkg)

	
def TwoD(sig,bkg,type='DZ'):
	''' Create the figure of merit for the two di signal. Correlated with Mt.
	Mt is the X 
	'''
	nY = sig.GetNbinsY()
	nX = sig.GetNbinsX()

	if type == 'ZD' : type = 'DZ'

	name = "Discr"
	if type == 'Z' : name = "CombinedSign"
	if type == 'DZ' : name = 'CombinedDiscr'
	if type == 'A' : name = 'Asymptotic'

	h = ROOT.TH1D(name,name, nY, sig.GetYaxis().GetBinLowEdge(1), sig.GetYaxis().GetBinLowEdge(nY+1) )

	threads = {}

	for y in range(1,nY+1):
		print "\r * " + str(y) + "/" + str(nY) + " | ",

		sys.stdout.flush()

		if opts.great:	
			px_s = sig.ProjectionX("s_px_bin%d"%y, y, nY)
			px_b = bkg.ProjectionX("b_px_bin%d"%y, y, nY)
		else :
			px_s = sig.ProjectionX("s_px_bin%d"%y, 0, y)
			px_b = bkg.ProjectionX("b_px_bin%d"%y, 0, y)

		while threading.activeCount() > 15 : 
			print "sleep ...",
			time.sleep(3)
			print "wake up!"

		if type == 'D':	
			threads[y] = DiscrPowerThread(px_s,px_b)
			threads[y].start()
			#d=DiscrPower( px_s, px_b) ## rescale px_s, and px_b
			#h.SetBinContent(y, d) 

		if type == 'A':	
			threads[y] = CombineThread( px_s,px_b, "_bin%d"%y)	
			threads[y].start()
			#a=Combine(px_s,px_b) ## Full combination
			#h.SetBinContent(y, a) 

		if type == 'Z':	
			threads[y] = CombinedSignThread( px_s,px_b)	
			threads[y].start()
			#CleanForCombine(px_s,px_b)
			#z=CombinedSign( px_s,px_b)
			#h.SetBinContent(y, z) 

		########## NOT THREADED YET ###########

		if type == 'DZ':
			CleanForCombine(px_s,px_b)
			z=CombinedSign( px_s,px_b)
			d=DiscrPower( px_s, px_b) ## rescale px_s, and px_b
			h.SetBinContent(y, d*z) 
		
	## end all threads, and get results
	for y in range(1,nY+1):
		if type == 'A' or type == 'D' or type == 'Z':	
			threads[y].join()
			a=threads[y].result
			h.SetBinContent(y, a) 

	return h


if __name__ == "__main__":
	sig= GetHistoFromFile(opts.sig)
	bkg= GetHistoFromFile(opts.bkg)

	sig.Write("sig_IN")
	bkg.Write("bkg_IN")

	if not opts.twoD and not opts.doubleTwoD: 
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

	if opts.doubleTwoD:
		## TODO
		print "TODO"
		print "-> 2D, Two boundaries"
		print "*** Eff Sig"
		CreateEffPlot2D(sig,"S").Write()
		print "*** Eff Bkg"
		CreateEffPlot2D(bkg,"B").Write()
		print "*** Eff SoSB"
		CreateSosB2D(sig,bkg).Write()
		# this will also write what it needs
		fOutput.mkdir("opt2D")
		fOutput.cd("opt2D")
		print "*** 2D Opt"
		TwoBoundariesOpt2D(sig,bkg)
		fOutput.cd("")


	if opts.twoD:
		print "-> 2D Discr Power"
		TwoD(sig,bkg,'D').Write()
		print "-> 2D Combined Significance"
		TwoD(sig,bkg,'Z').Write()
		#print "-> 2D Combined Significance * Discr Power"
		#TwoD(sig,bkg,'DZ').Write()
		print "-> 2D Discr Asymptotic"
		discr = TwoD(sig,bkg,'A')
		fOutput.cd() ### Asymptotic will open a set of root files
		discr.Write()
		discr.Scale(-1) ## asymptotic limit, want the minimum
		print " ->  One Boundary Correlated with Mt"
		(iMax,z) = FindMaximum(discr)
		print "max one boundary is",iMax,"->",discr.GetBinCenter(iMax)," Z = ",z

	
	## Two Boundaries Optimization
