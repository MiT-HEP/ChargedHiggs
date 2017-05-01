from optparse import OptionParser,OptionGroup
import ROOT
from array import array
import math

ROOT.gROOT.SetBatch(True)
ROOT.gROOT.IsBatch()

parser= OptionParser()
parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/APR29_Final_1l.root")
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/APR29_Final_2l.root")
parser.add_option("","--base",type='string',help="Base [Default=%default]", default="Baseline")
#parser.add_option("","--histo",type='string',help="Base [Default=%default]", default="HT_SR4_tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")
parser.add_option("","--histo",type='string',help="Base [Default=%default]", default="HT_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-k","--lepCat",type='string',help = "Which test runs. [%default]", default="1Mu") 

opts, args = parser.parse_args()

def Rebin1LHT(h):
        ''' Rebin with un-even bins '''
	mybins=array('d',[0, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 420, 440, 460, 480, 500, 550, 600, 700, 800, 1000, 1250, 1500, 2000, 3000, 4000, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1

def Rebin2LHT(h):
        ''' Rebin with un-even bins '''
        mybins=array('d',[0, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 420, 440, 460, 480, 500, 550, 600, 700, 800, 1000, 2000, 4000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1

def RebinN(h,nBin):
        ''' Rebin with un-even bins '''
        h1=h.Rebin(nBin)
        return h1


def RebinBDT1(h):
        ''' Rebin with un-even bins '''
	print 'using BDT1 high-1l'
        mybins=array('d',[-1,
			   -0.98,-0.95,-0.92,
			   -0.89,-0.86,-0.83,-0.8,
			   -0.75,-0.7,
			   -0.65, -0.60, -0.55, -0.50, -0.45, -0.40,
			   -0.35, -0.30, -0.25, -0.20, -0.15, -0.10,
			   -0.05, 0.0, 0.05, 0.10, 0.15, 0.20,
			   0.25,0.30,0.35,0.40,
			   0.45,0.50,0.55,0.60,0.65,0.68,0.70,
                           0.71,0.72,0.73,0.74,0.75,0.76,0.77,0.78,0.79,0.8,
                           0.81,0.82,0.83,0.84,0.85,0.86,0.87,0.88,0.89,0.9,
                           0.91,0.92,0.93,0.94,0.95,0.96,0.97,0.98,0.99,
                           1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1


def RebinBDT2(h):
        ''' Rebin with un-even bins '''
	print 'using BDT2'
        mybins=array('d',[-1,
			   -0.90,-0.8,-0.75,-0.7,
			   -0.65,-0.6,-0.55,-0.50,-0.45,-0.40,
			   -0.36,-0.32,-0.29,-0.26,-0.23,-0.20,-0.17,-0.14,
			   -0.11,-0.08,-0.05,-0.02,0.01,0.04,0.07,0.10,
			   0.13,0.16,0.19,0.22,0.25,0.28,0.31,0.34,0.37,0.40,
			   0.43,0.46,0.49,0.52,0.55,0.58,0.61,0.64,0.67,0.70,
			   0.8,0.9,
                           1.])

        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1


def RebinBDT3(h):
        ''' Rebin with un-even bins '''
        print 'using binning BDT3'
        mybins=array('d',[-1,
                           -0.95,-0.90,-0.85,-0.80,
			   -0.75,-0.70,-0.65,-0.60,-0.55,
			   -0.50,-0.45,-0.40,-0.35,-0.30,
			   -0.28,-0.26,-0.24,-0.22,-0.20,
			   -0.18,-0.16,-0.14,-0.12,-0.10,
			   -0.08,-0.06,-0.04,-0.02,0.,
                           0.02,0.04,0.06,0.08,0.1,
                           0.12,0.14,0.16,0.18,0.2,
                           0.22,0.24,0.26,0.28,0.3,
                           0.32,0.34,0.36,0.38,0.4,
                           0.42,0.44,0.46,0.48,0.5,
                           0.52,0.54,0.56,0.58,0.6,
                           0.65,0.70,0.75,0.80,0.85,
                           0.9,0.95,
                           1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def RebinBDT4(h):
        ''' Rebin with un-even bins '''
	print 'using BDT4 high-2l'
        mybins=array('d',[-1,
			   -0.98,-0.95,-0.92,
			   -0.89,-0.86,-0.83,-0.8,
			   -0.75,-0.7,
			   -0.65, -0.60, -0.55, -0.50, -0.45, -0.40,
			   -0.35, -0.30, -0.25, -0.20, -0.15, -0.10,
			   -0.05, 0.0, 0.05, 0.10, 0.15, 0.20,
			   0.25,0.30,
			   0.32,0.34,0.36,0.38,0.40,0.45,
			   0.60,0.80,
                           1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def RebinBDT6(h):
        ''' Rebin with un-even bins '''
	print 'using BDT6 low-2l'
        mybins=array('d',[-1,
			   -0.8,-0.6,-0.5,-0.4,
			   -0.35,-0.3,-0.25,-0.20,-0.15,-0.10,
			   -0.05, 0.0, 0.05, 0.10, 0.15, 0.20,
			   0.25,0.30,
			   0.35,0.40,0.45,
			   0.55,0.80,
                           1.])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

##### read files and histos

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
		if opts.lepCat == "1Mu" or opts.lepCat == "1Ele": h=Rebin1LHT(h)
		if opts.lepCat == "2Mu" or opts.lepCat == "2Ele" or opts.lepCat == "1Mu1Ele": h=Rebin2LHT(h)
		for idx,hTmp in enumerate(hL):
			if opts.lepCat == "1Mu" or opts.lepCat == "1Ele": hL[idx]=Rebin1LHT(hTmp)
			if opts.lepCat == "2Mu" or opts.lepCat == "2Ele" or opts.lepCat == "1Mu1Ele": hL[idx]=Rebin2LHT(hL[idx])

	#BDT
	if "bdt" in opts.histo:
		print ' ... using BDT bins'
		if "bdt3" in opts.histo: h=RebinBDT3(h)
		if "bdt2" in opts.histo or "bdt5" in opts.histo: h=RebinBDT2(h)
		if "bdt1" in opts.histo: h=RebinBDT1(h)
		if "bdt4" in opts.histo: h=RebinBDT4(h)
		if "bdt6" in opts.histo: h=RebinBDT6(h)

		for idx,hTmp in enumerate(hL):
			if "bdt3" in opts.histo: hL[idx]=RebinBDT3(hTmp)
			if "bdt2" in opts.histo or "bdt5" in opts.histo: hL[idx]=RebinBDT2(hTmp)
			if "bdt1" in opts.histo: hL[idx]=RebinBDT1(hTmp)
			if "bdt4" in opts.histo: hL[idx]=RebinBDT4(hTmp)
			if "bdt6" in opts.histo: hL[idx]=RebinBDT6(hTmp)

else:
	h.Rebin(opts.rebin)

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
h.GetYaxis().SetTitle("("+opts.syst+"-default)/default")
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

#draw legend
leg = ROOT.TLegend(0.45,0.65,0.65,0.9)
leg.SetFillStyle(0)
leg.SetBorderSize(0)

leg.AddEntry(rLStat[0], "stat")

#for rTmp in rL[0]:
leg.AddEntry(rL[0], opts.syst+"Up")
leg.AddEntry(rL[1], opts.syst+"Down")

leg.Draw("sames")

#l = ROOT.TLatex()
#l.SetNDC()
#l.SetTextSize(0.05)
#l.SetTextFont(42)
#l.SetTextAlign(13)
#l.DrawLatex(0.13,.88,"#bf{CMS}, #scale[0.75]{#it{Simulation}}")
#l.SetTextSize(0.05)
#l.SetTextAlign(31)
##l.DrawLatex(0.89,.91,"2.3 fb^{-1} (13 TeV)")                                                                                                                                                   #l.DrawLatex(0.9,.91,"35.87 fb^{-1} (13 TeV)")


raw_input("ok?")


c2.SaveAs("plot/APR29/syst" + opts.syst + "_" + opts.base + opts.lepCat + opts.histo + ".pdf")
c2.SaveAs("plot/APR29/syst" + opts.syst + "_" + opts.base + opts.lepCat + opts.histo + ".png")
