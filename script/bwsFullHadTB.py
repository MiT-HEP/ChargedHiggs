#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

parser= OptionParser()

##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/June12.root")
##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/hysteriaMeow.root")
##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ohho.root")
##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ssolstice.root")

parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ldoJune.root")

parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_STAT.root")
parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_STAT.txt")
parser.add_option("-l","--lumi",type='float',help="Luminosity. [%default]", default=35867)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-k","--kMass",type='string',help = "Which mass point. [%default]", default=1000)
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

fIn = ROOT.TFile.Open(opts.input,"READ")

if fIn == None: 
	print "ERROR: file",opts.input,"doesn't exist"
	exit(1)

basecat = []
channel = []

#channel = ["wbb","tb"]
channel = ["t0b","t1b","wbb"]
#channel = ["wbb"]
#basecat = ["OneBOneFat_three"]
basecat = ["OneBOneFat_one","OneBOneFat_two","OneBOneFat_three"]
#basecat = ["OneBOneFat_less","OneBOneFat_more"]

catStore = { } ## name -> {"file", extra options for syst}, hasSignal
statStore = {} ## used to store th1d for stat uncertainties

label="fullHad_"
VarTest=""

doSyst = False
doRebin = False

#if opts.kTest > 10:
#	doRebin = False
	
for y in channel:
	for x in basecat:

		region = ["_in","_above","_below"]
		for reg in region:

			masses = [ opts.kMass ]
##			masses = ["_500","_800","_1000","_2000","_3000"]
		#		region = [""]
			for mas in masses:

				name = x+ "_" + y + mas + reg
	#		catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"invariantExt"}


				if x=="OneBOneFat_one" and y=="wbb": continue
				if x=="OneBOneFat_one" and y=="t1b": continue

				catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["top"],"var":"HT_"+mas+reg}

				print '================'
				print 'name=',name


				catStore[name]['file'] = fIn
				catStore[name]["hasMC"]=["qcd","top","Hptb"]
				mcStore={
					"Hptb":{"name":"Hptb", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
					"qcd":{"name":"qcd", "hist":["QCD_HT"], "num":1 },
					"top":{ "name":"top","hist":["TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":2}
					}
				systStore={
					"None":None,
					"lumi_13TeV":{"type":"lnN", "value":["1.025"] ,"proc":[".*"],"wsname":"lumi_13TeV","name":"XXX"} ## name used for shape
					}

	
print "--------- CAT STORE IS --------"
for cat in catStore:
	print "* ",cat,":",catStore[cat]
print "---------------------- --------"

fileTmp="Analysis/csl/"+label+VarTest+opts.kMass+"_"+opts.output

w = ROOT.RooWorkspace("w","w")
datNameTmp = opts.datCardName
datName = "Analysis/csl/"+label+ VarTest+opts.kMass+"_" + datNameTmp

datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")

##exit(1)

w.factory("ht[800,6000]"); # RooRealVar
ht=w.var("ht")
#w.factory("invariantExt[400,3600]"); # RooRealVar
#invariantExt=w.var("invariantExt")

#arglist_obs = ROOT.RooArgList(invariantExt)
#argset_obs = ROOT.RooArgSet(invariantExt)

arglist_obs = ROOT.RooArgList(ht)
argset_obs = ROOT.RooArgSet(ht)

#arglist_obs_bdt2D = ROOT.RooArgList(bdt2D)
#argset_obs_bdt2D = ROOT.RooArgSet(bdt2D)
#w.factory("bdt[-1,1]")
#bdt=w.var("bdt")
#w.factory("bdt2D[-0.5,3.5]")
#bdt2D=w.var("bdt2D")


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
	      if c > 0 and err/c <.10: continue ## don't write less than 1% // TEMPORARY
	      hupbin.SetBinContent(i+1,cont+err)
	      hdnbin.SetBinContent(i+1,cont-err)
	      target = stat["target"]
	      cat = catStore[stat["cat"] ]
	      print "++++++++++++++++++++target",target,' cate',cat

	      al=arglist_obs
#	      al=arglist_obs_bdt
#	      if "bdt2D" in cat["var"]: al = arglist_obs_bdt2D
#	      if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs

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

#if doSyst: writeNormSyst("QCDscale_ttbar",["0.965/1.024","0.965/1.024","0.965/1.024","0.965/1.024","0.965/1.024"],["ttlf","ttcc","ttb","ttbb","tt2b"])
#if doSyst: writeNormSyst("pdf_gg",["1.042","1.042","1.042","1.042","1.042"],["ttlf","ttcc","ttb","ttbb","tt2b"])
###if doSyst: writeNormSyst("CMS_mass_ttbar",["1.027","1.027","1.027","1.027","1.027"],["ttlf","ttcc","ttb","ttbb","tt2b"])

#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttlf",["1.50"],["ttlf"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttcc",["1.50"],["ttcc"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttb",["1.50"],["ttb"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttbb",["1.50"],["ttbb"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_tt2b",["1.50"],["tt2b"])

#if doSyst: writeNormSyst("QCDscale_top",["0.90/1.10"],["top"])
#if doSyst: writeNormSyst("pdf_top",["1.03"],["top"])

#if doSyst: writeNormSyst("QCDscale_ewk",["0.98/1.02"],["ewk"])
#if doSyst: writeNormSyst("pdf_qqbar",["1.04"],["ewk"])

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
def importPdfFromTH1(cat,mc,syst=None):
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
#	if mc["name"]=="HPlus":masses=[180,200,220,250,300,350,400,500,800,1000,2000,3000]
#	if mc["name"]=="Hptb":masses=[180,200,220,250,300,350,400,500,800,1000,2000,3000]
	if mc["name"]=="Hptb":masses=[500,800,1000,2000,3000]
	else: masses=[0]

	if syst == None: shifts=["x"]
	else: shifts=["Up","Down"]

	scaleEveRemoval=1

	for m in masses:
	 for s in shifts:
	  h=None
	  print '==========> '
	  print '==========> cat["name"] ===> ', cat["name"]
	  print '==========> '

	  if str(m) not in cat["name"]:
		  continue

	  target = "pdf_" + mc["name"] +"_"+ cat["name"]
	  if m >10 :
		  target = "pdf_" + mc["name"] +"_M-%d"%m+"_"+ cat["name"]

	  if syst != None:
		  target += "_" + syst["wsname"] + s

#	  if syst == None and mc["name"]=="Hptb":
#		  mclabel="0"
#		  print 'ciao m=',m
#		  if m==180: mclabel="1"
#		  if m==200: mclabel="2"
#		  if m==220: mclabel="3"
#		  if m==250: mclabel="4"
#		  if m==300: mclabel="5"
#		  if m==350: mclabel="6"
#		  if m==400: mclabel="7"
###		  if m==450: mclabel="8"
#		  if m==500: mclabel="9"
###		  if m==750: mclabel="10"
#		  if m==800: mclabel="11"
#		  if m==1000: mclabel="12"
#		  if m==2000: mclabel="13"
#		  if m==3000: mclabel="14"
##		  print 'mc=',mclabel
#		  hscale="SplitMC/CutFlow/CutFlow_"+mclabel
#		  hScale=tfile.Get(hscale)
#		  scaleEveRemoval=hScale.GetBinContent(1)/hScale.GetBinContent(2)

#		  print '=============================='
#		  print 'mc=',mclabel,'total=',hScale.GetBinContent(1),' pass=',hScale.GetBinContent(2),'scale=',scale
#		  print '=============================='


	  for hname in mc["hist"]:
	        toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname
		print 'hname=',hname

#		if mc["name"]=="HPlus":
		if mc["name"]=="Hptb":
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


### -- MC --
##		print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]
#####1L

		h=RebinN(h,10);

		if doRebin:
			if  ("1Ele" in cat["dir"] or "1Mu" in cat["dir"]) and not ("1Mu1Ele" in cat["dir"]):

				if "Baseline" in cat["dir"]:
					if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR13": h=Rebin1LHT(h)
					elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR24": h=Rebin1LHT(h)
					else:
						if opts.kTest==3: h=RebinBDT3(h)
						if opts.kTest==2: h=RebinBDT2(h)
						if opts.kTest==1: h=RebinBDT1(h)

				if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LHT(h)
				if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LHT(h)
				if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LHT(h)
##### 2L 			
#
			if  "2Ele" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]:
				#
				if "Baseline" in cat["dir"]:
					if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR13": h=Rebin2LHT(h)
					elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR24": h=Rebin2LHT(h)
					else:
						if opts.kTest==6: h=RebinBDT6(h)
						if opts.kTest==5: h=RebinBDT2(h)
						if opts.kTest==4: h=RebinBDT4(h)

				if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin2LHT(h)
				if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin2LHT(h)
				if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin2LHT(h)

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

	  al=arglist_obs
#	  al=arglist_obs_bdt
#          if "bdt2D" in cat["var"]: al=arglist_obs_bdt2D
#	  if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3"or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs
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
			if mc["name"] == 'top':  ## put TT and reference
##			if mc["name"] == 'TTlight':  ## put TTlight and reference
##			if mc["name"] == 'ttlf':  ## put TTlight and reference
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
#if doSyst: importStat()
importStat()

#import data
for c in catStore:
	cat=catStore[c]
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	target = "data_obs_"+ cat["name"] 
## MARIA temporaty fix since the data histo is not filled
#	toget=base + "/" +cat["dir"] + "/" +  cat["var"]  +"_Data"
	toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_QCD_HT" 
#	if  ("1Ele" in cat["dir"] or "2Ele" in cat["dir"]) and not ("1Mu1Ele" in cat["dir"]): toget+="_SingleElectron"
#	elif  "1Mu" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]: toget+="_SingleMuon"

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

	h=RebinN(h,10);

	if doRebin:
		if  ("1Ele" in cat["dir"] or "1Mu" in cat["dir"]) and not ("1Mu1Ele" in cat["dir"]):
		
			if "Baseline" in cat["dir"]:   
				if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR13": h=Rebin1LHT(h)
				elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR24": h=Rebin1LHT(h)
				else: 
					if opts.kTest==3: h=RebinBDT3(h)
					if opts.kTest==2: h=RebinBDT2(h)
					if opts.kTest==1: h=RebinBDT1(h)

			if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LHT(h)
			if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LHT(h)
			if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin1LHT(h)
#### 2L 			

		if  "2Ele" in cat["dir"] or "2Mu" in cat["dir"] or "1Mu1Ele" in cat["dir"]:
		
			if "Baseline" in cat["dir"]:   
				if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR13": h=Rebin2LHT(h)
				elif cat["var"] == "HT_SR2" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR24": h=Rebin2LHT(h)
				else: 
					if opts.kTest==6: h=RebinBDT6(h)
					if opts.kTest==5: h=RebinBDT2(h)
					if opts.kTest==4: h=RebinBDT4(h)
					
			if "topCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin2LHT(h)
			if "charmCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin2LHT(h)
			if "extraRadCR" in cat["dir"] and cat["var"] == "HT" : h=Rebin2LHT(h)


	al=arglist_obs
#	al=arglist_obs_bdt
#	if "bdt2D" in cat["var"]: al = arglist_obs_bdt2D
#	if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs

	roo_data= ROOT.RooDataHist("data_obs_%s"%c,"Mass",al,h)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h,roo_data])

w.writeToFile(fileTmp)
print "--------------------" 
print "datacard=",datName
print "ws=",fileTmp
print " --- DONE --- "

