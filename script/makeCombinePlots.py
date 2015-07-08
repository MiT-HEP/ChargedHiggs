#!/usr/bin/env python

# Usual reasons to love python imports
import os
import sys
import array 

from optparse import OptionParser
parser = OptionParser()
#parser.add_option("-f","--file",dest="files",default=[],action="append",help="Add a file")
parser.add_option("-f","--file",dest="file",default="",type="string",help="Input file")
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C")
parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true")
parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
parser.add_option("-x","--xSec",dest="xsec",help="Print limit vs xSec instead of mu",default=False,action="store_true")
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

def interpolate(x1,y1,x2,y2,x):
	''' linear interpolation between the points (x1,y1) (x2,y2) evaluated at x'''
	#y = mx +q
	if x1==x2 and y1==y2 and x==x1 : return y1

	m= (y2-y1)/(x2-x1)
	q= y2 - m*x2
	return m*x+q


## Make Limit plot
fInput= ROOT.TFile.Open(opts.file)
limitTree = fInput.Get("limit")

#Expected
graph=ROOT.TGraphAsymmErrors()
graph.SetName("graph")

exp=ROOT.TGraphAsymmErrors()
exp.SetName("exp")

oneSigma=ROOT.TGraphAsymmErrors()
oneSigma.SetName("oneSigma")

twoSigma=ROOT.TGraphAsymmErrors()
twoSigma.SetName("twoSigma")


median 	= []
Up 	= []
Up2 	= []
Down 	= []
Down2 	= []


for iEntry in range(0,limitTree.GetEntries()):
	limitTree.GetEntry(iEntry)
	mh = limitTree.mh
	l  = limitTree.limit
	q  = limitTree.quantileExpected
	type= 0

	## TODO OBS

	if q==0.5 : 
		#exp.SetPoint(g.GetN(), mh,l ) 
		median.append(  (mh,l) ) 
	
	if abs(q -0.0250000) <1e-5:  ## 95% 2sDown
		type=-2
		Down2.append( (mh,l) )
	if abs(q-0.1599999) <1e-5 :  ## 68% 1sDown
		type=-1
		Down.append( (mh,l) )
	if abs(q-0.8399999) <1e-5 :  ## 68% 1sUp
		type=1
		Up.append( (mh,l) )
	if abs(q-0.9750000) <1e-5 :  ## 95% 2sUp
		type=2
		Up2.append( (mh,l) )

if len(Up2) != len(Down2) :print "[ERROR] Count 2s"
if len(Up) != len(Down) :print "[ERROR] Count 1s"


if opts.xsec:
	if opts.verbose: print "-> Looking for basepath"
	basepath = ""
	mypath = os.path.abspath(os.getcwd())
	while mypath != "" and mypath != "/":
		if "ChargedHiggs" in os.path.basename(mypath):
			basepath = os.path.abspath(mypath)
		mypath = os.path.dirname(mypath)
	
	if opts.verbose: print "-> Base Path is " + basepath
	
	sys.path.insert(0,basepath)
	sys.path.insert(0,basepath+"/python/")
	sys.path.insert(0,os.getcwd())
	
	from ParseDat import ReadMCDB
	
	mcdb=ReadMCDB(basepath+"/dat/mc_database.txt") 
	#R[label] = (dir,sumw,xsec)


xsections_mcdb=[]
xsections_interpolated={}
if opts.xsec:
	for i in range(0,len(median)):
		mh= median[i][0]
		xSec= -1
		for label in mcdb:
			if "M%.0f"%mh in label:
				xSec = mcdb[label][2]
		if xSec <0 :  continue
		print "Found xSec for mh=",mh,"xSec=",xSec
		xsections_mcdb.append(  (mh,xSec) )
	for i in range(0,len(median)):
		mh= median[i][0]
		bin=-1
		for j in range(0,len(xsections_mcdb) -1):
			if mh >= xsections_mcdb[j][0] and mh<  xsections_mcdb[j+1][0]:
				bin = j
			if j ==len(xsections_mcdb)-2 and mh == xsections_mcdb[j+1][0]:
				bin = j  # last bin is closed on both sides
		if bin <0 : 
			print "[ERROR]: unable to find cross section bin for mh=", mh


		xsections_interpolated[mh] = interpolate(xsections_mcdb[bin][0],float(xsections_mcdb[bin][1]),xsections_mcdb[bin+1][0],float(xsections_mcdb[bin+1][1]),mh )

for i in range(0,len(median)):
	count= exp.GetN()
	exp.SetPoint( count, median[i][0] ,median[i][1])

	mh= median[i][0]
	if mh != Up[i][0] : print "[ERROR]: MH mismatch"
	if mh != Up2[i][0] : print "[ERROR]: MH mismatch"
	if mh != Down[i][0] : print "[ERROR]: MH mismatch"
	if mh != Down2[i][0] : print "[ERROR]: MH mismatch"
	
	if opts.xsec:
		print "mh=",mh,"xSec=",xSec, "median = ",median[i][1]
		xSec =  xsections_interpolated[mh]
		median[i]   = (median[i][0], median[i][1]*xSec /1000.)
		Up[i]       = (mh, Up[i][1] * xSec /1000.) 
		Up2[i]      = (mh, Up2[i][1] * xSec /1000.)
		Down[i]     = (mh, Down[i][1] * xSec /1000.)
		Down2[i]    = (mh, Down2[i][1] * xSec /1000.)

	oneSigma.SetPoint(count, mh, median[i][1] ) 
	oneSigma.SetPointError(count, 0, 0 , median[i][1] - Down[i][1], Up[i][1]-median[i][1] ) 
	twoSigma.SetPoint(count, mh , median[i][1] ) 
	twoSigma.SetPointError(count, 0, 0 , median[i][1] - Down2[i][1], Up2[i][1]-median[i][1] ) 


graph.SetMarkerStyle(21)
graph.SetMarkerSize(0.5)
graph.SetLineColor(1)
graph.SetLineWidth(2)
exp.SetLineColor(1)
exp.SetLineStyle(2)
oneSigma.SetLineStyle(2)
twoSigma.SetLineStyle(2)
oneSigma.SetFillColor(ROOT.kGreen)
twoSigma.SetFillColor(ROOT.kYellow)

c=ROOT.TCanvas()
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)

dummy = ROOT.TH1D("dummy","dummy",1000, 0, 1000)
dummy.GetXaxis().SetRangeUser(200,900)
dummy.GetYaxis().SetRangeUser(1e2,1e8)

dummy.GetXaxis().SetTitle("m_{H^{+}}")
dummy.GetYaxis().SetTitle("#sigma/#sigma_{MSSM}")
if opts.xsec:
	dummy.GetYaxis().SetTitle("#sigma [fb]")

dummy.Draw("AXIS")

line = ROOT.TGraph()
line.SetLineColor(ROOT.kGray)
line.SetPoint(0,0,1)
line.SetPoint(1,1000,1)

c.SetLogy()

mg = ROOT.TMultiGraph()
mg.Add(twoSigma)
mg.Add(oneSigma)
mg.Add(exp)
#mg.Draw("A")
mg.Draw("3")
mg.Draw("LPX")
line.Draw("L SAME")

dummy.Draw("AXIS SAME")
dummy.Draw("AXIS X+ Y+ SAME")
dummy.Draw("AXIG SAME")

l = ROOT.TLatex()
l.SetNDC()
l.SetTextSize(0.05)
l.SetTextFont(42)
l.SetTextAlign(13)
l.DrawLatex(0.13,.88,"#bf{CMS}, #scale[0.75]{#it{Simulation}}")
l.SetTextSize(0.03)
l.SetTextAlign(31)
l.DrawLatex(0.89,.91,"5 fb^{-1} (13 TeV)")

raw_input("Looks ok?")

c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")
