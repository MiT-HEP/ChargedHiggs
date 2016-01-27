import sys, os
from optparse import OptionParser,OptionGroup

parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="GG.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [Default=%default]", default="GGWs.root")
parser.add_option("-L","--lumi",type='float',help="Luminosity pb. [Default=%default]", default=5000)
parser.add_option("-n","--ncat",type='int',help="Number of cat. [Default=%default]", default=1)

extra = OptionGroup(parser,"Extra options:","")
#extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. [Default=%default]", default=1)
extra.add_option("","--datacard",type='string',help="Output datacard extra. [Default=%default]", default="")

parser.add_option_group(extra)


opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

g=[] ## garbage un-collector

ROOT.RooMsgService.instance().setSilentMode(True);


################### OPEN OUTPUT ############
w = ROOT.RooWorkspace("w","w")

############ DATACARD ###########
datName = "cms_datacard_ws_gg"
if opts.datacard != "": datName += "_" + opts.datacard
datName += ".txt"

datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")


########################## IMPORT DATA #############
w.factory("mgg[500,1200]"); # RooRealVar
mgg=w.var("mgg")
arglist_obs = ROOT.RooArgList(mgg)
argset_obs = ROOT.RooArgSet(mgg)

fIn = ROOT.TFile.Open(opts.input,"READ")

#basedir = "ChargedHiggsTauNu/Vars/"

t_data = fIn.Get( "ggTree")

if t_data == None:
	print "<*> ERROR tree ggTree not found "

## Fit the t_data mgg tree

#h_data = fIn.Get( lastget)
#roo_data= ROOT.RooDataHist("data_obs_cat%d"%cat,"M_{T}",arglist_obs,h_data)

#RooDataSet (const char *name, const char *title, TTree *ntuple, const RooArgSet &vars, const char *cuts=0, const char *wgtVarName=0)
roo_data=ROOT.RooDataSet("data_obs_cat0","data obs", t_data, argset_obs,"mgg>500")
getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## 

datacard.write("shapes data_obs *\t" + opts.output)
datacard.write("\tw:data_obs_$CHANNEL")
datacard.write("\n")


######################## WRITE SIG ##########################

w.factory("MH[500,1000]"); # RooRealVar
mX=w.var("MH")
#w.factory("sX[0.,100]"); # RooRealVar
#sX=w.var("sX")

sX = ROOT.RooFormulaVar("sX","sX","0.05*MH",ROOT.RooArgList(mX))

for n in range(0,opts.ncat):
	
	pdf_sig = ROOT.RooGaussian("pdf_cat%d_SIG"%n,"gauspdf", mgg, mX,sX)
	
	getattr(w,'import')(pdf_sig,ROOT.RooCmdArg())
	w.factory("pdf_cat%d_SIG_norm[%f]"% (n,1.0)) ## I need EFF and ACCP

	g.extend([pdf_sig])	

	datacard.write("shapes SIG *\t" + opts.output)
	datacard.write("\tw:pdf_$CHANNEL_SIG")
	datacard.write("\n")

######################## WRITE/FIT BKG ##########################
#FIXME NCAT
ROOT.gSystem.Load("libHiggsAnalysisCombinedLimit.so")

datacard.write("shapes bkg *\t" + opts.output)
datacard.write("\tw:pdf_cat0_bkg")
datacard.write("\n")

storedPdfs = ROOT.RooArgList("store");

def GoodnessOfFit(data,pdf,mass,name=""):  
	plot_chi2 = mass.frame();
	data.plotOn(plot_chi2,ROOT.RooFit.Binning(100),ROOT.RooFit.Name("data"));
	pdf.plotOn(plot_chi2,ROOT.RooFit.Name("pdf"));
	np = pdf.getParameters(data).getSize()
	chi2 = plot_chi2.chiSquare("pdf","data",np);
	if name !="":
		c=ROOT.TCanvas(name,name)
		plot_chi2.Draw()
		c.SaveAs(name)
	return ROOT.TMath.Prob(chi2,np)

def SetConstant( par, value=True):
	par.setConstant(value)

storeBern = None
prevNll = -1;
prev_order = 0;

## guess bernstain order
useOrder=4

for order in range(1,8): ## 8 is not defined in combine
	print "* Testing Bernstain order",order
	if useOrder> 0 and order != useOrder : continue

	coeff = ROOT.RooArgList("bern_coeff")
	for i in range(0,order+1):
		b = ROOT.RooRealVar("bern_ord%d_%d"%(order,i),"bern_coeff",0.1*(i+1),-10.,10.)
		coeff.add( b ) 
		g.extend([b])
	bern = ROOT.RooBernsteinFast(order)("pdf_bern","bernstain",mgg, coeff)
	print "Berns=",order,bern
	fitres=bern.fitTo(roo_data,ROOT.RooFit.Save(), 
			ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
			)

	thisNll= fitres.minNll()
	## for i in range(0,1000):	
	## 	if coeff.at(i) == None : break
	## 	coeff.at(i).setConstant() ## fix them to the backgr

	chi2 = 2.*(prevNll-thisNll);
	if (chi2<0. and  order>0): 
		chi2=0.;

	if order >1 : prob = ROOT.TMath.Prob(chi2,order-prev_order);
	else:  prob =0

	gof=GoodnessOfFit( roo_data,bern,mgg,"plotGG/bern%d.pdf"%order)
	print "\t\ chi2=",chi2,"prevNLL",prevNll,"thisNll",thisNll,"prob=",prob,"gof=",gof
	
	prevNll = thisNll
	prev_order = order

	for idx in range(0,coeff.getSize()):
		coeff.at(idx).setConstant(False)

	g.extend([coeff,bern])
	
	print "\t\t -> Probability",prob
	if prob> 0.05 or (useOrder >0 and order==useOrder): 
		print  " SELECTED BERNSTAIN ORDER",order
		storedPdfs.add(bern)
		break

## Exponential
#for order
l = ROOT.RooRealVar("exp_lambda","exp_lambda",-1e-3,-2, 0)
exp = ROOT.RooExponential("exp","exp",mgg,l)
fitres=exp.fitTo(roo_data,
			ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
			)#ROOT.RooFit.Extended())
gof=GoodnessOfFit( roo_data,exp,mgg,"plotGG/exp.pdf")
l.setConstant(False)
print "* EXP l=",l.getVal()
storedPdfs.add(exp)

## Power Law
plaw_exp = ROOT.RooRealVar("plaw_exp","plaw",-5,-10,-1)
powlaw = ROOT.RooPower("plaw","plaw",mgg,plaw_exp)
gof=GoodnessOfFit( roo_data,powlaw,mgg,"plotGG/powlaw.pdf")
fitres=powlaw.fitTo(roo_data,
			ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
		) 
plaw_exp.setConstant(False)
print "* POWERLAW c=",plaw_exp.getVal()
storedPdfs.add(powlaw)

### Modified PowerLaw
alpha= ROOT.RooRealVar("plawlog_a","pla",-5,-10,-1)
beta= ROOT.RooRealVar("plawlog_b","plb",1,-10,10)
myfunc= ROOT.RooGenericPdf("plaw_log","plaw2","TMath::Power(mgg,plawlog_a + plawlog_b* TMath::Log(mgg))",ROOT.RooArgList(mgg,alpha,beta))
fitres=myfunc.fitTo(roo_data,
			ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
		)
alpha.setConstant(False)
beta.setConstant(False)
gof=GoodnessOfFit( roo_data,myfunc,mgg,"plotGG/powlawLog.pdf")
storedPdfs.add(myfunc)

pdf_cat = ROOT.RooCategory("pdfindex_cat0","c")
pdf_bkg = ROOT.RooMultiPdf("pdf_cat0_bkg","multipdf",pdf_cat,storedPdfs)
pdf_bkg_norm = ROOT.RooRealVar("pdf_cat0_bkg_norm","",roo_data.sumEntries(),roo_data.sumEntries()*.5,roo_data.sumEntries()*2.0)
pdf_bkg_norm.setConstant(False)
getattr(w,'import')(pdf_bkg,ROOT.RooCmdArg())
getattr(w,'import')(pdf_bkg_norm,ROOT.RooCmdArg())


######################## WRITE DATACARD ##########################
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
datacard.write("-------------------------------------\n")
datacard.write("bin\t")
for cat in range(0,opts.ncat):
   for proc in ['SIG',"bkg"]:
	datacard.write("\tcat%d"%cat)
datacard.write("\n")
datacard.write("process\t")
for cat in range(0,opts.ncat):
   for proc in ['SIG',"bkg"]:
	datacard.write("\t%s"%proc)
datacard.write("\n")
datacard.write("process\t")
for cat in range(0,opts.ncat):
   for idx,proc in enumerate(['SIG',"bkg"]): # trick, SIG first
	datacard.write("\t%d"%idx)
datacard.write("\n")
datacard.write("rate\t")
for cat in range(0,opts.ncat):
   for proc in ['SIG',"bkg"]:
	if proc == 'SIG':
		datacard.write("\t%.0f"%opts.lumi)
	else:
		datacard.write("\t1.0")
datacard.write("\n")
############ SYST########
datacard.write("-------------------------------------\n")

########## LUMI ###
datacard.write("lumi\tlnN")
for cat in range(0,opts.ncat):
   for proc in ['SIG',"bkg"]:
	if proc=="bkg":
	   datacard.write("\t-")
	else:
	   datacard.write("\t1.13")
datacard.write("\n")


datacard.write("\n")
for cat in range(0,opts.ncat):
	datacard.write("pdfindex_cat%d discrete\n"%cat)

#fOut=ROOT.TFile.Open(opts.output,"RECREATE")
w.writeToFile(opts.output)

print " --- DONE --- "
