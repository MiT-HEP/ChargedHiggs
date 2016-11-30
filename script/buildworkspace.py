#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="ChHiggs.root")
parser.add_option("","--qcd",type='string',help="Input ROOT file. Set Null to use QCD from above [%default]", default="QCDPurity.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="ChHiggsWs.root")
parser.add_option("-L","--lumi",type='float',help="Luminosity pb. [%default]", default=2308)
parser.add_option("","--qcdlumi",type='float',help="QCD Luminosity pb. [%default]", default=2308)
parser.add_option("-n","--ncat",type='int',help="Number of cat. [%default]", default=1)
parser.add_option("","--nosyst",action='store_true',help="Do not look for syst. [%default]", default=False)
parser.add_option("","--ewk",type='string',help="Input ROOT file for ewk. Set Null to use EWK from MC [%default]", default="")
parser.add_option("","--ewklumi",type='float',help="EWK Luminosity pb. [%default]", default=2308)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. if >1000 variable bin [%default]", default=-1)
extra.add_option("","--datacard",type='string',help="Output datacard extra. [%default]", default="")
extra.add_option("","--basedir",type='string',help="Base Dir. [%default]", default="ChargedHiggsTauNu/Vars/")

parser.add_option_group(extra)


opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

g=[] ## garbage un-collector

######### EXTRA CONFIGURATION
fullstat=True
savePdf=False
tailfit=False
#systsMC=["BTAG","JES","TAU","TRIG","TRIGMET","TAUHIGHPT","TAUSCALE","ELEVETO","MUVETO","JER","UNCLUSTER","PU","TOPRW"]
#systsMC=["BTAG","JES","TAU","TRIGMET","TAUHIGHPT","TAUSCALE","ELEVETO","MUVETO","JER","UNCLUSTER","PU","TOPRW","TRIG1p","TRIG3p"]
systsMC=["BTAGB","BTAGL","JES","TAU","TRIGMET","TAUHIGHPT","TAUSCALE","ELEVETO","MUVETO","JER","UNCLUSTER","PU","TOPRW","TRIG"]
#systsQCD=["RFAC"]
systsQCD=["RFAC1p","RFAC3p"]
systsEWK=["TRIGMET","TRIG","MUEFF","MURECOEFF","TAU","TAUHIGHPT"]

###########################

if not savePdf and opts.qcdlumi/opts.lumi !=1 : 
	print "WARINING DONT WORK!!"

#systs=["BTAG","JES","TAU","TRIG","TRIGMET","TAUHIGHPT","TAUSCALE"]
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
	#h1 = ROOT.TH1D(h.GetName()+"_rebin",h.GetTitle(),len(mybins)-1,mybins)
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1

#mcList=['DY','TT','WW','WZ','ZZ','WJets']
#mcList=['DY','TT','ST','WW','WZ','ZZ','WJets']
mcList=[]
if opts.qcd == "" : mcList.append("QCD")
if opts.ewk == "" : mcList.extend(["DY","TT","ST","WW","WZ","ZZ","WJets"])

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
	if h_data == None: 
		print "<*> No Data Found for cat%d last get '%s'"%(cat,lastget)
		raise IOError

	if opts.rebin>999:
		h_data=Rebin(h_data)
	elif opts.rebin >0 :
		h_data.Rebin(opts.rebin)

	roo_data= ROOT.RooDataHist("data_obs_cat%d"%cat,"M_{T}",arglist_obs,h_data)
	getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
	g.extend([h_data,roo_data])

datacard.write("shapes data_obs *\t" + opts.output)
datacard.write("\tw:data_obs_$CHANNEL")
datacard.write("\n")


def Smooth(h):
	''' Smooth out the tail of the mT distribution'''
	#return;### It's not working ... need to figure out the range dynamically
	### wjets has some problem
	if not tailfit:
		return  ## NO SMOOTH

	if True and \
	   'WJets' not in h.GetName() and \
	   'TT' not in h.GetName() and \
	   'MtIsoInv_Data' not in h.GetName() : 
		   return
	x0=250.
	bin0=h.FindBin(x0)
	bin1=h.GetNbinsX()
	n=h.Integral(bin0,bin1)
	print "*** Considering hist",h.GetName() ## DEBUG
	print "*** Integral is ",n
	print "*** Function is","[0]*TMath::Exp(-(x-%f)*[1])"%(x0)
	## evaluate parameters
	    

	f=ROOT.TF1("myfunc","[0]*TMath::Exp(-(x-%f)/[1])"%(x0),x0,8000)
	f.SetParameter(0,n)
	f.SetParameter(1,1)
	if True:
		#linear fit is more robust
		h2=h.Clone("myH")
		for i in range(1,h2.GetNbinsX()+1):
			x=h2.GetBinCenter(i)
			c=h2.GetBinContent(i)
			if x<x0: h2.SetBinContent(i,0)
			elif c>0: h2.SetBinContent(i, ROOT.TMath.Log( c) )
			else : h2.SetBinContent(i,0)
		f2=ROOT.TF1("myfunc2","[0]+x*[1]",x0,8000)
		h2.Fit(f2,"NQR")
		h2.Fit(f2,"NQMR")
		beta=-1./f2.GetParameter(1)
		alpha=ROOT.TMath.Exp(f2.GetParameter(0) + x0/beta)
		f.SetParameter(0,alpha)
	h.Fit(f,"NQR")
	h.Fit(f,"NQMR")
	h.Fit(f,"LNQMR")
	
	h0=h.Clone("tmp")  ### DEBUG
	for i in range(bin0,bin1+1):
		x=h.GetBinCenter(i)
		h.SetBinContent(i,f.Eval(x))
		h.SetBinError(i,h0.GetBinError(i))
		print "*** DEBUG [0]=",f.GetParameter(0),"[1]=",f.GetParameter(1),"x=",x,"y=",f.Eval(x),"y0=",h0.GetBinContent(i)
	c=ROOT.TCanvas() ### DEBUG
	c.SetLogy()
	h0.SetLineColor(ROOT.kGreen+2)
	h0.SetMarkerColor(ROOT.kGreen+2)
	h0.Draw("PE")
	h0.GetXaxis().SetRangeUser(200,1000)
	h.Draw("HIST SAME")
	h.Draw("PE SAME")
	f.SetLineColor(ROOT.kRed)
	f.Draw("L SAME")
	c.SaveAs("plot/smooth/"+re.sub('/','_',h.GetName()) + ".pdf")
	f.Delete()
	h0.Delete()
	return

def GetHistoFromFile(tfile, name):
	''' Get Histogram, Rebin, Smooth ...'''

	if tfile == None:
		print "<*> File not exists"
		raise IOError

	h = tfile.Get(name)

	if h == None:
		print "<*> Unable to find '%s' in '%s'"%(name,tfile.GetName())
		raise Exception("No Such Histogram")


	minError=-1
	for iBin in range(1,h.GetNbinsX()+1):
		c= h.GetBinContent(iBin)
		if c<=0: continue;
		e=h.GetBinError(iBin)
		if e>0 and (e<minError or minError<0): 
			minError=e

	if opts.rebin>999:
		h=Rebin(h)

	elif opts.rebin >0 :
		h.Rebin(opts.rebin)

	## assign it after rebin
	for iBin in range(1,h.GetNbinsX()+1):
		c= h.GetBinContent(iBin)
		if c<=0: continue;
		h.SetBinError(iBin,minError)

	return h

def ImportPdfFromTH1(tfile, name, target, add=[]): ## w is global as arglist_obs and argset_obs and rebin
	''' Import the pdf a th1 in tfile, with name and renaming it as target. You can add a set of (name,scale) to the base hist'''
	print "ImportPdfFromTH1 name=",name,"target=",target,"add=",add

	target=WorkspaceSubstitution(target)

	h=GetHistoFromFile(tfile,name)
	#h = tfile.Get(name)

	#if h == None:
	#	print "<*> Unable to find '%s' in '%s'"%(name,tfile.GetName())
	#	raise Exception("No Such Histogram")

	#if opts.rebin>999:
	#	h=Rebin(h)
	#elif opts.rebin >0 :
	#	h.Rebin(opts.rebin)

	for name2,c in add:
		h_tmp=GetHistoFromFile(tfile,name2).Clone(name2)
		#h_tmp=tfile.Get(name2).Clone(name2)
		#if h_tmp==None:
		#	print "<*> Unable to find '%s' in '%s': Ignoring it!"%(name2,tfile.GetTitle())
		#	continue
		#if opts.rebin>999:
		#	h_tmp=Rebin(h_tmp)
		#elif opts.rebin >0 :
		#	h_tmp.Rebin(opts.rebin)

		h_tmp.Scale(c)
		h.Add(h_tmp)

	if 'Hplus' not in name:
		Smooth(h)


	if h.Integral() <= 0.: 
		print " Integral for hist '%s' is null"%name
		print " Integral=",h.Integral()
		print "FIXME implement a negligible shape with constrain"
		return
	else:
		print "Normalization for '%s' is %e"%(name,h.Integral())

	h.SetBinContent(0,0) ##underflow
	h.SetBinContent(h.GetNbinsX()+1,0) #overflow
	
	## ---ZERO ---
	for i in range(0,h.GetNbinsX()):
		if h.GetBinContent(i+1)<0: h.SetBinContent(i+1,0)

	## save RooHistPdf
	if savePdf:
		roo_mc = ROOT.RooDataHist("hist_"+target,target,arglist_obs,h)
		pdf_mc = ROOT.RooHistPdf(target, target,argset_obs, roo_mc)
		getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
		w.factory(target + "_norm[%e]"% h.Integral())

		#if opts.qcd!="" and 'QCD' in target:
		#	print "PDF WRITING NORM=%e"%(h.Integral()),target
		#else:
		#	print "PDF WRITING NORM=%e"%(h.Integral()*opts.lumi),target
	else:
	## save RooDataHist
		if opts.qcd!="" and 'QCD' in target:
			print "HIST SCALING QCD by",opts.lumi/opts.qcdlumi,"I was",h.Integral()
			h.Scale(opts.lumi/opts.qcdlumi)
		elif opts.ewk!="" and 'ewk' in target:
			print "HIST SCALING EWK by",opts.lumi/opts.ewklumi,"I was",h.Integral()
			h.Scale(opts.lumi/opts.ewklumi)
		else:
			#print "HIST SCALING by",opts.lumi/opts.qcdlumi
			h.Scale(opts.lumi) ## I need to scale it here

		roo_mc = ROOT.RooDataHist(target,target,arglist_obs,h)
		pdf_mc = roo_mc
		getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
		#print "HIST WRITING NORM=%e"%h.Integral(),target

	g.extend([h,roo_mc,pdf_mc])

	return

def ImportPdfStatUncFromTH1(tfile, name,syst="TTSTAT", target="", add=[]): ## w is global as arglist_obs and argset_obs and rebin
	''' It is not working properly. Limits are better'''
	target=WorkspaceSubstitution(target)

	h=GetHistoFromFile(tfile,name)

	for name2,c in add:
		h_tmp=GetHistoFromFile(tfile,name2).Clone(name2)
		h_tmp.Scale(c)
		h.Add(h_tmp)

	if 'Hplus' not in name:
		Smooth(h)

	if h.Integral() <= 0.: 
		print " Integral for hist '%s' is null"%name
		print "FIXME implement a negligible shape with constrain"
		return
	else:
		print "Normalization for '%s' is %e"%(name,h.Integral())
	h.SetBinContent(0,0) ##underflow
	h.SetBinContent(h.GetNbinsX()+1,0) #overflow
	
	## ---ZERO ---
	for i in range(0,h.GetNbinsX()):
		if h.GetBinContent(i+1)<0: h.SetBinContent(i+1,0)

	hup=h.Clone(h.GetName() +"_STATUp")
	hdn=h.Clone(h.GetName() +"_STATDown")

	for i in range(0,h.GetNbinsX()):
		## one of these guys per bin
		if fullstat:
			hupbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
			hdnbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
			cont=h.GetBinContent(i+1)
			# 0 is for sure wrong in case of mc, fixed in GetHisto
			hupbin.SetBinContent(i+1, h.GetBinContent(i+1) + h.GetBinError(i+1) )
			hdnbin.SetBinContent(i+1, h.GetBinContent(i+1) + h.GetBinError(i+1) )
			
			if savePdf:
				roo_mc_binup = ROOT.RooDataHist("hist_"+target+"_Bin%d"%(i+1)+"_"+syst+"Up",target,arglist_obs,hupbin)
				pdf_mc_binup = ROOT.RooHistPdf(target+"_Bin%d"%(i+1)+"_"+syst+"Up", target+"_"+syst+"Up",argset_obs, roo_mc_binup)
				roo_mc_bindn = ROOT.RooDataHist("hist_"+target+"_Bin%d"%(i+1)+"_"+syst+"Down",target,arglist_obs,hdnbin)
				pdf_mc_bindn = ROOT.RooHistPdf(target +"_Bin%d"%(i+1)+"_"+syst+"Down", target+"_"+syst+"Down",argset_obs, roo_mc_bindn)

				getattr(w,'import')(pdf_mc_binup,ROOT.RooCmdArg())
				w.factory(target +"_Bin%d"%(i+1)+"_"+syst+"Up"+ "_norm[%e]"% hupbin.Integral())
				g.extend([hupbin,roo_mc_binup,pdf_mc_binup])

				getattr(w,'import')(pdf_mc_bindn,ROOT.RooCmdArg())
				w.factory(target +"_Bin%d"%(i+1)+"_"+syst+"Up"+ "_norm[%e]"% hdnbin.Integral())
				g.extend([hdnbin,roo_mc_bindn,pdf_mc_bindn])
			else:
				if opts.qcd!="" and 'QCD' in target:
					hupbin.Scale(opts.lumi/opts.qcdlumi)
					hdnbin.Scale(opts.lumi/opts.qcdlumi)
				elif opts.ewk!="" and 'ewk' in target:
					hupbin.Scale(opts.lumi/opts.ewklumi)
					hdnbin.Scale(opts.lumi/opts.ewklumi)
				else:
					hupbin.Scale(opts.lumi) ## I need to scale it here
					hdnbin.Scale(opts.lumi) ## I need to scale it here
				roo_mc_binup = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Up",target,arglist_obs,hupbin)
				pdf_mc_binup = roo_mc_binup
				roo_mc_bindn = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Down",target,arglist_obs,hdnbin)
				pdf_mc_bindn = roo_mc_bindn

				getattr(w,'import')(pdf_mc_binup,ROOT.RooCmdArg())
				#w.factory(target +"_Bin%d"%(i+1)+"_"+syst+"Up"+ "_norm[%e]"% hupbin.Integral())
				g.extend([hupbin,roo_mc_binup,pdf_mc_binup])

				getattr(w,'import')(pdf_mc_bindn,ROOT.RooCmdArg())
				#w.factory(target +"_Bin%d"%(i+1)+"_"+syst+"Up"+ "_norm[%e]"% hdnbin.Integral())
				g.extend([hdnbin,roo_mc_bindn,pdf_mc_bindn])


		## total
		hup.SetBinContent(i+1, h.GetBinContent(i+1) + h.GetBinError(i+1) )
		hdn.SetBinContent(i+1, h.GetBinContent(i+1) - h.GetBinError(i+1) )

	if savePdf:
		roo_mc_up = ROOT.RooDataHist("hist_"+target+"_"+syst+"Up",target,arglist_obs,hup)
		pdf_mc_up = ROOT.RooHistPdf(target+"_"+syst+"Up", target+"_"+syst+"Up",argset_obs, roo_mc_up)
		roo_mc_dn = ROOT.RooDataHist("hist_"+target+"_"+syst+"Down",target,arglist_obs,hdn)
		pdf_mc_dn = ROOT.RooHistPdf(target +"_"+syst+"Down", target+"_"+syst+"Down",argset_obs, roo_mc_dn)
		getattr(w,'import')(pdf_mc_up,ROOT.RooCmdArg())
		w.factory(target +"_"+syst+"Up"+ "_norm[%e]"% hup.Integral())
		g.extend([hup,roo_mc_up,pdf_mc_up])

		getattr(w,'import')(pdf_mc_dn,ROOT.RooCmdArg())
		w.factory(target +"_"+syst+"Up"+ "_norm[%e]"% hdn.Integral())
		g.extend([hdn,roo_mc_dn,pdf_mc_dn])
	else:
		if opts.qcd!="" and 'QCD' in target:
			hup.Scale(opts.lumi/opts.qcdlumi)
			hdn.Scale(opts.lumi/opts.qcdlumi)
		elif opts.ewk!="" and 'ewk' in target:
			hup.Scale(opts.lumi/opts.ewklumi)
			hdn.Scale(opts.lumi/opts.ewklumi)
		else:
			hup.Scale(opts.lumi) ## I need to scale it here
			hdn.Scale(opts.lumi) ## I need to scale it here
		roo_mc_up = ROOT.RooDataHist(target+"_"+syst+"Up",target,arglist_obs,hup)
		pdf_mc_up = roo_mc_up
		roo_mc_dn = ROOT.RooDataHist(target+"_"+syst+"Down",target,arglist_obs,hdn)
		pdf_mc_dn = roo_mc_dn

		getattr(w,'import')(pdf_mc_up,ROOT.RooCmdArg())
		g.extend([hup,roo_mc_up,pdf_mc_up])

		getattr(w,'import')(pdf_mc_dn,ROOT.RooCmdArg())
		g.extend([hdn,roo_mc_dn,pdf_mc_dn])

	## NORM

	return

### BKG ###
######### import mc based background contributions
systs=systsMC[:]
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
	if syst=="":
		ImportPdfStatUncFromTH1(fIn, lastget,mc+"STAT", target)

   #if syst=="":
   #	datacard.write("shapes %s *\t"%mc + opts.output)
   #	datacard.write("\tw:pdf_$CHANNEL_"+mc)
   #	datacard.write("\tw:pdf_$CHANNEL_"+mc+"_$SYSTEMATIC")
   #	datacard.write("\n")

################# Import SIGNAL CONTRIBUTIONS ##############
systs=systsMC[:]
if opts.nosyst: systs=[]
systSig=[""]
for shift in ["Up","Down"]: 
	for s in systs: 
		systSig.append(s + shift)

## print "#########################"
## print "     FIX MH POINTS       "
## print "#########################"
for syst in systSig:
 for sigMH in [ 200,250,300,350,400,500,750,1000,3000]:
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

   if fInQCD == None: 
	   print "<*> NO QCD File '%s'"%opts.qcd
	   raise IOError

   #systs=["BTAG","RFAC"] ## no JES here
   #systs=["RFAC"] ## no JES here
   systs=systsQCD[:]
   #systs=["BTAG","RFAC"]
   if opts.nosyst: systs=[]
   _systQCD_=[""]
   for shift in ["Up","Down"]: 
   	for s in systs: 
   		_systQCD_.append(s + shift)

   for syst in _systQCD_:
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
	print "*** Considering QCD","syst=",syst,"target=",target,"addlist=",addlist
	ImportPdfFromTH1(fInQCD, lastget,target ,addlist)
	if syst=="":
		ImportPdfStatUncFromTH1(fInQCD, lastget,"QCDSTAT", target,addlist)

   datacard.write("shapes QCD *\t"+opts.output)
   datacard.write("\tw:"+"pdf_inviso_$CHANNEL_QCD" )
   datacard.write("\tw:"+"pdf_inviso_$CHANNEL_QCD_$SYSTEMATIC" )
   datacard.write("\n")

if opts.ewk!="":
   fInEWK=ROOT.TFile.Open(opts.ewk,"READ")

   if fInEWK == None: 
	   print "<*> NO EWK File '%s'"%opts.ewk
	   raise IOError

   systs=systsEWK[:]
   if opts.nosyst: systs=[]
   _systEWK_=[""]
   for shift in ["Up","Down"]: 
   	for s in systs: 
   		_systEWK_.append(s + shift)

   for syst in _systEWK_:
    for cat in range(0,opts.ncat):
	if opts.ncat == 1:
		lastget="ChargedHiggsEWKPurity/Vars/Mt_Embed_Data"
	else:
		lastget="ChargedHiggsEWKPurity/Vars/Mt_Embed_cat%d_Data"%cat

	if syst !="":lastget+="_"+syst

	if syst == "": systName=""
	else: systName="_"+syst

	addlist=[]
	target="pdf_cat%d_ewkembed"%cat + systName
	print "*** Considering EWK","syst=",syst,"target=",target,"addlist=",addlist
	ImportPdfFromTH1(fInEWK, lastget,target ,addlist)
	if syst=="":
		ImportPdfStatUncFromTH1(fInEWK, lastget,"EWKSTAT", target,addlist)

   datacard.write("shapes EWK *\t"+opts.output)
   datacard.write("\tw:"+"pdf_$CHANNEL_ewkembed" )
   datacard.write("\tw:"+"pdf_$CHANNEL_ewkembed_$SYSTEMATIC" )
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
if opts.ewk != "" : mcAll.append("EWK")

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
		if savePdf:
			datacard.write("\t%.2f"%(opts.lumi/opts.qcdlumi)) ## lumi factor 5000/2110, should be 1.00 FIXME
		else:
			datacard.write("\t-1")
	elif proc=="EWK" and opts.ewk!="":
		if savePdf:
			datacard.write("\t%.2f"%(opts.lumi/opts.ewklumi) ) ## lumi factor 5000/2110, should be 1.00 FIXME
		else:
			datacard.write("\t-1")

	else:
		if savePdf:
			datacard.write("\t%.0f"%(opts.lumi) )
		else:
			datacard.write("\t-1")
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

def writeNormSystList(name="lumi",valueL=["1.027","1.026"], regexpL=["TT","ST"]):
	########## LUMI ###
	datacard.write(name+"\tlnN")
	invert=False

	for cat in range(0,opts.ncat):
	   for proc in mcAll:
		idx=-1
		invert=False
		for i,regexp in enumerate(regexpL):
			match=re.search(regexp,proc)
			if regexp != "" and regexp[0] == '!':
				invert=True
				regexp=regexp[1:]
			if (match and not invert) or (not match and invert): 
				idx=i
				continue

		if (idx>=0):
		   datacard.write("\t"+valueL[idx])
		else:
		   datacard.write("\t-")
	datacard.write("\n")

if opts.qcd=="":
	#writeNormSyst("lumi_13TeV","1.027","")
	writeNormSyst("lumi_13TeV","1.023","")
	writeNormSyst("lumi_13TeV_stability","1.015","")
else:
	#writeNormSyst("lumi_13TeV","1.027","!QCD")
	writeNormSyst("lumi_13TeV","1.023","!QCD")
	writeNormSyst("lumi_13TeV_stability","1.015","!QCD")

if opts.nosyst: 
	w.writeToFile(opts.output)
	print " --- DONE --- "
	exit(0)

def writeSyst(syst="JES"):
	systOrig=syst[:]
	syst=WorkspaceSubstitution(syst)
	datacard.write(syst+" shape")
	for cat in range(0,opts.ncat):
	   for proc in mcAll:
		if 'STAT' in systOrig:
			if proc in re.sub('STAT','',systOrig):
	        		datacard.write("\t1")
			else:
	        		datacard.write("\t-")
		elif proc=="QCD":
			#if opts.qcd !="" and systOrig + "Up"  in _systQCD_:
			if opts.qcd !="" and systOrig  in systsQCD:
	        		datacard.write("\t1")
			else: datacard.write("\t-")
		elif proc == "EWK" and opts.ewk != "":
			if systOrig in systsEWK:
	        		datacard.write("\t1")
			else: datacard.write("\t-")

		elif proc in mcList :
			if systOrig + "Up" in systBkg: ## Change to systsMC TODO
	        		datacard.write("\t1")
			else:
	        		datacard.write("\t-")
		elif proc == "Hplus" :
			if systOrig + "Up" in systSig:
	        		datacard.write("\t1")
			else:
	        		datacard.write("\t-")
		else:
			print "DONT KNOW WHAT TO DO WITH proc=",proc,"syst=",syst,"systOrig=",systOrig
	datacard.write("\n")

allSyst=set(systsMC) 
if opts.qcd!="":
	allSyst |= set(systsQCD)
if opts.ewk!="":
	allSyst |= set(systsEWK)

for x in allSyst:
	writeSyst(x)
### write shape syst
#writeSyst('JES')
#writeSyst('JER')
#writeSyst('TAU')
#writeSyst('BTAG')
#writeSyst('RFAC')
#writeSyst('TRIG')
#writeSyst('TRIGMET')
#writeSyst('TAUHIGHPT')
#writeSyst('TAUSCALE')
#writeSyst('TAUANTIE')
#writeSyst('ELEVETO')
#writeSyst('MUVETO')
#writeSyst('UNCLUSTER')
#writeSyst('PU')

#if opts.ewk !="" :
#	writeSyst('MUEFF')
#	writeSyst('MURECOEFF')
##
## write STAT syst
for mc in mcAll:
 	if 'Hplus' in mc: continue
	#not full stat
	#fullstat
	if fullstat:
		for i in range(0, g[0].GetNbinsX() ): ## mt data
			writeSyst("Bin%d"%(i+1)+"_"+mc+'STAT')
	else:
 		writeSyst(mc+'STAT')

## write norm syst ### FIXME
writeNormSystList("CMS_scale_ttbar",["0.965/1.024","0.977/1.028"],["TT","ST"])
writeNormSystList("CMS_pdf_ttbar",["1.042","1.026"],["TT","ST"])
writeNormSystList("CMS_mass_ttbar",["1.027","1.022"],["TT","ST"])
###

## UNCORRELATED
#writeNormSyst("CMS_scale_DY","0.9963/1.0065","DY")
#writeNormSyst("CMS_pdf_DY","1.037","DY")

#writeNormSyst("CMS_scale_Wjets","0.996/1.008","WJets")
#writeNormSyst("CMS_pdf_Wjets","1.0375","WJets")

## CORRELATED
writeNormSystList("CMS_scale_V",["0.9963/1.0065","0.996/1.008"],["DY","WJets"])
writeNormSystList("CMS_pdf_V",["1.037","1.0375"],["DY","WJets"])

## UNCORRELATED
#writeNormSyst("CMS_scale_ww","1.025","WW")
#writeNormSyst("CMS_pdf_ww","1.022","WW")
#
#writeNormSyst("CMS_scale_wz","1.032","WZ")
#writeNormSyst("CMS_pdf_wz","1.044","WZ")
#
#writeNormSyst("CMS_scale_zz","1.031","ZZ")
#writeNormSyst("CMS_pdf_zz","1.037","ZZ")

## CORRELATED
writeNormSystList("CMS_scale_VV",["1.025","1.032","1.031"],["WW","WZ","ZZ"])
writeNormSystList("CMS_pdf_VV",["1.022","1.044","1.037"],["WW","WZ","ZZ"])

w.writeToFile(opts.output)

print "--------------------" 
print "savePdf=",savePdf
print "fullstat=",fullstat
print "tailfit=",tailfit
print "datacard=",datName
print "ws=",opts.output
print " --- DONE --- "
