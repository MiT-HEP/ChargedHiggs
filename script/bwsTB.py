#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

parser= OptionParser()

#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="feb7_1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="feb7_1l.root")

#parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/feb12final_1l.root")
#parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/feb12final_2l.root")

parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/feb14_1l.root")
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/work/d/dalfonso/CMSSW_8_0_11_testNERO/src/ChargedHiggs/feb14_2l.root")

#parser.add_option("","--input1Lsig",type='string',help="Input ROOT file. [%default]", default="1l_signal.root")
#parser.add_option("","--input2Lsig",type='string',help="Input ROOT file. [%default]", default="2l_signal.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_STAT.root")
parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_STAT.txt")
#parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_SYST.root")
#parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_SYST.txt")
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
	res = re.sub("muID","CMS_eff_m_reco",res)
	res = re.sub("muISO","CMS_eff_m_iso",res)
	res = re.sub("TOPRW","CMS_topreweight",res)
	return res

def RebinBDT(h):
	''' Rebin with un-even bins '''
## this is equivalent to rebin5
	mybins=array('d',[-1,-0.8,-0.6,-0.4,-0.2,0.,0.2,0.4,0.6,0.8,1.])
##	mybins=array('d',[-1.,-0.8,-0.6,-0.4,-0.3,-0.2,-0.1,0.,0.1,0.2,0.3,0.4,0.6,0.8,1.])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1

def Rebin5(h):
	''' Rebin with un-even bins '''
##	mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
##	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
#	h1=h.Rebin(5)
	h1=h.Rebin(1)
	return h1

def Rebin(h):
	''' Rebin with un-even bins '''
	mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
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

if opts.kTest==1 or opts.kTest==2 or opts.kTest==3 or opts.kTest==0:
	channel = ["1Mu","1Ele"]
	basecat = ["Baseline","extraRadCR","topCR","charmCR"]
if opts.kTest==4 or opts.kTest==5 or opts.kTest==6 or opts.kTest==7:
	channel = ["1Mu1Ele","2Mu","2Ele"]
	basecat = ["Baseline","extraRadCR","topCR"]

catStore = { } ## name -> {"file", extra options for syst}, hasSignal
statStore = {} ## used to store th1d for stat uncertainties

label=""
VarTest=""

for x in basecat:
	for y in channel:

		label="2l_"
		if y == "1Ele" or y == "1Mu": label="1l_"

#	name= x+ "_" + y
#       catStore [ name ] = { "name": name,"file": None, "hasMC":["all"],"var":"HT"}
		srList = [""]
		region = [""]
		if x=="Baseline" or x=="extraRadCR" or x=="topCR" or x=="charmCR":

			if x=="Baseline": region = ["_SR1","_SR2","_SR3","_SR4"]
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
			if opts.kTest==0 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = [""] #this is the 1d
			if opts.kTest==1 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["1"]
			if opts.kTest==2 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["2"]
			if opts.kTest==3 and (y == "1Ele" or y == "1Mu") and x=="Baseline": srList = ["3"]
##			if y == "1Ele" or y == "1Mu": srList = ["2"]
##			else : srList = ["_SR1","_SR2","_SR3","_SR4"]
##			else : srList = [""]
##			else : srList = ["3","4"]
			else :
				if x=="Baseline" and opts.kTest==4: srList = ["4"]
				if x=="Baseline" and opts.kTest==5: srList = ["5"]
				if x=="Baseline" and opts.kTest==6: srList = ["6"]
				if x=="Baseline" and opts.kTest==7: srList = [""]#this is the 1d
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

			for reg in region:
				name = x+ "_" + y + "" + sr + "" + reg
#				if opts.kTest==0 or opts.kTest==7:catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"ST"+reg}
				if opts.kTest>0 and opts.kTest<7 and x=="Baseline":
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"bdt"+sr+""+reg}
				else:
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"HT"+reg}

				print 'name=',name,'sr=',sr,'reg=',reg,"VarTest=",VarTest

       ## set files
				if y == "1Ele" or y == "1Mu": catStore [ name ]['file'] = fIn1L
				else : catStore[name]['file'] = fIn2L

       #these have null norm so far
				if x=="charmCR":
					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
				if x=="extraRadCR":
					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
				if x=="topCR":
					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]
				if x=="Baseline":
					catStore[name]["hasMC"]=["TT","TTX","ST","HPlus"]


#			if x=="charmCR"  and (y=="2Mu" or y=="2Ele" or "1Mu1Ele"):
#				catStore[name]["hasMC"]=["xxx"]
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

			mcStore={
##				"HPlus":{"name":"HPlus", "hist":["HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
#				"HPlus":{"name":"HPlus", "hist":["HplusToTB_M-%d"], "num":0 },
				"HPlus":{"name":"HPlus", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
####				"WJets":{"name":"WJets", "hist":["WJetsToLNu","DYJetsToLL_M"],"num":1 },
####				"VV":{ "name":"VV","hist":["WWTo2L2Nu","WWToLNuQQ","WZTo1L1Nu2Q","WZTo1L3Nu","WZTo2L2Q","WZTo3LNu","ZZTo2L2Nu","ZZTo2L2Q","ZZTo4L"],"num":2},
				"VV":{ "name":"VV","hist":["WWTo","WZTo","ZZTo"],"num":2},
				#	"WW":{ "name":"WW","hist":["WWTo2L2Nu","WWToLNuQQ"],"num":2},
				#	"WZ":{ "name":"WZ","hist":["WZTo1L1Nu2Q","WZTo1L3Nu","WZTo2L2Q","WZTo3LNu"],"num":3},
				#	"ZZ":{ "name":"ZZ","hist":["ZZTo2L2Nu","ZZTo2L2Q","ZZTo4L"],"num":4},
####				"TT":{ "name":"TT","hist":["TTJets_SingleLeptFromT","TTJets_DiLept_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"],"num":1},
				"TT":{ "name":"TT","hist":["TTToSemilepton","TTTo2L2Nu"],"num":1},
				"ST":{ "name":"ST","hist":["ST","tZq"],"num":4},
####				"TTX":{ "name":"TTX","hist":["TTTT","TTZ","TTW","TTG","ttH"],"num":5}
				"TTX":{ "name":"TTX","hist":["TTZ","TTW","TTG","ttH"],"num":5}
				}
			systStore={
				"None":None,
#				"CMS_pileup":{"type":"shape", "wsname":"PU","name":"PU","proc":[".*"]}, ## name used for shape
#				"CMS_scale_uncluster":{"type":"shape", "wsname":"UNCLUSTER","name":"UNCLUSTER","proc":[".*"]}, ## name used for shape
#				"CMS_scale_j":{"type":"shape", "wsname":"JES","name":"JES","proc":[".*"]}, ## name used for shape
###				"CMS_res_j":{"type":"shape", "wsname":"JER","name":"JER","proc":[".*"]}, ## name used for shape
#				"CMS_eff_b":{"type":"shape", "wsname":"BTAGB","name":"BTAGB","proc":[".*"]}, ## name used for shape
#				"CMS_fake_b":{"type":"shape", "wsname":"BTAGL","name":"BTAGL","proc":[".*"]}, ## name used for shape
#				"CMS_topreweight":{"type":"shape", "wsname":"TOPRW","name":"TOPRW","proc":[".*"]}, ## name used for shape
#				"CMS_eff_m_trigger":{"type":"shape", "wsname":"muTRG","name":"muTRG","proc":[".*"]}, ## name used for shape
#				"CMS_eff_m_reco":{"type":"shape", "wsname":"muID","name":"muID","proc":[".*"]}, ## name used for shape
#				"CMS_eff_m_iso":{"type":"shape", "wsname":"muISO","name":"muISO","proc":[".*"]}, ## name used for shape
#				"lumi13TeV":{"type":"lnN", "value":["1.06"] ,"proc":[".*"],"wsname":"lumi13TeV","name":"XXX"} ## name used for shape
				"lumi13TeV":{"type":"lnN", "value":["1.025"] ,"proc":[".*"],"wsname":"lumi13TeV","name":"XXX"} ## name used for shape

				}

print "--------- CAT STORE IS --------"
for cat in catStore:
	print "* ",cat,":",catStore[cat]
print "---------------------- --------"

fileTmp="FEB12/"+label+VarTest+opts.output

w = ROOT.RooWorkspace("w","w")
datNameTmp = opts.datCardName
datName = "FEB12/"+ label + VarTest + datNameTmp

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

arglist_obs = ROOT.RooArgList(ht)
argset_obs = ROOT.RooArgSet(ht)

arglist_obs_bdt = ROOT.RooArgList(bdt)
argset_obs_bdt = ROOT.RooArgSet(bdt)


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
 	      if c<=0: continue;
 	      h.SetBinError(iBin,minError)
	   for i in range(0,h.GetNbinsX()):
	      statsyst = { "wsname": statName + "_Bin%d"%(i+1) +"_STAT" , "name": statName + "_Bin%d"%(i+1) + "_STAT"}
	      hupbin=h0.Clone(h.GetName() +"_STATUp")
	      hdnbin=h0.Clone(h.GetName() +"_STATDown")
	      cont=h0.GetBinContent(i+1)
	      err = h . GetBinError(i+1) ## err is referred to the sum
	      c = h.GetBinContent(i+1) ## this is to check the magnitude, not to apply it
	      if c > 0 and err/c <.01: continue ## don't write less than 1%
	      hupbin.SetBinContent(i+1,cont+err)
	      hdnbin.SetBinContent(i+1,cont-err)
	      target = stat["target"]
	      cat = catStore[stat["cat"] ]

	      al=arglist_obs_bdt
	      if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4": al = arglist_obs

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
##	if mc["name"]=="HPlus":masses=[300,400,500,800,1000,2000,3000,180,200,220,250,350,450,750]
	if mc["name"]=="HPlus":masses=[180,200,220,250,300,400,500,800,3000]
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
	  if opts.kTest==0 or opts.kTest==7: h=Rebin(h)
#	  else: h=RebinBDT(h)
	  else: h=Rebin5(h)
	  for b in range(1,h.GetNbinsX()+1):
		  if h.GetBinContent(b) <0 : h.SetBinContent(b,0)
	  if h.Integral() <= 0 :
		print "ERROR histogram", h.GetName(),"has null norm"
		raise ValueError
	  #save RooDataHist
	  h.Scale(opts.lumi)
	  print "* Importing ",target

	  al=arglist_obs_bdt
	  if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4": al = arglist_obs
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
			if mc["name"] == 'TT':  ## put TT and reference
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
##importStat()

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
#	h=Rebin(h)
	if opts.kTest==0 or opts.kTest==7: h=Rebin(h)
	else: h=Rebin5(h)

	al=arglist_obs_bdt
	if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4": al = arglist_obs

	roo_data= ROOT.RooDataHist("data_obs_%s"%c,"H_{T}",al,h)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h,roo_data])

w.writeToFile(fileTmp)
print "--------------------" 
print "datacard=",datName
print "ws=",fileTmp
print " --- DONE --- "

