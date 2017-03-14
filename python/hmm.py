import os,sys

## to compute versioning
import hashlib

## Hmm common configuration

class HmmConfig():
	''' Hmm configuration for scripts'''
	def __init__(self):
		## CATEGORIES and MAP ##
		self.muCategories=["BB","BO","BE","OO","OE","EE"]
		self.procCategories= [ "VBF0","OneB","GF","VBF1","Untag0","Untag1"]
		self.categories=[]
		for muStr in self.muCategories:
	  		for catStr in self.procCategories:
				self.categories.append(catStr+"_"+muStr);

		### FIT ###
		self.xmin = 105
		self.xmax=300
		self.sig_mass_points=[120,125,130]
		self.processes=["GluGlu","VBF","ZH","WPlusH","WMinusH","ttH"]
		self.sigfit_gaussians={}
		#self.sigfit_gaussians[("Untag0_BB","GluGlu")] = 3
		
		## DATACARD specific ##

		self.datacard_procs=['BKG','GluGlu','VBF','WPlusH','WMinusH','ttH','ZH']
		self.computeVersioning()
		self.Print()

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
hmm=HmmConfig()
