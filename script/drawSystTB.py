from optparse import OptionParser,OptionGroup
import ROOT
from array import array
import math

ROOT.gROOT.SetBatch(True) 
ROOT.gROOT.IsBatch()

parser= OptionParser()
parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/MARCH31_ttOnlySYST/1l/1l.root")
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/MARCH31_ttOnlySYST/2l/2l.root")
parser.add_option("","--base",type='string',help="Base [Default=%default]", default="Baseline")
parser.add_option("","--histo",type='string',help="Base [Default=%default]", default="HT_ttlight_TTToSemilepton")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-k","--lepCat",type='string',help = "Which test runs. [%default]", default="1Mu") 

opts, args = parser.parse_args()

## original
def Rebin1L(h):
        ''' Rebin with un-even bins '''
	#original
        mybins=array('d',[0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 810, 820, 830, 840, 850, 860, 870, 880, 890, 900, 910, 920, 930, 940, 950, 960, 970, 980, 990, 1000, 1020, 1040, 1060, 1080, 1100, 1120, 1140, 1160, 1180, 1200, 1220, 1240, 1260, 1280, 1300, 1320, 1340, 1360, 1380, 1400, 1420, 1440, 1460, 1480, 1500, 1550, 1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000, 8000])

        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def Rebin2L(h):
        ''' Rebin with un-even bins '''
	#original
        mybins=array('d',[0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760, 780, 800, 820, 840, 860, 880, 900, 920, 940, 960, 980, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1750, 1800, 1850, 1900, 1950, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def RebinN(h,nBin):
        ''' Rebin with un-even bins '''
        h1=h.Rebin(nBin)
        return h1

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

if opts.rebin> 10:
	if "HT" in opts.histo:
	#HT
		print ' ... HT bins'
#		h.Smooth(2);
		h=Rebin1L(h)
		for idx,hTmp in enumerate(hL):
#			hL[idx].Smooth(2);
			hL[idx]=Rebin1L(hTmp)
	#BDT
	if "bdt" in opts.histo:
		print ' ... using BDT bins'
#		h.Smooth(2);
		h=RebinN(h,1)
		for idx,hTmp in enumerate(hL):
#			hL[idx].Smooth(2);
			hL[idx]=RebinN(hTmp,1)
else:
	h.Rebin(opts.rebin)

#for idx,hTmp in enumerate(hL):
#	if opts.rebin> 999:
#		hL[idx]=Rebin(hTmp)
#	else:
#		hTmp.Rebin(opts.rebin)
#	#hUp.Rebin(opts.rebin)
#	#hDown.Rebin(opts.rebin)

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


ROOT.gStyle.SetOptStat(0)
h.SetTitle(opts.base + "_" + opts.lepCat + "_" + opts.histo)
h.GetYaxis().SetTitle(opts.syst)
h.Draw("HIST")
for hTmp in hL:
	hTmp.Draw("HIST SAME")

c2=ROOT.TCanvas("c2")
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

#check if there are bins with gt 50%
c2.SetFillColor(ROOT.kWhite)

for rTmp in rL:
	if rTmp.GetMaximum()>1.5:
		c2.SetFillColor(ROOT.kYellow)

r.Draw("HIST")
for rTmp in rL:
	rTmp.Draw("HIST SAME")
	# Predefined function:
#	fit_syst = ROOT.TF1("fit_syst", "pol2", 100., 3500.)
#	fit_syst.SetLineColor(ROOT.kGreen)
#	rTmp.Fit(fit_syst);	
#	fit_syst.Draw("SAME")

r.GetYaxis().SetRangeUser(0.,2.)
#raw_input("ok?")

c2.SaveAs("plot/syst" + opts.syst + "_" + opts.base + opts.lepCat + opts.histo + ".pdf")
c2.SaveAs("plot/syst" + opts.syst + "_" + opts.base + opts.lepCat + opts.histo + ".png")
