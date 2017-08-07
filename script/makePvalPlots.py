#!/usr/bin/env python

# Usual reasons to love python imports
import os
import sys
import array 

from optparse import OptionParser
parser = OptionParser()
#parser.add_option("-f","--file",dest="files",default=[],action="append",help="Add a file")
parser.add_option("-f","--file",dest="file",default="",type="string",help="Input file")
parser.add_option("-l","--label",dest="label",default="",type="string",help="Labels, or none or one for input file")
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
	allValues= []	
	for iEntry in range(0,limitTree.GetEntries()):	
		limitTree.GetEntry(iEntry)
		mh = limitTree.mh
		pval = limitTree.limit
		allValues.append( (mh,pval))
	for mh,pval in sorted(allValues):
	    obs.GetN()
	    obs.SetPoint(obs.GetN(), mh, pval)

	obs.SetLineColor(ROOT.kRed);
	obs.SetLineWidth(2);
	return obs

objs=[]
graphs=[]
for f in opts.file.split(','):
    #obs= GetPvalFromTree(opts.file)
    obs= GetPvalFromTree(f)
    graphs.append(obs)
obs=graphs[0]

sigmas=[1,2,3,4,5,6]

## Start Drawing
c=ROOT.TCanvas()

ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)

xmin=120
xmax=130
dummy = ROOT.TH1D("dummy","dummy",1000, 0, 1000)
dummy.GetXaxis().SetRangeUser(xmin,xmax)
#dummy.GetYaxis().SetRangeUser(1.e-10,1)
dummy.GetYaxis().SetRangeUser(.5e-3,1)

dummy.GetXaxis().SetTitle("m_{H}")
dummy.GetYaxis().SetTitle("local p-value")

dummy.Draw("AXIS")
dummy.Draw("AXIG SAME")

colors=[46,38,ROOT.kBlack,ROOT.kGreen+2]
for idx,g in enumerate(graphs):
    g.SetLineColor(colors[idx])
    g.Draw("L SAME")

if opts.label !="":
    # do Legend
    leg = ROOT.TLegend(0.15,.20,.40,.45)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    ## reversed so if there are data, are the 3rd
    for idx in reversed(range(0,len(graphs))):
        g=graphs[idx]
        leg.AddEntry(g,opts.label.split(',')[idx],"L")
    leg.Draw("SAME")
    objs.append(leg)

# draw sigma lines
sigmas=[1,2,3,4,5,6]
lines=[]
labels=[]
for i,sig in enumerate(sigmas):
  y = ROOT.RooStats.SignificanceToPValue(sig)
  lines.append(ROOT.TLine(xmin,y,xmax,y))

  lines[i].SetLineWidth(2)
  lines[i].SetLineStyle(2)
  lines[i].SetLineColor(13) # greay Lines 
  labels.append(ROOT.TLatex(xmax+0.02*(xmax-xmin), y * 0.8, "%d #sigma" % (i+1)))
  labels[i].SetTextColor(13)
  labels[i].SetTextAlign(11);
  lines[i].Draw('SAME')
  labels[i].Draw('SAME')

# draw CMS and lumi
l = ROOT.TLatex()
l.SetNDC()
l.SetTextSize(0.055)
l.SetTextFont(42)
l.SetTextAlign(13)
l.DrawLatex(0.13,.88,"#bf{CMS} #scale[0.75]{#it{Preliminary}}")
l.SetTextSize(0.035)
l.SetTextAlign(31)
l.DrawLatex(0.90,.91,"35.9 fb^{-1} (13 TeV)")

c.SetLogy(True)
c.Update()

raw_input("ok?")

doTable=True
if doTable:
    try:
        tex=open(opts.outname +".tex","w")
        print>>tex, "\\begin{tabular}{lc}"
        print>>tex, "\\hlinewd{1.2pt}"
        print>>tex, "$m_{H}$ & pval & sig.\\\\"
        print>>tex, "\\hline"
        for i in range(0,obs.GetN()):
            print>>tex, "$", obs.GetX()[i],"$ &",
            print>>tex, "$", obs.GetY()[i],"$ &",
            print>>tex, "$", ROOT.RooStats.PValueToSignificance(obs.GetY()[i]),"$ \\\\"
        print>>tex, "\\hlinewd{1.2pt}"
        print>>tex, "\\end{tabular}"
        tex.close()
    except IOError:
        print "Unable to open tex file"

c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")

