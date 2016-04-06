import os,sys
from optparse import OptionParser
import re

parser= OptionParser()
parser.add_option("-b","--batch",dest="batch",action="store_true",help="Run in Batch mode",default=False);
#parser.add_option("-L","--lumi",dest="lumi",type='float',help="Luminosity. [%default]",default=2215);
parser.add_option("","--base",dest="base",type='string',help="Base Directory to fetch the plots. [%default]",default='QGAnalysis/Vars');
parser.add_option("","--mc",dest="mc",type='string',help="MC to Draw. [%default]",default="DY");
parser.add_option("-f","--file",dest="file",type='string',help="file. [%default]",default="test/mysub/QG/QG.root");
#parser.add_option("","--rebin",dest="rebin",type='int',help="rebin. [%default]",default=1);
parser.add_option("","--plot",dest="plot",type='string',help="plot dir. [%default]",default="plot/qg");
parser.add_option("","--extensions",dest="exts",type='string',help="extensions. [%default]",default="pdf,png,root");

opts, args = parser.parse_args()

sys.argv=[]

import ROOT
if opts.batch:
        ROOT.gROOT.SetBatch()

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

## mkdir -- mkdirs = mkdir -p
try:
  os.makedirs(opts.plot)
except OSError: pass


garbage=[]

class Plot:
	def __init__(self):
		self.dict_={}
		self.canv_=None
		self.lumi_=-1
		self.extra_="Preliminary"
		self.current_=0; ## iterator
		self.legpos=(0.65,.60,.93,.85)
		self.legheader=""

	def __contains__(self, key): 
		return key in self.dict_

	def getitem(self,key):
		return self.dict_[key]
	
	def __getitem__(self,key):
		return self.dict_[key]

	def __setitem__(self,key,value):
		self.dict_[key] = value
		## raise TypeError KeyError

	def __iter__(self):
		return self

	def next( self ):
		self.current_ +=1
		if self.current_ >= len(self) : 
			self.current_ = 0
			raise StopIteration
		return self.dict_.items()[ self.current_ -1 ]  ## this is a tuple

	def __len__(self):
		return len(self.dict_)
	#def delitem(self,x):

	def SetLumi(self,lumi):
		self.lumi_ = lumi

	## plot stuff
	def PrepareCanvas(self,name="c"):
	        self.canv_=ROOT.TCanvas(name,name,800,800)
	        self.canv_.SetTopMargin(0.05)
	        self.canv_.SetRightMargin(0.05)
	        self.canv_.SetLeftMargin(0.15)
	        self.canv_.SetBottomMargin(0.15)

	def PrepareTwoPads(self):
		self.pup=ROOT.TPad("_pad1" ,"pad1", 0,0.25, 1 ,1)
		self.pdn=ROOT.TPad("_pad2" ,"pad2", 0,0, 1 ,.25)
		
		self.pup.SetTopMargin(0.05)
		self.pup.SetBottomMargin(0.15)
		self.pup.SetLeftMargin(0.15)
		self.pup.SetRightMargin(0.05)
		self.pdn.SetTopMargin(0.05)
		self.pdn.SetBottomMargin(0.40)
		self.pdn.SetLeftMargin(0.15)
		self.pdn.SetRightMargin(0.05)
		self.pup.Draw()
		self.pdn.Draw()

	def cd(self,where="canvas"):
		if where == "canvas": self.canv_.cd()
		if where == "up" : self.pup.cd()
		if where == "down" : self.pdn.cd()

	def Update(self):
		self.canv_.Update()

	def SaveAs(self,name):
		self.canv_.SaveAs(name)

	def CMS(self):
		self.canv_.cd()
	        ltx=ROOT.TLatex()
	        ltx.SetNDC()
	        ltx.SetTextFont(43)
	        ltx.SetTextSize(28)
	        ltx.SetTextAlign(31)
		if self.lumi_<0 : ltx.DrawLatex(.94,.96,"13 TeV")
		elif self.lumi_ > 1000: ltx.DrawLatex(.94,.96,"%.1f fb^{-1} (13 TeV)" % (self.lumi_/1000.))  
		else : ltx.DrawLatex(.94,.96,"%.1f pb^{-1} (13 TeV)" % (self.lumi_)) 
	        ltx.SetTextAlign(13)
	        ltx.SetTextSize(32)
	        ltx.SetTextAlign(33)
	        ltx.DrawLatex(.87,.92,"#bf{CMS}#scale[0.75]{#it{ %s}}"%self.extra_) #
		garbage.append(ltx)

	def Legend(self):
		self.canv_.cd()
		l = ROOT.TLegend(self.legpos[0],self.legpos[1],self.legpos[2],self.legpos[3])
		l.SetHeader(self.legheader)
		l.AddEntry(self.dict_["Data"],"Data","PE")
		if 'DY' in self.dict_: l.AddEntry(self.dict_["DY"],"DY","F")
		if 'TT' in self.dict_: l.AddEntry(self.dict_["TT"],"TT","F")
		if 'WJets' in self.dict_: l.AddEntry(self.dict_["WJets"],"WJets","F")
		if 'WW' in self.dict_: l.AddEntry(self.dict_["WW"],"EWK","F")
		if 'QCD' in self.dict_: l.AddEntry(self.dict_["QCD"],"QCD","F")

		# dictionaries are not sorted
		toadd = {}
		for name in self.dict_:
			if 'Data' in name: continue
			if '_Q_' in name: toadd['Q']=name
			if '_G_' in name: toadd['G']=name
			if '_U_' in name: toadd['U']=name
		if 'Q' in toadd: l.AddEntry(self.dict_[toadd['Q']],"quark","F")
		if 'G' in toadd: l.AddEntry(self.dict_[toadd['G']],"gluon","F")
		if 'U' in toadd: l.AddEntry(self.dict_[toadd['U']],"pile-up","F")

		l.Draw()
		l.SetBorderSize(0)
		l.SetFillStyle(0)
		garbage.append(l)

	def Range(self,target="Data"):
		## to do.. something good
		for name in self.dict_:
			if 'mult' in name:
				self.dict_[target].GetXaxis().SetRangeUser(0,50)
				self.dict_[target].GetXaxis().SetTitle("Multiplicity")
			elif 'ptD' in name or 'ptd' in name or 'PtD' in name:
				self.dict_[target].GetXaxis().SetRangeUser(0,1.0)
				self.dict_[target].GetXaxis().SetTitle("p_{T}#it{D}")
				self.legpos=(0.15,.60,.45,.85)
			elif 'QGL' in name:
				self.dict_[target].GetXaxis().SetRangeUser(0,1.0)
				self.legpos=(0.35,.60,.65,.85)
				self.dict_[target].GetXaxis().SetTitle("QGL (76X)")
			elif 'axis2' in name:
				self.dict_[target].GetXaxis().SetRangeUser(0,10)
				self.dict_[target].GetXaxis().SetTitle("-log(#sigma_{2})")

			# figure out header
			if 'mult' in name or 'ptD' in name or 'QGL' in name or 'axis2' in name:
				parts=name.split('_')
				header=""
				if len(parts) >= 4 and 'pt' in parts[2]:
					ptmin=re.sub('pt','',parts[2])
					ptmax=parts[3]
					header += "#splitline{" + ptmin+"<p_{T}[GeV]<" + ptmax + "}{"
				if len(parts) >= 6 and 'eta' in parts[4]:
					etamin=re.sub('eta','',parts[4])
					etamax=parts[5]
					header += " "+etamin+"<#||{#eta}<" + etamax
					header += "}"
				self.legheader = header
			## Rebin TODO, mv in a rebin macro. to make sure is called only once
			if 'QGL_pt30_50_eta0.0_2.0' in name: self.dict_[name].Rebin(2)
			elif 'QGL' in name and 'eta3.0_4.7' in name: self.dict_[name].Rebin(5)
			elif 'QGL_pt30_50_eta2.0_2.5' in name: self.dict_[name].Rebin(5)
			elif 'QGL_pt30_50_eta2.5_3.0' in name: self.dict_[name].Rebin(5)
			elif 'QGL_pt30_50' in name: self.dict_[name].Rebin(5)
			elif 'QGL' in name: self.dict_[name].Rebin(2)
			elif 'pt250_500' in name: self.dict_[name].Rebin(5)
			elif 'pt120_250' in name: self.dict_[name].Rebin(5)
			elif 'pt80_120_eta0.0_2.0' in name: self.dict_[name].Rebin(2)
			elif 'pt80_120_eta2.5_3.0' in name: self.dict_[name].Rebin(5)
			elif 'pt80_120_eta2.0_2.5' in name: self.dict_[name].Rebin(2)
			elif 'pt50_80_eta2.5_3.0' in name: self.dict_[name].Rebin(2)
			elif 'pt50_80_eta2.0_2.5' in name: self.dict_[name].Rebin(2)
			elif 'pt50_80_eta0.0_2.0' in name: self.dict_[name].Rebin(2)
			elif 'pt30_50_eta2.5_3.0' in name: self.dict_[name].Rebin(2)
			elif 'pt30_50_eta2.0_2.5' in name: self.dict_[name].Rebin(2)
			elif 'eta3.0_4.7' in name: self.dict_[name].Rebin(5)


## can I fetch these automatically?
ptBins=[30.,50.,80.,120.,250.,500.,8000.]
etaBins=[0.,2.,2.5,3.,4.7]
#jetTypes=["Q","G","U"]
jetTypes=["U","G","Q"]
jetVars=["QGL","mult","ptD","axis2"]

#loop over bins
# KEY: TH1D      ptD_U_pt80_120_eta0.0_2.0_DY;1  ptD_U
# KEY: TH1D      ptD_U_pt80_120_eta0.0_2.0_Data;1

fIn = ROOT.TFile.Open(opts.file)

if fIn== None:
	print "ERROR: File ",opts.file,'does not exist'

for var in jetVars:
 for ptbin in range(0, len(ptBins)-1):
  for etabin in range(0, len(etaBins)-1):
    try:
	# fetch histograms
	plot=Plot()
        for t in jetTypes:
	  for what in ["Data"]+ opts.mc.split(","):
	     if what == "Data" and t!="U": continue # data is only U, no MC matching
	     name= "_".join([var,t, "pt%.0f"%(ptBins[ptbin]) , "%.0f"%(ptBins[ptbin+1]), "eta%.1f"%(etaBins[etabin]),"%.1f"%(etaBins[etabin+1]), what ])
	     h=fIn.Get(opts.base + "/" + name)
	     if h==None: print "-> fetch of histos",name,"in base",opts.base,"FAILED!"
	     plot[name] = h
	     if what == "Data" and t=="U" : plot["Data"] = h ## short link, in Data there is no Q/G Truth information



	## Prepare canvas###
	c = plot.PrepareCanvas()

	s = ROOT.THStack("S","MC")
	plot["all"] = None

        for t in jetTypes:
	  #if t=='U' and 'DiJet' in opts.base: continue ### FIXME, ugly removal
	  for mc in opts.mc.split(","):
	      name= "_".join([var,t, "pt%.0f"%(ptBins[ptbin]) , "%.0f"%(ptBins[ptbin+1]), "eta%.1f"%(etaBins[etabin]),"%.1f"%(etaBins[etabin+1]), what ])
	      plot[name].SetLineColor(ROOT.kBlack)
	      plot[name].SetLineWidth(2)
	      if   t  == 'Q' : plot[name].SetFillColor( 38 ) # Blue
	      elif t  == 'G' : plot[name].SetFillColor( 46 ) # Red
	      elif t  == 'U' : plot[name].SetFillColor( ROOT.kGray )
	
	      #s.Add(plot[name])
	      if plot["all"] == None : plot["all"] = plot[name].Clone("all")
	      else: plot["all"].Add(plot[name]) 


	plot["axis"] = plot["Data"].Clone("axis")
	plot["axis"].Draw("AXIS")

	#Correct for range and bin
	plot.Range("axis")
	#plot.Range("Data")

	plot.SetLumi(2300)
	plot.CMS()

	ndata=plot["Data"].Integral()
	nmc = plot["all"].Integral()
	## DEBUG
	print "DEBUG: ", ndata, nmc
	for name,h in plot:
		print name,":",h.Integral()

	if ndata==0 and nmc ==0 : continue
	plot["all"].Scale( ndata/nmc )

	## two loops, because I cannot scale the THStack, I'll scale the histogram before adding them to the THStack
        for t in jetTypes:
	  #if t=='U' and 'DiJet' in opts.base: continue ### FIXME, ugly removal
	  for mc in opts.mc.split(","):
	      name= "_".join([var,t, "pt%.0f"%(ptBins[ptbin]) , "%.0f"%(ptBins[ptbin+1]), "eta%.1f"%(etaBins[etabin]),"%.1f"%(etaBins[etabin+1]), what ])
	      plot[name].Scale(ndata/nmc)
	      s.Add(plot[name])
	
	plot.Update()

	s.Draw("HIST SAME")
	plot["Data"].SetBinErrorOption(ROOT.TH1.kPoisson)
	plot["Data"].Draw("PE X0 SAME")


	maxY = max( plot["Data"].GetMaximum(), plot["all"].GetMaximum() )

	plot["axis"].GetXaxis().SetLabelFont(43) ## for the ratio
	plot["axis"].GetXaxis().SetTitleFont(43)
	plot["axis"].GetYaxis().SetLabelFont(43)
	plot["axis"].GetYaxis().SetTitleFont(43)
	plot["axis"].GetXaxis().SetLabelSize(26)
	plot["axis"].GetYaxis().SetLabelSize(26)
	plot["axis"].GetXaxis().SetTitleSize(28)
	plot["axis"].GetYaxis().SetTitleSize(28)
	plot["Data"].SetMarkerColor(ROOT.kBlack)
	plot["Data"].SetLineColor(ROOT.kBlack)
	plot["Data"].SetMarkerStyle(20)
	plot["axis"].GetYaxis().SetRangeUser(0, maxY*1.25)
	plot["axis"].GetYaxis().SetTitleOffset(1.2)
	
	plot["axis"].Draw("AXIS SAME")
	plot["axis"].Draw("AXIS X+ Y+ SAME")

	plot.Update()
	plot.Legend()
	plot.Update()

	if not opts.batch: raw_input("ok?")

	outname= "_".join([var, "pt%.0f"%(ptBins[ptbin]) , "%.0f"%(ptBins[ptbin+1]), "eta%.1f"%(etaBins[etabin]),"%.1f"%(etaBins[etabin+1]) ])
	plot.Update()

	for ext in opts.exts.split(','):
		plot.SaveAs( opts.plot+ "/" + outname +"." + ext)
    except:
	    print "Caught Exception. Try to continue"
	    pass

print "-- DONE --"
exit(0)


