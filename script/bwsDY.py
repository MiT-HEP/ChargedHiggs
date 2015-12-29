import sys, os
from optparse import OptionParser,OptionGroup

parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="DY.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [Default=%default]", default="DYWs.root")
parser.add_option("-L","--lumi",type='float',help="Luminosity pb. [Default=%default]", default=2215)
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
datName = "cms_datacard_ws_dy"
if opts.datacard != "": datName += "_" + opts.datacard
datName += ".txt"

datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")


########################## IMPORT DATA #############
w.factory("mmm[130,1000]"); # RooRealVar
mmm=w.var("mmm")
arglist_obs = ROOT.RooArgList(mmm)
argset_obs = ROOT.RooArgSet(mmm)

fIn = ROOT.TFile.Open(opts.input,"READ")

#basedir = "ChargedHiggsTauNu/Vars/"

t_data = fIn.Get( "ggTree")

if t_data == None:
	print "<*> ERROR tree ggTree not found "

## Fit the t_data mmm tree
data=[]
for cat in range(0,opts.ncat):
	if cat== 0 :lastget="DYAnalysis/Vars/MHighPtmm_Data"
	if cat== 1 :lastget="DYAnalysis/Vars/MHighPtee_Data"
	h_data = fIn.Get( lastget)
	roo_data= ROOT.RooDataHist("data_obs_cat%d"%cat,"Mmm",arglist_obs,h_data)
	data.append(roo_data)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## 

#RooDataSet (const char *name, const char *title, TTree *ntuple, const RooArgSet &vars, const char *cuts=0, const char *wgtVarName=0)
#roo_data=ROOT.RooDataSet("data_obs_cat0","data obs", t_data, argset_obs,"mmm>130")
#getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## 

datacard.write("shapes data_obs *\t" + opts.output)
datacard.write("\tw:data_obs_$CHANNEL")
datacard.write("\n")


######################## WRITE SIG ##########################

w.factory("MH[150,250]"); # RooRealVar
mX=w.var("MH")
#w.factory("sX[0.,100]"); # RooRealVar
#sX=w.var("sX")


for n in range(0,opts.ncat):
	#sX = ROOT.RooFormulaVar("sX_cat%d"%n,"sX","0.01*MH",ROOT.RooArgList(mX))
	sX = ROOT.RooFormulaVar("sX_cat%d"%n,"sX","4",ROOT.RooArgList(mX))
	
	pdf_sig = ROOT.RooGaussian("pdf_cat%d_SIG"%n,"gauspdf", mmm, mX,sX)

	print "->Importing",pdf_sig.GetName()
	getattr(w,'import')(pdf_sig,ROOT.RooCmdArg())
	w.factory("pdf_cat%d_SIG_norm[%f]"% (n,1.0)) ## I need EFF and ACCP
	g.extend([pdf_sig,sX])	

datacard.write("shapes SIG *\t" + opts.output)
datacard.write("\tw:pdf_$CHANNEL_SIG")
datacard.write("\n")

######################## WRITE/FIT BKG ##########################
ROOT.gSystem.Load("libHiggsAnalysisCombinedLimit.so")


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

useMultiPdf = False
if useMultiPdf:
	datacard.write("shapes bkg *\t" + opts.output)
	datacard.write("\tw:pdf_$CHANNEL_bkg")
	datacard.write("\n")
else:
	print " ***********************"
	print " *     EXP BKG         *"
	print " ***********************"
	datacard.write("shapes bkg *\t" + opts.output)
	datacard.write("\tw:exp_$CHANNEL")
	datacard.write("\n")

for cat in range(0,opts.ncat):
	print "--> Doing bkg for cat",cat

	storedPdfs = ROOT.RooArgList("store_cat%d"%cat);
	storeBern = None
	prevNll = -1;
	prev_order = 0;
	
	## guess bernstain order
	useOrder=4
	
	#for order in range(1,8): ## 8 is not defined in combine"
	MultiPdf=False
	if MultiPdf: ## 8 is not defined in combine
		print "* Testing Bernstain order",order
		if useOrder> 0 and order != useOrder : continue
	
		coeff = ROOT.RooArgList("bern_coeff_cat%d"%cat)
		for i in range(0,order+1):
			b = ROOT.RooRealVar("bern_cat%d_ord%d_%d"%(cat,order,i),"bern_coeff",0.1*(i+1),-10.,10.)
			coeff.add( b ) 
			g.extend([b])
		bern = ROOT.RooBernsteinFast(order)("pdf_bern_cat%d"%cat,"bernstain",mmm, coeff)
		print "Berns=",order,bern
		fitres=bern.fitTo(data[cat],ROOT.RooFit.Save(), 
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
	
		gof=GoodnessOfFit( data[cat],bern,mmm,"plotDY/bern%d_cat%d.pdf"%(order,cat))
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
	l = ROOT.RooRealVar("exp_lambda_cat%d"%cat,"exp_lambda",-1e-3,-2, 0)
	exp = ROOT.RooExponential("exp_cat%d"%cat,"exp",mmm,l)
	fitres=exp.fitTo(data[cat],
				ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
				)#ROOT.RooFit.Extended())
	gof=GoodnessOfFit( data[cat],exp,mmm,"plotDY/exp_cat%d.pdf"%cat)
	l.setConstant(False)
	print "* EXP l=",l.getVal()
	storedPdfs.add(exp)
	
	## Power Law
	plaw_exp = ROOT.RooRealVar("plaw_exp_cat%d"%cat,"plaw",-5,-10,-1)
	powlaw = ROOT.RooPower("plaw_cat%d"%cat,"plaw",mmm,plaw_exp)
	gof=GoodnessOfFit( data[cat],powlaw,mmm,"plotDY/powlaw_cat%d.pdf"%cat)
	fitres=powlaw.fitTo(data[cat],
				ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
			) 
	plaw_exp.setConstant(False)
	print "* POWERLAW c=",plaw_exp.getVal()
	storedPdfs.add(powlaw)
	
	### Modified PowerLaw
	alpha= ROOT.RooRealVar("plawlog_cat%d_a"%cat,"pla",-5,-10,-1)
	beta= ROOT.RooRealVar("plawlog_cat%d_b"%cat,"plb",1,-10,10)
	myfunc= ROOT.RooGenericPdf("plaw_log_cat%d"%cat,"plaw2","TMath::Power(mmm,plawlog_cat%d_a + plawlog_cat%d_b* TMath::Log(mmm))"%(cat,cat),ROOT.RooArgList(mmm,alpha,beta))
	fitres=myfunc.fitTo(data[cat],
				ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Warnings(0)
			)
	alpha.setConstant(False)
	beta.setConstant(False)
	gof=GoodnessOfFit( data[cat],myfunc,mmm,"plotDY/powlawLog_cat%d.pdf"%cat)
	storedPdfs.add(myfunc)
	
	pdf_cat = ROOT.RooCategory("pdfindex_cat%d"%cat,"c")
	pdf_bkg = ROOT.RooMultiPdf("pdf_cat%d_bkg"%cat,"multipdf",pdf_cat,storedPdfs)
	pdf_bkg_norm = ROOT.RooRealVar("pdf_cat%d_bkg_norm"%cat,"",data[cat].sumEntries(),data[cat].sumEntries()*.5,data[cat].sumEntries()*2.0)
	pdf_bkg_norm.setConstant(False)

	if MultiPdf:
		print "-> importing",pdf_bkg.GetName()
		getattr(w,'import')(pdf_bkg,ROOT.RooCmdArg())
		getattr(w,'import')(pdf_bkg_norm,ROOT.RooCmdArg())
	else:
		print "-> importing",pdf_bkg.GetName()
		exp_norm = ROOT.RooRealVar("exp_cat%d_norm"%cat,"",data[cat].sumEntries(),data[cat].sumEntries()*.5,data[cat].sumEntries()*2.0)
		getattr(w,'import')(exp,ROOT.RooCmdArg())
		getattr(w,'import')(exp_norm,ROOT.RooCmdArg())


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


if MultiPdf:
	datacard.write("\n")
	for cat in range(0,opts.ncat):
		datacard.write("pdfindex_cat%d discrete\n"%cat)

#fOut=ROOT.TFile.Open(opts.output,"RECREATE")
w.writeToFile(opts.output)

print " --- DONE --- "
