import os,sys
from optparse import OptionParser
import re

parser= OptionParser()
parser.add_option("-b","--batch",dest="batch",action="store_true",help="Run in Batch mode",default=False);
parser.add_option("-L","--lumi",dest="lumi",type='float',help="Luminosity. [%default]",default=[],action='append');
parser.add_option("","--base",dest="base",type='string',help="Base Directory to fetch the plots. [%default]",default='DYAnalysis/Vars');
parser.add_option("-v","--var",dest="var",type='string',help="Var to Draw. [%default]",default="Ptem");
parser.add_option("-f","--file",dest="file",type='string',help="file. [%default]",default=[],action='append');
parser.add_option("","--rebin",dest="rebin",type='int',help="rebin. [%default]",default=1);
parser.add_option("","--xrange",dest="xrange",help="xrange. [%default]",default=None);
parser.add_option("","--latex",dest="latex",help="Extra text. [%default]",default=None);

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
        ltx.SetTextSize(0.035)
        ltx.SetTextAlign(31)
        #if opts.lumi > 1000: ltx.DrawLatex(.94,.96,"%.1f fb^{-1} (13TeV)" % (opts.lumi/1000.))  ## ONLY TCANVAS
        #else : ltx.DrawLatex(.94,.96,"%.1f pb^{-1} (13TeV)" % (opts.lumi))  ## ONLY TCANVAS
        ltx.SetTextSize(0.05)
        ltx.SetTextAlign(13)
        #ltx.SetTextAlign(33)
        #ltx.DrawLatex(.87,.92,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") #
        ltx.DrawLatex(.20,.92,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") #

def Range(dict):
    if opts.xrange:
        for h in dict:
           if h == 'years' : continue
           print "Setting range for",h
           dict[h].GetXaxis().SetRangeUser(float(opts.xrange.split(',')[0]),float(opts.xrange.split(',')[1]))
        return
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
    #l = ROOT.TLegend(0.65,.60,.93,.85)
    l = ROOT.TLegend(0.70,.68,.93,.93)
    for year in dict['years']:
        l.AddEntry(dict[year+"_Data"],"data "+ year,"PE")
    l.Draw()
    l.SetBorderSize(0)
    garbage.append(l)

# fetch histograms
dict={}

for ifile,fname in enumerate(opts.file):
    fIn = ROOT.TFile.Open(fname)
    if fIn== None:
        print "File ",fname,'does not exist'

    ext="Data"
    print " * Fetching ",ext
    full = opts.base + '/' + opts.var + "_" + ext
    year="xxxx"
    if "2016" in fname: year="2016"
    if "2017" in fname: year="2017"
    if "2018" in fname: year="2018"
    dict[year+"_"+ext] = fIn.Get( full ). Clone(year+"_"+ext)
    if dict[year+"_"+ext] == None: print "Unable to fetch ",full
    if opts.rebin >0: dict[year+"_"+ext].Rebin(opts.rebin)
    if opts.lumi!=[] : 
        print "-> Scaling year",year,"by lumi",opts.lumi[ifile]
        dict[year+"_"+ext].Scale(1./opts.lumi[ifile])
    if 'years' not in dict: dict['years'] = []
    dict['years'].append(year)
    garbage.append(fIn)

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

print "Draw 1"
c.Update()

pup.cd()
dict["axis"] = dict[dict['years'][0]+"_Data"].Clone("axis")
dict["axis"].Draw("AXIS")

c.Update()

for year in dict['years']:
    col=ROOT.kBlack
    if year =='2016': col = 8
    if year =='2017': col = 38
    if year =='2018': col = 46
    dict[year+"_Data"].SetMarkerColor(col)
    dict[year+"_Data"].SetLineColor(col)
    dict[year+"_Data"].SetMarkerStyle(20)
    dict[year+"_Data"].Draw("PE SAME")

c.Update()

maxY=dict[dict['years'][0]+"_Data"].GetMaximum()
for year in dict['years']:
    maxY = max( dict[year+"_Data"].GetMaximum(), maxY )

dict["axis"].GetXaxis().SetLabelFont(43) ## for the ratio
dict["axis"].GetXaxis().SetTitleFont(43)
dict["axis"].GetYaxis().SetLabelFont(43)
dict["axis"].GetYaxis().SetTitleFont(43)
dict["axis"].GetXaxis().SetLabelSize(26)
dict["axis"].GetYaxis().SetLabelSize(26)
dict["axis"].GetXaxis().SetTitleSize(28)
dict["axis"].GetYaxis().SetTitleSize(28)
dict["axis"].GetYaxis().SetRangeUser(0, maxY*1.25)
dict["axis"].GetYaxis().SetTitleOffset(1.2)

dict["axis"].Draw("AXIS SAME")
dict["axis"].Draw("AXIS X+ Y+ SAME")

c.Update()
############# DRAW RATIO ############
pdn.cd()

pdn.SetGridy()
dict["all" ] = dict["2018_Data"].Clone("all")
for i in range(0, dict["all"].GetNbinsX() ) : dict["all"].SetBinError(i+1,0) ## reset Error
dict[ "axis_ratio" ] = dict["axis"].Clone("axis_ratio")
dict[ "axis_ratio" ] . Draw("AXIS")

for year in dict['years']:
    dict[ year+"_ratio" ] = dict[year+"_Data"] . Clone(  year+"_ratio")
    dict[ year+"_ratio" ] . Divide( dict [ "all" ] )
    dict[ year+"_ratio" ] . Draw(" PE  SAME")

dict[ "axis_ratio" ] . GetYaxis() . SetRangeUser(0.6,1.4)
dict[ "axis_ratio" ] . GetYaxis() . SetNdivisions(202)
dict[ "axis_ratio" ] . GetYaxis() . SetTitle("data/2018")
dict[ "axis_ratio" ] . GetYaxis() . SetTitleOffset(1.2)
dict[ "axis_ratio" ] . GetXaxis() . SetTitleOffset(4)

bands = {}
if False: ## band 10/20 in the ratio plot
    for i in [10., 20.]:
        bands[ int(i) ] = ROOT.TGraph()
        bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["2016_Data"].GetBinLowEdge(0)                           ,1. + i/100 )
        bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["2016_Data"].GetBinLowEdge(dict["2016_Data"].GetNbinsX() +1) ,1. + i/100 )
        bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["2016_Data"].GetBinLowEdge(dict["2016_Data"].GetNbinsX() +1) ,1. - i/100 )
        bands[ int(i) ].SetPoint(bands[ int(i)].GetN(),dict["2016_Data"].GetBinLowEdge(0)                           ,1. - i/100 )
    if 20 in bands: 
        #bands[20].SetFillColor(ROOT.kGray)
        bands[20].SetFillColor(ROOT.kAzure-9)
        bands[20].Draw("F SAME")
    if 10 in bands: 
        #bands[10].SetFillColor(ROOT.kGray +1)
        bands[10].SetFillColor(ROOT.kBlue -9)
        bands[10].Draw("F SAME")

## STAT UNC
bands[0] = ROOT.TGraph()
bands[0].SetPoint(0, dict[dict['years'][0]+"_Data"].GetBinLowEdge(0) ,1.)
bands[0].SetPoint(1, dict[dict['years'][0]+"_Data"].GetBinLowEdge(dict[dict['years'][0]+"_Data"].GetNbinsX() +1)  ,1.)
bands[0].SetLineColor(ROOT.kBlack)
bands[0].Draw("L SAME")

dict[ "axis_ratio" ] . Draw(" AXIS SAME ")
dict[ "axis_ratio" ] . Draw(" AXIS X+ Y+ SAME ")

pup.cd()

Legend(dict)
CMS()

if opts.latex:
    ltx=ROOT.TLatex()
    ltx.SetNDC()
    ltx.SetTextFont(42)
    ltx.SetTextSize(0.04)
    ltx.SetTextAlign(33)
    ltx.DrawLatex(.87,.92,opts.latex) 

c.Update()
if not opts.batch: raw_input("ok?")

c.Update()
c.SaveAs( "plot/data_" + opts.var +".png")
c.SaveAs( "plot/data_" + opts.var +".pdf")
c.SaveAs( "plot/data_" + opts.var +".root")

exit(0)

