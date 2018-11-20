#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

sys.path.insert(0, 'script')
import FwBinning as FwRebin
import SystematicSmoother as SS
#maxStat=0.13
maxStat=0.3

LikeBins=100
# in the histos nbin=1000 then rebin5, those with
nBinsHT=1
#nBinsHT=2
#nBinsHT=4
#nBinsHT=5

doSyst = True

likelihoodBinning = FwRebin.RebinLikelihood(LikeBins)

parser= OptionParser()

#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JUNE29_FINAL_2ThirdSYST_bin200_wDATA_1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JUNE29_FINAL_2ThirdSYST_bin200_wDATA_2l.root")

#parser.add_option("","--input1LBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JUNE1_1l_1Third.root")
#parser.add_option("","--input2LBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JUNE1_2l_1Third.root")

## 650 missing, more signal stat
#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JULY29_FINAL_2ThirdSYST_bin200_wDATA_1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JULY29_FINAL_2ThirdSYST_bin200_wDATA_2l.root")

#parser.add_option("","--input1LBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JULY29_1l_1Third.root")
#parser.add_option("","--input2LBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/JULY29_2l_1Third.root")


##more signal stat , 2L bTag for signal and top

#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/AUG6_FINAL_2ThirdSYST_bin200_wDATA_1l.root")
##parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/AUG6_FINAL_2ThirdSYST_bin200_wDATA_2l.root")

###---- these are the good files
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/NOV1_FINAL_2ThirdSYST_wDATA_2l.root")
parser.add_option("","--input2LBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/NOV1_2l_1Third.root")

parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs//NOV1_FINAL_2ThirdSYST_wDATA_1l.root")
parser.add_option("","--input1LBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_9_1_0_pre2_ReTestNERO/src/ChargedHiggs/NOV1_1l_1Third.root")

######
######
######

if doSyst:
	parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="_workspace_SYST.root")
	parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="_cms_datacard_topbottom_SYST.txt")
else :
	parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_STAT.root")
	parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_STAT.txt")

parser.add_option("-l","--lumi",type='float',help="Luminosity. [%default]", default=35867)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-k","--kTest",type='int',help = "Which test runs. [%default]", default=2)
extra.add_option("-n","--kMass",type='int',help = "Which mass runs. [%default]", default=500)
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
	res = re.sub('ELE','CMS_eff_e',res)
	res = re.sub('MU','CMS_eff_m',res)
	res = re.sub('TAU','CMS_eff_t',res)
	return res

def PartonShowerSyst(type, cat, direction, njet, nbjet, proc):

	filename="/afs/cern.ch/user/j/jaeyserm/public/ChargedHiggs/PSNew.root"

        if cat == "1L":
                cat = "1l"
                njet = njet +1
                nbjet = nbjet +1
        if cat == "2L":
                cat = "2l"
                njet = njet + 1
                nbjet = nbjet + 1

        fIn = ROOT.TFile(filename, "READ")

        h = fIn.Get("%s_%s%s_%s" % (proc,type, direction, cat))
        syst = h.GetBinContent(nbjet, njet)
        fIn.Close()
        return syst


def RebinN(h,nBin):
	''' Rebin with un-even bins '''
	h1=h.Rebin(nBin)
	return h1

fIn1L = ROOT.TFile.Open(opts.input1L,"READ")
fIn2L = ROOT.TFile.Open(opts.input2L,"READ")

fIn1Lbin = ROOT.TFile.Open(opts.input1LBin,"READ")
fIn2Lbin = ROOT.TFile.Open(opts.input2LBin,"READ")

if fIn1L == None: 
	print "ERROR: file",opts.input1L,"doesn't exist"
	exit(1)
if fIn2L == None: 
	print "ERROR: file",opts.input2L,"doesn't exist"
	exit(1)

basecat = []
channel = []

#1l
if opts.kTest==0 or opts.kTest==1 or opts.kTest==2 or opts.kTest==3 or opts.kTest==4 or opts.kTest==5 or opts.kTest==6 or opts.kTest==7 or opts.kTest==8 or opts.kTest==9 or opts.kTest==10 or opts.kTest==11 or opts.kTest==12 or opts.kTest==13 or opts.kTest==14 or opts.kTest==15 or opts.kTest==16 or opts.kTest==100 or opts.kTest==101  or opts.kTest==102:
#	channel = ["1Ele"]
#	channel = ["1Mu"]
#	channel = ["1Mu","1Ele"]
	channel = ["1L"]
	basecat = ["Baseline","topCRR4","topCRR5","extraRadCRR10","extraRadCRR7","charmCR"]

#2l
if opts.kTest==30 or opts.kTest==31 or opts.kTest==32 or opts.kTest==33 or opts.kTest==34 or opts.kTest==35 or opts.kTest==36 or opts.kTest==37 or opts.kTest==38 or opts.kTest==39 or opts.kTest==40 or opts.kTest==41 or opts.kTest==42 or opts.kTest==43 or opts.kTest==44 or opts.kTest==45 or opts.kTest==46 or opts.kTest==200 or opts.kTest==201  or opts.kTest==202:
#	channel = ["2Mu"]
#	channel = ["2Ele"]
#	channel = ["1Mu1Ele"]
#	channel = ["1Mu1Ele","2Mu","2Ele"]
	channel = ["2L"]
	basecat = ["Baseline","topCRR4","topCRR5","extraRadCRR10","extraRadCRR7"]

### combined
#if opts.kTest==20 or opts.kTest==21:
#	channel = ["1Mu","1Ele","1Mu1Ele","2Mu","2Ele"]
#	basecat = ["Baseline","extraRadCR","topCR","charmCR"]

catStore = { } ## name -> {"file", extra options for syst}, hasSignal
statStore = {} ## used to store th1d for stat uncertainties

label=""
VarTest=""

doST = False
doRebinStaticHTCR = False
doRebinStaticBDT = True ## True: use this for the final limit for BDT,HT
doRebin = False

doJanTEST = False

# 2D
#if opts.kTest > 13:
#	doRebin = False
#	doRebinStatic = False

for y in channel:
	for x in basecat:
		label="2l_"
		if y == "1L": label="1L_"
#		if y == "1Ele" or y == "1Mu": label="1l_"
		if y == "1Mu": label="1Mu_"
		if y == "1Ele": label="1Ele_"
		#dilep
		if y == "2Ele": label="2Ele_"
		if y == "2Mu": label="2Mu_"
		if y == "1Mu1Ele": label="1Mu1Ele_"
		if y == "2L": label="2L_"
##		if opts.kTest==20: label="combined_"

		srList = [""]
		region = [""]
		if x=="Baseline" or ("extraRadCR" in x) or ("topCR" in x) or x=="charmCR":

			if x=="Baseline":
				region = [""]
#				if (y == "1Ele" or y == "1Mu" or y == "1L"): region = ["_SR13","_SR24"]
#				else: region = ["_SR13","_SR24"]
				if (y == "1Ele" or y == "1Mu"): region = ["_SR1","_SR2","_SR3","_SR4"]
				else: region = ["_SR1","_SR2","_SR3","_SR4"]
##				else: region = ["_SR13","_SR24"]

##			if y == "1Ele" or y == "1Mu": srList = [""]
			## BDT1 is 1l high mass
			## BDT2 is 1l medium mass
			## BDT3 is 1l low mass
			## BDT4 is 2l high mass
			## BDT5 is 2l medium mass
			## BDT6 is 2l low mass
#			if opts.kTest==20 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = [""] #this is the 1d combined
			## HT
			if opts.kTest==0 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = [""] #this is the 1
			## 1d-1l
			if opts.kTest==100 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x): srList = ["1ll"]
			if opts.kTest==101 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x ): srList = ["1lm"]
			if opts.kTest==102 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x): srList = ["1lh"]
			## 1d-1l

			if opts.kTest==1 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["1"]
			if opts.kTest==2 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["2"]
			if opts.kTest==3 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["3"]
			if opts.kTest==4 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["4"]
			if opts.kTest==5 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["5"]
			if opts.kTest==6 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["6"]
			if opts.kTest==7 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["7"]
			if opts.kTest==8 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["8"]
			if opts.kTest==9 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["9"]
			if opts.kTest==10 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["10"]
			if opts.kTest==11 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["11"]
			if opts.kTest==12 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["12"]
			if opts.kTest==13 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["13"]
			if opts.kTest==14 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["14"]
			if opts.kTest==15 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["15"]
			if opts.kTest==16 and (y == "1Ele" or y == "1Mu" or y == "1L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["16"]

			## 2d-1l
##			if opts.kTest==14 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["_1ll"]
##			if opts.kTest==15 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["_1lm"]
##			if opts.kTest==16 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["_1lh"]
			## 1d-1l-CR7-10
##			if opts.kTest==25 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["25"]
##			if opts.kTest==26 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["26"]
##			if opts.kTest==27 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["27"]
##			if opts.kTest==28 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["28"]
##			if opts.kTest==29 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["29"]
##			if opts.kTest==30 and (y == "1Ele" or y == "1Mu" or y == "1L") and x=="Baseline": srList = ["30"]
			else :
			## HT-2l
				if x=="Baseline" and opts.kTest==30: srList = [""]
			## 1d-2l
				if opts.kTest==31 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["1"]
				if opts.kTest==32 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["2"]
				if opts.kTest==33 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["3"]
				if opts.kTest==34 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["4"]
				if opts.kTest==35 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["5"]
				if opts.kTest==36 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["6"]
				if opts.kTest==37 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["7"]
				if opts.kTest==38 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["8"]
				if opts.kTest==39 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["9"]
				if opts.kTest==40 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["10"]
				if opts.kTest==41 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["11"]
				if opts.kTest==42 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["12"]
				if opts.kTest==43 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["13"]
				if opts.kTest==44 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["14"]
				if opts.kTest==45 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["15"]
				if opts.kTest==46 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x or "topCR" in x): srList = ["16"]
				##
			## 1d-2l
				if opts.kTest==200 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x ): srList = ["2ll"]
				if opts.kTest==201 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x ): srList = ["2lm"]
				if opts.kTest==202 and (y == "2Ele" or y == "2Mu" or y == "1Mu1Ele" or y == "2L") and ( x=="Baseline" or x=="charmCR" or "extraRadCR" in x ): srList = ["2lh"]
			## 1d-2l-CR7-10
#				if x=="Baseline" and opts.kTest==31: srList = ["31"]
#				if x=="Baseline" and opts.kTest==32: srList = ["32"]
#				if x=="Baseline" and opts.kTest==33: srList = ["33"]
#				if x=="Baseline" and opts.kTest==34: srList = ["34"]
#				if x=="Baseline" and opts.kTest==35: srList = ["35"]
#				if x=="Baseline" and opts.kTest==36: srList = ["36"]

		for sr in srList:
#			if opts.kTest==0: VarTest="1d_" #1l
#			if opts.kTest==13: VarTest="1d_" #2l
			if opts.kTest==0 and doST: VarTest="1dST_" #1l
			if opts.kTest==30 and doST: VarTest="1dST_" #2l

			if doJanTEST:
			##
				if opts.kTest==100 or opts.kTest==200: VarTest="HTDEtaMaxBB_High" #VarTest="low_"
				if opts.kTest==101 or opts.kTest==201: VarTest="AvCSVPt_Low_"     #VarTest="medium_"
				if opts.kTest==102 or opts.kTest==202: VarTest="Base_High_"       #VarTest="high_"
			##
				if opts.kTest==1 or opts.kTest==31: VarTest="low180_"
				if opts.kTest==2 or opts.kTest==32: VarTest="HTMJJJmaxPt_Low_"     #VarTest="low200_"
				if opts.kTest==3 or opts.kTest==33: VarTest="HTMJJJmaxPt_Medium_"  #VarTest="low220_"
				if opts.kTest==4 or opts.kTest==34: VarTest="HTMJJJmaxPt_High_"    #VarTest="low250_"
				if opts.kTest==5 or opts.kTest==35: VarTest="HTDEtaMaxBB_Low"      #VarTest="low300_"
				if opts.kTest==6 or opts.kTest==36: VarTest="HTDEtaMaxBB_Medium"   #VarTest="medium350_"
				if opts.kTest==7 or opts.kTest==37: VarTest="medium400_"
				if opts.kTest==8 or opts.kTest==38: VarTest="medium450_"
				if opts.kTest==9 or opts.kTest==39: VarTest="medium500_"
				if opts.kTest==10 or opts.kTest==40: VarTest="high750_"
				if opts.kTest==11 or opts.kTest==41: VarTest="AvCSVPt_Medium_" #VarTest="high800_"
				if opts.kTest==12 or opts.kTest==42: VarTest="AvCSVPt_High_"   #VarTest="high1000_"
				if opts.kTest==13 or opts.kTest==43: VarTest="Base_Low_"       #VarTest="high2000_"
				if opts.kTest==14 or opts.kTest==44: VarTest="Base_Medium_"    #VarTest="high3000_"
				if opts.kTest==15 or opts.kTest==45: VarTest="high1500_"
				if opts.kTest==16 or opts.kTest==46: VarTest="high1500_"
			########################

			else:
			##
				if opts.kTest==100 or opts.kTest==200: VarTest="low_"
				if opts.kTest==101 or opts.kTest==201: VarTest="medium_"
				if opts.kTest==102 or opts.kTest==202: VarTest="high_"
			##
				if (opts.kTest>0 and opts.kTest<18) or (opts.kTest>30 and opts.kTest<48):
					if True:
						#temporary for keras
						VarTest="keras_"
					else:
						if opts.kTest==1 or opts.kTest==31: VarTest="low180_"
						if opts.kTest==2 or opts.kTest==32: VarTest="low200_"
						if opts.kTest==3 or opts.kTest==33: VarTest="low220_"
						if opts.kTest==4 or opts.kTest==34: VarTest="low250_"
						if opts.kTest==5 or opts.kTest==35: VarTest="low300_"
						if opts.kTest==6 or opts.kTest==36: VarTest="low350_"
						if opts.kTest==7 or opts.kTest==37: VarTest="medium400_"
						if opts.kTest==8 or opts.kTest==38: VarTest="medium450_"
						if opts.kTest==9 or opts.kTest==39: VarTest="medium500_"
						if opts.kTest==10 or opts.kTest==40: VarTest="high750_"
						if opts.kTest==11 or opts.kTest==41: VarTest="high800_"
						if opts.kTest==12 or opts.kTest==42: VarTest="high1000_"
						if opts.kTest==13 or opts.kTest==43: VarTest="high2000_"
						if opts.kTest==14 or opts.kTest==44: VarTest="high3000_"
						if opts.kTest==15 or opts.kTest==45: VarTest="high1500_"
						if opts.kTest==16 or opts.kTest==46: VarTest="high2500_"
						if opts.kTest==17 or opts.kTest==47: VarTest="high3000_"


			########################

			##
#			if opts.kTest==14: VarTest="2d_low_"
#			if opts.kTest==15: VarTest="2d_medium_"
#			if opts.kTest==16: VarTest="2d_high_"
#			if opts.kTest==17: VarTest="2d_low_"
#			if opts.kTest==18: VarTest="2d_medium_"
#			if opts.kTest==19: VarTest="2d_high_"

			for reg in region:
				# use the HT in the topCR
#				name = x+ "_" + y + "" + reg
				# use the DNN everywhere in the topCR
				name = x+ "_" + y + "" + sr + "" + reg

#				x=macroRegion  y=1L  sr=bdtX reg=SR12 if thee

				## BDT 1-14 for each mass point and for the merged set
				# use the HT in the topCR
#				if (( opts.kTest>0 and opts.kTest<18) or (opts.kTest>30 and opts.kTest<48 ) or opts.kTest>99 ) and ("Baseline" in x or "extraRadCR" in x or "charmCR" in x):
				# use the DNN everywhere in the topCR
				if (( opts.kTest>0 and opts.kTest<18) or (opts.kTest>30 and opts.kTest<48 ) or opts.kTest>99 ) and ("Baseline" in x or "extraRadCR" in x or "charmCR" in x or "topCR" in x):
					name = x + "_" + sr + "_" + reg + "_" + y
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"bdt"+sr+""+reg}
				## BDT 2d to be updated
#				elif opts.kTest>13 and opts.kTest<20 and x=="Baseline":
#					name = x + "_" + sr + "_" + reg + "_" + y
#					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"bdt2D"+sr+""+reg}
				else:
				## BDT 0,30
					name = x + "" + reg + "_" + y
					if not doST : catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"HT"+reg}
					if doST: catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"ST"+reg}

       ## set files
				if y == "1Ele" or y == "1Mu" or y == "1L": catStore [ name ]['file'] = fIn1L
				else : catStore[name]['file'] = fIn2L

       ## set the MC sample
				if x=="charmCR":
					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","Hptb","ttx"]
#					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","dy_hf","Hptb","ttx"]
				if "extraRadCR" in x:
					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","Hptb","ttx"]
#					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","dy_hf","Hptb","ttx"]
				if "topCR" in x:
					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","Hptb","ttx"]
#					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","dy_hf","Hptb","ttx"]
				if x=="Baseline":
					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","Hptb","ttx"]
#					catStore[name]["hasMC"]=["ttlf","ttb","ttcc","top","ewk","dy_hf","Hptb","ttx"]

			if y == "1Ele" or y == "1Mu" or y == "1L":
				mcStore={
					"Hptb":{"name":"Hptb", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
##MADGAPH
#					"ttb":{ "name":"ttb","hist":["tt2b_TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8","tt2bMerged_TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8","tt1b_TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8"],"num":2},
#					"ttcc":{ "name":"ttcc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
#					"ttlf":{ "name":"ttlf","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
##POWHEG
###					"ttb":{ "name":"ttb","hist":["tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
###					"ttbb":{ "name":"ttbb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
###					"tt2b":{ "name":"tt2b","hist":["tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":4},
###					"ttbb":{ "name":"ttbb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8","tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
### NOMINAL
					"ttb":{ "name":"ttb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8","tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8","tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
					"ttcc":{ "name":"ttcc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
					"ttlf":{ "name":"ttlf","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
##
					"ewk":{ "name":"ewk","hist":["WJetsToLNu_HT","DYJetsToLL_M-50_HT","DYJetsToLL_M-5to50_HT","EWK"],"num":4},
					"top":{ "name":"top","hist":["ST"],"num":5},
					"ttx":{ "name":"ttx","hist":["TTX"],"num":7}
#					"qcd":{ "name":"top","hist":["QCD_HT"],"num":8}
					}
			else:
				mcStore={
					"Hptb":{"name":"Hptb", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
##MADGAPH
#					"ttb":{ "name":"ttb","hist":["tt2b_TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8","tt2bMerged_TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8","tt1b_TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8"],"num":2},
#					"ttcc":{ "name":"ttcc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
#					"ttlf":{ "name":"ttlf","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
##POWHEG
###					"ttb":{ "name":"ttb","hist":["tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
###					"ttbb":{ "name":"ttbb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
###					"tt2b":{ "name":"tt2b","hist":["tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":4},
###					"ttbb":{ "name":"ttbb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8","tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
#### NOMINAL
					"ttb":{ "name":"ttb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8","tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8","tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
					"ttcc":{ "name":"ttcc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
					"ttlf":{ "name":"ttlf","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
##					"ewk":{ "name":"ewk","hist":["WJetsToLNu_HT","DYJetsToLL_M-50_HT","EWK"],"num":4},
					"ewk":{ "name":"ewk","hist":["WJetsToLNu_HT","light_DYJetsToLL_M-50_HT","EWK"],"num":4},
					"dy_hf":{ "name":"dy_hf","hist":["bb_DYJetsToLL_M-50_HT"],"num":6},
					"top":{ "name":"top","hist":["ST"],"num":5},
					"ttx":{ "name":"ttx","hist":["TTX"],"num":7}
#					"qcd":{ "name":"top","hist":["QCD_HT"],"num":8}
					}

			if doSyst:
				systStore={
					"None":None,
#					"CMS_scale_j":{"type":"shape", "wsname":"CMS_scale_j","name":"JESANDCSV","proc":[".*"]} ## name used for shape

					"lumi_13TeV":{"type":"lnN", "value":["1.025"] ,"proc":[".*"],"wsname":"lumi_13TeV","name":"XXX"}, ## name used for shape
					## 2l
					"CMS_eff_l":{"type":"lnN", "value":["1.04"] ,"proc":[".*"],"wsname":"CMS_eff_l","name":"XXX"}, ## name used for shape
					## 1l
					"CMS_eff_m":{"type":"lnN", "value":["1.04"] ,"proc":[".*"],"wsname":"CMS_eff_m","name":"XXX"}, ## name used for shape
					"CMS_eff_e":{"type":"lnN", "value":["1.03"] ,"proc":[".*"],"wsname":"CMS_eff_e","name":"XXX"}, ## name used for shape
					"CMS_eff_t_veto":{"type":"lnN", "value":["1.03"] ,"proc":[".*"],"wsname":"CMS_eff_t_veto","name":"XXX"}, ## name used for shape
					"CMS_pileup":{"type":"shape", "wsname":"CMS_pileup","name":"PU","proc":[".*"]}, ## name used for shape
##                                        "CMS_scale_uncluster":{"type":"shape", "wsname":"CMS_scale_uncluster","name":"UNCLUSTER","proc":[".*"]},
					"CMS_scale_uncluster":{"type":"lnN", "value":["1.02"],"proc":[".*"],"wsname":"CMS_scale_uncluster","name":"XXX"}, ## name used for shape
                                        ####
                                        #### Theo
                                        ####
#					"CMS_topreweight":{"type":"shape", "wsname":"CMS_topreweight","name":"TOPRW","proc":["ttlf","ttb","ttcc"]}, ## name used for shape
					"CMS_HPTB_mcreweight_ewk":{"type":"shape", "wsname":"CMS_HPTB_mcreweight_ewk","name":"htRECO","proc":["ewk","dy_hf"]}, ## name used for shape
##					"QCDscale":{"type":"shape", "wsname":"QCDscale","name":"ScaleRF","proc":["Hptb","ttlf","ttb","ttbb","tt2b","ttcc"]} ## name used for shape
#					"muRFenv_ttbar":{"type":"shape", "wsname":"muRFenv_ttbar","name":"Scale","proc":["ttlf","ttb","ttcc"]}, ## name used for shape
					"muRF_ttbar":{"type":"shape", "wsname":"muRF_ttbar","name":"ScaleRF","proc":["ttlf","ttb","ttcc"]}, ## name used for shape
					"muR_ttbar":{"type":"shape", "wsname":"muR_ttbar","name":"ScaleR","proc":["ttlf","ttb","ttcc"]}, ## name used for shape
					"muF_ttbar":{"type":"shape", "wsname":"muF_ttbar","name":"ScaleF","proc":["ttlf","ttb","ttcc"]}, ## name used for shape

#					"muRFenv_ewk":{"type":"shape", "wsname":"muRFenv_ewk","name":"Scale","proc":["ewk"]}, ## name used for shape
#					"muRFenv_ewk":{"type":"shape", "wsname":"muRFenv_ewk","name":"Scale","proc":["ewk","dy_hf"]}, ## name used for shape
					"muRF_ewk":{"type":"shape", "wsname":"muRF_ewk","name":"ScaleRF","proc":["ewk","dy_hf"]}, ## name used for shape
					"muR_ewk":{"type":"shape", "wsname":"muR_ewk","name":"ScaleR","proc":["ewk","dy_hf"]}, ## name used for shape
					"muF_ewk":{"type":"shape", "wsname":"muF_ewk","name":"ScaleF","proc":["ewk","dy_hf"]}, ## name used for shape

#					"muRFenv_Hptb":{"type":"shape", "wsname":"muRFenv_Hptb","name":"Scale","proc":["Hptb"]}, ## name used for shape
					"muRF_Hptb":{"type":"shape", "wsname":"muRF_Hptb","name":"ScaleRF","proc":["Hptb"]}, ## name used for shape
					"muR_Hptb":{"type":"shape", "wsname":"muR_Hptb","name":"ScaleR","proc":["Hptb"]}, ## name used for shape
					"muF_Hptb":{"type":"shape", "wsname":"muF_Hptb","name":"ScaleF","proc":["Hptb"]}, ## name used for shape

#					"muRFenv_top":{"type":"shape", "wsname":"muRFenv_top","name":"Scale","proc":["top"]}, ## name used for shape
#					"muRF_top":{"type":"shape", "wsname":"muRF_top","name":"Scale","proc":["top"]}, ## name used for shape
#					"muR_top":{"type":"shape", "wsname":"muR_top","name":"Scale","proc":["top"]}, ## name used for shape
#					"muF_top":{"type":"shape", "wsname":"muF_top","name":"Scale","proc":["top"]}, ## name used for shape

#					"muRFenv_ttx":{"type":"shape", "wsname":"muRFenv_ttx","name":"Scale","proc":["ttx"]}, ## name used for shape
					"muRF_ttx":{"type":"shape", "wsname":"muRF_ttx","name":"ScaleRF","proc":["ttx"]}, ## name used for shape
					"muR_ttx":{"type":"shape", "wsname":"muR_ttx","name":"ScaleR","proc":["ttx"]}, ## name used for shape
					"muF_ttx":{"type":"shape", "wsname":"muF_ttx","name":"ScaleF","proc":["ttx"]}, ## name used for shape

					"CMS_res_j":{"type":"shape", "wsname":"CMS_res_j","name":"JER","proc":[".*"]}, ## name used for shape
                                        ####
                                        #### JEC sources
                                        ####
#					"CMS_scale_j":{"type":"shape", "wsname":"CMS_scale_j","name":"JES","proc":[".*"]}, ## name used for shape
					"CMS_scale_j":{"type":"shape", "wsname":"CMS_scale_j","name":"JESANDCSV","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_AbsoluteStat_j":{"type":"shape", "wsname":"CMS_scale_AbsoluteStat_j","name":"JES_AbsoluteStat","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_AbsoluteScale_j":{"type":"shape", "wsname":"CMS_scale_AbsoluteScale_j","name":"JES_AbsoluteScale","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_AbsoluteMPFBias_j":{"type":"shape", "wsname":"CMS_scale_AbsoluteMPFBias_j","name":"JES_AbsoluteMPFBias","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_Fragmentation_j":{"type":"shape", "wsname":"CMS_scale_Fragmentation_j","name":"JES_Fragmentation","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_SinglePionECAL_j":{"type":"shape", "wsname":"CMS_scale_SinglePionECAL_j","name":"JES_SinglePionECAL","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_SinglePionHCAL_j":{"type":"shape", "wsname":"CMS_scale_SinglePionECAL_j","name":"JES_SinglePionHCAL","proc":[".*"]}, ## name used for shape
# 					"CMS_scale_FlavorQCD_j":{"type":"shape", "wsname":"CMS_scale_FlavorQCD_j","name":"JES_FlavorQCD","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_TimePtEta_j":{"type":"shape", "wsname":"CMS_scale_TimePtEta_j","name":"JES_TimePtEta","proc":[".*"]}, ## name used for shape
					#####
#                                        "CMS_scale_RelativeJEREC1_j":{"type":"shape", "wsname":"CMS_scale_RelativeJEREC1_j","name":"JES_RelativeJEREC1","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativePtBB_j":{"type":"shape", "wsname":"CMS_scale_RelativePtBB_j","name":"JES_RelativePtBB","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativePtEC1_j":{"type":"shape", "wsname":"CMS_scale_RelativePtEC1_j","name":"JES_RelativePtEC1","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativeBalance_j":{"type":"shape", "wsname":"CMS_scale_RelativePtBalance_j","name":"JES_RelativeBal","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativeFSR_j":{"type":"shape", "wsname":"CMS_scale_RelativeFSR_j","name":"JES_RelativeFSR","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativeStatFSR_j":{"type":"shape", "wsname":"CMS_scale_RelativeStatFSR_j","name":"JES_RelativeStatFSR","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativeStatEC_j":{"type":"shape", "wsname":"CMS_scale_RelativeStatEC_j","name":"JES_RelativeStatEC","proc":[".*"]}, ## name used for shape
					#####
#                                        "CMS_scale_PileUpPtBB_j":{"type":"shape", "wsname":"CMS_scale_PileUpPtBB_j","name":"JES_PileUpPtBB","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_PileUpPtEC1_j":{"type":"shape", "wsname":"CMS_scale_PileUpPtEC1_j","name":"JES_PileUpPtEC1","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_PileUpDataMC_j":{"type":"shape", "wsname":"CMS_scale_PileUpDataMC_j","name":"JES_PileUpDataMC","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_PileUpPtRef_j":{"type":"shape", "wsname":"CMS_scale_PileUpPtRef_j","name":"JES_PileUpPtRef","proc":[".*"]}, ## name used for shape
					##### comment since are null
#                                        "CMS_scale_RelativeJEREC2_j":{"type":"shape", "wsname":"CMS_scale_RelativeJEREC2_j","name":"JES_RelativeJEREC2","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_PileUpPtEC2_j":{"type":"shape", "wsname":"CMS_scale_PileUpPtEC2_j","name":"JES_PileUpPtEC2","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativeJERHF_j":{"type":"shape", "wsname":"CMS_scale_RelativeJERHF_j","name":"JES_RelativeJERHF","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativePtHF_j":{"type":"shape", "wsname":"CMS_scale_RelativePtHF_j","name":"JES_RelativePtHF","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_RelativeStatHF_j":{"type":"shape", "wsname":"CMS_scale_RelativeStatHF_j","name":"JES_RelativeStatHF","proc":[".*"]}, ## name used for shape
#                                        "CMS_scale_PileUpPtHF_j":{"type":"shape", "wsname":"CMS_scale_PileUpPtHF_j","name":"JES_PileUpPtHF","proc":[".*"]}, ## name used for shape
                                        ####
                                        #### BTAG UNC
                                        ####
					##Light jets Heavy flavor contamination
					"CMS_btag_HF":{"type":"shape", "wsname":"CMS_btag_HF","name":"CSVRHF","proc":[".*"]}, ## name used for shape
					##Heavy jets light flavor contamination
					"CMS_btag_LF":{"type":"shape", "wsname":"CMS_btag_LF","name":"CSVRLF","proc":[".*"]}, ## name used for shape
					##Linear and quadratic uncertainties
###					CMS_eff_b_CFerr2 --> CMS_bTag_CFerr2
					"CMS_btag_HFstat1":{"type":"shape", "wsname":"CMS_btag_HFstat1","name":"CSVRHFSTAT1","proc":[".*"]}, ## name used for shape
					"CMS_btag_HFstat2":{"type":"shape", "wsname":"CMS_btag_HFstat2","name":"CSVRHFSTAT2","proc":[".*"]}, ## name used for shape
					"CMS_btag_LFstat1":{"type":"shape", "wsname":"CMS_btag_LFstat1","name":"CSVRLFSTAT1","proc":[".*"]}, ## name used for shape
					"CMS_btag_LFstat2":{"type":"shape", "wsname":"CMS_btag_LFstat2","name":"CSVRLFSTAT2","proc":[".*"]}, ## name used for shape
					"CMS_btag_CFerr1":{"type":"shape", "wsname":"CMS_btag_CFerr1","name":"CSVRCERR1","proc":[".*"]}, ## name used for shape
					"CMS_btag_CFerr2":{"type":"shape", "wsname":"CMS_btag_CFerr2","name":"CSVRCERR2","proc":[".*"]} ## name used for shape
					####
					}
			else:
				systStore={
					"None":None
					}


#print "--------- CAT STORE IS --------"
#for cat in catStore:
#	print "* ",cat,":",catStore[cat]
#print "---------------------- --------"

#fileTmp="AUG6_HT/"+label+VarTest+opts.output
fileTmp="NOV17_HT200_UE_1Mu_muRF/"+ label + VarTest + str(opts.kMass) + opts.output

w = ROOT.RooWorkspace("w","w")
datNameTmp = opts.datCardName
#datName = "AUG6_HT/"+ label + VarTest + datNameTmp
datName = "NOV17_HT200_UE_1Mu_muRF/"+ label + VarTest + str(opts.kMass) + datNameTmp

datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")

w.factory("ht[0,8000]"); # RooRealVar
ht=w.var("ht")

w.factory("bdt[0,1]")
bdt=w.var("bdt")

w.factory("bdt2D[-0.5,3.5]")
bdt2D=w.var("bdt2D")

arglist_obs = ROOT.RooArgList(ht)

arglist_obs_bdt = ROOT.RooArgList(bdt)

arglist_obs_bdt2D = ROOT.RooArgList(bdt2D)


for x in catStore:
	w.factory("ht_"+x+"[0,8000]"); # RooRealVar
	htx=w.var("ht_"+x)
	htx.setBins(LikeBins)
	arglist_obs.add(htx)

	w.factory("bdt_"+x+"[0,1]"); # RooRealVar
	bdtx=w.var("bdt_"+x)
	bdtx.setBins(LikeBins)
	arglist_obs_bdt.add(bdtx)

	w.factory("bdt2D_"+x+"[-0.5,6.5]"); # RooRealVar
	bdt2Dx=w.var("bdt2D_"+x)
	bdt2Dx.setBins(LikeBins)
	arglist_obs_bdt2D.add(bdt2Dx)


def skip(cat,mc):
	if cat["hasMC"][0] == "all": return False
	for x in cat["hasMC"]:
		if mc["name"] == x : return False
	return True


## write shapes in workspace
#if True: # data
#	datacard.write("shapes data_obs *\t" + fileTmp +"\t")
#	datacard.write("w:data_obs_$CHANNEL")
#	datacard.write("\n")
#if True: # Sig
#	datacard.write("shapes Hptb *\t" + fileTmp +"\t")
###	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL\t")
###	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL_$SYSTEMATIC")
#	datacard.write("w:pdf_$PROCESS_$CHANNEL\t")
#	datacard.write("w:pdf_$PROCESS_$CHANNEL_$SYSTEMATIC")
#	datacard.write("\n")
#if True: #bkg
#	datacard.write("shapes * * %s\t"%fileTmp +"\t")
#	datacard.write("w:pdf_$PROCESS_$CHANNEL\t")
#	datacard.write("w:pdf_$PROCESS_$CHANNEL_$SYSTEMATIC\n")
#datacard.write("-------------------------------------\n")
#datacard.write("## Observation\n")


## write shapes in workspace
if True: # data
	datacard.write("shapes data_obs *\t" + fileTmp.split('/')[1] +"\t")
	datacard.write("data_obs_$CHANNEL")
	datacard.write("\n")
if True: # Sig
	datacard.write("shapes Hptb *\t" + fileTmp.split('/')[1] +"\t")
###	datacard.write("pdf_$PROCESS_M-$MASS_$CHANNEL\t")
###	datacard.write("pdf_$PROCESS_M-$MASS_$CHANNEL_$SYSTEMATIC")
	datacard.write("pdf_$PROCESS_$CHANNEL\t")
	datacard.write("pdf_$PROCESS_$CHANNEL_$SYSTEMATIC")
	datacard.write("\n")
if True: #bkg
	datacard.write("shapes * * %s\t"%fileTmp.split('/')[1] +"\t")
	datacard.write("pdf_$PROCESS_$CHANNEL\t")
	datacard.write("pdf_$PROCESS_$CHANNEL_$SYSTEMATIC\n")
datacard.write("-------------------------------------\n")
datacard.write("## Observation\n")


# write observation
datacard.write("bin\t")
for cat in catStore:
	datacard.write("%s\t"%cat)
datacard.write("\n")
datacard.write("observation\t")
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
def writeSystISRFSR(name="test",valueL=["1.027","1.026"], regexpL=["TT","ST",""]):

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

		if (idx<0): datacard.write("\t-")
		if (idx<0): continue

		valueUp=0
		valueDown=0

		if "ISR" in name:
			mySyst="isr"
		if "FSR" in name:
			mySyst="fsr"
		if "HDAMP" in name:
			mySyst="hdamp"

##		regexpL=["ttlf","ttb","ttbb","tt2b","ttcc"]

#		if "1L" in cat:
		if "1L" in cat or "1Ele" in cat or "1Mu" in cat and not "1Ele1Mu" in cat :
		###
			if "topCRR4" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 4, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 4, 1, proc)
			if "topCRR5" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 4, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 4, 2, proc)
			if "charmCR" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 4, 3, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 4, 3, proc)
			if "extraRadCRR7" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 5, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 5, 1, proc)
			if "extraRadCRR10" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 6, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 6, 1, proc)
			###
			if "SR1" in cat and not ("SR13" in cat):
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 5, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 5, 2, proc)
			if "SR2" in cat and not ("SR24" in cat):
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 5, 3, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 5, 3, proc)
			if "SR3" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 6, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 6, 2, proc)
			if "SR4" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 6, 3, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 6, 3, proc)
			###
			if "extraRadCR" in cat and not ("extraRadCRR10" in cat) and not ("extraRadCRR7" in cat):
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 6, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 6, 1, proc)
			if "SR13" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 5, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 5, 2, proc)
			if "SR24" in cat:
				valueUp = PartonShowerSyst(mySyst, "1L", "up", 5, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "1L", "down", 5, 2, proc)
		else :

			if "topCRR4" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 2, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 2, 1, proc)
			if "topCRR5" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 2, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 2, 2, proc)
			if "charmCR" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 2, 3, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 2, 3, proc)
			if "extraRadCRR7" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 3, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 3, 1, proc)
			if "extraRadCRR10" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 4, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 4, 1, proc)
			if "SR1" in cat and not ("SR13" in cat):
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 3, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 3, 2, proc)
			if "SR2" in cat and not ("SR24" in cat):
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 3, 3, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 3, 3, proc)
			if "SR3" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 4, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 4, 2, proc)
			if "SR4" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 4, 3, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 4, 3, proc)
			###
			if "extraRadCR" in cat and not ("extraRadCRR10" in cat) and not ("extraRadCRR7" in cat):
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 3, 1, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 3, 1, proc)
			if "SR13" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 3, 2, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 3, 2, proc)
			if "SR24" in cat:
				valueUp = PartonShowerSyst(mySyst, "2L", "up", 3, 3, proc)
				valueDown = PartonShowerSyst(mySyst, "2L", "down", 3, 3, proc)

		if (idx>=0):
##		   print 'out of the function valueUp=',valueUp,' valueDown=',valueDown
                   valueUp=1+valueUp/100.
		   valueDown=1+valueDown/100.
		   mystring='%.2f/%.2f' % (valueDown, valueUp)
##		   datacard.write("\t"+str(valueL[idx]))
##		   print 'normalized things=',mystring
		   datacard.write("\t"+mystring)
		else:
		   datacard.write("\t-")
	datacard.write("\n")


## 
##def writeNormSyst(name="lumi",valueL=["1.027","1.026"], regexpL=["TT"]):
def writeNormSyst(name="lumi",valueL=["1.027","1.026"], regexpL=["TT","ST",""],regexpCat=[]):
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
		idx2=-1
		invert=False
		for i,regexp in enumerate(regexpCat):
			match=re.search(regexp,cat)
			if regexp != "" and regexp[0] == '!':
				invert=True
				regexp=regexp[1:]
			if (match and not invert) or (not match and invert):
				idx2=i
				break

		if (idx>=0 and (idx2>=0 or len(regexpCat)==0 )):
		   datacard.write("\t"+valueL[idx])
		else:
		   datacard.write("\t-")
	datacard.write("\n")

def writeSystShape(syst,regexpL=[],regexpCat=None):
# this function put a bunch of 1
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
	      ##### TO CHECK #####
 	      h.SetBinError(iBin,minError)
	   for i in range(0,h.GetNbinsX()):
	      statsyst = { "wsname": statName + "_Bin%d"%(i+1) +"_STAT" , "name": statName + "_Bin%d"%(i+1) + "_STAT"}
	      hupbin=h0.Clone(h.GetName() +"_STATUp")
	      hdnbin=h0.Clone(h.GetName() +"_STATDown")
	      cont=h0.GetBinContent(i+1)
	      err = h . GetBinError(i+1) ## err is referred to the sum
	      c = h.GetBinContent(i+1) ## this is to check the magnitude, not to apply it
	      if c == 0: continue
#	      if c > 0 and err/c <.01: continue ## don't write less than 1%
	      if c > 0 and err/c <.10: continue ## don't write less than 10% // TEMPORARY
	      hupbin.SetBinContent(i+1,cont+err)
	      hdnbin.SetBinContent(i+1,cont-err)

	      if hupbin.GetBinContent(i+1) <0 : hupbin.SetBinContent(i+1,cont)
	      if hdnbin.GetBinContent(i+1) <0 : hdnbin.SetBinContent(i+1,cont)

	      target = stat["target"]
	      cat = catStore[stat["cat"] ]
	      print "++++++++++++++++++++target",target,' category',cat

#######

	      for i in range(0,len(arglist_obs)):

		      if cat["name"] in arglist_obs.at(i).GetName():

			      al=arglist_obs_bdt.at(i)
			      if "bdt2D" in cat["var"]: al=arglist_obs_bdt2D.at(i)

			      if not doST:
				      if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs.at(i)
			      if doST:
				      if cat["var"] == "ST" or cat["var"] == "ST_SR1" or cat["var"] == "ST_SR2" or cat["var"] == "ST_SR3"or cat["var"] == "ST_SR4" or cat["var"] == "ST_SR13" or cat["var"] == "ST_SR24": al = arglist_obs.at(i)
#			      print "-> with var", al

	      roo_mc_binup = ROOT.RooDataHist(target+"_"+statsyst["wsname"]+"Up",target + "STAT",ROOT.RooArgList(al),hupbin)
	      pdf_mc_binup = roo_mc_binup
	      roo_mc_bindn = ROOT.RooDataHist(target+"_"+statsyst["wsname"]+"Down",target + "STAT",ROOT.RooArgList(al),hdnbin)
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
		if "CMS_eff_m" in systStore[syst]["wsname"]:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["1Mu"])
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["1Mu1Ele"])
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["2Mu"])
		elif "CMS_eff_e" in systStore[syst]["wsname"]:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["1Ele"])
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["2Ele"])
		elif "CMS_eff_l" in systStore[syst]["wsname"]:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["2L"])
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["1L"])
		elif "CMS_eff_t_veto" in systStore[syst]["wsname"]:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["1Mu$","1Ele$"])
		else:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"])
	if systStore[syst]["type"] == "shape":

#		if opts.kTest==0 or opts.kTest==1 or opts.kTest==2 or opts.kTest==3 or opts.kTest==4 or opts.kTest==5 or opts.kTest==6 or opts.kTest==7 or opts.kTest==8 or opts.kTest==9 or opts.kTest==10 or opts.kTest==11 or opts.kTest==12 or opts.kTest==13 or opts.kTest==14 or opts.kTest==15 or opts.kTest==16 or opts.kTest==100 or opts.kTest==101  or opts.kTest==102:
#			if ('CMS_scale_j' in syst): systStore[syst]["wsname"] += "_1l"
#			if ('CMS_btag_LFstat1' in syst): systStore[syst]["wsname"] += "_1l"
#			if ('CMS_btag_LFstat2' in syst): systStore[syst]["wsname"] += "_1l"
#			if ('CMS_btag_LF' in syst and not 'LFstat' in syst): systStore[syst]["wsname"] += "_1l"

#		elif opts.kTest==30 or opts.kTest==31 or opts.kTest==32 or opts.kTest==33 or opts.kTest==34 or opts.kTest==35 or opts.kTest==36 or opts.kTest==37 or opts.kTest==38 or opts.kTest==39 or opts.kTest==40 or opts.kTest==41 or opts.kTest==42 or opts.kTest==43 or opts.kTest==44 or opts.kTest==45 or opts.kTest==46 or opts.kTest==200 or opts.kTest==201  or opts.kTest==202:
#			if ('CMS_scale_j' in syst): systStore[syst]["wsname"] += "_2l"
#			if ('CMS_btag_LFstat1' in syst): systStore[syst]["wsname"] += "_2l"
#			if ('CMS_btag_LFstat2' in syst): systStore[syst]["wsname"] += "_2l"
#			if ('CMS_btag_LF' in syst and not 'LFstat' in syst): systStore[syst]["wsname"] += "_2l"

		writeSystShape(systStore[syst],systStore[syst]["proc"])



if doSyst: writeNormSyst("QCDscale_ttbar",["0.965/1.024","0.965/1.024","0.965/1.024"],["ttlf","ttcc","ttb"])
if doSyst: writeNormSyst("pdf_gg",["1.042","1.042","1.042"],["ttlf","ttcc","ttb"])
if doSyst: writeNormSyst("CMS_mass_ttbar",["1.027","1.027","1.027"],["ttlf","ttcc","ttb"])

###if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttlf",["1.50"],["ttlf"])
if doSyst: writeNormSyst("bgnorm_ttcc",["1.50"],["ttcc"])
if doSyst: writeNormSyst("bgnorm_ttb",["1.50"],["ttb$"])
#if doSyst: writeNormSyst("bgnorm_ttbb",["1.50"],["ttbb$"])
##if doSyst: writeNormSyst("bgnorm_tt2b",["1.50"],["tt2b"])

## V+jets HF
#if doSyst: writeNormSyst("bgnorm_dy_hf",["1.50"],["dy_hf"])

## "top" get the value od the single top
if doSyst: writeNormSyst("QCDscale_top",["0.977/1.028"],["top"])
if doSyst: writeNormSyst("pdf_qg",["1.026"],["top"])
if doSyst: writeNormSyst("CMS_mass_top",["1.022"],["top"])

## "ewk" get the envelop of the DY and W+jets from the AN2017_063_v11
if doSyst: writeNormSyst("QCDscale_ewk",["0.996/1.008"],["ewk"])
if doSyst: writeNormSyst("pdf_qqbar",["1.04"],["ewk"])
#if doSyst: writeNormSyst("QCDscale_ewk",["0.996/1.008","0.996/1.008"],["ewk","dy_hf"])
#if doSyst: writeNormSyst("pdf_qqbar",["1.04","1.04"],["ewk","dy_hf"])

### "ttx" take the ttW from the AN2017_063_v11
if doSyst: writeNormSyst("QCDscale_ttx",["0.88/1.13"],["ttx"])
if doSyst: writeNormSyst("pdf_qqbar",["1.02"],["ttx"])
if doSyst: writeNormSyst("CMS_mass_top",["1.022"],["ttx"])

if doSyst: writeSystISRFSR(name="CMS_HPTB_ISR",valueL=["-999","999"], regexpL=["ttlf","ttb","ttcc"])
if doSyst: writeSystISRFSR(name="CMS_HPTB_FSR",valueL=["-999","999"], regexpL=["ttlf","ttb","ttcc"])
if doSyst: writeSystISRFSR(name="CMS_HPTB_HDAMP",valueL=["-999","999"], regexpL=["ttlf","ttb","ttcc"])

## "ISR/FSR"
#if opts.kTest==0 or opts.kTest==1 or opts.kTest==2 or opts.kTest==3 or opts.kTest==4 or opts.kTest==5 or opts.kTest==6 or opts.kTest==7 or opts.kTest==8 or opts.kTest==9 or opts.kTest==10 or opts.kTest==11 or opts.kTest==12 or opts.kTest==13 or opts.kTest==14 or opts.kTest==15 or opts.kTest==16 or opts.kTest==100 or opts.kTest==101  or opts.kTest==102:

#	if doSyst: writeSystISRFSR(name="CMS_HPTB_ISR_1l",valueL=["-999","999"], regexpL=["ttlf","ttb","ttbb","tt2b","ttcc"])
#	if doSyst: writeSystISRFSR(name="CMS_HPTB_FSR_1l",valueL=["-999","999"], regexpL=["ttlf","ttb","ttbb","tt2b","ttcc"])
#	if doSyst: writeSystISRFSR(name="CMS_HPTB_HDAMP_1l",valueL=["-999","999"], regexpL=["ttlf","ttb","ttbb","tt2b","ttcc"])

#elif opts.kTest==30 or opts.kTest==31 or opts.kTest==32 or opts.kTest==33 or opts.kTest==34 or opts.kTest==35 or opts.kTest==36 or opts.kTest==37 or opts.kTest==38 or opts.kTest==39 or opts.kTest==40 or opts.kTest==41 or opts.kTest==42 or opts.kTest==43 or opts.kTest==44 or opts.kTest==45 or opts.kTest==46 or opts.kTest==200 or opts.kTest==201  or opts.kTest==202:

#	if doSyst: writeSystISRFSR(name="CMS_HPTB_ISR_2l",valueL=["-999","999"], regexpL=["ttlf","ttb","ttbb","tt2b","ttcc"])
#	if doSyst: writeSystISRFSR(name="CMS_HPTB_FSR_2l",valueL=["-999","999"], regexpL=["ttlf","ttb","ttbb","tt2b","ttcc"])
#	if doSyst: writeSystISRFSR(name="CMS_HPTB_HDAMP_2l",valueL=["-999","999"], regexpL=["ttlf","ttb","ttbb","tt2b","ttcc"])


#if doSyst: writeNormSyst("QCDscale_ttbar",["0.977/1.028"],["ST"])
#if doSyst: writeNormSyst("pdf_ttbar",["1.026"],["ST"])
#if doSyst: writeNormSyst("CMS_mass_ttbar",["1.022"],["ST"])

##https://twiki.cern.ch/twiki/bin/view/LHCPhysics/LHCHXSWGTTH
#if doSyst: writeNormSyst("QCDscale_ttZ",["0.887/1.096"],["TTZ"])
#if doSyst: writeNormSyst("pdf_ttZ",["1.028"],["TTZ"])
#if doSyst: writeNormSyst("CMS_mass_ttZ",["1.028"],["TTZ"])

#if doSyst: writeNormSyst("QCDscale_ttH",["0.908/1.058"],["ttH"])
#if doSyst: writeNormSyst("pdf_ttH",["1.036"],["ttH"])
#if doSyst: writeNormSyst("CMS_mass_ttH",["1.036"],["ttH"])

##writeNormSyst("QCDscale_VV",["1.025","1.032","1.031"],["WWTo","WZTo","ZZTo"])
##writeNormSyst("pdf_VV",["1.022","1.044","1.037"],["WWTo","WZTo","ZZTo"])

##writeNormSyst("QCDscale_V",["0.9963/1.0065","0.996/1.008"],["DYJetsToLL_M","WJetsToLNu"])
##writeNormSyst("pdf_V",["1.037","1.0375"],["DYJetsToLL_M","WJetsToLNu"])

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

def MergeCategory(tfile,hname):

	if "1L" in hname:
		toget_1=hname.replace("_1L", "_1Mu")
		toget_2=hname.replace("_1L", "_1Ele")
		hTmp=tfile.Get(toget_1)
		hTmp2=tfile.Get(toget_2)
		if hTmp!=None and hTmp2!=None: hTmp.Add(hTmp2)
	elif "2L" in hname:
		toget_1=hname.replace("_2L", "_2Mu")
		toget_2=hname.replace("_2L", "_2Ele")
		toget_3=hname.replace("_2L", "_1Mu1Ele")
		hTmp=tfile.Get(toget_1)
		hTmp2=tfile.Get(toget_2)
		hTmp3=tfile.Get(toget_3)
		if hTmp!=None and hTmp2!=None: hTmp.Add(hTmp2)
		if hTmp!=None and hTmp3!=None: hTmp.Add(hTmp3)
	else:

		hTmp=tfile.Get(hname)
## temporaty need to fix for the merged categories
#		print 'MergeCategory ',hname
#		if "bb_DYJetsTo" in hname:
#			hTmp = MergeDYHF(tfile,hname)
#		else: hTmp=tfile.Get(hname)


		#nominal
	if hTmp!= None and "bdt" in hname:
		hTmp.Rebin(nBinsHT)
	elif hTmp!= None:
		hTmp.Rebin(nBinsHT)


	return hTmp




def SmoothAndMergeSyst(tfile,togetNom,togetSyst,s):


	hTmpNom=MergeCategory(tfile,togetNom)
	hTmpUp=MergeCategory(tfile,togetSyst+'Up')
	hTmpDown=MergeCategory(tfile,togetSyst+'Down')

	if hTmpUp==None: return hTmpNom
	if hTmpDown==None: return hTmpNom

	for iBin in range(1,hTmpNom.GetNbinsX()+1):
#		c= hTmpNom.SetBinError(iBin,0)
		c= hTmpUp.SetBinError(iBin,0)
		c= hTmpDown.SetBinError(iBin,0)

        if LikeBins==1 or LikeBins==2:

                hUp = hTmpUp
                hDown = hTmpDown

        else:

		# SMOOTH ALL
		hUp, hDown = SS.SystematicSmoother(hTmpNom, hTmpUp, hTmpDown,"~/www/ChargedHiggsTB/MAY3/SYST/").smooth()

	if 'Up' in s:
		return hUp

	if 'Down' in s:
		return hDown

def envelop(tfile,togetNom, togetSyst, s) :

	hTmpNom=MergeCategory(tfile,togetNom)

	hname=togetSyst+'RF'+s
	hTmp=MergeCategory(tfile,hname)
	h=hTmp.Clone()

	if hTmp==None: return

	for w in [ 'R','F','RF']:
		for var in [ 'Up','Down']:

			hnameSyst=togetSyst+w+var

			hTmp=MergeCategory(tfile,hnameSyst)

			for iBin in range(1,h.GetNbinsX()+1):
				c= h.GetBinContent(iBin)

			## take the maximum for the Up
				if 'Up' in s and hTmp.GetBinContent(iBin)>c:
					h.SetBinContent(iBin,hTmp.GetBinContent(iBin))

			## take the minimum for the Down
				if 'Down' in s and hTmp.GetBinContent(iBin)<c:
					h.SetBinContent(iBin,hTmp.GetBinContent(iBin))

	if 'Up' in s:
		hTmpUp = h
		hUp, hDown = SS.SystematicSmoother(hTmpNom, hTmpUp, hTmpUp.Clone("test1"),"~/www/ChargedHiggsTB/JULY30/SYST/").smooth()
		return hUp

	if 'Down' in s:
		hTmpDown = h
		hUp, hDown = SS.SystematicSmoother(hTmpNom, hTmpDown.Clone("test1"), hTmpDown,"~/www/ChargedHiggsTB/JULY30/SYST/").smooth()
		return hDown


def MergeDYHF(tfile,hname):

	hname_1=hname.replace("bb_DYJetsTo", "b_DYJetsTo")
	hname_2=hname.replace("bb_DYJetsTo", "other_DYJetsTo")
	hTmp=tfile.Get(hname)
	hTmp1=tfile.Get(hname_1)
	hTmp2=tfile.Get(hname_2)
	if hTmp!=None and hTmp1!=None: hTmp.Add(hTmp1)
	if hTmp!=None and hTmp2!=None: hTmp.Add(hTmp2)

        return hTmp



## import Everything in ws TODO
def importPdfFromTH1(cat,mc,myBin,LikelihoodMapping,syst=None):

	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	if mc["name"]=="Hptb":masses=[180,200,220,250,300,350,400,500,650,800,1000,1500,2000,2500,3000]
	else: masses=[0]

	if syst == None: shifts=["x"]
	else: shifts=["Up","Down"]
#	else: shifts=["Down","Up"]

	skip=False
	if syst != None:
		  skip=True

		  invert=False
		  regexpL = syst["proc"]
		  print '==================='
		  print '====',syst
		  print '====',syst["proc"],'  ',mc["name"]
		  print '==================='
		  for i,regexp in enumerate(regexpL):
		  	if regexp != "" and regexp[0] == '!':
		  		invert=True
		  		regexp=regexp[1:]
		  	match=re.search(regexp,mc["name"])
		  	if (match  and not invert) or (not match and invert):
		  		idx=i
		  		skip=False
		  		break

	if skip: return

	scaleEveRemoval=1
	scaleEveRemovalW=1
	delta=0
	deltaW=0

####
####

	h=None

	for s in shifts:

		hTmpSum=None

		for m in masses:

			target = "pdf_" + mc["name"] +"_"+ cat["name"]


			if syst != None:
				target += "_" + syst["wsname"] + s

#				if ('CMS_scale_j' in syst["wsname"] or 'CMS_btag_LF' in syst["wsname"]) and ('2Mu' in cat["dir"] or '2Ele' in cat["dir"] or '1Mu1Ele' in cat["dir"] or '2L' in cat["dir"]):
#					target += "_2l"
#				target += s

##			print  mc["hist"]

			for hname in mc["hist"]:
				toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

				hTmp=None

				if mc["name"]=="Hptb" and m==opts.kMass:
					toget=toget%m

				if mc["name"]=="Hptb" and m!=opts.kMass:
					continue

				togetNom = toget
				togetSyst = ''
				if syst != None:
					togetSyst = toget + "_" + syst["name"]
					toget += "_" + syst["name"] + s


###
##				if syst != None and "QCDscale" in target:
				if syst != None and "muRFenv" in target:
					if  mc["name"]=="Hptb" or "WJetsToLNu_HT" in togetSyst or "DYJetsToLL_M-50_HT" in togetSyst or "DYJetsToLL_M-5to50_HT" in togetSyst or "ttlf" in mc["name"] or "ttb" in mc["name"] or "ttbb" in mc["name"] or "tt2b" in mc["name"] or "ttcc" in mc["name"] or "ST" in togetSyst or "TTX" in togetSyst:

						hTmp = envelop(tfile,togetNom,togetSyst,s)

				elif syst != None:

					hTmp = SmoothAndMergeSyst(tfile,togetNom,togetSyst,s)
				else:
					hTmp = MergeCategory(tfile,togetNom)
######

				if hTmp!= None: print "<*> Reading Hist '"+toget+"' integral=",hTmp.Integral(),' nBin=',hTmp.GetNbinsX(), 'underflow=',hTmp.GetBinContent(0), 'overflow=',hTmp.GetBinContent(hTmp.GetNbinsX()+1),' entries=',hTmp.GetEntries()
				if hTmp == None:
					if syst == None:
						print "<*> Hist '"+toget+"' doesn't exist"
						raise IOError
					if syst != None:
						print "<*> Hist '"+toget+"' doesn't exist with the syst cloning now from the central "
#						toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

						hTmp = MergeCategory(tfile,togetNom)

						if hTmp!= None: print "<*> Reading Hist '"+toget+"' integral=",hTmp.Integral(),' nBin=',hTmp.GetNbinsX(), 'underflow=',hTmp.GetBinContent(0), 'overflow=',hTmp.GetBinContent(hTmp.GetNbinsX()+1),' entries=',hTmp.GetEntries()

				# added the UE events
				if hTmp and ( mc["name"]=="ttb" or mc["name"]=="ttbb" or mc["name"]=="ttcc" or mc["name"]=="ttlf" ) :
					hTmp.Scale(1./4)

### SCALE for the 2/3
#				if mc["name"]!="qcd" and not "EWK" in hname and not "TT_TuneCUETP8M2T4_13TeV" in hname:
# only the SIGNAL/TTBAR and V+jets are skimmed
				if hTmp and mc["name"]!="qcd" and not "EWK" in hname and not "TTX" in hname and not "ST" in hname:
					hTmp.Scale(1.5)

### -- MC --
##		print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]
#####1L
		## tranform the HT into the likelihood
				hTmp = likelihoodBinning.applyMapping(LikelihoodMapping, hTmp)
#		print LikelihoodMapping


#				if mc["name"]=="Hptb" and hTmpSum==None: hTmpSum = hTmp
#				if mc["name"]=="Hptb":
#					print 'doing Hptb'
#					if hTmpSum==None:
#						print 'doing check on None'
#						hTmpSum = hTmp
#
#				if mc["name"]!="Hptb":
				if hTmpSum==None:hTmpSum = hTmp
				else:
					hTmpSum.Add(hTmp)


		#clean h

			h=hTmpSum

		if h!= None: print "<*> Read Hist '"+toget+"'",h.Integral(),' nBin=',h.GetNbinsX(), 'underflow=',h.GetBinContent(0), 'overflow=',h.GetBinContent(h.GetNbinsX()+1),' entries=',h.GetEntries()

		if h.GetBinContent(0)<0:
			print "ERROR histogram", h.GetName(),"has negative underflow norm"
			raise ValueError

		#	  print ' underflow=',h.SetBinContent(0,0)
		#	  print ' overflow=',h.SetBinContent(h.GetNbinsX()+1,0)

		if h: h.SetBinContent(0,0) ##underflow
		if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

		if h: ##negative yield
			for b in range(1,h.GetNbinsX()+1):
				#			  if h.GetBinContent(b) <0 : print ' negative weights in bin',b
				if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

		if h:
			if h.Integral() <= 0 :
				print "ERROR histogram", h.GetName(),"has null norm"
				raise ValueError

	 #save RooDataHist

		h.Scale(opts.lumi)

		print "* Importing ",target

		for i in range(0,len(arglist_obs)):

			if cat["name"] in arglist_obs.at(i).GetName():

				al=arglist_obs_bdt.at(i)
				if "bdt2D" in cat["var"]: al=arglist_obs_bdt2D.at(i)

				if not doST:
					if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs.at(i)
				if doST:
					if cat["var"] == "ST" or cat["var"] == "ST_SR1" or cat["var"] == "ST_SR2" or cat["var"] == "ST_SR3"or cat["var"] == "ST_SR4" or cat["var"] == "ST_SR13" or cat["var"] == "ST_SR24": al = arglist_obs.at(i)

		roo_mc = ROOT.RooDataHist(target,target, ROOT.RooArgList(al),h)

		pdf_mc = roo_mc
		getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
		g.extend([h,roo_mc,pdf_mc])

### WRITE HISTOGRAMS
#	 w.writeToFile(fileTmp)
		f = ROOT.TFile.Open(fileTmp,"update")
		h.SetName(target)
		h.Write()
		f.Write()
		f.Close()

		if syst==None and m< 10 :
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
				if mc["name"] == 'ttlf':  ## put TTlight and reference
					statStore[histName]["mc"]= mc["name"]
					statStore[histName]["target"]= target
					statStore[histName]["hist0"] . Delete()
					statStore[histName]["hist0"] = h.Clone(histName + "_STAT0")
	return

#################

## improt Everything in ws TODO
def importPdfFromTH1SumBKG(cat,mc,syst=None,do1Third=False):

	if do1Third:
		if ((("1Mu" in cat["dir"] or "1Ele" in cat["dir"]) and not ("1Mu1Ele" in cat["dir"])) or "1L" in cat["dir"]): tfile = fIn1Lbin
		if (("2Ele" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]) or "2L" in cat["dir"]): tfile = fIn2Lbin
		print '`+++> going to use the do1Third sample',tfile
	else:
		if ((("1Mu" in cat["dir"] or "1Ele" in cat["dir"]) and not ("1Mu1Ele" in cat["dir"])) or "1L" in cat["dir"]): tfile = fIn1L
		if (("2Ele" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]) or "2L" in cat["dir"]): tfile = fIn2
		print '`+++> going to use the 2/3 sample',tfile

	if tfile == None:
		print "<*> File not exists"
		raise IOError

	base="ChargedHiggsTopBottom"
	if mc["name"]=="Hptb":masses=[180,200,220,250,300,350,400,500,650,800,1000,1500,2000,2500,3000]
	else: masses=[0]

	if syst == None: shifts=["x"]
	else: shifts=["Up","Down"]

	skip=False
	if syst != None:
		  skip=True

		  invert=False
		  regexpL = syst["proc"]
		  print '==================='
		  print '====',syst
		  print '====',syst["proc"],'  ',mc["name"]
		  print '==================='
		  for i,regexp in enumerate(regexpL):
		  	if regexp != "" and regexp[0] == '!':
		  		invert=True
		  		regexp=regexp[1:]
		  	match=re.search(regexp,mc["name"])
		  	if (match  and not invert) or (not match and invert):
		  		idx=i
		  		skip=False
		  		break

	if skip: return

####
####

	hSum=None
	hRef=None
	hSig=None

	for m in masses:

	 for s in shifts:

	  for hname in mc["hist"]:

##                if mc["name"]=="Hptb": continue
##                if m==opts.kMass:

		  toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

		  if mc["name"]=="Hptb" and m==opts.kMass:
			  toget=toget%m

		  if mc["name"]=="Hptb" and m!=opts.kMass:
			  continue

		  hTmp = MergeCategory(tfile,toget)

		  # added the UE events
		  if hTmp and ( mc["name"]=="ttb" or mc["name"]=="ttbb" or mc["name"]=="ttcc" or mc["name"]=="ttlf") :
			  hTmp.Scale(1./4)

		  if hTmp!= None: print "<*> Reading Hist '"+toget+"'",hTmp.Integral(),' nBin=',hTmp.GetNbinsX(),'underflow=',hTmp.GetBinContent(0), 'overflow=',hTmp.GetBinContent(hTmp.GetNbinsX()+1), ' entries=',hTmp.GetEntries()

		  if hTmp == None:
			  print "<*> Hist '"+toget+"' doesn't exist"
			  ## raise error only if is on the 2Third and for the syst central value
			  if not do1Third:
				  if syst == None:
					  raise IOError

		  if mc["name"]!="Hptb" and hTmp!=None:
			  if hSum==None: hSum = hTmp
			  else:
#				  print 'going to add = ',hTmp.GetName()
				  hSum.Add(hTmp)

		  if mc["name"]=="ttlf" and hTmp!=None: hRef=hTmp.Clone()
		  if mc["name"]=="Hptb" and m==opts.kMass and hTmp!= None: hSig=hTmp.Clone()

	## here I have hRef, hSum, hSig

	if hRef: hRef.SetBinContent(0,0) ##underflow
	if hRef: hRef.SetBinContent(hRef.GetNbinsX()+1,0) #overflow

	if hSum: hSum.SetBinContent(0,0) ##underflow
	if hSum: hSum.SetBinContent(hSum.GetNbinsX()+1,0) #overflow

	if hSig: hSig.SetBinContent(0,0) ##underflow
	if hSig: hSig.SetBinContent(hSig.GetNbinsX()+1,0) #overflow

##	if h: ##negative yield
##		for b in range(1,h.GetNbinsX()+1):
##			if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

	if hRef:
		if hRef.Integral() <= 0 :
			print "ERROR histogram", hRef.GetName(),"has null norm"
			raise ValueError

	if hSum:
		if hSum.Integral() <= 0 :
			print "ERROR histogram", hSum.GetName(),"has null norm"
			raise ValueError

	if hSig:
		if hSig.Integral() <= 0 :
			print "ERROR histogram", hSig.GetName(),"has null norm"
			raise ValueError

	if hSum!=None:
		print "hSum.Integral",hSum.Integral()
		hSum.Scale(opts.lumi)

	if hRef!=None:
		hRef.Scale(opts.lumi)

	if hSig!=None:
		print "hSig.Integral",hSig.Integral()
		hSig.Scale(opts.lumi)

	return hSum,hRef,hSig



for c in catStore:

#import MC
	hSumAll1Third=None
	hSumAll2Third=None
	hRef1Third=None
	hRef2Third=None
	hSig1Third=None
	hSig2Third=None

	myBin=1

	## use the 1/3 of the sample
	for proc in mcStore:
		if skip(catStore[c],mcStore[proc]): continue
		hSumTMP,hRef1Third,sig=importPdfFromTH1SumBKG(catStore[c],mcStore[proc],None,True)
		mc=mcStore[proc]
		if mc["name"]=="Hptb":
			print '1/3 hSig outside importPdfFromTH1SumBKG',sig.Integral()
			hSig1Third=sig
		if hSumTMP!=None:
			if hSumAll1Third==None:
				print 'before hSumAll1Third',hSumAll1Third
				hSumAll1Third=hSumTMP
			else:
				print 'after else',hSumTMP
				hSumAll1Third.Add(hSumTMP)

#	## use the 2/3 of the sample
#	for proc in mcStore:
#		if skip(catStore[c],mcStore[proc]): continue
#		hSumTMP,hRef2Third,sig=importPdfFromTH1SumBKG(catStore[c],mcStore[proc],None,False)
#		mc=mcStore[proc]
#		if mc["name"]=="Hptb":
#			print 'hSig outside importPdfFromTH1SumBKG',sig.Integral()
#			hSig2Third=sig
#		if hSumAll2Third==None:
#			hSumAll2Third=hSumTMP
#		else:
#			hSumAll2Third.Add(hSumTMP)

	##MARIA
##	hSumAll=Rebin1LHT(hSumAll)
##	hSumAll=Rebin2LHT(hSumAll)
##	for b in range(1,hSumAll.GetNbinsX()+1):
##                if hSumAll.GetBinContent(b) == 0 :
##			print '+++++++++++++++++++++++++++++++'
##			print '+++++++++++++++++++++++++++++++'
##			print 'HEY: Zero Bin in the prediction bin = ', hSumAll.GetBinCenter(b)
##			print '+++++++++++++++++++++++++++++++'
##			print '+++++++++++++++++++++++++++++++'
##			raise IOError

#	print '+++++++++++++++++++++++++++++++'
#	print '+++++++++++++++++++++++++++++++'
#	print '+++++++++++++++++++++++++++++++'
###	print '===> before all=',hSumAll.Integral(),' ref=',hRef.Integral()
#	print '++++++++ ',catStore[c]
#	print '===> ++++++++',catStore[c],'hSumAll1Third.Integral()=',hSumAll1Third.Integral(), 'hSumAll1Third.GetEntries()=',hSumAll1Third.GetEntries() ,' ref=',hRef1Third.Integral(), ' sig=',hSig1Third.Integral()

### standard rebin
#	b = FwRebin.Rebin(hSumAll1Third, hSumAll1Third, maxStat)
### w/ merge to hget 0 negative events
##	b = FwRebin.RebinNeg(hSumAll, hSumAll, hSig, maxStat)
### do not use the ssqrt()
##	b =  FwRebin.RebinSsqrtB(hSumAll, hSumAll, hSig, maxStat)
#	print 'out of Rebin'
#	b.directionalRebin()
#	b.rebin()
#	myBinTMP = b.getBinArray()
#	print 'myBin=',myBinTMP,' yield=',hSumAll1Third.Integral()

#	b2= FwRebin.RebinNeg(hSumAll2Third, hSumAll2Third, hSig2Third, maxStat,myBinTMP)
#	b2.rebin()
#	myBin = b2.getBinArray()

#	LikelihoodMapping = []
	LikelihoodMapping = likelihoodBinning.createMapping(hSumAll1Third, hSig1Third)
#	print LikelihoodMapping

	for proc in mcStore:
		if skip(catStore[c],mcStore[proc]): continue
		for syst in systStore:
#			print "-> 2/3 calling import pdf with cat=",cat,"proc=",proc,"syst=",syst
#			print " * cat:",catStore[c]
#			print " * syst:", systStore[syst]
			if systStore[syst] == None or systStore[syst]["type"] == "shape" :
				importPdfFromTH1(catStore[c],mcStore[proc],myBin,LikelihoodMapping,systStore[syst])

##import data
###for c in catStore:


	cat=catStore[c]
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	target = "data_obs_"+ cat["name"] 
#	toget=base + "/" +cat["dir"] + "/" +  cat["var"]  +"_Data"

	toget=base + "/" +cat["dir"] + "/" +  cat["var"]
	if  ("1Ele" in cat["dir"] or "2Ele" in cat["dir"]) and not ("1Mu1Ele" in cat["dir"]): toget+="_SingleElectron"
	elif  "1Mu" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]: toget+="_SingleMuon"

	if "1Mu1Ele" in cat["dir"]:
		h=tfile.Get(toget)
		toget_2=toget.replace("_SingleMuon", "_SingleElectron")
		hTmp2=tfile.Get(toget_2)
		if h!= None and hTmp2: h.Add(hTmp2)
	elif "1L" in cat["dir"]:
		toget_1=toget.replace("_1L", "_1Mu")
		toget_1+="_SingleMuon"
		toget_2=toget.replace("_1L", "_1Ele")
		toget_2+="_SingleElectron"
		h=tfile.Get(toget_1)
		hTmp2=tfile.Get(toget_2)
		if h!= None: h.Add(hTmp2)
	elif "2L" in cat["dir"]:
		toget_1=toget.replace("_2L", "_2Mu")
		toget_1+="_SingleMuon"
		toget_2=toget.replace("_2L", "_2Ele")
		toget_2+="_SingleElectron"
		toget_3=toget.replace("_2L", "_1Mu1Ele")
		toget_3+="_SingleMuon"
		toget_4=toget.replace("_2L", "_1Mu1Ele")
		toget_4+="_SingleElectron"
		h=tfile.Get(toget_1)
		hTmp2=tfile.Get(toget_2)
		hTmp3=tfile.Get(toget_3)
		hTmp4=tfile.Get(toget_4)
		if h!= None and hTmp2: h.Add(hTmp2)
		if h!= None and hTmp3: h.Add(hTmp3)
		if h!= None and hTmp4: h.Add(hTmp4)
	else:
		h=tfile.Get(toget)

	if h != None and "bdt" in cat["var"]:
		h.Rebin(nBinsHT)
	elif h != None:
		h.Rebin(nBinsHT)

	if h == None:
		print "<*> Hist do not exists ",toget
		raise IOError

####This is the data
####1L
##		print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]

	if h != None:
		h = likelihoodBinning.applyMapping(LikelihoodMapping, h)

	if h != None:
		if h.GetBinContent(0)<0:
			print "ERROR histogram", h.GetName(),"has negative underflow obs"
			raise ValueError

		if h: h.SetBinContent(0,0) ##underflow
		if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

		for b in range(1,h.GetNbinsX()+1): ##negative yield
			if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

		if h:
			if h.Integral() <= 0 :
				print "ERROR histogram", h.GetName(),"has null norm"
				raise ValueError

#######
#######

	for i in range(0,len(arglist_obs)):
#		print '===> name',arglist_obs.at(i).GetName(), 'for ',i

		if cat["name"] in arglist_obs.at(i).GetName():

			al=arglist_obs_bdt.at(i)
			if "bdt2D" in cat["var"]: al=arglist_obs_bdt2D.at(i)

			if not doST:
				if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs.at(i)
			if doST:
				if cat["var"] == "ST" or cat["var"] == "ST_SR1" or cat["var"] == "ST_SR2" or cat["var"] == "ST_SR3"or cat["var"] == "ST_SR4" or cat["var"] == "ST_SR13" or cat["var"] == "ST_SR24": al = arglist_obs.at(i)
##			  al = arglist_obs.at(i)
#	print "-> with var", al

#######
#######

	if h != None:
		roo_data= ROOT.RooDataHist("data_obs_%s"%c,"H_{T}",ROOT.RooArgList(al),h)
		getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
		g.extend([h,roo_data])
		f = ROOT.TFile.Open(fileTmp,"update")
		h.SetName(target)
		h.Write()
		f.Write()
		f.Close()

## import and write statistical uncertainties
#importStat()

'* autoMCStats 0'
datacard.write("* autoMCStats 0")


w.writeToFile(fileTmp,False)
print "--------------------" 
print "datacard=",datName
print "ws=",fileTmp
print " --- DONE --- "

