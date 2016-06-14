import sys, os
import re
from optparse import OptionParser,OptionGroup

parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="ChHiggs.root")
parser.add_option("","--qcd",type='string',help="Input ROOT file. Set Null to use QCD from above [%default]", default="QCDPurity.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="ChHiggsWs.root")
parser.add_option("-L","--lumi",type='float',help="Luminosity pb. [%default]", default=2308)
parser.add_option("","--qcdlumi",type='float',help="QCD Luminosity pb. [%default]", default=2308)
parser.add_option("-n","--ncat",type='int',help="Number of cat. [%default]", default=1)
parser.add_option("","--nosyst",action='store_true',help="Do not look for syst. [%default]", default=False)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. [%default]", default=-1)
extra.add_option("","--datacard",type='string',help="Output datacard extra. [%default]", default="")
extra.add_option("","--basedir",type='string',help="Base Dir. [%default]", default="ChargedHiggsTauNu/Vars/")

parser.add_option_group(extra)


opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

g=[] ## garbage un-collector

#mcList=['DY','TT','WW','WZ','ZZ','WJets']
mcList=['DY','TT','ST','WW','WZ','ZZ','WJets']
if opts.qcd == "" : mcList.append("QCD")

################### OPEN OUTPUT ############
w = ROOT.RooWorkspace("w","w")

############ DATACARD ###########
datName = "cms_datacard_ws"
if opts.qcd != "": datName+="_qcddata"
if opts.datacard != "": datName += "_" + opts.datacard
datName += ".txt"

print "-> Opening datacard",datName
datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")


########################## IMPORT DATA #############
w.factory("mt[0,8000]"); # RooRealVar
mt=w.var("mt")
arglist_obs = ROOT.RooArgList(mt)
argset_obs = ROOT.RooArgSet(mt)

fIn = ROOT.TFile.Open(opts.input,"READ")

#basedir = "ChargedHiggsTauNu/Vars/"
basedir = opts.basedir
lastget=""

for cat in range(0,opts.ncat):
	if opts.ncat==1:
		lastget=basedir+"Mt_Data"
	else:
		lastget=basedir+"Mt_cat%d_Data"%cat
	h_data = fIn.Get( lastget)
	if h_data == None: print "<*> No Data Found for cat%d last get '%s'"%(cat,lastget)

	if opts.rebin >0 :
		h_data.Rebin(opts.rebin)

	roo_data= ROOT.RooDataHist("data_obs_cat%d"%cat,"M_{T}",arglist_obs,h_data)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h_data,roo_data])

datacard.write("shapes data_obs *\t" + opts.output)
datacard.write("\tw:data_obs_$CHANNEL")
datacard.write("\n")

def ImportPdfFromTH1(tfile, name, target, add=[]): ## w is global as arglist_obs and argset_obs and rebin
	''' Import the pdf a th1 in tfile, with name and renaming it as target. You can add a set of (name,scale) to the base hist'''
	if tfile == None:
		print "<*> File not exists"
	h = tfile.Get(name)

	if h == None:
		print "<*> Unable to find '%s' in '%s'"%(name,tfile.GetName())
		raise Exception("No Such Histogram")

	if opts.rebin >0 :
		h.Rebin(opts.rebin)

	for name2,c in add:
		h_tmp=tfile.Get(name2)
		if h_tmp==None:
			print "<*> Unable to find '%s' in '%s': Ignoring it!"%(name2,tfile.GetTitle())
			continue
		h_tmp.Scale(c)
		h.Add(h_tmp)

	if h.Integral() <= 0.: 
		print " Integral for hist '%s' is null"%name
		print "FIXME implement a negligible shape with constrain"
		return
	else:
		print "Normalization for '%s' is %e"%(name,h.Integral())
	
	## ---ZERO ---
	for i in range(0,h.GetNbinsX()):
		if h.GetBinContent(i+1)<0: h.SetBinContent(i+1,0)
	roo_mc = ROOT.RooDataHist("hist_"+target,target,arglist_obs,h)
	pdf_mc = ROOT.RooHistPdf(target, target,argset_obs, roo_mc)
	## NORM
	getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
	w.factory(target + "_norm[%e]"% h.Integral())
	g.extend([h,roo_mc,pdf_mc])

	return

### BKG ###
######### import mc based background contributions
systs=["BTAG","JES","TAU","TRIG","TRIGMET"]
#systs=["BTAG","TAU"]
if opts.nosyst: systs=[]

systBkg=[""]
for shift in ["Up","Down"]: 
	for s in systs: 
		systBkg.append(s + shift)

for syst in systBkg:
 for mc in mcList:
   #if 'JER' in syst and (mc== 'DY' or mc=='TT' or mc=='ST' or mc=='WW') : continue ###
   for cat in range(0,opts.ncat):
	if opts.ncat==1:
		lastget=basedir+"Mt_"+ mc
	else:
		lastget=basedir+"Mt_cat%d_"%cat+ mc
	if syst !="":lastget+="_"+syst
	h_mc=fIn.Get(lastget)

	if syst == "": systName=""
	else: systName="_"+syst

	target="pdf_cat%d_"%cat + mc + systName
	print "*** Considering MC=",mc,"cat=",cat,"syst=",syst,"target=",target
	ImportPdfFromTH1(fIn,lastget,target )

   #if syst=="":
   #	datacard.write("shapes %s *\t"%mc + opts.output)
   #	datacard.write("\tw:pdf_$CHANNEL_"+mc)
   #	datacard.write("\tw:pdf_$CHANNEL_"+mc+"_$SYSTEMATIC")
   #	datacard.write("\n")

################# Import SIGNAL CONTRIBUTIONS ##############
systs=["BTAG","TAU","JES","TRIG","TRIGMET"]
if opts.nosyst: systs=[]
systSig=[""]
for shift in ["Up","Down"]: 
	for s in systs: 
		systSig.append(s + shift)

## print "#########################"
## print "     FIX MH POINTS       "
## print "#########################"
for syst in systSig:
 for sigMH in [ 200,220,250,300,350,400,500]:
 #for sigMH in [ 200,350,400,500]:
   for cat in range(0,opts.ncat):
	sigStr="HplusToTauNu_M-"+str(sigMH)+"_13TeV_amcatnlo"
	if opts.ncat==1:
		lastget=basedir+"Mt_"+ sigStr
	else:
		lastget=basedir+"Mt_cat%d_"%cat+ sigStr

	if syst !="":lastget+="_"+syst

	if syst == "": systName=""
	else: systName="_"+syst

	target="pdf_cat%d_Hplus_MH"%cat + str(sigMH)  +systName
	print "*** Considering MH=",sigMH,"cat=",cat,"syst=",syst,"target=",target
	ImportPdfFromTH1(fIn, lastget, target)

datacard.write("shapes Hplus *\t"+opts.output)
datacard.write("\tw:"+"pdf_$CHANNEL_Hplus_MH$MASS" )
datacard.write("\tw:"+"pdf_$CHANNEL_Hplus_MH$MASS_$SYSTEMATIC" )
datacard.write("\n")

########################### IMPORT QCD from inverse isolation ############
if opts.qcd != "":
   #print "FIXME ISOInv"
   fInQCD=ROOT.TFile.Open(opts.qcd,"READ")

   if fInQCD == None: print "<*> NO QCD File '%s'"%opts.qcd

   systs=["BTAG","RFAC"] ## no JES here
   #systs=["BTAG","RFAC"]
   if opts.nosyst: systs=[]
   systQCD=[""]
   for shift in ["Up","Down"]: 
   	for s in systs: 
   		systQCD.append(s + shift)

   for syst in systQCD:
    for cat in range(0,opts.ncat):
	if opts.ncat == 1:
		lastget="ChargedHiggsQCDPurity/Vars/MtIsoInv_Data"
	else:
		lastget="ChargedHiggsQCDPurity/Vars/MtIsoInv_cat%d_Data"%cat

	if syst !="":lastget+="_"+syst

	if syst == "": systName=""
	else: systName="_"+syst

	addlist=[]
	for bkg in ['WJets','TT','WW','WZ','ZZ','DY']:
		addlist.append( (re.sub('Data',bkg,lastget),-opts.qcdlumi) ) ## the scaling to 5 is done later in the datacard, minus because needs to be subtracted
	target="pdf_inviso_cat%d_QCD"%cat + systName
	print "*** Considering QCD","syst=",syst,"target=",target
	ImportPdfFromTH1(fInQCD, lastget,target ,addlist)

   datacard.write("shapes QCD *\t"+opts.output)
   datacard.write("\tw:"+"pdf_inviso_$CHANNEL_QCD" )
   datacard.write("\tw:"+"pdf_inviso_$CHANNEL_QCD_$SYSTEMATIC" )
   datacard.write("\n")

### GENERAL LINE
datacard.write("shapes\t*\t*\t"+opts.output+"\tw:pdf_$CHANNEL_$PROCESS\tw:pdf_$CHANNEL_$PROCESS_$SYSTEMATIC\n")

#### OBSERVATION
datacard.write("-------------------------------------\n")
datacard.write("bin")
for cat in range(0,opts.ncat):
	datacard.write("\tcat%d"%cat)
datacard.write("\n")
datacard.write("observation")
for cat in range(0,opts.ncat):
	datacard.write("\t-1")
datacard.write("\n")
#### RATE
mcAll=["Hplus"]
mcAll.extend(mcList)
if 'QCD' not in mcAll: mcAll.append("QCD")
datacard.write("-------------------------------------\n")
datacard.write("bin\t")
for cat in range(0,opts.ncat):
   for proc in mcAll:
	datacard.write("\tcat%d"%cat)
datacard.write("\n")
datacard.write("process\t")
for cat in range(0,opts.ncat):
   for proc in mcAll:
	datacard.write("\t%s"%proc)
datacard.write("\n")
datacard.write("process\t")
for cat in range(0,opts.ncat):
   for idx,proc in enumerate(mcAll): ## TRICK, put the only signal first
	datacard.write("\t%d"%idx)
datacard.write("\n")
datacard.write("rate\t")
for cat in range(0,opts.ncat):
   for proc in mcAll:
	if proc=="QCD" and opts.qcd!="":
		datacard.write("\t%.2f"%(opts.lumi/opts.qcdlumi)) ## lumi factor 5000/2110, should be 1.00 FIXME
	else:
		datacard.write("\t%.0f"%opts.lumi)
datacard.write("\n")
############ SYST########
datacard.write("-------------------------------------\n")

def writeNormSyst(name="lumi",value="1.027", regexp=".*"):
	########## LUMI ###
	datacard.write(name+"\tlnN")
	invert=False
	if regexp != "" and regexp[0] == '!':
		invert=True
		regexp=regexp[1:]

	for cat in range(0,opts.ncat):
	   for proc in mcAll:
		match=re.search(regexp,proc)
		if (match and not invert) or (not match and invert):
		   datacard.write("\t"+value)
		else:
		   datacard.write("\t-")
	datacard.write("\n")

if opts.qcd=="":
	writeNormSyst("lumi","1.027","")
else:
	writeNormSyst("lumi","1.027","!QCD")
########## LUMI ###
### datacard.write("lumi\tlnN")
### for cat in range(0,opts.ncat):
###    for proc in mcAll:
### 	if proc=="QCD" and opts.qcd!="":
### 	   datacard.write("\t-")
### 	else:
### 	   datacard.write("\t1.027")
### datacard.write("\n")

if opts.nosyst: 
	w.writeToFile(opts.output)
	print " --- DONE --- "
	exit(0)

########### RFAC ###############
##if opts.qcd != "":
##   datacard.write("RFAC shape")
##   #RFACUp RFACDown
##   for proc in mcAll:
##	if proc=="QCD":
##	   datacard.write("\t1")
##	else:
##	   datacard.write("\t-")
##   datacard.write("\n")
############ BTAG ###############
##datacard.write("BTAG shape")
##syst="BTAG"
##for proc in mcAll:
##	if proc=="QCD":
##		if opts.qcd !="" and syst in systQCD:
##        		datacard.write("\t1")
##		else: datacard.write("\t-")
##	else:
##        	datacard.write("\t1")
##datacard.write("\n")
############ JES ###############
##datacard.write("JES shape")
##syst="JES"
##for proc in mcAll:
##	if proc=="QCD":
##		if opts.qcd !="" and syst in systQCD:
##        		datacard.write("\t1")
##		else: datacard.write("\t-")
##	else:
##        	datacard.write("\t1")
##datacard.write("\n")
########## JER ###############
#datacard.write("JER shape")
#syst="JER"
#for proc in mcAll:
#	if proc=="QCD":
#		if opts.qcd !="" and syst in systQCD:
#        		datacard.write("\t1")
#		else: datacard.write("\t-")
#	else:
#        	datacard.write("\t1")
#datacard.write("\n")
########## TAU ###############
##datacard.write("TAU shape")
##syst="TAU"
##for proc in mcAll:
##	if proc=="QCD":
##		if opts.qcd !="" and syst in systQCD:
##        		datacard.write("\t1")
##		else: datacard.write("\t-")
##	else:
##        	datacard.write("\t1")
##datacard.write("\n")

def writeSyst(syst="JES"):
	datacard.write(syst+" shape")
	for proc in mcAll:
		if proc=="QCD":
			if opts.qcd !="" and syst + "Up"  in systQCD:
	        		datacard.write("\t1")
			else: datacard.write("\t-")
		elif proc in mcList :
			if syst + "Up" in systBkg:
	        		datacard.write("\t1")
			else:
	        		datacard.write("\t-")
		elif proc == "Hplus" :
			if syst + "Up" in systSig:
	        		datacard.write("\t1")
			else:
	        		datacard.write("\t-")
		else:
			print "DONT KNOW WHAT TO DO WITH proc=",proc,"syst=",syst
	datacard.write("\n")

### write shape syst
writeSyst('JES')
writeSyst('JER')
writeSyst('TAU')
writeSyst('BTAG')
writeSyst('RFAC')
writeSyst('TRIG')
writeSyst('TRIGMET')
## write norm syst
writeNormSyst("TTSCALE","0.965/1.024","TT")
writeNormSyst("TTPDF","1.042","TT")
writeNormSyst("TTMASS","1.027","TT")

writeNormSyst("STSCALE","0.977/1.028","ST")
writeNormSyst("STPDF","1.026","ST")
writeNormSyst("STMASS","1.022","ST")

writeNormSyst("DYSCALE","0.9963/1.0065","DY")
writeNormSyst("DYPDF","1.037","DY")

writeNormSyst("WJetsSCALE","0.996/1.008","WJets")
writeNormSyst("WJetsPDF","1.0375","WJets")

writeNormSyst("WWSCALE","1.025","WW")
writeNormSyst("WWPDF","1.022","WW")

writeNormSyst("WZSCALE","1.032","WZ")
writeNormSyst("WZPDF","1.044","WZ")

writeNormSyst("ZZSCALE","1.031","ZZ")
writeNormSyst("ZZPDF","1.037","ZZ")

#fOut=ROOT.TFile.Open(opts.output,"RECREATE")
w.writeToFile(opts.output)

print "--------------------" 
print "datacard=",datName
print "ws=",opts.output
print " --- DONE --- "
