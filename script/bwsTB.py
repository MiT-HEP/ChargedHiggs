#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

parser= OptionParser()
parser.add_option("","--input1L",type='string',help="Input ROOT file. [%default]", default="ChHiggs.root")
parser.add_option("","--input2L",type='string',help="Input ROOT file. [%default]", default="ChHiggs.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace.root")
parser.add_option("-l","--lumi",type='float',help="Luminosity. [%default]", default=12000)

#extra = OptionGroup(parser,"Extra options:","")
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
	res = re.sub("MUEFF","CMS_eff_m_trigger",res)
	res = re.sub("MUEFF","CMS_eff_m_reco",res)
	res = re.sub("TOPRW","CMS_topreweight",res)
	return res

def Rebin(h):
	''' Rebin with un-even bins '''
	mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1


w = ROOT.RooWorkspace("w","w")
datName = "cms_datacard_topbottom.txt"

datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")

w.factory("ht[0,8000]"); # RooRealVar
ht=w.var("ht")
arglist_obs = ROOT.RooArgList(ht)
argset_obs = ROOT.RooArgSet(ht)

fIn1L = ROOT.TFile.Open(opts.input1L,"READ")
fIn2L = ROOT.TFile.Open(opts.input2L,"READ")


if fIn1L == None: 
	print "ERROR: file",opts.input1L,"doesn't exist"
	exit(1)
if fIn2L == None: 
	print "ERROR: file",opts.input2L,"doesn't exist"
	exit(1)

channel =  ["1Ele","1Mu","1Mu1Ele","2Mu","2Ele"]
basecat = ["Baseline","charmCR","extraRadCR","topCR"]

catStore = { } ## name -> {"file", extra options for syst}, hasSignal

for x in basecat:
   for y in channel:
       name= x+ "_" + y 
       catStore [ name ] = { "name": name,"file": None, "hasMC":["all"],"var":"HT"}
       ## set files
       if y == "1Ele" or y == "1Mu": catStore [ name ]['file'] = fIn1L
       else : catStore[name]['file'] = fIn2L

       #these have null norm so far
       if x=="charmCR":
	       catStore[name]["hasMC"]=["WW","WZ","ZZ","WJets"]
       if x=="topCR":
	       catStore[name]["hasMC"]=["TT","ST"]
       if x=="Baseline" and y=="2Mu":
	       catStore[name]["hasMC"]=["WW","WZ","ZZ","TT","ST","HPlus"]
       if x=="Baseline" and y=="2Ele":
	       catStore[name]["hasMC"]=["WW","WZ","ZZ","TT","ST","HPlus"]
       if x=="charmCR"  and (y=="2Mu" or y=="2Ele" or "1Mu1Ele"):
	       catStore[name]["hasMC"]=["xxx"]
       if x=="Baseline" and y=="1Mu1Ele":
	       catStore[name]["hasMC"]=["WW","WZ","ZZ","TT","ST","HPlus"]

mcStore={
	"HPlus":{"name":"HPlus", "hist":["HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0},
	"WJets":{"name":"WJets", "hist":["WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"],"num":1 },
	"WW":{ "name":"WW","hist":[ "WWTo2L2Nu","WWToLNuQQ"],"num":2},
	"WZ":{ "name":"WZ","hist":["WZTo1L1Nu2Q","WZTo1L3Nu","WZTo2L2Q","WZTo3LNu"],"num":3},
	"ZZ":{ "name":"ZZ","hist":["ZZTo2L2Nu","ZZTo2L2Q","ZZTo4L"],"num":4},
	"TT":{ "name":"TT","hist":["TTJets_SingleLeptFromT_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1","TTJets_DiLept_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"],"num":5},
	"ST":{ "name":"ST","hist":["ST_t-channel_antitop","ST_t-channel_top","ST_tW_antitop","ST_tW_top"],"num":6}
	}
systStore={
	"None":None,
	"lumi13TeV":{"type":"lnN", "value":["1.06"] ,"proc":[".*"],"wsname":"lumi13TeV","name":"XXX"} ## name used for shape
	}

def skip(cat,mc):
	if cat["hasMC"][0] == "all": return False
	for x in cat["hasMC"]:
		if mc["name"] == x : return False
	return True


## write shapes
if True: # data
	datacard.write("shapes data_obs *\t" + opts.output +"\t")
	datacard.write("w:data_obs_$CHANNEL")
	datacard.write("\n")
if True: # Sig
	datacard.write("shapes HPlus *\t" + opts.output +"\t")
	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL\t")
	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL_$SYSTEMATIC")
	datacard.write("\n")
if True: #bkg
	datacard.write("shapes * * %s\t"%opts.output +"\t")
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
def writeNormSyst(name="lumi",valueL=["1.027","1.026"], regexpL=["TT","ST"]):
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

def writeSystShape(syst,regexpL=[]):
	name=syst["wsname"]
	datacard.write(name+"\tshape")

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
		   datacard.write("\t1")
		else:
		   datacard.write("\t-")
	datacard.write("\n")

################### WRITE SYST ################
for syst in systStore:
	if syst == "None": continue
	if systStore[syst]["type"] == "lnN":
		writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"])
	if systStore[syst]["type"] == "shape":
		writeSystShape(syst,systStore[syst]["proc"])

################### IMPORT ################
def importStat(h,target,syst="STAT"):
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

	hup=h.Clone(h.GetName() +"_STATUp")
	hdn=h.Clone(h.GetName() +"_STATDown")
	for i in range(0,h.GetNbinsX()):
		hupbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
		hdnbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
		cont=h.GetBinContent(i+1)
		# 0 is for sure wrong in case of mc, fixed in GetHisto
		hupbin.SetBinContent(i+1, h.GetBinContent(i+1) + h.GetBinError(i+1) )
		hdnbin.SetBinContent(i+1, h.GetBinContent(i+1) + h.GetBinError(i+1) )
		roo_mc_binup = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Up",target,arglist_obs,hupbin)
		pdf_mc_binup = roo_mc_binup
		roo_mc_bindn = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Down",target,arglist_obs,hdnbin)
		pdf_mc_bindn = roo_mc_bindn
		getattr(w,'import')(pdf_mc_binup,ROOT.RooCmdArg())
		getattr(w,'import')(pdf_mc_bindn,ROOT.RooCmdArg())
		g.extend([hupbin,roo_mc_binup,pdf_mc_binup])
		g.extend([hdnbin,roo_mc_bindn,pdf_mc_bindn])

## improt Everything in ws TODO
def importPdfFromTH1(cat,mc,syst=None):
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	if mc["name"]=="HPlus":masses=[300,400,500]	
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
		  target += syst["wsname"] + s

	  for hname in mc["hist"]:
		toget=base + "/" +cat["name"] + "/" +  cat["var"] + "_" + hname
		if mc["name"]=="HPlus":
		   toget=toget%m
		if syst != None:
			toget += "_" + syst["name"] + s
		hTmp=tfile.Get(toget)
		if hTmp == None:
			print "<*> Hist '"+toget+"' doesn't exist" 
			raise IOError
		if h==None:h = hTmp
		else: h.Add(hTmp)
	  #clean h
	  h=Rebin(h)
	  for b in range(1,h.GetNbinsX()+1):
		  if h.GetBinContent(b) <0 : h.SetBinContent(b,0)
	  #save RooDataHist
	  h.Scale(opts.lumi)
	  print "* Importing ",target
	  roo_mc = ROOT.RooDataHist(target,target,arglist_obs,h)
	  pdf_mc = roo_mc
	  getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
	  g.extend([h,roo_mc,pdf_mc])
	  if syst==None: # DEBUG
		print "DEBUG calling stat with target",target
	  	importStat(h,target,mc["name"]+"_STAT")
		for i in range(0,h.GetNbinsX()):
			#hupbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
			statsyst = { "wsname":"Bin%d"%(i+1) +"_"+mc["name"]+ "_STAT" , "name": "Bin%d"%(i+1) +"_"+mc["name"]+"_STAT"}
			writeSystShape(statsyst,[mc["name"]])
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

#import data
for c in catStore:
	cat=catStore[c]
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	target = "data_obs_"+ cat["name"] 
	toget=base + "/" +cat["name"] + "/" +  cat["var"]  +"_Data"
	h=tfile.Get(toget)
	if h == None:
		print "<*> File not exists"
		raise IOError
	h=Rebin(h)
	roo_data= ROOT.RooDataHist("data_obs_%s"%c,"H_{T}",arglist_obs,h)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h,roo_data])

w.writeToFile(opts.output)
print "--------------------" 
print "datacard=",datName
print "ws=",opts.output
print " --- DONE --- "


