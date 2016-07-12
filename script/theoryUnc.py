import sys, os
import re

from optparse import OptionParser,OptionGroup
parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="ChHiggs.root")
parser.add_option("-m","--mass",type='string',help="Masses to test, comma separated. [%default]", default="200")
parser.add_option("-r","--rebin",type='int',help="Rebin [%default]", default=-1)

opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

fIn=ROOT.TFile.Open(opts.input,"READ")
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_Pdf99;1    Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleFDown;2       Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleFDown;1       Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleFUp;2 Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleFUp;1 Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRDown;2       Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRDown;1       Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRFDown;2      Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRFDown;1      Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRFUp;2        Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRFUp;1        Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRUp;2 Mt HplusToTauNu_M-500_13TeV_amcatnlo
##  KEY: TH1D      Mt_HplusToTauNu_M-500_13TeV_amcatnlo_ScaleRUp;1 Mt HplusToTau

hPdf=[]
hScales=[]

name="ChargedHiggsTauNu/Vars/Mt_HplusToTauNu_M-%(mass)s_13TeV_amcatnlo"%{"mass":opts.mass}
print "->Trying to get",name
nominal=fIn.Get(name)
if opts.rebin>0:
	nominal.Rebin(opts.rebin)


hPdf.append(nominal)
hScales.append(nominal)

for i in range(0,100):
	name="ChargedHiggsTauNu/Vars/Mt_HplusToTauNu_M-%(mass)s_13TeV_amcatnlo_Pdf%(num)d"%{"mass":opts.mass,"num":i}
	print "->Trying to get",name
	h=fIn.Get(name)
	if opts.rebin>0:
		h.Rebin(opts.rebin)
	hPdf.append(h)
for what in ["F","R","RF"]:
   for shift in ["Up","Down"]:
	name="ChargedHiggsTauNu/Vars/Mt_HplusToTauNu_M-%(mass)s_13TeV_amcatnlo_Scale%(what)s%(shift)s"%{"mass":opts.mass,"what":what,"shift":shift}
	print "->Trying to get",name
        h=fIn.Get(name)
	if opts.rebin>0:
		h.Rebin(opts.rebin)
	hScales.append(h)

def envelope(l,name=""):
	target=l[0].Clone(name)
	target.Reset("ACE")
	for idx,h in enumerate(l):
	    for i in range(1,target.GetNbinsX()+1):
		c=target.GetBinContent(i)
		e=target.GetBinError(i)
		c1=h.GetBinContent(i)
		print "c=",c,"e=",e,"c1=",c1
		if idx==0:
			c=c1
			e=0
		else:
			if c1 > c+e:
				c2= (c-e + c1)/2
				e2= abs(c-e-c1)/2 
				c=c2
				e=c2
			elif c1 < c-e:
				c2=( c+e+c1)/2
				e2=abs(c-e - c1 )/2
				c=c2
				e=c2
		target.SetBinContent(i,c)
		target.SetBinError(i,e)
	return target

def printRelative(h):
	print h.GetName(),"(%)",
	for i in range(1,h.GetNbinsX()+1):
		c=h.GetBinContent(i)
		e=h.GetBinError(i)
		try:
			print "%.2f"%(e/c*100),
		except :
			print "-",
	print

scale=envelope(hScales,"scale")
pdf=envelope(hPdf,"pdf")

printRelative(scale)
printRelative(pdf)


