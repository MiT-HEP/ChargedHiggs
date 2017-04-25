from optparse import OptionParser,OptionGroup
import ROOT
from array import array
import math

ROOT.gROOT.SetBatch(True) 
ROOT.gROOT.IsBatch()

parser= OptionParser()
parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/APR1_finalSYST/1l/1l.root")
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/APR1_finalSYST/2l/2l.root")
parser.add_option("","--base",type='string',help="Base [Default=%default]", default="topCR")
parser.add_option("","--histo",type='string',help="Base [Default=%default]", default="bdt1_SR4_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-k","--lepCat",type='string',help = "Which test runs. [%default]", default="1Mu") 

opts, args = parser.parse_args()

## original
def Rebin1LTop(h):
        ''' Rebin with un-even bins '''
	## up to 2000
	mybins=array('d',[0, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 820, 840, 860, 880, 900, 920, 940, 960, 1000, 1050, 1100, 1150, 1200, 1250, 1350, 1500, 1600, 1750, 2000, 2500, 3000, 4000, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def Rebin1LCharm(h):
        ''' Rebin with un-even bins '''
	## up to 1000
	mybins=array('d',[0, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 420, 440, 460, 480, 500, 550, 600, 700, 800, 1000, 1250, 1500, 2000, 3000, 4000, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def Rebin1LExtraRad(h):
        ''' Rebin with un-even bins '''
	## up to 2500
        mybins=array('d',[0, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 810, 820, 830, 840, 850, 860, 880, 900, 920, 940, 960, 980, 1000, 1020, 1040, 1060, 1080, 1100, 1120, 1150, 1200, 1250, 1300, 1350, 1400, 1500, 1600, 1800, 2000, 2300, 2600, 3000, 4000, 6000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1

def Rebin1L(h):
	#original
	mybins=array('d',[0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 810, 820, 830, 840, 850, 860, 870, 880, 890, 900, 910, 920, 930, 940, 950, 960, 970, 980, 990, 1000, 1020, 1040, 1060, 1080, 1100, 1120, 1140, 1160, 1180, 1200, 1220, 1240, 1260, 1280, 1300, 1320, 1340, 1360, 1380, 1400, 1420, 1440, 1460, 1480, 1500, 1550, 1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def Rebin2L(h):
        ''' Rebin with un-even bins '''
	#original
        mybins=array('d',[0, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760, 780, 800, 820, 840, 860, 880, 900, 920, 940, 960, 980, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1750, 1800, 1850, 1900, 1950, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3500, 4000, 4500, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def RebinN(h,nBin):
        ''' Rebin with un-even bins '''
        h1=h.Rebin(nBin)
        return h1


def RebinBDT1(h):
        ''' Rebin with un-even bins '''
        mybins=array('d',[-1,
                           -0.99,-0.98,-0.97,-0.96,-0.95,-0.94,-0.93,-0.92,-0.91,-0.9,
                           -0.89,-0.88,-0.87,-0.86,-0.85,-0.84,-0.83,-0.82,-0.81,-0.8,
                           -0.79,-0.78,-0.77,-0.76,-0.75,-0.74,-0.73,-0.72,-0.71,-0.7,
			   -0.68,-0.66,-0.64,-0.62,-0.60,
			   -0.58,-0.56,-0.54,-0.52,-0.50,
			   -0.48,-0.46,-0.44,-0.42,-0.40,
                           -0.36,-0.32,
                           -0.28,-0.24,-0.2,
                           -0.16,-0.12,
			   -0.08,-0.04,
                           0.0,0.04,0.08,
                           0.12,0.16,0.2,
                           0.24,0.28,
                           0.32,0.36,
			   0.40,0.42,0.44,0.46,0.48,0.5,
			   0.50,0.52,0.54,0.56,0.58,0.6,
			   0.60,0.62,0.64,0.66,0.68,0.7,
                           0.71,0.72,0.73,0.74,0.75,0.76,0.77,0.78,0.79,0.8,
                           0.81,0.82,0.83,0.84,0.85,0.86,0.87,0.88,0.89,0.9,
                           0.91,0.92,0.93,0.94,0.95,0.96,0.97,0.98,0.99,
                           1.])
##      mybins=array('d',[-1.,-0.8,-0.6,-0.4,-0.3,-0.2,-0.1,0.,0.1,0.2,0.3,0.4,0.6,0.8,1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1


def RebinBDT2(h):
        ''' Rebin with un-even bins '''
	print 'using BDT2'
        mybins=array('d',[-1,
                           -0.9,
			   -0.85,-0.8,
                           -0.78,-0.76,-0.74,-0.72,-0.7,
                           -0.69,-0.68,-0.67,-0.66,-0.65,-0.64,-0.63,-0.62,-0.61,-0.6,
                           -0.59,-0.58,-0.57,-0.56,-0.55,-0.54,-0.53,-0.52,-0.51,-0.5,
                           -0.49,-0.48,-0.47,-0.46,-0.45,-0.44,-0.43,-0.42,-0.41,-0.4,
                           -0.39,-0.38,-0.37,-0.36,-0.35,-0.34,-0.33,-0.32,-0.31,-0.3,
                           -0.29,-0.28,-0.27,-0.26,-0.25,-0.24,-0.23,-0.22,-0.21,-0.2,
                           -0.19,-0.18,-0.17,-0.16,-0.15,-0.14,-0.13,-0.12,-0.11,-0.1,
                           -0.09,-0.08,-0.07,-0.06,-0.05,-0.04,-0.03,-0.02,-0.01,-0.,
                           0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1,
                           0.11,0.12,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.2,
                           0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.3,
                           0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.4,
                           0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.5,
                           0.51,0.52,0.53,0.54,0.55,0.56,0.57,0.58,0.59,0.6,
                           0.61,0.62,0.63,0.64,0.65,0.66,0.67,0.68,0.69,0.7,
			   0.72,0.74,0.76,0.78,0.8,
			   0.85,0.9,
                           1.])

##      mybins=array('d',[-1.,-0.8,-0.6,-0.4,-0.3,-0.2,-0.1,0.,0.1,0.2,0.3,0.4,0.6,0.8,1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1


def RebinBDT3(h):
        ''' Rebin with un-even bins '''
## this is equivalent to rebin5

        print 'using binning BDT3'
        mybins=array('d',[-1,
                           -0.95,-0.9,
                           -0.85,-0.8,
                           -0.75,-0.7,
                           -0.65,-0.6,
                           -0.55,-0.5,
                           -0.45,-0.42,-0.4,
                           -0.38,-0.37,-0.36,-0.35,-0.34,-0.33,-0.32,-0.31,-0.3,
                           -0.29,-0.28,-0.27,-0.26,-0.25,-0.24,-0.23,-0.22,-0.21,-0.2,
                           -0.19,-0.18,-0.17,-0.16,-0.15,-0.14,-0.13,-0.12,-0.11,-0.1,
                           -0.09,-0.08,-0.07,-0.06,-0.05,-0.04,-0.03,-0.02,-0.01,-0.,
                           0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1,
                           0.11,0.12,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.2,
                           0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.3,
                           0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.4,
                           0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.5,
                           0.52,0.54,0.56,0.58,0.6,
                           0.65,0.7,
                           0.75,0.8,
                           0.85,0.9,
                           0.95,
                           1.])
##      mybins=array('d',[-1.,-0.8,-0.6,-0.4,-0.3,-0.2,-0.1,0.,0.1,0.2,0.3,0.4,0.6,0.8,1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

#def RebinBDT(h):
#	''' Rebin with un-even bins '''
#	mybins=array('d',[-1,-0.8,-0.6,-0.4,-0.2,0.,0.2,0.4,0.6,0.8,1.])
# 200 bins between -1,1steps od 0.01
#	mybins=array('d',[-1,-0.9,-0.8,-0.6,-0.4,-0.2,0.,0.2,0.4,0.6,0.8,1.])
#        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
#        return h1


#fIn=ROOT.TFile.Open(opts.input)
#if fIn==None:
#	print "[ERROR] File", opts.input, "doesn't exist"
#	raise IOError
#h=fIn.Get(opts.base)

fIn1L = ROOT.TFile.Open(opts.input1L,"READ")
fIn2L = ROOT.TFile.Open(opts.input2L,"READ")

if fIn1L == None:
        print "ERROR: file",opts.input1L,"doesn't exist"
        exit(1)
if fIn2L == None:
	print "ERROR: file",opts.input2L,"doesn't exist"
        exit(1)

if opts.lepCat == "1Mu" or opts.lepCat  == "1Ele": h=fIn1L.Get("ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo)
else : h=fIn2L.Get("ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo )

print "->histo -->","ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo

if h==None:
	print "[ERROR] Hist", opts.base, "doesn't exist"
	raise IOError

hL=[]


if opts.syst!='Pdf' and opts.syst != 'Scale':
	if opts.lepCat == "1Mu" or opts.lepCat  == "1Ele": 
		print "->","ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo+ "_" + opts.syst+"Up"
		hUp=fIn1L.Get("ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo+ "_" + opts.syst+"Up")
		hDown=fIn1L.Get("ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo+ "_" + opts.syst+"Down")
	else : 
		print "->","ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo+ "_" + opts.syst+"Up"
		hUp=fIn2L.Get("ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo+ "_" + opts.syst+"Up")
		hDown=fIn2L.Get("ChargedHiggsTopBottom/" + opts.base + "_" + opts.lepCat + "/" + opts.histo+ "_" + opts.syst+"Down")
	if hUp==None or hDown==None:
		print "[ERROR] Hist", opts.base+"_"+opts.syst+"Up/Down", "doesn't exist"
		raise IOError
	hL = [hUp,hDown]
elif opts.syst=='Scale':
	for w in [ 'R','F','RF']:
		for s in ['Up','Down']:
			hTmp=fIn.Get("ChargedHiggsTopBottom/" + opts.base+"_" + opts.lepCat + "/" + opts.histo+"_Scale"+w+s)
			if hTmp==None:
				print "[ERROR] Hist", opts.base+"_Scale"+w+s, "doesn't exist"
				raise IOError
			hL.append(hTmp)
elif opts.syst=='Pdf':
	for i in range(0,100):
		hTmp=fIn.Get("ChargedHiggsTopBottom/" + opts.base+"_" + opts.lepCat + "/" + opts.histo+"_Pdf%d"%i)
		if hTmp==None:
			print "[ERROR] Hist", opts.base+"_Pdf%d"%i, "doesn't exist"
			raise IOError
		hL.append(hTmp)


for j in range(h.GetNbinsX()):
	if h.GetBinContent(j) < 0:
		print '$$$$$$$$$$$$$$$$$$$$'
		print 'Andrea negative test'
		print '$$$$$$$$$$$$$$$$$$$$'

for idx,hTmp in enumerate(hL):
	for j in range(hL[idx].GetNbinsX()):
		if hL[idx].GetBinContent(j) < 0:
			print '$$$$$$$$$$$$$$$$$$$$'
			print 'Andrea negative test'
			print '$$$$$$$$$$$$$$$$$$$$'

if opts.rebin> 10:
	if "HT" in opts.histo:
	#HT
		print ' ... HT bins'
#		h.Smooth(2);
		if opts.lepCat == "1Mu" or opts.lepCat == "1Ele":
			if opts.base == "topCR": h=Rebin1LTop(h)
			if opts.base == "extraRadCR" or opts.base == "Baseline" or  opts.base == "Baseline_SR1" or opts.base == "Baseline_SR2" or opts.base == "Baseline_SR3" or opts.base == "Baseline_SR4": h=Rebin1LExtraRad(h)
			if opts.base == "charmCR": h=Rebin1LCharm(h)
#			h=Rebin1L(h)
		else:
			h=Rebin2L(h)
		for idx,hTmp in enumerate(hL):
#			hL[idx].Smooth(2);
			if opts.lepCat == "1Mu" or opts.lepCat == "1Ele":
				if opts.base == "topCR": hL[idx]=Rebin1LTop(hTmp)
				if opts.base == "extraRadCR" or opts.base == "Baseline": hL[idx]=Rebin1LExtraRad(hTmp)
				if opts.base == "charmCR": hL[idx]=Rebin1LCharm(hTmp)
			else:
				hL[idx]=Rebin2L(hTmp)

	#BDT
	if "bdt" in opts.histo:
		print ' ... using BDT bins'
#		h.Smooth(2);
		if "bdt3" in opts.histo: h=RebinBDT3(h)
		if "bdt2" in opts.histo: h=RebinBDT2(h)
		if "bdt1" in opts.histo: h=RebinBDT1(h)

		if "SR2" in opts.histo or "SR4" in opts.histo: h.Rebin(2)

		for idx,hTmp in enumerate(hL):
#			hL[idx].Smooth(2);
			if "bdt3" in opts.histo: hL[idx]=RebinBDT3(hTmp)
			if "bdt2" in opts.histo: hL[idx]=RebinBDT2(hTmp)
			if "bdt1" in opts.histo: hL[idx]=RebinBDT1(hTmp)
			if "SR2" in opts.histo or "SR4" in opts.histo: hL[idx].Rebin(2)

else:
	h.Rebin(opts.rebin)

#for idx,hTmp in enumerate(hL):
#	if opts.rebin> 999:
#		hL[idx]=Rebin(hTmp)
#	else:
#		hTmp.Rebin(opts.rebin)
#	#hUp.Rebin(opts.rebin)
#	#hDown.Rebin(opts.rebin)


#########################################################
################### IMPORT STAT #########################
#########################################################

hupbin=h.Clone(h.GetName() +"_STATUp")
hdnbin=h.Clone(h.GetName() +"_STATDown")

minError=-1
for iBin in range(1,h.GetNbinsX()+1):
	c= h.GetBinContent(iBin)
	if c<=0: continue;
	e=h.GetBinError(iBin)
	if e>0 and (e<minError or minError<0):
		minError=e
for iBin in range(1,h.GetNbinsX()+1):
	c=h.GetBinContent(iBin)
	if c>0: continue
	h.SetBinError(iBin,minError)
for i in range(0,h.GetNbinsX()):
	cont=h.GetBinContent(i+1)
	err = h. GetBinError(i+1) ## err is referred to the sum
	c = h.GetBinContent(i+1) ## this is to check the magnitude, not to apply it

	if c > 0 and err/c <.01: continue ## don't write less than 1%

	hupbin.SetBinContent(i+1,cont+err)
	hdnbin.SetBinContent(i+1,cont-err)


hLStat = [hupbin,hdnbin]

#########################################################
#########################################################
#########################################################

h.SetLineColor(ROOT.kBlack)

if len(hL)==2:
	colors=[ROOT.kBlue,ROOT.kRed]
elif len(hL)==6:
	colors=[ROOT.kBlue,ROOT.kBlue-4,ROOT.kRed,ROOT.kRed-4,ROOT.kGreen+2,ROOT.kGreen]
else:
	colors=[ 
		 ROOT.kOrange-7,ROOT.kOrange-4,ROOT.kOrange,ROOT.kOrange+2,
		 ROOT.kRed+2,ROOT.kRed,ROOT.kRed-4,ROOT.kRed-7,
		 ROOT.kMagenta-7,ROOT.kMagenta-4,ROOT.kMagenta,ROOT.kMagenta+2,
		 ROOT.kBlue+2,ROOT.kBlue,ROOT.kBlue-4,ROOT.kBlue-7,
		 ROOT.kCyan,
		 ROOT.kGreen-7,ROOT.kGreen-4,ROOT.kGreen,ROOT.kGreen+2
		 ]
for hTmp in hL:
	mycolor=colors[0]
	colors=colors[1:]+ [mycolor]
	hTmp.SetLineColor(mycolor)

h.SetLineWidth(2)

for hTmp in hL:
	hTmp.SetLineWidth(2)
	hTmp.SetLineStyle(7)


for hTmp in hLStat:
	hTmp.SetLineWidth(4)
	hTmp.SetLineStyle(7)
	hTmp.SetLineColor(ROOT.kGreen+1)


ROOT.gStyle.SetOptStat(0)
h.SetTitle(opts.base + "_" + opts.lepCat + "_" + opts.histo)
h.GetYaxis().SetTitle(opts.syst)
h.Draw("HIST")
for hTmp in hL:
	hTmp.Draw("HIST SAME")

c2=ROOT.TCanvas("c2")
c2.SetGridy()
#ROOT.gROOT.SetTextSize(0.08)
#ROOT.gROOT.SetTitleSize(0.06,"x")
#ROOT.gROOT.SetTitleSize(0.06,"y")

r=h.Clone("r")

rL=[]
for hTmp in hL:
	rTmp=hTmp.Clone("r"+hTmp.GetName())
	rL.append(rTmp)

r.Divide(h)
for rTmp in rL:
	rTmp.Divide(h)

rLStat=[]
for hTmp in hLStat:
	rTmp=hTmp.Clone("r"+hTmp.GetName())
	rTmp.Divide(h) # divide by nominal
	rLStat.append(rTmp)


#check if there are bins with gt 50%
c2.SetFillColor(ROOT.kWhite)

for rTmp in rL:
	if rTmp.GetMaximum()>1.5 or (rTmp.GetMaximum()<0.5 and rTmp.GetMaximum()>0.):
		c2.SetFillColor(ROOT.kYellow)

r.Draw("HIST")
for rTmp in rL:
	rTmp.Draw("HIST SAME")
	# Predefined function:
#	fit_syst = ROOT.TF1("fit_syst", "pol2", 100., 3500.)
#	fit_syst.SetLineColor(ROOT.kGreen)
#	rTmp.Fit(fit_syst);	
#	fit_syst.Draw("SAME")


for rTmp in rLStat:
	rTmp.Draw("HIST SAME")

r.GetYaxis().SetRangeUser(0.5,1.5)
#r.GetYaxis().SetRangeUser(0.,2.)
#raw_input("ok?")

c2.SaveAs("plot/syst" + opts.syst + "_" + opts.base + opts.lepCat + opts.histo + ".pdf")
c2.SaveAs("plot/syst" + opts.syst + "_" + opts.base + opts.lepCat + opts.histo + ".png")
