#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

parser= OptionParser()

#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_7_6_5_testNEROforCOMBINE/src/ChargedHiggs/APR8_1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_7_6_5_testNEROforCOMBINE/src/ChargedHiggs/APR8_2l.root")

parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_7_6_5_testNEROforCOMBINE/src/ChargedHiggs/splitted_APR8_1l.root") 
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_7_6_5_testNEROforCOMBINE/src/ChargedHiggs/splitted_APR8_2l.root") 

#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="feb7_1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="feb7_1l.root")
#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/feb12final_1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/feb12final_2l.root")
#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/FEB26_final2/1l/1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/FEB26_final2/2l/2l.root")
#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/APR1_finalSYST/1l/1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/APR4_finalNew/2l/2l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/APR1_finalSYST/2l/2l.root")
#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/MARCH19_dataMC/1l/1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/MARCH19_dataMC/2l/2l.root")

#parser.add_option("","--input1Lsig",type='string',help="Input ROOT file. [%default]", default="1l_signal.root")
#parser.add_option("","--input2Lsig",type='string',help="Input ROOT file. [%default]", default="2l_signal.root")
#parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_STAT.root")
#parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_STAT.txt")
#parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_SYST_1Mu_BaselineExtraRad.root")
#parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_SYST_1Mu_BaselineExtraRad.txt")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_SYST.root")
parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_SYST.txt")
parser.add_option("-l","--lumi",type='float',help="Luminosity. [%default]", default=35867)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-k","--kTest",type='int',help = "Which test runs. [%default]", default=2)
#extra.add_option("-r","--rebin",type='int',help = "Rebin Histogram. [%default]", default=-1)
#
#parser.add_option_group(extra)

opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

g=[] ## garbage un-collector

def WorkspaceSubstitution(string):
	res = re.sub('JES','CMS_scale_j',string)	
	res = re.sub('JER','CMS_res_j',res)	
	res = re.sub('BTAGB','CMS_eff_b',res)	
	res = re.sub('BTAGL','CMS_fake_b',res)	
	res = re.sub('BTAG','CMS_eff_b',res)	
	res = re.sub('TAUANTIE','CMS_eff_t_ele',res)	
	res = re.sub('TAUSCALE','CMS_scale_t',res)	
	res = re.sub('TAUHIGHPT','CMS_eff_t_highpt',res)	
	res = re.sub('TAU','CMS_eff_t',res)	
	res = re.sub('TRIGMET','CMS_trig_met',res)	
	res = re.sub('TRIG1p','CMS_trg_t_1p',res)	
	res = re.sub('TRIG3p','CMS_trg_t_3p',res)	
	res = re.sub('TRIG','CMS_trg_t',res)	
	res = re.sub('RFAC1p','CMS_fake_t_1p',res)	
	res = re.sub('RFAC3p','CMS_fake_t_3p',res)	
	res = re.sub('RFAC','CMS_fake_t',res)	
	res = re.sub('ELEVETO','CMS_eff_e_veto',res)	
	res = re.sub('MUVETO','CMS_eff_m_veto',res)	
	res = re.sub("UNCLUSTER","CMS_scale_uncluster",res)
	res = re.sub("PU","CMS_pileup",res)
	res = re.sub("muTRG","CMS_eff_m_trigger",res)
	res = re.sub("muID","CMS_eff_m_id",res)
	res = re.sub("muISO","CMS_eff_m_iso",res)
	res = re.sub("muRECO","CMS_eff_m_reco",res)
	res = re.sub("TOPRW","CMS_topreweight",res)
	return res

def RebinN(h,nBin):
	''' Rebin with un-even bins '''
	h1=h.Rebin(nBin)
	return h1

def Rebin(h):
	''' Rebin with un-even bins '''
	mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1


def Rebin1LTop(h):
        ''' Rebin with un-even bins '''
        mybins=array('d',[0, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 820, 840, 860, 880, 900, 920, 940, 960, 1000, 1050, 1100, 1150, 1200, 1250, 1350, 1500, 1600, 1750, 2000, 2500, 3000, 4000, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def Rebin1LCharm(h):
        ''' Rebin with un-even bins '''
        mybins=array('d',[0, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 420, 440, 460, 480, 500, 550, 600, 700, 800, 1000, 1250, 1500, 2000, 3000, 4000, 5000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def Rebin1LExtraRad(h):
        ''' Rebin with un-even bins '''
        mybins=array('d',[0, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 810, 820, 830, 840, 850, 860, 880, 900, 920, 940, 960, 980, 1000, 1020, 1040, 1060, 1080, 1100, 1120, 1150, 1200, 1250, 1300, 1350, 1400, 1500, 1600, 1800, 2000, 2300, 2600, 3000, 4000, 6000, 8000])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
##	h1=h.Rebin(2)
        return h1


def Rebin1L(h):
	''' Rebin with un-even bins '''
## ORIGINAL
	mybins=array('d',[0, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 810, 820, 830, 840, 850, 860, 870, 880, 890, 900, 910, 920, 930, 940, 950, 960, 970, 980, 990, 1000, 1020, 1040, 1060, 1080, 1100, 1120, 1140, 1160, 1180, 1200, 1220, 1240, 1260, 1280, 1300, 1320, 1340, 1360, 1380, 1400, 1420, 1440, 1460, 1480, 1500, 1550, 1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000, 8000])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
##	h1=h.Rebin(2)
	return h1

def Rebin2L(h):
	''' Rebin with un-even bins '''
	mybins=array('d',[0, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760, 780, 800, 820, 840, 860, 880, 900, 920, 940, 960, 980, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1750, 1800, 1850, 1900, 1950, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000, 8000])
##	mybins=array('d',[0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760, 780, 800, 820, 840, 860, 880, 900, 920, 940, 960, 980, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1750, 1800, 1850, 1900, 1950, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000, 8000])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1


def RebinBDT1(h):
        mybins=array('d',[-1,
                           -0.99,-0.98,-0.97,-0.96,-0.95,-0.94,-0.93,-0.92,-0.91,-0.9,
                           -0.89,-0.88,-0.87,-0.86,-0.85,-0.84,-0.83,-0.82,-0.81,-0.8,
                           -0.79,-0.78,-0.77,-0.76,-0.75,-0.74,-0.73,-0.72,-0.71,-0.7,
                           -0.68,-0.66,-0.64,-0.62,-0.60,
                           -0.58,-0.56,-0.54,-0.52,-0.50,
                           -0.48,-0.46,-0.44,-0.42,-0.40,
                           -0.36,-0.32,
                           -0.28,-0.24,
                           -0.2,-0.16,-0.12,
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

##	mybins=array('d',[-1.,-0.8,-0.6,-0.4,-0.3,-0.2,-0.1,0.,0.1,0.2,0.3,0.4,0.6,0.8,1.])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1

def RebinBDT2(h):
        ''' Rebin with un-even bins '''
## this is equivalent to rebin5
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


fIn1L = ROOT.TFile.Open(opts.input1L,"READ")
fIn2L = ROOT.TFile.Open(opts.input2L,"READ")

if fIn1L == None: 
	print "ERROR: file",opts.input1L,"doesn't exist"
	exit(1)
if fIn2L == None: 
	print "ERROR: file",opts.input2L,"doesn't exist"
	exit(1)

basecat = []
channel = []

if opts.kTest==1 or opts.kTest==2 or opts.kTest==3 or opts.kTest==0 or opts.kTest==11 or opts.kTest==12 or opts.kTest==13:
#	channel = ["1Mu"]
#	basecat = ["Baseline","extraRadCR"]
	channel = ["1Mu","1Ele"]
#	channel = ["1Mu"]
#	basecat = ["Baseline"]
#	basecat = ["Baseline"]
	basecat = ["Baseline","topCRR4","topCRR5","extraRadCRR10","extraRadCRR7","charmCR"]
#	basecat = ["Baseline","topCR","extraRadCR","charmCR"]
#	basecat = ["topCR"]
#	basecat = ["charmCR"]
#	basecat = ["extraRadCR"]
#	basecat = ["Baseline"]

if opts.kTest==4 or opts.kTest==5 or opts.kTest==6 or opts.kTest==7 or opts.kTest==14 or opts.kTest==15 or opts.kTest==16:
	channel = ["2Mu","2Ele"]
#	channel = ["1Mu1Ele","2Mu","2Ele"]
#	channel = ["2Mu"]
#	basecat = ["Baseline","extraRadCR","topCR"]
	basecat = ["Baseline","topCRR4","topCRR5","extraRadCRR10","extraRadCRR7"]

if opts.kTest==8 or opts.kTest==9:
	channel = ["1Mu","1Ele","1Mu1Ele","2Mu","2Ele"]
	basecat = ["Baseline","extraRadCR","topCR","charmCR"]

catStore = { } ## name -> {"file", extra options for syst}, hasSignal
statStore = {} ## used to store th1d for stat uncertainties

label=""
VarTest=""

doSyst = True

for y in channel:
#	if y == "2Ele" or y == "2Mu" or y == "1Mu1Ele": basecat = ["Baseline","extraRadCR","topCR"]
	if y == "2Ele" or y == "2Mu" or y == "1Mu1Ele": basecat = ["Baseline","topCRR4","topCRR5","extraRadCRR10","extraRadCRR7"]
	for x in basecat:
		label="2l_"
		if y == "1Ele" or y == "1Mu": label="1l_"
		if opts.kTest==8: label="combined_"

#	name= x+ "_" + y
#       catStore [ name ] = { "name": name,"file": None, "hasMC":["all"],"var":"HT"}
		srList = [""]
		region = [""]
##		if x=="Baseline" or x=="extraRadCR" or x=="topCR" or x=="charmCR":
		if x=="Baseline" or "extraRadCR" in x or "topCR" in x or x=="charmCR":
#			if x=="charmCR" and (y == "2Ele" or y == "2Mu" or y =="1Mu1Ele"): continue

			if x=="Baseline": region = ["_SR1","_SR2","_SR3","_SR4"]
#			if x=="Baseline": region = ["_SR4"]
#			if x=="Baseline": region = ["_SR2","_SR4"] #3b only
#			if x=="Baseline": region = ["_SR1","_SR3"] #2b only
###			if x=="Baseline" and (opts.kTest==0 or opts.kTest==7): region = [""]## not sure why this
#			if x=="Baseline": region = ["_SR1"]

##			if y == "1Ele" or y == "1Mu": srList = ["_SR1","_SR2","_SR3","_SR4"]
##			if y == "1Ele" or y == "1Mu": srList = [""]
			## BDT1 is 1l high mass
			## BDT2 is 1l medium mass
			## BDT3 is 1l low mass
			## BDT4 is 2l high mass
			## BDT5 is 2l medium mass
			## BDT6 is 2l low mass
##			if y == "1Ele" or y == "1Mu": srList = ["1","2"]
			if opts.kTest==8 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = [""] #this is the 1d combined
			if opts.kTest==0 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = [""] #this is the 1d
			if opts.kTest==1 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["1"]
			if opts.kTest==2 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["2"]
			if opts.kTest==3 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["3"]
			if opts.kTest==11 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["_1ll"]
			if opts.kTest==12 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["_1lm"]
			if opts.kTest==13 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["_1lh"]
##			if y == "1Ele" or y == "1Mu": srList = ["2"]
##			else : srList = ["_SR1","_SR2","_SR3","_SR4"]
##			else : srList = [""]
##			else : srList = ["3","4"]
			else :
				if x=="Baseline" and opts.kTest==4: srList = ["4"]
				if x=="Baseline" and opts.kTest==5: srList = ["5"]
				if x=="Baseline" and opts.kTest==6: srList = ["6"]
				if x=="Baseline" and opts.kTest==7: srList = [""]#this is the 1d
				if x=="Baseline" and opts.kTest==8: srList = [""]#this is the 1d combined
				if x=="Baseline" and opts.kTest==14: srList = ["_2ll"]
				if x=="Baseline" and opts.kTest==15: srList = ["_2lm"]
				if x=="Baseline" and opts.kTest==16: srList = ["_2lh"]
##			else : srList = ["4"]

		for sr in srList:
			if opts.kTest==1: VarTest="high_"
			if opts.kTest==2: VarTest="medium_"
			if opts.kTest==3: VarTest="low_"
			if opts.kTest==4: VarTest="high_"
			if opts.kTest==5: VarTest="medium_"
			if opts.kTest==6: VarTest="low_"
			if opts.kTest==7: VarTest="1d_"
			if opts.kTest==0: VarTest="1d_"
			if opts.kTest==8: VarTest="1d_"
			if opts.kTest==11: VarTest="2d_low_"
			if opts.kTest==12: VarTest="2d_medium_"
			if opts.kTest==13: VarTest="2d_high_"
			if opts.kTest==14: VarTest="2d_low_"
			if opts.kTest==15: VarTest="2d_medium_"
			if opts.kTest==16: VarTest="2d_high_"

			for reg in region:
				name = x+ "_" + y + "" + sr + "" + reg
#				if opts.kTest==0 or opts.kTest==7:catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"ST"+reg}
				if opts.kTest>0 and opts.kTest<7 and x=="Baseline":
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"bdt"+sr+""+reg}
				elif opts.kTest>10 and x=="Baseline":
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"bdt2D"+sr+""+reg}
				else:
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"HT"+reg}
#					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"ST"+reg}

				print 'name=',name,'sr=',sr,'reg=',reg,"VarTest=",VarTest

       ## set files
				if y == "1Ele" or y == "1Mu": catStore [ name ]['file'] = fIn1L
				else : catStore[name]['file'] = fIn2L

       #these have null norm so far
				if x=="charmCR":
#					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TTX","EWK","ST","HPlus"]
					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TOP","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","HPlus"]
				if x=="extraRadCR":
#					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TTX","EWK","ST","HPlus"]
					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TOP","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","HPlus"]
				if x=="topCR":
#					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TTX","EWK","ST","HPlus"]
					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TOP","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","HPlus"]
				if x=="Baseline":
#					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TTX","EWK","ST","HPlus"]
					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","TOP","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TT2B","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","TT1b","TT2b","TTc","HPlus"]
#					catStore[name]["hasMC"]=["TTlight","HPlus"]

#				if x=="charmCR"  and (y=="2Mu" or y=="2Ele" or "1Mu1Ele"):
#					catStore[name]["hasMC"]=[""]
#			if x=="Baseline" and y=="2Mu":
#				catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
#			if x=="Baseline" and y=="2Ele":
#				catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
#			if x=="Baseline" and y=="1Mu1Ele" and opts.kTest==6 and reg=="_SR4":
#				catStore[name]["hasMC"]=[""]
#			if x=="Baseline" and y=="2Mu" and opts.kTest==6 and reg=="_SR4":
#				catStore[name]["hasMC"]=[""]
#			if x=="Baseline" and y=="2Ele" and opts.kTest==6 and reg=="_SR4":
#				catStore[name]["hasMC"]=[""]


#			if opts.kTest==1 or opts.kTest==2 or opts.kTest==3 or opts.kTest==0:
			if y == "1Ele" or y == "1Mu":
				mcStore={
					"HPlus":{"name":"HPlus", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
					"TTlight":{ "name":"TTlight","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
					"TT1b":{ "name":"TT1b","hist":["tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
					"TT2b":{ "name":"TT2b","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
					"TT2B":{ "name":"TT2B","hist":["tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":4},
					"TTc":{ "name":"TTc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":5},
					#				"TT":{ "name":"TT","hist":["TTToSemilepton","TTTo2L2Nu"],"num":1},
					#				"ST":{ "name":"ST","hist":["ST","tZq"],"num":2},
#					"ST":{ "name":"ST","hist":["ST"],"num":6},
#					"EWK":{ "name":"EWK","hist":["WJetsToLNu","WZTo","ZZTo"],"num":7},
#					"TTX":{ "name":"TTX","hist":["TTZ","TTW","TTG","ttH","TTTT"],"num":8}
					"TTX":{ "name":"TTX","hist":["TTZ","TTW","TTG","ttH","TTTT","ST"],"num":8}
					}
			else:
				mcStore={
					"HPlus":{"name":"HPlus", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
					"TTlight":{ "name":"TTlight","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
					"TT1b":{ "name":"TT1b","hist":["tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
					"TT2b":{ "name":"TT2b","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
					"TT2B":{ "name":"TT2B","hist":["tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":4},
					"TTc":{ "name":"TTc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":5},
					#				"TT":{ "name":"TT","hist":["TTToSemilepton","TTTo2L2Nu"],"num":1},
					#				"ST":{ "name":"ST","hist":["ST","tZq"],"num":2},
#					"ST":{ "name":"ST","hist":["ST"],"num":6},
#					"EWK":{ "name":"EWK","hist":["WJetsToLNu","ZZTo"],"num":7},
#					"TTX":{ "name":"TTX","hist":["TTZ","TTW","TTG","ttH","TTTT"],"num":8}
					"TTX":{ "name":"TTX","hist":["TTZ","TTW","TTG","ttH","TTTT","ST"],"num":8}
					}

			systStore={
				"None":None,
				"CMS_pileup":{"type":"shape", "wsname":"PU","name":"PU","proc":[".*"]}, ## name used for shape
				"CMS_scale_uncluster":{"type":"shape", "wsname":"UNCLUSTER","name":"UNCLUSTER","proc":[".*"]}, ## name used for shape
				"CMS_scale_j":{"type":"shape", "wsname":"JES","name":"JES","proc":[".*"]}, ## name used for shape
###				"CMS_res_j":{"type":"shape", "wsname":"JER","name":"JER","proc":[".*"]}, ## name used for shape
#				"CMS_eff_b":{"type":"shape", "wsname":"BTAGB","name":"BTAGB","proc":[".*"]}, ## name used for shape
#				"CMS_fake_b":{"type":"shape", "wsname":"BTAGL","name":"BTAGL","proc":[".*"]}, ## name used for shape
				"CMS_topreweight":{"type":"shape", "wsname":"TOPRW","name":"TOPRW","proc":[".*"]}, ## name used for shape
				"CMS_eff_m_trigger":{"type":"shape", "wsname":"muTRG","name":"muTRG","proc":[".*"]}, ## name used for shae
				"CMS_eff_m_reco":{"type":"lnN", "value":["1.05"] ,"proc":[".*"],"wsname":"muRECO","name":"XXX"}, ## name used for shape
				"CMS_eff_m_id":{"type":"lnN", "value":["1.05"] ,"proc":[".*"],"wsname":"muID","name":"XXX"}, ## name used for shape
				"CMS_eff_m_iso":{"type":"lnN", "value":["1.05"] ,"proc":[".*"],"wsname":"muISO","name":"XXX"}, ## name used for shape
#				"CMS_eff_m_reco":{"type":"shape", "wsname":"muRECO","name":"muRECO","proc":[".*"]}, ## name used for shape
#				"CMS_eff_m_id":{"type":"shape", "wsname":"muID","name":"muID","proc":[".*"]}, ## name used for shape
#				"CMS_eff_m_iso":{"type":"shape", "wsname":"muISO","name":"muISO","proc":[".*"]}, ## name used for shape
###				"lumi13TeV":{"type":"lnN", "value":["1.06"] ,"proc":[".*"],"wsname":"lumi13TeV","name":"XXX"} ## name used for shape
				"lumi13TeV":{"type":"lnN", "value":["1.025"] ,"proc":[".*"],"wsname":"lumi13TeV","name":"XXX"} ## name used for shape
###
				}

print "--------- CAT STORE IS --------"
for cat in catStore:
	print "* ",cat,":",catStore[cat]
print "---------------------- --------"

fileTmp="APR8/"+label+VarTest+opts.output

w = ROOT.RooWorkspace("w","w")
datNameTmp = opts.datCardName
datName = "APR8/"+ label + VarTest + datNameTmp

datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")


w.factory("ht[0,8000]"); # RooRealVar
ht=w.var("ht")
w.factory("bdt[-1,1]")
bdt=w.var("bdt")
w.factory("bdt2D[-0.5,3.5]")
bdt2D=w.var("bdt2D")

arglist_obs = ROOT.RooArgList(ht)
argset_obs = ROOT.RooArgSet(ht)

arglist_obs_bdt = ROOT.RooArgList(bdt)
argset_obs_bdt = ROOT.RooArgSet(bdt)

arglist_obs_bdt2D = ROOT.RooArgList(bdt2D)
argset_obs_bdt2D = ROOT.RooArgSet(bdt2D)


def skip(cat,mc):
	if cat["hasMC"][0] == "all": return False
	for x in cat["hasMC"]:
		if mc["name"] == x : return False
	return True


## write shapes
if True: # data
	datacard.write("shapes data_obs *\t" + fileTmp +"\t")
	datacard.write("w:data_obs_$CHANNEL")
	datacard.write("\n")
if True: # Sig
	datacard.write("shapes HPlus *\t" + fileTmp +"\t")
	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL\t")
	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL_$SYSTEMATIC")
	datacard.write("\n")
if True: #bkg
	datacard.write("shapes * * %s\t"%fileTmp +"\t")
	datacard.write("w:pdf_$PROCESS_$CHANNEL\t")
	datacard.write("w:pdf_$PROCESS_$CHANNEL_$SYSTEMATIC\n")
datacard.write("-------------------------------------\n")
datacard.write("## Observation\n")


# write observation
datacard.write("bin\t")
for cat in catStore:
	datacard.write("%s\t"%cat)
datacard.write("\n")
datacard.write("bin\t")
for cat in catStore:
	datacard.write("-1\t")
datacard.write("\n")

datacard.write("-------------------------------------\n")
# write expectations
datacard.write("bin\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("%s\t"%cat)
datacard.write("\n")

datacard.write("process\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("%s\t"%mc)
datacard.write("\n")

datacard.write("process\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("%d\t"%mcStore[mc]["num"])
datacard.write("\n")

datacard.write("rate\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("-1\t")
datacard.write("\n")
datacard.write("-------------------------------------\n")
# write systematics

## 
def writeNormSyst(name="lumi",valueL=["1.027","1.026"], regexpL=["TT","ST","TTX"]):
##def writeNormSyst(name="lumi",valueL=["1.027","1.026"], regexpL=["TT"]):
	datacard.write(name+"\tlnN")
	invert=False

        for cat in catStore:
            for proc in mcStore:
                if skip(catStore[cat],mcStore[proc]): continue
		idx=-1
		invert=False
		for i,regexp in enumerate(regexpL):
			match=re.search(regexp,proc)
			if regexp != "" and regexp[0] == '!':
				invert=True
				regexp=regexp[1:]
			if (match and not invert) or (not match and invert):
				idx=i
				break

		if (idx>=0):
		   datacard.write("\t"+valueL[idx])
		else:
		   datacard.write("\t-")
	datacard.write("\n")

def writeSystShape(syst,regexpL=[],regexpCat=None):
	name=syst["wsname"]
	datacard.write(name+"\tshape")

        for cat in catStore:
	    matchCat=True
	    if regexpCat != None:
		matchCat=re.search(regexpCat,cat)

            for proc in mcStore:
                if skip(catStore[cat],mcStore[proc]): continue
		idx=-1
		invert=False
		for i,regexp in enumerate(regexpL):
			match=re.search(regexp,proc)
			if regexp != "" and regexp[0] == '!':
				invert=True
				regexp=regexp[1:]
			if (match and matchCat and not invert) or (not match and matchCat and invert):
				idx=i
				break

		if (idx>=0):
		   datacard.write("\t1")
		else:
		   datacard.write("\t-")
	datacard.write("\n")

def importStat():
	for statName in statStore:
	   stat= statStore[statName]
 	   h=stat["hist"] ## all MC Sum up correctly
	   h0=stat["hist0"] ## nominal
	   minError=-1
 	   for iBin in range(1,h.GetNbinsX()+1):
 	      c= h.GetBinContent(iBin)
 	      if c<=0: continue;
 	      e=h.GetBinError(iBin)
 	      if e>0 and (e<minError or minError<0):
		      minError=e
 	   for iBin in range(1,h.GetNbinsX()+1):
 	      c= h.GetBinContent(iBin)
 	      if c>0: continue
 	      h.SetBinError(iBin,minError)
	   for i in range(0,h.GetNbinsX()):
	      statsyst = { "wsname": statName + "_Bin%d"%(i+1) +"_STAT" , "name": statName + "_Bin%d"%(i+1) + "_STAT"}
	      hupbin=h0.Clone(h.GetName() +"_STATUp")
	      hdnbin=h0.Clone(h.GetName() +"_STATDown")
	      cont=h0.GetBinContent(i+1)
	      err = h . GetBinError(i+1) ## err is referred to the sum
	      c = h.GetBinContent(i+1) ## this is to check the magnitude, not to apply it
#	      if c > 0 and err/c <.01: continue ## don't write less than 1%
	      if c > 0 and err/c <.10: continue ## don't write less than 1% // TEMPORARY
	      hupbin.SetBinContent(i+1,cont+err)
	      hdnbin.SetBinContent(i+1,cont-err)
	      target = stat["target"]
	      cat = catStore[stat["cat"] ]
	      print "++++++++++++++++++++target",target,' cate',cat

#	      al=arglist_obs
#	      if cat["var"] == "bdt": al = arglist_obs_bdt
	      al=arglist_obs_bdt
	      if "bdt2D" in cat["var"]: al = arglist_obs_bdt2D
	      if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4": al = arglist_obs
#	      if cat["var"] == "ST" or cat["var"] == "ST_SR1" or cat["var"] == "ST_SR2" or cat["var"] == "ST_SR3" or cat["var"] == "ST_SR4": al = arglist_obs

 	      roo_mc_binup = ROOT.RooDataHist(target+"_"+statsyst["wsname"]+"Up",target + "STAT",al,hupbin)
 	      pdf_mc_binup = roo_mc_binup
 	      roo_mc_bindn = ROOT.RooDataHist(target+"_"+statsyst["wsname"]+"Down",target + "STAT",al,hdnbin)
 	      pdf_mc_bindn = roo_mc_bindn
 	      getattr(w,'import')(pdf_mc_binup,ROOT.RooCmdArg())
 	      getattr(w,'import')(pdf_mc_bindn,ROOT.RooCmdArg())
 	      g.extend([hupbin,roo_mc_binup,pdf_mc_binup])
 	      g.extend([hdnbin,roo_mc_bindn,pdf_mc_bindn])
	
	      ## attribute it only to one MC, since is summed
	      #print "*** DEBUG *** writing matching for",stat["mc"]+"_","pdfname=",target+"_"+statsyst["wsname"]+"Up",
	      #print "--- statsyst",statsyst
	      writeSystShape(statsyst,[stat["mc"]+"$"],stat["cat"])

################### WRITE SYST ################
for syst in systStore:
	if syst == "None": continue
	if systStore[syst]["type"] == "lnN":
		writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"])
	if systStore[syst]["type"] == "shape":
		writeSystShape(systStore[syst],systStore[syst]["proc"])

if doSyst: writeNormSyst("CMS_scale_ttbar",["0.965/1.024","0.965/1.024","0.965/1.024","0.965/1.024","0.965/1.024","0.977/1.028"],["TTlight","TTc","TT1b","TT2b","TT2B","ST"])
if doSyst: writeNormSyst("CMS_pdf_ttbar",["1.042","1.042","1.042","1.042","1.042","1.026"],["TTlight","TTc","TT1b","TT2b","TT2B","ST"])
if doSyst: writeNormSyst("CMS_mass_ttbar",["1.027","1.027","1.027","1.027","1.027","1.022"],["TTlight","TTc","TT1b","TT2b","TT2B","ST"])

if doSyst: writeNormSyst("CMS_pdf_ttlight",["1.50"],["TTlight"])
if doSyst: writeNormSyst("CMS_pdf_ttc",["1.50"],["TTc"])
if doSyst: writeNormSyst("CMS_pdf_tt1b",["1.50"],["TT1b"])
if doSyst: writeNormSyst("CMS_pdf_tt2b",["1.50"],["TT2b"])
if doSyst: writeNormSyst("CMS_pdf_tt2B",["1.50"],["TT2B"])

##https://twiki.cern.ch/twiki/bin/view/LHCPhysics/LHCHXSWGTTH
if doSyst: writeNormSyst("CMS_scale_ttZ",["0.887/1.096"],["TTZ"])
if doSyst: writeNormSyst("CMS_pdf_ttZ",["1.028"],["TTZ"])
if doSyst: writeNormSyst("CMS_mass_ttZ",["1.028"],["TTZ"])

if doSyst: writeNormSyst("CMS_scale_ttH",["0.908/1.058"],["ttH"])
if doSyst: writeNormSyst("CMS_pdf_ttH",["1.036"],["ttH"])
if doSyst: writeNormSyst("CMS_mass_ttH",["1.036"],["ttH"])

#writeNormSyst("CMS_scale_VV",["1.025","1.032","1.031"],["WWTo","WZTo","ZZTo"])
#writeNormSyst("CMS_pdf_VV",["1.022","1.044","1.037"],["WWTo","WZTo","ZZTo"])

#writeNormSyst("CMS_scale_V",["0.9963/1.0065","0.996/1.008"],["DYJetsToLL_M","WJetsToLNu"])
#writeNormSyst("CMS_pdf_V",["1.037","1.0375"],["DYJetsToLL_M","WJetsToLNu"])

################### IMPORT ################
## def importStat(h,target,syst="STAT"):
## 	minError=-1
## 	for iBin in range(1,h.GetNbinsX()+1):
## 		c= h.GetBinContent(iBin)
## 		if c<=0: continue;
## 		e=h.GetBinError(iBin)
## 		if e>0 and (e<minError or minError<0):
## 			minError=e
## 	for iBin in range(1,h.GetNbinsX()+1):
## 		c= h.GetBinContent(iBin)
## 		if c<=0: continue;
## 		h.SetBinError(iBin,minError)
##
## 	hup=h.Clone(h.GetName() +"_STATUp")
## 	hdn=h.Clone(h.GetName() +"_STATDown")
## 	for i in range(0,h.GetNbinsX()):
## 		hupbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
## 		hdnbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
## 		cont=h.GetBinContent(i+1)
## 		# 0 is for sure wrong in case of mc, fixed in GetHisto
## 		hupbin.SetBinContent(i+1, h.GetBinContent(i+1) + h.GetBinError(i+1) )
## 		hdnbin.SetBinContent(i+1, h.GetBinContent(i+1) - h.GetBinError(i+1) )
## 		roo_mc_binup = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Up",target,arglist_obs,hupbin)
## 		pdf_mc_binup = roo_mc_binup
## 		roo_mc_bindn = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Down",target,arglist_obs,hdnbin)
## 		pdf_mc_bindn = roo_mc_bindn
## 		getattr(w,'import')(pdf_mc_binup,ROOT.RooCmdArg())
## 		getattr(w,'import')(pdf_mc_bindn,ROOT.RooCmdArg())
## 		g.extend([hupbin,roo_mc_binup,pdf_mc_binup])
## 		g.extend([hdnbin,roo_mc_bindn,pdf_mc_bindn])

## improt Everything in ws TODO
def importPdfFromTH1(cat,mc,syst=None):
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	if mc["name"]=="HPlus":masses=[180,200,220,250,300,350,400,500,800,1000,2000,3000]

##	if mc["name"]=="HPlus":masses=[300,400,500,800,1000,2000,3000,180,200,220,250,350,450,750]
##	if mc["name"]=="HPlus":masses=[180,200,220,250,300,400,500,800,1000,2000,3000]
##	if mc["name"]=="HPlus":masses=[180,200,220,250,300,400,500,800]
##	if mc["name"]=="HPlus":masses=[300,400,500,800,1000,2000,180,200,220,250,350,450,750]
##	if mc["name"]=="HPlus":masses=[180,200,220,250,350,450,750]
##	if mc["name"]=="HPlus":masses=[300,400,500,800,1000,2000,3000]
	else: masses=[0]

	if syst == None: shifts=["x"]
	else: shifts=["Up","Down"]


	for m in masses:
	 for s in shifts:
	  h=None
	  target = "pdf_" + mc["name"] +"_"+ cat["name"]
	  if m >10 :
		  target = "pdf_" + mc["name"] +"_M-%d"%m+"_"+ cat["name"]

	  if syst != None:
		  target += "_" + syst["wsname"] + s

	  for hname in mc["hist"]:
	        toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

		if mc["name"]=="HPlus":
		   toget=toget%m
		if syst != None:
			toget += "_" + syst["name"] + s
		hTmp=tfile.Get(toget)
		print "<*> Reading Hist '"+toget+"'",hTmp.Integral()
		if hTmp == None:
			print "<*> Hist '"+toget+"' doesn't exist" 
			raise IOError
		if h==None:h = hTmp
		else: h.Add(hTmp)
	  #clean h
#	  if opts.kTest==0 or opts.kTest==7: h=Rebin(h)
#	  if opts.kTest==0 or opts.kTest==7: h=RebinN(h,1)
#	  else: h=RebinN(h,1)

####1L
#		print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]

		if  "1Ele" in cat["dir"] or "1Mu" in cat["dir"]:

			if "Baseline" in cat["dir"]:   
				if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3": h=Rebin1LCharm(h)
				elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4": h=Rebin1LCharm(h) 
#				if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3": h=Rebin1LExtraRad(h)
#				elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4": h=Rebin1LExtraRad(h) 
				else: 
#					if opts.kTest==3: h.Rebin(10)
#					if opts.kTest==2: h.Rebin(10)
#					if opts.kTest==1: h.Rebin(10)
					if opts.kTest==3: h=RebinBDT3(h)
					if opts.kTest==2: h=RebinBDT2(h)
					if opts.kTest==1: h=RebinBDT1(h)
					if opts.kTest<10:
						if "TT1b" in catStore[name] or "TT2b" in catStore[name] or "TT2B" in catStore[name]: 
							if h: h.Rebin(10)
						if "SR2" in cat["var"] or "SR4" in cat["var"]: 
							if h: h.Rebin(4)
					
			if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
			if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
			if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
#			if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LTop(h) 
#			if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
#			if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LExtraRad(h) 

#### 2L 			

		if  "2Ele" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]:
			if h:
				if "Baseline" in cat["dir"]:   
					if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3": h=Rebin1LCharm(h)
					elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4": Rebin1LCharm(h) 
					if cat["var"] == "HT" and h: h.Rebin(2)				
					#				if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3": h=Rebin1LExtraRad(h)
					#				elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4": h=Rebin1LExtraRad(h) 
					else: 
						if opts.kTest==6: h=RebinBDT3(h) 
						if opts.kTest==5: h=RebinBDT2(h)
						if opts.kTest==4: h=RebinBDT1(h)
						if opts.kTest<10:
							if "TT1b" in catStore[name] or "TT2b" in catStore[name] or "TT2B" in catStore[name]: 
								if h: h.Rebin(10)
							if "SR2" in cat["var"] or "SR4" in cat["var"]: 
								if h: h.Rebin(4)					
								
				if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
				if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
				if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 

#			if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LTop(h) 
#			if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
#			if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LExtraRad(h) 

			## extra Rebin(2) for dilepton
#			if h: h.Rebin(2)


	  if h: h.SetBinContent(0,0) ##underflow
	  if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

	  if h:
		  for b in range(1,h.GetNbinsX()+1):
			  if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

	  if h:
		  if h.Integral() <= 0 :
			  print "ERROR histogram", h.GetName(),"has null norm"
			  raise ValueError

	  #save RooDataHist
	  h.Scale(opts.lumi)
	  print "* Importing ",target
#	  al=arglist_obs
#	  if cat["var"] == "bdt": al = arglist_obs_bdt
	  al=arglist_obs_bdt
          if "bdt2D" in cat["var"]: al=arglist_obs_bdt2D
	  if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4": al = arglist_obs
#	  if cat["var"] == "ST" or cat["var"] == "ST_SR1" or cat["var"] == "ST_SR2" or cat["var"] == "ST_SR3"or cat["var"] == "ST_SR4": al = arglist_obs
	  print "-> with var", al[0]

	  roo_mc = ROOT.RooDataHist(target,target,al,h)
	  pdf_mc = roo_mc
	  getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
	  g.extend([h,roo_mc,pdf_mc])
	  if syst==None and m< 10 : # not for signal,
		print "DEBUG calling stat with target",target
		histName=cat["name"]
		if histName not in statStore:
			statStore[histName] = { "hist":h.Clone(histName + "_STAT"),
					"mc":mc["name"],
					"cat":cat["name"],
					"target":target,
					"hist0":h.Clone(histName + "_STAT0")
					}
		else:
			statStore[histName]["hist"] . Add (h )
##			if mc["name"] == 'TT':  ## put TT and reference
			if mc["name"] == 'TTlight':  ## put TTlight and reference
				statStore[histName]["mc"]= mc["name"]
				statStore[histName]["target"]= target
				statStore[histName]["hist0"] . Delete()
				statStore[histName]["hist0"] = h.Clone(histName + "_STAT0")
	return

#import MC
for cat in catStore:
    for proc in mcStore:
          if skip(catStore[cat],mcStore[proc]): continue
	  for syst in systStore:
		 print "->calling import pdf with cat=",cat,"proc=",proc,"syst=",syst
		 print " * cat:",catStore[cat]
		 print " * syst:", systStore[syst]
		 if systStore[syst] == None or systStore[syst]["type"] == "shape" :
		 	importPdfFromTH1(catStore[cat],mcStore[proc],systStore[syst])

## import and write statistical uncertainties
if doSyst: importStat()

#import data
for c in catStore:
	cat=catStore[c]
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	target = "data_obs_"+ cat["name"] 
	toget=base + "/" +cat["dir"] + "/" +  cat["var"]  +"_Data"
	h=tfile.Get(toget)
	if h == None:
		print "<*> File not exists"
		raise IOError

	if h: h.SetBinContent(0,0) ##underflow
	if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

	for b in range(1,h.GetNbinsX()+1):
		if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

#	h=Rebin(h)
#	if opts.kTest==0 or opts.kTest==7: h=Rebin(h)
#	if opts.kTest==0 or opts.kTest==7: h=RebinN(h,1)
#	else: h=RebinN(h,1)

#	if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4": h=Rebin(h)
#	else: h=RebinN(h,1)


####1L
#		print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]

	if  "1Ele" in cat["dir"] or "1Mu" in cat["dir"]:
		
		if "Baseline" in cat["dir"]:   
			if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3": h=Rebin1LExtraRad(h)
			elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4": h=Rebin1LExtraRad(h) 
			else: 
				if opts.kTest==3: h=RebinBDT3(h)
				if opts.kTest==2: h=RebinBDT2(h)
				if opts.kTest==1: h=RebinBDT1(h)
#				if "SR2" in cat["var"] or "SR4" in cat["var"]: h.Rebin(2)

		if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LTop(h) 
		if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
		if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LExtraRad(h) 

#### 2L 			

	if  "2Ele" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]:
		
		if "Baseline" in cat["dir"]:   
			if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3": h=Rebin1LExtraRad(h)
			elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4": h=Rebin1LExtraRad(h) 
			else: 
				if opts.kTest==3: h=RebinBDT3(h) 
				if opts.kTest==2: h=RebinBDT2(h)
				if opts.kTest==1: h=RebinBDT1(h)
					
		if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LTop(h) 
		if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LCharm(h) 
		if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LExtraRad(h) 


#	for y in channel:
#		if y == "1Ele" or y == "1Mu":
#			if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4": h=Rebin1L(h)
#			else: h=RebinN(h,1)
#	for y in channel:
#		if y == "2Ele" or y == "2Mu" or y == "1Mu1Ele":
#			if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4": h=Rebin2L(h)
#			else: h=RebinN(h,1)
#			else: h=RebinN(h,2)

#	al=arglist_obs
#	if cat["var"] == "bdt": al = arglist_obs_bdt
	al=arglist_obs_bdt
	if "bdt2D" in cat["var"]: al = arglist_obs_bdt2D
	if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4": al = arglist_obs
#	if cat["var"] == "ST" or cat["var"] == "ST_SR1" or cat["var"] == "ST_SR2" or cat["var"] == "ST_SR3" or cat["var"] == "ST_SR4": al = arglist_obs

	roo_data= ROOT.RooDataHist("data_obs_%s"%c,"H_{T}",al,h)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h,roo_data])

w.writeToFile(fileTmp)
print "--------------------" 
print "datacard=",datName
print "ws=",fileTmp
print " --- DONE --- "

