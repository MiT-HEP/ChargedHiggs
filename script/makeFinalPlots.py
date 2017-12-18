import os,sys
from optparse import OptionParser, OptionGroup

parser=OptionParser()
#parser.add_option("-f","--file",help="Input file. combine binary datacard [%default]",default="cms_datacard_ws_qcddata_Augustv4_M200.root")
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="ChHiggs.root")
parser.add_option("","--qcd",type='string',help="Input ROOT file. Set Null to use QCD from above [%default]", default="QCDPurity.root")
parser.add_option("-o","--output",type='string',help="Output PDF File. [%default]", default="plot/mt.pdf")
parser.add_option("-L","--lumi",type='float',help="Luminosity pb. [%default]", default=2308)
parser.add_option("","--qcdlumi",type='float',help="QCD Luminosity pb. [%default]", default=2308)
parser.add_option("","--ewk",type='string',help="Input ROOT file for ewk. Set Null to use EWK from MC [%default]", default="")
parser.add_option("","--ewklumi",type='float',help="EWK Luminosity pb. [%default]", default=2308)
parser.add_option("-b","--batch",action='store_true',help="Batch [%default]",default=True)
parser.add_option("-x","--no-batch",dest='batch',action='store_false',help="non batch")
parser.add_option("-v","--var",dest='var',type="string",help="variable [%default]",default="Mt")
parser.add_option("-d","--dir",dest='dir',type="string",help="directory [%default]",default="Vars")

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. if >1000 variable bin [%default]", default=1000)

opts,args= parser.parse_args()

sys.argv=[]
import ROOT
from array import array
import re

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

if opts.batch: 
    print "* entering batch mode"
    ROOT.gROOT.SetBatch()

print "inserting in path cwd/script"
sys.path.insert(0,os.getcwd()+'/script')
print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

def Smooth(h):
	''' Smooth out the tail of the mT distribution'''
	#return;### It's not working ... need to figure out the range dynamically
	### wjets has some problem
	if not tailfit:
		return  ## NO SMOOTH

	if True and \
	   'WJets' not in h.GetName() and \
	   'TT' not in h.GetName() and \
	   'IsoInv_Data' not in h.GetName() : 
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

def Blind(h,mt=80):
	bin=h.FindBin(mt)
	for b in range(bin,h.GetNbinsX()+1):
		h.SetBinContent(b,0)
		h.SetBinError(b,1)
	return h

def Normalize(h):
	if opts.qcd!="" and 'QCD' in h.GetName():
		print "Scaling QCD by",opts.lumi/opts.qcdlumi,"(",h.GetName(),")"
		h.Scale(opts.lumi/opts.qcdlumi)
	elif opts.ewk!="" and 'ewk' in h.GetName():
		print "Scaling EWK by",opts.lumi/opts.ewklumi
		h.Scale(opts.lumi/opts.ewklumi)
	else:
		print "Scaling MC",h.GetName(),"by",opts.lumi
		h.Scale(opts.lumi) ## I need to scale it here
	return h

def Envelope(h,list):
	for h2 in list:
	   for b in range(1,h.GetNbinsX()+2):
		   c=h.GetBinContent(b)
		   e=h.GetBinError(b)

		   c2=h2.GetBinContent(b)
		   changed=False
		   low=0
		   high=0
		   if c2>c+e:
			   low=c-e
			   high=c2
			   changed=True
		   elif c2<c-e:
			   low=c2
			   high=c+e
			   changed=True
		   if changed:
			   h.SetBinContent(b, (low+high)/2.)
			   h.SetBinError(b, (high-low)/2.)
	return h

import math
def SqrAdd(h,h1,h2):
	''' h1/h2 is an envelope'''
	for b in range(1,h.GetNbinsX()+2):
		c=h.GetBinContent(b)
		c1=h1.GetBinContent(b)
		e1=h1.GetBinError(b)
		c2=h2.GetBinContent(b)
		e2=h2.GetBinError(b)

		elow= math.sqrt(max(c-(c1-e1),0)**2 + max(c-(c2-e2),0)**2)
		ehigh= math.sqrt( max(c1+e1-c,0)**2 + max(c2+e2-c,0)**2 )

		high=c+ehigh
		low=c-elow

		if (b==1 ):
			print "---------- SQR ADD -------------"
			print "h=",h.GetName()
			print "h1=",h1.GetName()
			print "h2=",h2.GetName()
			print "c=",c
			print "c1=",c1,"e1=",e1
			print "c2=",c2,"e2=",e2
			print "elow=",elow,"ehigh=",ehigh
			print "high=",high,"low=",low
			print "________________________________"

		h1.SetBinContent(b, (high+low)/2.)
		h1.SetBinError(b, (high-low)/2.)

def Rebin(h):
	''' Rebin with un-even bins '''
	mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
	#h1 = ROOT.TH1D(h.GetName()+"_rebin",h.GetTitle(),len(mybins)-1,mybins)
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1


mcList=[]
if opts.qcd == "" : mcList.append("QCD")
if opts.ewk == "" : mcList.extend(["DY","TT","ST","WW","WZ","ZZ","WJets"]) 

#from buildworkspace import systsMC,systsQCD,systsEWK,tailfit,Rebin,Smooth, GetHistoFromFile
tailfit=False
systsMC=["BTAGB","BTAGL","JES","TAU","TRIGMET","TAUHIGHPT","TAUSCALE","ELEVETO","MUVETO","JER","UNCLUSTER","PU","TOPRW","TRIG"]
systsQCD=["RFAC1p","RFAC3p"]
systsEWK=["TRIGMET","TRIG","MUEFF","MURECOEFF","TAU","TAUHIGHPT"]

print "--> NO SYST <--"
systsMC=[]
systsQCD=[]
systsEWK=[]
systsNorm={"CMS_scale_ttbar":(["0.965/1.024","0.977/1.028"],["TT","ST"]),
 	   "CMS_pdf_ttbar": (["1.042","1.026"],["TT","ST"]),
 	   "CMS_mass_ttbar":(["1.027","1.022"],["TT","ST"]),
 	   "CMS_scale_V":(["0.9963/1.0065","0.996/1.008"],["DY","WJets"]),
 	   "CMS_pdf_V":(["1.037","1.0375"],["DY","WJets"]) ,
 	   "CMS_scale_VV":(["1.025","1.032","1.031"],["WW","WZ","ZZ"]),
 	   "CMS_pdf_VV":(["1.022","1.044","1.037"],["WW","WZ","ZZ"]) 
}
##  systsNorm={}


## get files
fIn = ROOT.TFile.Open(opts.input,"READ")
if fIn == None: 
	   print "<*> NO File '%s'"%opts.input
	   raise IOError

if opts.qcd != "":
   fInQCD=ROOT.TFile.Open(opts.qcd,"READ")

   if fInQCD == None: 
	   print "<*> NO QCD File '%s'"%opts.qcd
	   raise IOError

if opts.ewk != "":
   fInEWK=ROOT.TFile.Open(opts.ewk,"READ")

   if fInEWK == None: 
	   print "<*> NO EWK File '%s'"%opts.ewk
	   raise IOError

#################
### Get Data  ###
#################
basedir = "ChargedHiggsTauNu/"+opts.dir+"/"

lastget=basedir+opts.var+"_Data"
h_data = fIn.Get( lastget)
if opts.rebin>999:
	h_data=Rebin(h_data)
elif opts.rebin >0 :
	h_data.Rebin(opts.rebin)

if opts.var == "Mt":
	Blind(h_data)

#################
### Get BKG   ###
#################
histos={}

### MC BASE BKG
systAll=[""]
#for shift in ["Up","Down"]: 
systs=systsMC[:]
for s in systs: 
	if s not in systAll:
		systAll.append(s )


systs=systsQCD[:]
if opts.qcd != "":
   for s in systs: 
	if s not in systAll:
   		systAll.append(s )

systs=systsEWK[:]
if opts.qcd != "":
   for s in systs: 
	if s not in systAll:
   		systAll.append(s )

for key in systsNorm:
	systAll.append(key)

h_bkg=h_data.Clone("Bkg_nominal")
h_bkg.Reset("ACE")
h_bkg_env=None
h_bkg_err=None


print "systAll:",systAll

for syst in systAll:
  print "----------------- DOING",syst,"----------------"
  h_bkg_syst_up=h_bkg.Clone("Bkg_syst_"+syst+"Up")
  h_bkg_syst_up.Reset("ACE")
  h_bkg_syst_dn=h_bkg.Clone("Bkg_syst_"+syst+"Down")
  h_bkg_syst_dn.Reset("ACE")

  for mc in mcList:
    print "*) MC:",mc
    isNormForMC=False
    # systsNorm= key : [norm,] [match]
    if syst in systsNorm:
       for match in systsNorm[syst][1]:
    	if match in mc:
    		isNormForMC=True
    
    if syst=="" or (syst not in systsMC and not isNormForMC):
    	lastget=basedir+opts.var+"_"+ mc
    	h_mc=GetHistoFromFile(fIn,lastget)
    	if 'Hplus' not in mc:
    		Smooth(h_mc)
    	else:
    		print "SIGNAL?!? WHY?!?"
    
    	Normalize(h_mc)
    	if syst=="":
    		print "-> Adding",mc,"to bkg",
    		h_bkg.Add(h_mc)
    		print " * integral now is",h_bkg.Integral()
    		histos[mc] = h_mc ## save the nominal point
    	else:
    		h_bkg_syst_up.Add(h_mc)
    		h_bkg_syst_dn.Add(h_mc)
    
    elif isNormForMC:
    	for idx in range(0,len(systsNorm[syst][0])):
    		match=systsNorm[syst][1][idx]
    		scalestr=systsNorm[syst][0][idx]
    		if match in mc:
    			#print "INITEGRAL For MC=",mc,"=",histos[mc].Integral()
    			h_mc_up= histos[mc].Clone(mc+"_"+syst+"Up")
    			h_mc_dn= histos[mc].Clone(mc+"_"+syst+"Down")
    			if '/' in scalestr: ## '0.98/1.1'
    				up=float(scalestr.split('/')[1])
    				down=float(scalestr.split('/')[0])
    				print "NORM syst=",syst,"MC=",mc, "scaling up by",up,"and down by",down
    				h_mc_up.Scale(up)
    				h_mc_dn.Scale(down)
    			else:
    				up=float(scalestr)
    				print "NORM syst=",syst,"MC=",mc, "scaling up by",up,"and down by",1./up
    				h_mc_up.Scale(up)
    				h_mc_dn.Scale(1./up)
    	h_bkg_syst_up.Add(h_mc_up)
    	h_bkg_syst_dn.Add(h_mc_dn)
    else:
    	lastget=basedir+opts.var+"_"+ mc + "_" + syst +"Up"
    	h_mc_up=GetHistoFromFile(fIn,lastget)
    	if 'Hplus' not in lastget:
    		Smooth(h_mc_up)
    	Normalize(h_mc_up)
    	h_bkg_syst_up.Add(h_mc_up)
    
    	lastget=basedir+opts.var+"_"+ mc + "_" + syst +"Down"
    	h_mc_dn=GetHistoFromFile(fIn,lastget)
    	if 'Hplus' not in lastget:
    		Smooth(h_mc_dn)
    	Normalize(h_mc_dn)
    	h_bkg_syst_dn.Add(h_mc_dn)

    if opts.qcd!="" :
        if syst=="" or syst not in systsQCD:
        	lastget="ChargedHiggsQCDPurity/"+opts.dir+"/"+opts.var+"IsoInv_Data"
        	addlist=[]
        	for bkg in ['WJets','TT','WW','WZ','ZZ','DY','ST']:
        		addlist.append( (re.sub('Data',bkg,lastget),-opts.qcdlumi) ) #
        
        	h_qcd=GetHistoFromFile(fInQCD,lastget)
        	for name2,c in addlist:
        		h_tmp=GetHistoFromFile(fInQCD,name2).Clone(name2)
        		h_tmp.Scale(c)
        		h_qcd.Add(h_tmp)
        
        	Normalize(h_qcd) ## scale for qcd lumi and lumi
        
        	if syst=="":
        		print "-> Adding QCD to bkg",
        		h_bkg.Add(h_qcd)
        		print " * integral now is",h_bkg.Integral()
        		histos['QCD'] = h_qcd ## save the nominal point
        	else:
        		h_bkg_syst_up.Add(h_qcd)
        		h_bkg_syst_dn.Add(h_qcd)

    else:
    	lastget="ChargedHiggsQCDPurity/"+opts.dir+"/"+opts.var+"IsoInv_Data"
    	lastget+="_"+syst
    	lastget+="Up"
    	addlist=[]
    	for bkg in ['WJets','TT','WW','WZ','ZZ','DY','ST']:
    		addlist.append( (re.sub('Data',bkg,lastget),-opts.qcdlumi) ) #
    
    	h_qcd_up=GetHistoFromFile(fInQCD,lastget)
    	for name2,c in addlist:
    		h_tmp=GetHistoFromFile(fInQCD,name2).Clone(name2)
    		h_tmp.Scale(c)
    		h_qcd_up.Add(h_tmp)
    
    	Normalize(h_qcd_up)
    	h_bkg_syst_up.Add(h_qcd_up)
    
    	lastget="ChargedHiggsQCDPurity/"+opts.dir+"/"+opts.var+"IsoInv_Data"
    	lastget+="_"+syst
    	lastget+="Down"
    	addlist=[]
    	for bkg in ['WJets','TT','WW','WZ','ZZ','DY','ST']:
    		addlist.append( (re.sub('Data',bkg,lastget),-opts.qcdlumi) ) #
    
    	h_qcd_dn=GetHistoFromFile(fInQCD,lastget)
    	for name2,c in addlist:
    		h_tmp=GetHistoFromFile(fInQCD,name2).Clone(name2)
    		h_tmp.Scale(c)
    		h_qcd_dn.Add(h_tmp)
    
    	Normalize(h_qcd_dn)
    	h_bkg_syst_dn.Add(h_qcd_dn)

    if opts.ewk!="":
      print "TODO"
      raise ValueError("TO implement")
      #env=histos[mc].Clone("env_"+mc+"_"+syst)
      #Envelope(env,[h_mc_up,h_mc_dn])

      #if syst !="":lastget+="_"+syst
      #h_mc=fIn.Get(lastget)
    if syst=="":
        h_bkg_err=h_bkg.Clone("Bkg_err") ## stat error
    else:
        h_bkg_env=h_bkg.Clone("Bkg_env_" + syst)
        Envelope(h_bkg_env,[h_bkg_syst_up,h_bkg_syst_dn])
        SqrAdd(h_bkg,h_bkg_err,h_bkg_env) ## add to h_bkg_err, the error of h_bkg_env, considering the midpoint is h_bkg


h_bkg.SetLineColor(ROOT.kBlack)
h_bkg.SetLineWidth(2)
h_bkg_err.SetFillColor(ROOT.kBlack)
h_bkg_err.SetMarkerColor(ROOT.kBlack)
h_bkg_err.SetFillStyle(3004)

h_qcd=histos["QCD"]
h_qcd.SetLineColor(ROOT.kRed)
h_qcd.SetLineWidth(2)
h_qcd.SetLineStyle(7)

h_data.SetMarkerStyle(20)
h_data.SetMarkerColor(ROOT.kBlack)
h_data.SetLineColor(ROOT.kBlack)


st=ROOT.THStack()
st.SetName("stack")

leg=ROOT.TLegend(.65,.61,.92,.93)
leg.SetBorderSize(0)
leg.SetFillStyle(0)

drawList=["WW","WZ","ZZ","ST","DY","TT","WJets","QCD"]

leg.AddEntry(h_data,"Data","PE")

def IntegrateStack(ts):
    S=0.
    l=ts.GetHists()
    for i in range(0,l.GetSize()):
        S+=l.At(i).Integral()
    return S

for mc in drawList:
    #["DY","TT","ST","WW","WZ","ZZ","WJets"]
    histos[mc].SetLineColor(ROOT.kBlack)
    histos[mc].SetLineWidth(1)
    histos[mc].SetLineStyle(1)
    
    if mc=="DY": 
        histos[mc].SetFillColor(ROOT.kTeal-9)
    if mc=="TT":
        histos[mc].SetFillColor(ROOT.kMagenta+2)
    if mc=="ST":
        histos[mc].SetFillColor(ROOT.kGreen+2)
    if mc=="WW" or mc=="WZ":
        histos[mc].SetFillColor(ROOT.kBlue)
        histos[mc].SetLineColor(ROOT.kBlue)
    if mc== "WJets":
        histos[mc].SetFillColor(ROOT.kRed+1)
    if mc=="ZZ":
        histos[mc].SetFillColor(ROOT.kBlue)
    if mc=="QCD":
        histos[mc].SetFillColor(ROOT.kOrange)
    
    st.Add(histos[mc])
    
    print "->Adding",mc,"to stack","integral is now",IntegrateStack(st)

drawList.reverse()
for mc in drawList:
    if mc=="DY": 
        leg.AddEntry(histos[mc],"Z/#gamma*+jets","F")
    if mc=="TT":
        leg.AddEntry(histos[mc],"t#bar{t}","F")
    if mc=="ST":
        leg.AddEntry(histos[mc],"Single top quark","F")
    if mc=="ZZ":
        leg.AddEntry(histos[mc],"Diboson","F")
    if mc=="QCD":
        leg.AddEntry(histos[mc],"mis-ID #tau^{h}","F")
    if mc== "WJets":
        leg.AddEntry(histos[mc],"W+jets","F")

print "-> start drawing"
c=ROOT.TCanvas("c","c",800,800)
c.SetTopMargin(0.05)
c.SetRightMargin(0.05)
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
#c.SetLogy()
#c.SetLogx()

st.Draw("HIST")


for mc in drawList:
    print mc," BIN1=",histos[mc].GetBinContent(1)
print "BKG",h_bkg.GetBinContent(1)

#h_data.Draw("AXIS SAME")
if opts.var == "Mt":
    st.GetXaxis().SetTitle("m_{T} [GeV]")
elif opts.var == "EtMiss":
    st.GetXaxis().SetTitle("E_{T}^{miss} [GeV]")
else:
    st.GetXaxis().SetTitle(opts.var)

st.GetXaxis().SetTitleOffset(1.4)
st.GetXaxis().SetRangeUser(0,500)
st.GetYaxis().SetRangeUser(0,1000)
st.SetMaximum(850)
st.GetYaxis().SetTitle("Events")
st.GetYaxis().SetTitleOffset(1.4)

h_bkg_err.Draw("E2 SAME")
h_bkg.Draw("HIST SAME")
#h_qcd.Draw("HIST SAME")

h_data.Draw("PE SAME")
h_data.Draw("AXIS SAME")
h_data.Draw("AXIS SAME X+ Y+")


leg.Draw()

txt=ROOT.TLatex()
txt.SetNDC()
txt.SetTextFont(43)
txt.SetTextSize(20)
txt.SetTextAlign(31)
txt.DrawLatex(.95,.96,"%.1f fb^{-1} (13 TeV)"%(float(opts.lumi/1000.)))

txt.SetTextSize(30)
txt.SetTextAlign(13)
txt.DrawLatex(.16,.93,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")

c.Modify()
c.Update()

if not opts.batch:
    raw_input("ok?")

c.SaveAs(opts.output)

if '.pdf' in opts.output:
    c.SaveAs(re.sub('.pdf','.png',opts.output))
    c.SaveAs(re.sub('.pdf','.root',opts.output))

