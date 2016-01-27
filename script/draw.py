import os,sys
from optparse import OptionParser
import re

parser= OptionParser()
parser.add_option("-b","--batch",dest="batch",action="store_true",help="Run in Batch mode",default=False);
parser.add_option("-L","--lumi",dest="lumi",type='float',help="Luminosity. [%default]",default=2215);
parser.add_option("","--base",dest="base",type='string',help="Base Directory to fetch the plots. [%default]",default='DYAnalysis/Vars');
parser.add_option("","--mc",dest="mc",type='string',help="MC to Draw. [%default]",default="DY,TT,WW,WZ,ZZ,WJets");
parser.add_option("-v","--var",dest="var",type='string',help="Var to Draw. [%default]",default="Ptem");
parser.add_option("-f","--file",dest="file",type='string',help="file. [%default]",default="test/mysub/DY/DY.root");
parser.add_option("","--rebin",dest="rebin",type='int',help="rebin. [%default]",default=1);

opts, args = parser.parse_args()

sys.argv=[]

import ROOT
if opts.batch:
        ROOT.gROOT.SetBatch()

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

garbage=[]
## plot stuff
def PrepareCanvas(name="c"):
        c=ROOT.TCanvas(name,name,800,800)
        c.SetTopMargin(0.05)
        c.SetRightMargin(0.05)
        c.SetLeftMargin(0.15)
        c.SetBottomMargin(0.15)
        return c

def CMS():
        ltx=ROOT.TLatex()
        ltx.SetNDC()
        ltx.SetTextFont(42)
        ltx.SetTextSize(0.03)
        ltx.SetTextAlign(31)
	if opts.lumi > 1000: ltx.DrawLatex(.94,.96,"%.1f fb^{-1} (13TeV)" % (opts.lumi/1000.))  ## ONLY TCANVAS
	else : ltx.DrawLatex(.94,.96,"%.1f pb^{-1} (13TeV)" % (opts.lumi))  ## ONLY TCANVAS
        #ltx.DrawLatex(.96,.98,"%.1f pb^{-1} (13TeV)"%lumi)
        ltx.SetTextAlign(13)
        ltx.SetTextSize(0.05)
        #ltx.DrawLatex(.17,.93,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") #
        ltx.SetTextAlign(33)
        ltx.DrawLatex(.87,.92,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") #
        #ltx.DrawLatex(.17,.95,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") 

def Range(dict):
	if opts.var == 'Ptmm' or \
	   opts.var == 'Ptem' or \
	   opts.var == 'Ptee' or \
	   opts.var == 'PtemNoMCut' :
		   for h in dict:
			   dict[h].Rebin(5)
			   dict[h].GetXaxis().SetRangeUser(0,200)
	if opts.var == 'EtaMu1':
		   for h in dict:
			   dict[h].Rebin(4)
			   dict[h].GetXaxis().SetRangeUser(-2.5,2.5)

def Legend(dict):
	l = ROOT.TLegend(0.65,.60,.93,.85)
	l.AddEntry(dict["Data"],"data","PE")
	if 'DY' in dict: l.AddEntry(dict["DY"],"DY","F")
	if 'TT' in dict: l.AddEntry(dict["TT"],"TT","F")
	if 'WJets' in dict: l.AddEntry(dict["WJets"],"WJets","F")
	if 'WW' in dict: l.AddEntry(dict["WW"],"EWK","F")
	if 'QCD' in dict: l.AddEntry(dict["QCD"],"QCD","F")
	l.Draw()
	l.SetBorderSize(0)
	garbage.append(l)


# fetch histograms
dict={}
fIn = ROOT.TFile.Open(opts.file)
if fIn== None:
	print "File ",opts.file,'does not exist'

for ext in ["Data"] + opts.mc.split(','):
	print " * Fetching ",ext
	full = opts.base + '/' + opts.var + "_" + ext
	dict[ext] = fIn.Get( full )  
	if dict[ext] == None:
		print "Unable to fetch ",full
	if opts.rebin >0: dict[ext].Rebin(opts.rebin)
	if ext != "Data" : dict[ext].Scale(opts.lumi)

##Correct for range and bin
Range(dict)

## Prepare canvas###
c = PrepareCanvas()
pup=ROOT.TPad(opts.var+ "_pad1" ,"pad1", 0,0.25, 1 ,1)
pdn=ROOT.TPad(opts.var+ "_pad2" ,"pad2", 0,0, 1 ,.25)

pup.SetTopMargin(0.05)
pup.SetBottomMargin(0.15)
pup.SetLeftMargin(0.15)
pup.SetRightMargin(0.05)
pdn.SetTopMargin(0.05)
pdn.SetBottomMargin(0.40)
pdn.SetLeftMargin(0.15)
pdn.SetRightMargin(0.05)
pup.Draw()
pdn.Draw()

s = ROOT.THStack("S","MC")
dict["all"] = None

for mc in opts.mc.split(','):
      dict[mc].SetLineColor(ROOT.kBlack)
      dict[mc].SetLineWidth(2)
      if   mc == 'DY' : dict[mc].SetFillColor( ROOT.kCyan )
      elif mc == 'TT' : dict[mc].SetFillColor( ROOT.kMagenta+2 )
      elif mc == 'WJets' : dict[mc].SetFillColor( ROOT.kGreen+2 )
      elif mc == 'WW' : dict[mc].SetFillColor( ROOT.kGreen )
      elif mc == 'WZ' : dict[mc].SetFillColor( ROOT.kGreen )
      elif mc == 'ZZ' : dict[mc].SetFillColor( ROOT.kGreen )
      elif mc == 'QCD': dict[mc].SetFillColor( ROOT.kOrange )
      else            : dict[mc].SetFillColor( ROOT.kGray )

      s.Add(dict[mc])
      if dict["all"] == None : dict["all"] = dict[mc].Clone("all")
      else: dict["all"].Add(dict[mc]) 

print "Draw 1"
c.Update()

pup.cd()
dict["axis"] = dict["Data"].Clone("axis")
dict["axis"].Draw("AXIS")

c.Update()

s.Draw("HIST SAME")
dict["Data"].Draw("PE SAME")

c.Update()


## s.GetXaxis().SetTitle( dict["Data"].GetXaxis().GetTitle() )
## s.GetYaxis().SetTitle( dict["Data"].GetYaxis().GetTitle() )
## s.GetYaxis().SetTitleOffset(1.2)
#dict["Data"].Draw("AXIS")

maxY = max( dict["Data"].GetMaximum(), dict["all"].GetMaximum() )

## s.GetXaxis().SetLabelFont(43)
## s.GetXaxis().SetTitleFont(43)
## s.GetYaxis().SetLabelFont(43)
## s.GetYaxis().SetTitleFont(43)
## s.GetXaxis().SetLabelSize(26)
## s.GetYaxis().SetLabelSize(26)
## s.GetXaxis().SetTitleSize(28)
## s.GetYaxis().SetTitleSize(28)
## s.GetXaxis().SetTitleOffset(1.2)
## s.GetYaxis().SetTitleOffset(1.2)
## s.GetXaxis().SetRange(dict["Data"].GetXaxis().GetFirst(), dict["Data"].GetXaxis().GetLast() ) 
## s.GetYaxis().SetRangeUser(0, maxY*1.25)

dict["axis"].GetXaxis().SetLabelFont(43) ## for the ratio
dict["axis"].GetXaxis().SetTitleFont(43)
dict["axis"].GetYaxis().SetLabelFont(43)
dict["axis"].GetYaxis().SetTitleFont(43)
dict["axis"].GetXaxis().SetLabelSize(26)
dict["axis"].GetYaxis().SetLabelSize(26)
dict["axis"].GetXaxis().SetTitleSize(28)
dict["axis"].GetYaxis().SetTitleSize(28)
dict["Data"].SetMarkerColor(ROOT.kBlack)
dict["Data"].SetLineColor(ROOT.kBlack)
dict["Data"].SetMarkerStyle(20)
dict["axis"].GetYaxis().SetRangeUser(0, maxY*1.25)
dict["axis"].GetYaxis().SetTitleOffset(1.2)

dict["axis"].Draw("AXIS SAME")
dict["axis"].Draw("AXIS X+ Y+ SAME")

c.Update()
############# DRAW RATIO ############
pdn.cd()

pdn.SetGridy()
for i in range(0, dict["all"].GetNbinsX() ) : dict["all"].SetBinError(i+1,0)
dict[ "axis_ratio" ] = dict["axis"].Clone("axis_ratio")
dict[ "axis_ratio" ] . Draw("AXIS")
dict[ "ratio" ] = dict["Data"] . Clone(  "ratio")
dict[ "ratio" ] . Divide( dict [ "all" ] )
dict[ "ratio" ] . Draw(" PE  SAME")
dict[ "axis_ratio" ] . GetYaxis() . SetRangeUser(0.6,1.4)
dict[ "axis_ratio" ] . GetYaxis() . SetNdivisions(202)
dict[ "axis_ratio" ] . GetYaxis() . SetTitle("data/MC")
dict[ "axis_ratio" ] . GetYaxis() . SetTitleOffset(1.2)
dict[ "axis_ratio" ] . GetXaxis() . SetTitleOffset(4)

bands = {}
if True: ## band 10/20 in the ratio plot
	for i in [10., 20.]:
		bands[ int(i) ] = ROOT.TGraph()
		bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["Data"].GetBinLowEdge(0)                           ,1. + i/100 )
		bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["Data"].GetBinLowEdge(dict["Data"].GetNbinsX() +1) ,1. + i/100 )
		bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["Data"].GetBinLowEdge(dict["Data"].GetNbinsX() +1) ,1. - i/100 )
		bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["Data"].GetBinLowEdge(0)                           ,1. - i/100 )
	if 20 in bands: 
		bands[20].SetFillColor(ROOT.kGray)
		bands[20].Draw("F SAME")
	if 10 in bands: 
		bands[10].SetFillColor(ROOT.kGray +1)
		bands[10].Draw("F SAME")
	bands[0] = ROOT.TGraph()
	bands[0].SetPoint(0, dict["Data"].GetBinLowEdge(0) ,1.)
	bands[0].SetPoint(1, dict["Data"].GetBinLowEdge(dict["Data"].GetNbinsX() +1)  ,1.)
	bands[0].SetLineColor(ROOT.kBlack)
	bands[0].Draw("L SAME")
	dict[ "ratio" ] . Draw(" PE SAME")

dict[ "axis_ratio" ] . Draw(" AXIS SAME ")
dict[ "axis_ratio" ] . Draw(" AXIS X+ Y+ SAME ")

pup.cd()

Legend(dict)
CMS()

c.Update()
if not opts.batch: raw_input("ok?")

c.Update()
c.SaveAs( "plot/canv_" + opts.var +".png")
c.SaveAs( "plot/canv_" + opts.var +".pdf")
c.SaveAs( "plot/canv_" + opts.var +".root")

exit(0)

