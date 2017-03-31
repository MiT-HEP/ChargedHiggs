import os,sys

## to compute versioning
import hashlib

## Hmm common configuration

class HmmConfig():
	''' Hmm configuration for scripts. 
	    All the scripts except the Analyzer can should take the configuration from here.
	'''
	def __init__(self):
		## CATEGORIES and MAP ##
		self.muCategories=["BB","BO","BE","OO","OE","EE"]
		#self.procCategories= [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]
		self.procCategories= [ "VBF0","GF","VBF1","Untag0","Untag1"]
		self.categories=[]
		for muStr in self.muCategories:
	  		for catStr in self.procCategories:
				self.categories.append(catStr+"_"+muStr);

		### FIT ###
		self.xmin = 105
		self.xmax=150
		self.sig_mass_points=[120,125,130]
		self.processes=["GluGlu","VBF","ZH","WPlusH","WMinusH","ttH"]
		self.sigfit_gaussians={}
		#self.sigfit_gaussians[("Untag0_BB","GluGlu")] = 3

		for p in ["GluGlu","ZH","WPlusH","WMinusH"]: # all except vbf
			self.sigfit_gaussians[("VBF0_EE",p)] = 2  ## 25
			self.sigfit_gaussians[("GF_EE",p)] = 2  ## 26

		self.sigfit_gaussians[("GF_BE","GluGlu")] = 2  ## 11
		self.sigfit_gaussians[("VBF0_OE","GluGlu")] = 2  ## 20
		self.sigfit_gaussians[("VBF0_OE","WMinusH")] = 2  ## 20
		self.sigfit_gaussians[("Untag1_EE","WMinusH")] = 2  ## 29

		self.sigfit_gaussians[("GF_OE","ttH")] = 1  ## 20
		self.sigfit_gaussians[("VBF0_EE","ttH")] = 1  ## 25
		self.sigfit_gaussians[("GF_EE","ttH")] = 1  ## 26
		self.sigfit_gaussians[("VBF1_EE","ttH")] = 1  ## 27
		self.sigfit_gaussians[("Untag0_EE","ttH")] = 1  ## 28
		self.sigfit_gaussians[("Untag1_EE","ttH")] = 1  ## 29
		
		
		## DATACARD specific ##

		self.datacard_procs=['BKG','GluGlu','VBF','WPlusH','WMinusH','ttH','ZH']
		self.computeVersioning()
		self.Print()

	def catToStr(self,num):
		return self.categories[num]

	def computeVersioning(self):
		catInputs=",".join(self.categories)
		catInputs+=";"+",".join(self.muCategories)
		catInputs+=";"+",".join(self.procCategories)
		catInputs+=";"+",".join(self.processes)
		self.catVersion=hashlib.md5(catInputs).hexdigest()
		fitInputs=catInputs
		fitInputs+=";"+",".join(["%d"%x for x in self.sig_mass_points])
		fitInputs+=";"+",".join(self.processes);
		fitInputs+=";"+",".join([cat+"_"+proc+"=%d"%self.sigfit_gaussians[(cat,proc)] for cat,proc in self.sigfit_gaussians ])
		self.fitVersion=hashlib.md5(fitInputs).hexdigest()

	def Print(self):
		print "--- Hmm Configurator ---"
		for x in ["xmin","xmax","muCategories","procCategories","categories","sig_mass_points","processes","datacard_procs","catVersion","fitVersion"]:
			print x+"=",eval("self."+x)
		print "------------------------"

	def br(self,mass=125):
	    ''' only 125 '''
	    br = 2.176E-04
	    return br

	def xsec(self,type,mass=125):
	    ''' only 125 '''
	    if type=="VBF" or type == "qqH" : xsec= 3.782E+00 
	    elif type == "WH": xsec=1.373E+00
	    elif type == "ZH": xsec =8.839E-01
	    elif type == "ggH" or type=="GluGlu": xsec =4.858E+01
	    elif type == "ttH": xsec =5.071E-01
	    elif type == "bbH" : xsec=4.880E-01
	    elif type == "WMinusH": xsec = 5.328E-01
	    elif type == "WPlusH": xsec =8.400E-01
	    else: xsec=1.
	    return xsec

	def lumi(self):
	    return 35867


hmm=HmmConfig()

class Stack:
    ''' This is a soft version of thstack that usually crash'''
    def __init__(self):
        self.hists_=[]
        self.name_="myStack"
    def SetName(self,name):
        self.name_ = name
    def Add(self,h):
        if len(self.hists_) == 0:
            self.hists_.append( h.Clone(self.name_ +"_"+h.GetName()) )
        else:
            self.hists_.append( h.Clone(self.name_ +"_"+h.GetName()) )
            self.hists_[-1].Add(self.hists_[-2])
    def Draw(self,opts=""):
        for idx in reversed(range(0,len(self.hists_))):
            h= self.hists_[idx]
            if idx ==len(self.hists_)-1 or 'SAME' in opts: h.Draw(opts)
            else       : h.Draw(opts +" SAME")
    def GetHist(self):
        return self.hists_[-1]

    def GetFirstHist(self):
        return self.hists_[0]

    def Delete(self):
        for h in self.hists_: h.Delete()
        self.hists_=[]

    def Print(self):
        print "------",self.name_,"-----------"
        for h in self.hists_:
            print "*",h.GetName()
        print "-------------------------------"


