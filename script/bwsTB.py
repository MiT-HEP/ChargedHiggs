#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

sys.path.insert(0, 'script')
import FwBinning as FwRebin
maxStat=0.13

parser= OptionParser()

parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/JULY12_Green_Final_1l.root")
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/JULY12_Green_Final_2l.root")

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
	res = re.sub('ELE','CMS_eff_e',res)
	res = re.sub('MU','CMS_eff_m',res)
	res = re.sub('TAU','CMS_eff_t',res)
	return res

def RebinN(h,nBin):
	''' Rebin with un-even bins '''
	h1=h.Rebin(nBin)
	return h1

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

def RebinBDT1(h):
        ''' Rebin with un-even bins '''
	print 'using BDT1 - high 1l'
        mybins=array('d',[-1,
                           -0.95,-0.90,-0.85,-0.80,-0.75,-0.70,-0.65,-0.6,
                           -0.56,-0.52,-0.48,-0.44,-0.40,
                           -0.36,-0.32,
                           -0.28,-0.24,
                           -0.2,-0.16,-0.12,
                           -0.08,-0.04,
                           0.0,0.04,0.08,
                           0.12,0.16,0.2,
                           0.24,0.28,
                           0.32,0.36,0.40,
                           0.42,0.44,0.46,0.48,0.5,
                           0.52,0.54,0.56,0.58,0.6,
                           0.62,0.64,0.66,0.68,0.7,
                           0.72,0.74,0.76,0.78,0.8,
                           0.82,0.84,0.86,0.88,0.9,
                           0.92,0.94,0.96,0.98,
                           1.])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1

def RebinBDT2(h):
        ''' Rebin with un-even bins '''
	print 'using BDT2'
        mybins=array('d',[-1,
                           -0.90,-0.8,-0.75,-0.7,
                           -0.65,-0.6,-0.55,-0.50,-0.45,-0.40,
                           -0.36,-0.32,-0.28,-0.24,-0.20,
                           -0.16,-0.12,-0.08,-0.04,0.0,
                           0.04,0.08,0.12,
                           0.16,0.20,0.24,0.28,0.32,0.36,0.40,
                           0.44,0.48,0.52,0.56,0.60,0.64,0.68,
                           0.72,0.76,0.80,0.9,
                           1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1


def RebinBDT3(h):
        ''' Rebin with un-even bins '''
        print 'using binning BDT3'
        mybins=array('d',[-1,
                           -0.95,-0.9,-0.85,-0.8,-0.75,-0.7,-0.65,-0.6,-0.55,-0.5,
                           -0.46,-0.44,-0.4,-0.36,-0.32,-0.28,-0.24,-0.2,-0.16,-0.12,
                           -0.08,-0.04,-0., 0.04,0.08,0.12,0.16,0.2,0.24,0.28,
                           0.32,0.36,0.4,0.44,0.48,0.52,0.56,0.6,
                           0.65,0.7,
                           0.75,0.8,
                           0.85,0.9,
                           0.95,
			   1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def RebinBDT4(h):
        ''' Rebin with un-even bins '''
        print 'using BDT4 high-2l'
        mybins=array('d',[-1,
                           -0.95, -0.90,
                           -0.85, -0.80,
                           -0.75, -0.7,
                           -0.65, -0.60, -0.55, -0.50, -0.45, -0.40,
                           -0.35, -0.30, -0.25, -0.20, -0.15, -0.10,
                           -0.05,  0.0,   0.05, 0.10, 0.15,0.20,
                           0.25,0.30,
                           0.35,0.40,
                           0.45,0.50,
                           0.55,0.60,
                           0.65,0.70,
                           0.80,
			   1.])
        h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
        return h1

def RebinBDT6(h):
        ''' Rebin with un-even bins '''
        print 'using BDT6 low-2l'
        mybins=array('d',[-1,
                           -0.7,-0.6,-0.5,-0.4,
                           -0.3,-0.25,-0.20,-0.15,-0.10,
                           -0.05, 0.0, 0.05, 0.10, 0.15, 0.20,
                           0.25,0.30,
                           0.35,0.40,
                           0.45,0.50,0.55,0.60,0.65,
                           0.70,0.75,0.80,0.85,0.90,
                           1.])
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
	channel = ["1Mu","1Ele"]
	basecat = ["Baseline","topCRR4","topCRR5","extraRadCRR10","extraRadCRR7","charmCR"]
#	basecat = ["Baseline"]

if opts.kTest==4 or opts.kTest==5 or opts.kTest==6 or opts.kTest==7 or opts.kTest==14 or opts.kTest==15 or opts.kTest==16:
	channel = ["1Mu1Ele","2Mu","2Ele"]
	basecat = ["Baseline","topCRR4","topCRR5","extraRadCRR10","extraRadCRR7"]
#	basecat = ["Baseline"]

if opts.kTest==8 or opts.kTest==9:
	channel = ["1Mu","1Ele","1Mu1Ele","2Mu","2Ele"]
	basecat = ["Baseline","extraRadCR","topCR","charmCR"]

catStore = { } ## name -> {"file", extra options for syst}, hasSignal
statStore = {} ## used to store th1d for stat uncertainties

label=""
VarTest=""

doSyst = True
doRebinStatic = True
doRebin = False

if opts.kTest > 10:
	doRebin = False
	doRebinStatic = False

for y in channel:
	for x in basecat:
		label="2l_"
		if y == "1Ele" or y == "1Mu": label="1l_"
		if opts.kTest==8: label="combined_"

		srList = [""]
		region = [""]
		if x=="Baseline" or ("extraRadCR" in x) or ("topCR" in x) or x=="charmCR":

			if x=="Baseline":
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
			if opts.kTest==8 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = [""] #this is the 1d combined
			if opts.kTest==0 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = [""] #this is the 1d
			if opts.kTest==1 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["1"]
			if opts.kTest==2 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["2"]
			if opts.kTest==3 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["3"]
			if opts.kTest==11 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["_1ll"]
			if opts.kTest==12 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["_1lm"]
			if opts.kTest==13 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["_1lh"]
			else :
				if x=="Baseline" and opts.kTest==4: srList = ["4"]
				if x=="Baseline" and opts.kTest==5: srList = ["5"]
				if x=="Baseline" and opts.kTest==6: srList = ["6"]
				if x=="Baseline" and opts.kTest==7: srList = [""]#this is the 1d
				if x=="Baseline" and opts.kTest==8: srList = [""]#this is the 1d combined
				if x=="Baseline" and opts.kTest==14: srList = ["_2ll"]
				if x=="Baseline" and opts.kTest==15: srList = ["_2lm"]
				if x=="Baseline" and opts.kTest==16: srList = ["_2lh"]

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

       ## set the MC sample
				if x=="charmCR":
					catStore[name]["hasMC"]=["ttlf","ttb","ttbb","tt2b","ttcc","top","ewk","Hptb"]
				if "extraRadCR" in x:
					catStore[name]["hasMC"]=["ttlf","ttb","ttbb","tt2b","ttcc","top","ewk","Hptb"]
				if "topCR" in x:
					catStore[name]["hasMC"]=["ttlf","ttb","ttbb","tt2b","ttcc","top","ewk","Hptb"]
				if x=="Baseline":
					catStore[name]["hasMC"]=["ttlf","ttb","ttbb","tt2b","ttcc","top","ewk","Hptb"]

			if y == "1Ele" or y == "1Mu":
				mcStore={
					"Hptb":{"name":"Hptb", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
					"ttlf":{ "name":"ttlf","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
					"ttb":{ "name":"ttb","hist":["tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
					"ttbb":{ "name":"ttbb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
					"tt2b":{ "name":"tt2b","hist":["tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":4},
					"ttcc":{ "name":"ttcc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":5},
					"ewk":{ "name":"ewk","hist":["WJetsToLNu_HT","DYJetsToLL_M-50_HT"],"num":6},
					"top":{ "name":"top","hist":["TTZ","TTW","TTG","ttH","TTTT","ST"],"num":7}
##					"top":{ "name":"top","hist":["TTZ","TTW","TTG","ttH","TTTT"],"num":7}
					}
			else:
				mcStore={
					"Hptb":{"name":"Hptb", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
					"ttlf":{ "name":"ttlf","hist":["ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":1},
					"ttb":{ "name":"ttb","hist":["tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2},
					"ttbb":{ "name":"ttbb","hist":["tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":3},
					"tt2b":{ "name":"tt2b","hist":["tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":4},
					"ttcc":{ "name":"ttcc","hist":["ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":5},
					"ewk":{ "name":"ewk","hist":["WJetsToLNu_HT","DYJetsToLL_M-50_HT"],"num":6},
					"top":{ "name":"top","hist":["TTZ","TTW","TTG","ttH","TTTT","ST"],"num":7}
##					"top":{ "name":"top","hist":["TTZ","TTW","TTG","ttH","TTTT"],"num":7}
					}
				## NOTE: missing diboson + triboson in the EWK, QCD

			if doSyst:
				systStore={
					"None":None,
					"lumi_13TeV":{"type":"lnN", "value":["1.025"] ,"proc":[".*"],"wsname":"lumi_13TeV","name":"XXX"}, ## name used for shape
					"CMS_eff_m":{"type":"lnN", "value":["1.04"] ,"proc":[".*"],"wsname":"CMS_eff_m","name":"XXX"}, ## name used for shape
					"CMS_eff_e":{"type":"lnN", "value":["1.03"] ,"proc":[".*"],"wsname":"CMS_eff_e","name":"XXX"}, ## name used for shape
					"CMS_eff_t":{"type":"lnN", "value":["1.03"] ,"proc":[".*"],"wsname":"CMS_eff_t","name":"XXX"}, ## name used for shape
					"CMS_pileup":{"type":"shape", "wsname":"CMS_pileup","name":"PU","proc":[".*"]}, ## name used for shape
#					"CMS_scale_j":{"type":"shape", "wsname":"CMS_scale_j","name":"JES","proc":[".*"]}, ## name used for shape
					"CMS_scale_j":{"type":"shape", "wsname":"CMS_scale_j","name":"JESANDCSV","proc":[".*"]}, ## name used for shape
					"CMS_eff_b":{"type":"shape", "wsname":"CMS_eff_b","name":"CSVRHF","proc":[".*"]}, ## name used for shape
					"CMS_fake_b":{"type":"shape", "wsname":"CMS_fake_b","name":"CSVRLF","proc":[".*"]}, ## name used for shape
					"CMS_stat1_b":{"type":"shape", "wsname":"CMS_stat1_b","name":"CSVRSTAT1","proc":[".*"]}, ## name used for shape
					"CMS_stat2_b":{"type":"shape", "wsname":"CMS_stat2_b","name":"CSVRSTAT2","proc":[".*"]}, ## name used for shape
					"CMS_scale_uncluster":{"type":"lnN", "value":["1.02"],"proc":[".*"],"wsname":"CMS_scale_uncluster","name":"XXX"}, ## name used for shape
					"CMS_topreweight":{"type":"shape", "wsname":"CMS_topreweight","name":"TOPRW","proc":[".*"]}, ## name used for shape
					"QCDscale":{"type":"shape", "wsname":"QCDscale","name":"ScaleRF","proc":["Hptb","ttlf","ttb","ttbb","tt2b","ttcc"]} ## name used for shape
#					"CMS_res_j":{"type":"shape", "wsname":"JER","name":"CMS_scale_j","proc":[".*"]}, ## name used for shape
############
############
############
## need to add the shapes of the PDF and scales (for both take the envelop CMS_HPTB_Q2scale_xxx) farlo solo per il ttbar/signal ???
# Jan ISR up/down, name CMS_HPTB_IFSR_xxx, solo su top as lN
# check the topMass
#					"CMS_scale_uncluster":{"type":"shape", "wsname":"UNCLUSTER","name":"CMS_scale_uncluster","proc":[".*"]}, ## name used for shape
					}
			else:
				systStore={
					"None":None
					}


print "--------- CAT STORE IS --------"
for cat in catStore:
	print "* ",cat,":",catStore[cat]
print "---------------------- --------"

fileTmp="JULY12_GREEN/"+label+VarTest+opts.output

w = ROOT.RooWorkspace("w","w")
datNameTmp = opts.datCardName
datName = "JULY12_GREEN/"+ label + VarTest + datNameTmp

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
	datacard.write("shapes Hptb *\t" + fileTmp +"\t")
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
def writeNormSyst(name="lumi",valueL=["1.027","1.026"], regexpL=["TT","ST",""]):
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
	      if c > 0 and err/c <.10: continue ## don't write less than 10% // TEMPORARY
	      hupbin.SetBinContent(i+1,cont+err)
	      hdnbin.SetBinContent(i+1,cont-err)

	      if hupbin.GetBinContent(i+1) <0 : hupbin.SetBinContent(i+1,cont)
	      if hdnbin.GetBinContent(i+1) <0 : hdnbin.SetBinContent(i+1,cont)

	      target = stat["target"]
	      cat = catStore[stat["cat"] ]
	      print "++++++++++++++++++++target",target,' cate',cat

	      al=arglist_obs_bdt
	      if "bdt2D" in cat["var"]: al = arglist_obs_bdt2D
	      if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs

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

if doSyst: writeNormSyst("QCDscale_ttbar",["0.965/1.024","0.965/1.024","0.965/1.024","0.965/1.024","0.965/1.024"],["ttlf","ttcc","ttb","ttbb","tt2b"])
if doSyst: writeNormSyst("pdf_gg",["1.042","1.042","1.042","1.042","1.042"],["ttlf","ttcc","ttb","ttbb","tt2b"])
###if doSyst: writeNormSyst("CMS_mass_ttbar",["1.027","1.027","1.027","1.027","1.027"],["ttlf","ttcc","ttb","ttbb","tt2b"])

if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttlf",["1.50"],["ttlf"])
if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttcc",["1.50"],["ttcc"])
if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttb",["1.50"],["ttb"])
if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttbb",["1.50"],["ttbb"])
if doSyst: writeNormSyst("CMS_HPTB_QCDscale_tt2b",["1.50"],["tt2b"])

if doSyst: writeNormSyst("QCDscale_top",["0.90/1.10"],["top"])
if doSyst: writeNormSyst("pdf_top",["1.03"],["top"])

if doSyst: writeNormSyst("QCDscale_ewk",["0.98/1.02"],["ewk"])
if doSyst: writeNormSyst("pdf_qqbar",["1.04"],["ewk"])

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


## improt Everything in ws TODO
def importPdfFromTH1(cat,mc,myBin,syst=None):

	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	if mc["name"]=="Hptb":masses=[180,200,220,250,300,350,400,500,800,1000,2000,3000]
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

	print '====skip=',skip
	if skip: return

	scaleEveRemoval=1
	delta=0

####
####

	for m in masses:
	 for s in shifts:
	  h=None
	  target = "pdf_" + mc["name"] +"_"+ cat["name"]
	  if m >10 :
		  target = "pdf_" + mc["name"] +"_M-%d"%m+"_"+ cat["name"]

	  if mc["name"]=="Hptb" and "Baseline" in cat["dir"] :
##	  if mc["name"]=="Hptb":
		  mclabel="0"
		  #		  print 'ciao m=',m
		  if m==180: mclabel="1"
		  if m==200: mclabel="2"
		  if m==220: mclabel="3"
		  if m==250: mclabel="4"
		  if m==300: mclabel="5"
		  if m==350: mclabel="6"
		  if m==400: mclabel="7"
		  ###		  if m==450: mclabel="8"
		  if m==500: mclabel="9"
		  ###		  if m==750: mclabel="10"
		  if m==800: mclabel="11"
		  if m==1000: mclabel="12"
		  if m==2000: mclabel="13"
		  if m==3000: mclabel="14"
		  #		  print 'mc=',mclabel
		  hscale="SplitMC/CutFlow/CutFlow_"+mclabel
		  hScale=tfile.Get(hscale)
##		  scaleEveRemoval=hScale.GetBinContent(1)/hScale.GetBinContent(2)
		  delta=hScale.GetBinContent(1)-hScale.GetBinContent(2)

	  if syst != None:
		  target += "_" + syst["wsname"] + s

	  for hname in mc["hist"]:
	        toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

		if mc["name"]=="Hptb":
		   toget=toget%m

		if syst != None:
			toget += "_" + syst["name"] + s
		hTmp=tfile.Get(toget)
		if hTmp!= None: print "<*> Reading Hist '"+toget+"'",hTmp.Integral(),' nBin=',hTmp.GetNbinsX()
		if hTmp == None:
			print "<*> Hist '"+toget+"' doesn't exist"
			raise IOError

		if "Baseline" in cat["dir"]:
			survived=hTmp.GetEntries()
			dropFrac=delta/(survived+delta)
			survivedFrac=survived/(survived+delta)
			scaleEveRemoval=(survived+delta)/survived

			print '=============================='
			print 'mc=',m,'total(Baseline)=',survived+delta,' dropFrac=',dropFrac,' survidedFrac=',survivedFrac,'scale=',scaleEveRemoval
			print '=============================='


### -- MC --
##		print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]
#####1L

		if doRebin and hTmp:
			print '========'
			print '========'
			print '========'
			print '========'
			print myBin
			print '========'
			print '========'
			print '========'
			print '========'

			mybins=array('d',myBin)
			print 'before=',hTmp.GetNbinsX()
			hTmp=hTmp.Rebin(len(mybins)-1,hTmp.GetName()+"_rebin",mybins)
			print 'after=',hTmp.GetNbinsX()

		if doRebinStatic and hTmp:
			if "Baseline" in cat["dir"] and opts.kTest>0 and opts.kTest<7:
				if opts.kTest==3: hTmp=RebinBDT3(hTmp)
				if opts.kTest==2: hTmp=RebinBDT2(hTmp)
				if opts.kTest==1: hTmp=RebinBDT1(hTmp)
				if opts.kTest==6: hTmp=RebinBDT6(hTmp)
				if opts.kTest==5: hTmp=RebinBDT2(hTmp)
				if opts.kTest==4: hTmp=RebinBDT4(hTmp)
				print '------------------------'
				print '------------------------'
				print 'BASELINE and doing doRebinStatic now MC'
				print ' nBin=',hTmp.GetNbinsX()
				print '------------------------'
				print '------------------------'
			else:
				mybins=array('d',myBin)
				print 'before=',hTmp.GetNbinsX()
				hTmp=hTmp.Rebin(len(mybins)-1,hTmp.GetName()+"_rebin",mybins)
				print 'after=',hTmp.GetNbinsX()

		if h==None:h = hTmp
		else: h.Add(hTmp)
		#clean h

	  if h: h.SetBinContent(0,0) ##underflow
	  if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

	  if h: ##negative yield
		  for b in range(1,h.GetNbinsX()+1):
			  if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

	  if h:
		  if h.Integral() <= 0 :
			  print "ERROR histogram", h.GetName(),"has null norm"
			  raise ValueError

	  #save RooDataHist
	  h.Scale(scaleEveRemoval)
	  h.Scale(opts.lumi)
	  print "* Importing ",target

	  al=arglist_obs_bdt
          if "bdt2D" in cat["var"]: al=arglist_obs_bdt2D
	  if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs
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
##			if mc["name"] == 'TTlight':  ## put TTlight and reference
			if mc["name"] == 'ttlf':  ## put TTlight and reference
				statStore[histName]["mc"]= mc["name"]
				statStore[histName]["target"]= target
				statStore[histName]["hist0"] . Delete()
				statStore[histName]["hist0"] = h.Clone(histName + "_STAT0")
	return

#################

## improt Everything in ws TODO
def importPdfFromTH1SumBKG(cat,mc,syst=None):
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	if mc["name"]=="Hptb":masses=[180,200,220,250,300,350,400,500,800,1000,2000,3000]
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

	print '====skip=',skip
	if skip: return

####
####

	for m in masses:
	 for s in shifts:
	  h=None
	  hRef=None

	  for hname in mc["hist"]:

                if mc["name"]=="Hptb": continue

	        toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

		hTmp=tfile.Get(toget)
		if hTmp!= None: print "<*> Reading Hist '"+toget+"'",hTmp.Integral(),' nBin=',hTmp.GetNbinsX()
		if hTmp == None:
			print "<*> Hist '"+toget+"' doesn't exist"
			raise IOError

		if h==None:h = hTmp
		else: h.Add(hTmp)
		#clean h

		if mc["name"]=="ttlf": hRef=h.Clone()

	  if h: h.SetBinContent(0,0) ##underflow
	  if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

	  if h: ##negative yield
		  for b in range(1,h.GetNbinsX()+1):
			  if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

	  if h:
		  if h.Integral() <= 0 :
			  print "ERROR histogram", h.GetName(),"has null norm"
			  raise ValueError

	  if h!=None:
		  h.Scale(opts.lumi)

	  if hRef!=None:
		  hRef.Scale(opts.lumi)

	return h,hRef



#import MC
for c in catStore:

	hSumAll=None
	hRef=None

	myBin=1
	for proc in mcStore:
		if skip(catStore[c],mcStore[proc]): continue
		hSumTMP,hRef=importPdfFromTH1SumBKG(catStore[c],mcStore[proc],None)
		if hSumAll==None:
			hSumAll=hSumTMP
		else:
			hSumAll.Add(hSumTMP)

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

	print 'before all=',hSumAll.Integral(),' ref=',hRef.Integral()
	b = FwRebin.Rebin(hSumAll, hSumAll, maxStat)
	print 'out of Rebin'
	b.directionalRebin()
	myBin = b.getBinArray()
	print 'myBin=',myBin,' yield=',hSumAll.Integral()

	for proc in mcStore:
		if skip(catStore[c],mcStore[proc]): continue
		for syst in systStore:
			print "->calling import pdf with cat=",cat,"proc=",proc,"syst=",syst
			print " * cat:",catStore[c]
			print " * syst:", systStore[syst]
			if systStore[syst] == None or systStore[syst]["type"] == "shape" :
				importPdfFromTH1(catStore[c],mcStore[proc],myBin,systStore[syst])

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

	h=tfile.Get(toget)
	if h == None:
		print "<*> Hist do not exists ",toget
		raise IOError

	if h: h.SetBinContent(0,0) ##underflow
	if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

	for b in range(1,h.GetNbinsX()+1): ##negative yield
		if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

	if h:
		if h.Integral() <= 0 :
			print "ERROR histogram", h.GetName(),"has null norm"
			raise ValueError

####This is the data
####1L
##		print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]


	if doRebin and h:
		mybins=array('d',myBin)
		h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)

	if doRebinStatic and h:
		if "Baseline" in cat["dir"] and opts.kTest>0 and opts.kTest<7:
			print '------------------------'
			print '------------------------'
			print 'BASELINE and doing doRebinStatic now DATA'
			print '------------------------'
			print '------------------------'

			if opts.kTest==3: h=RebinBDT3(h)
			if opts.kTest==2: h=RebinBDT2(h)
			if opts.kTest==1: h=RebinBDT1(h)
			if opts.kTest==6: h=RebinBDT6(h)
			if opts.kTest==5: h=RebinBDT2(h)
			if opts.kTest==4: h=RebinBDT4(h)
		else:
			mybins=array('d',myBin)
			print 'before=',h.GetNbinsX()
			h=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
			print 'after=',h.GetNbinsX()

	al=arglist_obs_bdt
	if "bdt2D" in cat["var"]: al = arglist_obs_bdt2D
	if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs

	roo_data= ROOT.RooDataHist("data_obs_%s"%c,"H_{T}",al,h)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h,roo_data])

## import and write statistical uncertainties
importStat()



w.writeToFile(fileTmp)
print "--------------------" 
print "datacard=",datName
print "ws=",fileTmp
print " --- DONE --- "

