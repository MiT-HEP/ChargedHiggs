#!/usr/bin/env python

# Usual reasons to love python imports
import os
import sys
import array 

from optparse import OptionParser
parser = OptionParser()
#parser.add_option("-f","--file",dest="files",default=[],action="append",help="Add a file")
parser.add_option("-f","--file",dest="file",default="",type="string",help="Input file")
#parser.add_option("-l","--label",dest="label",default="",type="string",help="Labels, or none or one for input file")
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C")
#parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true")
#parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
parser.add_option("-u","--unblind",dest="unblind",default=False,action="store_true",help="Draw observation")
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT



def GetPvalFromTree(inputFile):
	''' Get Limit Histo from Combine tree'''

	## Make Limit plot
	fInput= ROOT.TFile.Open(inputFile)
	limitTree = fInput.Get("limit")
	
	#Expected
	obs=ROOT.TGraphAsymmErrors()
	obs.SetName("obs")
	
	for iEntry in range(0,limitTree.GetEntries()):	
		limitTree.GetEntry(iEntry)
		mh = limitTree.mh
		pval = limitTree.limit
		obs.GetN()
		obs.SetPoint(obs.GetN(), mh, pval)

	obs.SetLineColor(ROOT.kRed);
	obs.SetLineWidth(2);
	return obs

obs= GetPvalFromTree(opts.file)

sigmas=[1,2,3,4,5,6]

## Start Drawing
c=ROOT.TCanvas()

ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)

dummy = ROOT.TH1D("dummy","dummy",1000, 0, 1000)
dummy.GetXaxis().SetRangeUser(150,250)
dummy.GetYaxis().SetRangeUser(1.e-10,1)

dummy.GetXaxis().SetTitle("m_{H^{+}}")
dummy.GetYaxis().SetTitle("local p-value")

dummy.Draw("AXIS")
dummy.Draw("AXIG SAME")
obs.Draw("L SAME")

# draw sigma lines
sigmas=[1,2,3,4,5,6]
lines=[]
labels=[]
for i,sig in enumerate(sigmas):
  y = ROOT.RooStats.SignificanceToPValue(sig)
  lines.append(ROOT.TLine(0,y,1000,y))

  lines[i].SetLineWidth(2)
  lines[i].SetLineStyle(2)
  lines[i].SetLineColor(13) # greay Lines 
  labels.append(ROOT.TLatex(150+1, y * 0.8, "%d #sigma" % (i+1)))
  labels[i].SetTextColor(13)
  labels[i].SetTextAlign(11);
  lines[i].Draw('SAME')
  labels[i].Draw('SAME')

c.SetLogy(True)
c.Update()

raw_input("ok?")
