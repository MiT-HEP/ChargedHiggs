import sys, os
from optparse import OptionParser,OptionGroup

parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="ChHiggs.root")
parser.add_option("","--qcd",type='string',help="Input ROOT file. Set Null to use QCD from above [Default=%default]", default="QCDPurity.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [Default=%default]", default="ChHiggsWs.root")
parser.add_option("-L","--lumi",type='float',help="Luminosity pb. [Default=%default]", default=5000)
parser.add_option("-n","--ncat",type='int',help="Number of cat. [Default=%default]", default=1)

extra = OptionGroup(parser,"Extra options:","")
#extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. [Default=%default]", default=1)
extra.add_option("","--datacard",type='string',help="Output datacard extra. [Default=%default]", default="")
extra.add_option("","--basedir",type='string',help="Base Dir. [Default=%default]", default="ChargedHiggsTauNu/Vars/")

parser.add_option_group(extra)


opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

g=[] ## garbage un-collector

mcList=['DY','TTJets','WW','WZ','ZZ','WJets']
if opts.qcd == "" : mcList.append("QCD")

################### OPEN OUTPUT ############
w = ROOT.RooWorkspace("w","w")

############ DATACARD ###########
datName = "cms_datacard_ws"
if opts.qcd != "": datName+="_qcddata"
if opts.datacard != "": datName += "_" + opts.datacard
datName += ".txt"

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

	roo_data= ROOT.RooDataHist("data_obs_cat%d"%cat,"M_{T}",arglist_obs,h_data)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h_data,roo_data])

datacard.write("shapes data_obs *\t" + opts.output)
datacard.write("\tw:data_obs_$CHANNEL")
datacard.write("\n")

def ImportPdfFromTH1(tfile, name, target): ## w is global as arglist_obs and argset_obs
	if tfile == None:
		print "<*> File not exists"
	h = tfile.Get(name)

	if h == None:
		print "<*> Unable to find '%s' in '%s'"%(name,tfile.GetTitle())
		raise Exception("No Such Histogram")

	if h.Integral() <= 0.: 
		print " Integral for hist '%s' is null"%name
		print "FIXME implement a negligible shape with constrain"
		return
	else:
		print "Normalization for '%s' is %f"%(name,h.Integral())
	
	## ---ZERO ---
	for i in range(0,h.GetNbinsX()):
		if h.GetBinContent(i+1)<0: h.SetBinContent(i+1,0)
	roo_mc = ROOT.RooDataHist("hist_"+target,target,arglist_obs,h)
	pdf_mc = ROOT.RooHistPdf(target, target,argset_obs, roo_mc)
	## NORM
	getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
	w.factory(target + "_norm[%f]"% h.Integral())
	g.extend([h,roo_mc,pdf_mc])

	return

### BKG ###
for mc in mcList:
   for cat in range(0,opts.ncat):
	if opts.ncat==1:
		lastget=basedir+"Mt_"+ mc
	else:
		lastget=basedir+"Mt_cat%d_"%cat+ mc
	h_mc=fIn.Get(lastget)

	ImportPdfFromTH1(fIn,lastget,"pdf_cat%d_"%cat + mc )

   datacard.write("shapes %s *\t"%mc + opts.output)
   datacard.write("\tw:pdf_$CHANNEL_"+mc)
   datacard.write("\n")

for sigMH in [ 200,250,300,350,400,500]:
   for cat in range(0,opts.ncat):
	sigStr="HplusToTauNu_M-"+str(sigMH)+"_13TeV_amcatnlo"
	if opts.ncat==1:
		lastget=basedir+"Mt_"+ sigStr
	else:
		lastget=basedir+"Mt_cat%d_"%cat+ sigStr

	ImportPdfFromTH1(fIn, lastget, "pdf_cat%d_Hplus_MH"%cat + str(sigMH))

datacard.write("shapes Hplus *\t"+opts.output)
datacard.write("\tw:"+"pdf_$CHANNEL_Hplus_MH$MASS" )
datacard.write("\n")

if opts.qcd != "":
   print "FIXME ISOInv"
   fInQCD=ROOT.TFile.Open(opts.qcd,"READ")

   if fInQCD == None: print "<*> NO QCD File '%s'"%opts.qcd

   for cat in range(0,opts.ncat):
	if opts.ncat == 1:
		lastget="ChargedHiggsQCDPurity/Vars/Mt_Data"
	else:
		lastget="ChargedHiggsQCDPurity/Vars/Mt_cat%d_Data"%cat
	ImportPdfFromTH1(fInQCD, lastget,"pdf_inviso_cat%d_QCD"%cat )

   datacard.write("shapes QCD *\t"+opts.output)
   datacard.write("\tw:"+"pdf_inviso_$CHANNEL_QCD" )
   datacard.write("\n")

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
		datacard.write("\t2.37") ## lumi factor 5000/2110, should be 1.00 FIXME
	else:
		datacard.write("\t%.0f"%opts.lumi)
datacard.write("\n")
############ SYST########
datacard.write("-------------------------------------\n")

########## LUMI ###
datacard.write("lumi\tlnN")
for cat in range(0,opts.ncat):
   for proc in mcAll:
	if proc=="QCD" and opts.qcd!="":
	   datacard.write("\t-")
	else:
	   datacard.write("\t1.13")
datacard.write("\n")

#fOut=ROOT.TFile.Open(opts.output,"RECREATE")
w.writeToFile(opts.output)

print " --- DONE --- "
