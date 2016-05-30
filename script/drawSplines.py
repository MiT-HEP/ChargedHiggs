import sys, os
import re
from optparse import OptionParser,OptionGroup

parser=OptionParser()
parser.add_option("","--sfdb" ,dest='sfdb',type='string',help="SF DB comma separated [%default]",default="aux/RMet80.txt")
parser.add_option("-l","--label" ,dest='label',type='string',help="label [%default]",default="tauinvisospline")
parser.add_option("-o","--out" ,dest='out',type='string',help="outlabel [%default]",default="")
parser.add_option("","--legend" ,dest='legend',type='string',help="legend labels, comma separated [%default]",default="Fit")

opts,args=parser.parse_args()

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import *
import ROOT as r
r.gROOT.SetBatch()

graphs=[]
for file in opts.sfdb.split(","):
	sfdb=ReadSFDB(file)
	g=r.TGraph()
	g.SetName(file)
	#gErr= r.TGraphAsymmErrors()
	for key in sfdb:
		label = key['label']
		if label != opts.label : continue
		if key['type'] != 'spline': continue
		pt=key['pt']
		sf=key['sf']
		err=key['err']
		n=g.GetN()
		g.SetPoint(n,pt,sf)
		#gErr.SetPoint(n,pt,sf)
		#gErr.SetPointError(n,0,0,err,err)
	graphs.append(g)

c=r.TCanvas("c","c",800,800)
leg=r.TLegend(0.6,.6,.9,.9)
leg.SetBorderSize(0)
leg.SetFillColor(0)
colors=[r.kRed,r.kBlue,r.kGreen+2,r.kGray]
markers=[20,21,22,29,30]
splines=[]
for idx,g in enumerate(graphs):
	s=r.TSpline3("spline_"+g.GetName(),g)
	## sup, sdown
	
	s.SetLineColor( colors[idx])
	s.SetLineWidth(2)
	
	g.SetMarkerStyle(markers[idx])
	g.SetMarkerColor(r.kBlack)
	g.SetLineColor(r.kBlack)
	
	if idx==0:
		g.Draw("AP")
		g.GetXaxis().SetTitle("p_{T}^{#tau^{h}} [GeV]")
		g.GetYaxis().SetTitle("R")
	else:
		g.Draw("P SAME")
	s.Draw("L SAME")
	splines.append(s)
	if opts.legend != "":
		leg.AddEntry(g,opts.legend.split(',')[idx],"P")
		leg.AddEntry(s,opts.legend.split(',')[idx] + " spline","L")

if opts.legend != "":
	leg.Draw("SAME")
c.SaveAs("plot/spline_" + opts.label+ opts.out+".pdf")
c.SaveAs("plot/spline_" + opts.label+ opts.out+".png")
c.SaveAs("plot/spline_" + opts.label+ opts.out+".root")
