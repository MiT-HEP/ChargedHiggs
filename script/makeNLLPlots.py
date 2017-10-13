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
#parser.add_option("-u","--unblind",dest="unblind",default=False,action="store_true",help="Draw observation")
parser.add_option("-p","--poi",dest="poi",default="r",type="string",help="POI [%default]")
##
#parser.add_option("","--addSM",dest="addSM",default=False,action="store_true",help="Add SM Diamond to 2D plot")
parser.add_option("","--run12",dest="run12",default=False,action="store_true")
parser.add_option("","--paper",dest="paper",default=False,action="store_true")
parser.add_option("","--nosmooth",dest="nosmooth",default=False,action="store_true",help="No Smooth")
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT

def findQuantile(pts,cl):

	#gr is a list of r,nll
	# start by walking along the variable and check if crosses a CL point
  
	if cl<=0:  
	 min=pts[0][0]
	 mincl=pts[0][1]
	 for pt in pts: 
		if pt[1]<mincl: 
			mincl=pt[1]
			min = pt[0]
     
	 return min,min

	crossbound = [ pt[1]<=cl for pt in pts ]
	rcrossbound = crossbound[:]
	rcrossbound.reverse()

	minci = 0
	maxci = len(crossbound)-1
	min = pts[0][0]
	max = pts[maxci][0]

	for c_i,c in enumerate(crossbound): 
		if c : 
			minci=c_i
			break
	
	for c_i,c in enumerate(rcrossbound): 
		if c : 
			maxci=len(rcrossbound)-c_i-1
			break

	if minci>0: 
		y0,x0 = pts[minci-1][0],pts[minci-1][1]
		y1,x1 = pts[minci][0],pts[minci][1]
		min = y0+((cl-x0)*y1 - (cl-x0)*y0)/(x1-x0)
		
	if maxci<len(crossbound)-1: 
		y0,x0 = pts[maxci][0],pts[maxci][1]
		y1,x1 = pts[maxci+1][0],pts[maxci+1][1]
		max = y0+((cl-x0)*y1 - (cl-x0)*y0)/(x1-x0)

	return min,max

def smoothNLL(gr,res):
  print "-> Smooth"
  minVal = min([re[0] for re in res])
  maxVal = max([re[0] for re in res])
  sp = ROOT.TSpline3('sp_%s'%gr.GetName(),gr,"",minVal,maxVal)
  gr = ROOT.TGraph()
  for p in range(100):
    x = minVal+p*((maxVal-minVal)/100.)
    y = sp.Eval(x)
    if y<5:
        gr.SetPoint(p,x,y)

def smoothNLL_v2(gr,res,w=0.3):

  print "-> Smooth v2"
  minVal = min([re[0] for re in res])
  maxVal = max([re[0] for re in res])

  gr2=ROOT.TGraph()

  myfunc = ROOT.TF1("myfunc","pol2",-100,100)
  for p in range(100):
    x = minVal+p*((maxVal-minVal)/100.)
    fitmin = x-w
    fitmax = x+w
    myfunc.SetRange(fitmin,fitmax)
    gr.Fit("myfunc","RQN")
    gr.Fit("myfunc","RQNM")
    y = myfunc.Eval(x)
    if y<5:
        gr2.SetPoint(p,x,y)
  return gr2

def cleanSpikes1D(rfix):

 # cindex is where deltaNLL = 0 (pre anything)
 MAXDER = 1.0
 for i,r in enumerate(rfix):
   if abs(r[1]) <0.001: cindex = i

 lhs = rfix[0:cindex]; lhs.reverse()
 rhs= rfix[cindex:-1]
 keeplhs = []
 keeprhs = []

 for i,lr in enumerate(lhs): 
   if i==0: 
        prev = lr[1]
        idiff = 1
   if abs(lr[1]-prev) > MAXDER :
        idiff+=1
        continue 
   keeplhs.append(lr)
   prev = lr[1]
   idiff=1
 keeplhs.reverse()

 for i,rr in enumerate(rhs):
   if i==0: 
    prev = rr[1]
    idiff = 1
   if abs(rr[1]-prev) > MAXDER : 
   	idiff+=1
   	continue 
   keeprhs.append(rr)
   prev = rr[1]
   idiff=1
 
 rfix = keeplhs+keeprhs
 
 rkeep = []
 #now try to remove small jagged spikes
 for i,r in enumerate(rfix):
   if i==0 or i==len(rfix)-1: 
   	rkeep.append(r)
   	continue
   tres = [rfix[i-1][1],r[1],rfix[i+1][1]]
   mean = float(sum(tres))/3.
   mdiff = abs(max(tres)-min(tres))
   if abs(tres[1] - mean) > 0.6*mdiff :continue
   rkeep.append(r)
 return rkeep


objs=[]
graphs=[]

c=ROOT.TCanvas()
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)
xmin=-1
xmax=6
dummy = ROOT.TH1D("dummy","dummy",1000, xmin,xmax)
dummy.GetXaxis().SetRangeUser(xmin,xmax)
#dummy.GetYaxis().SetRangeUser(1.e-10,1)
dummy.GetYaxis().SetRangeUser(0,6)

dummy.GetXaxis().SetTitle("#mu")
dummy.GetYaxis().SetTitle("-2 #Delta Ln L")

dummy.GetXaxis().SetTitleSize(0.05)
dummy.GetYaxis().SetTitleSize(0.05)
dummy.GetXaxis().SetTitleOffset(1.2)
dummy.GetYaxis().SetTitleOffset(1.2)
dummy.GetXaxis().SetLabelSize(0.04)
dummy.GetYaxis().SetLabelSize(0.04)
dummy.GetXaxis().SetLabelOffset(0.01)
dummy.GetYaxis().SetLabelOffset(0.01)

dummy.Draw("AXIS")
dummy.Draw("AXIG SAME")


fIn=ROOT.TFile.Open(opts.file)
tree = fIn.Get('limit')
res=[]
for i in range(tree.GetEntries()):
  tree.GetEntry(i)
  xv = getattr(tree,opts.poi)
  if tree.deltaNLL<0 : print "Warning, found -ve deltaNLL = ",  tree.deltaNLL, " at ", xv 
  if 2*tree.deltaNLL < 100:
    res.append([xv,2*tree.deltaNLL])
res.sort()
## clean spikes
res = cleanSpikes1D(res)
minNLL = min([re[1] for re in res])

obs=ROOT.TGraph()
for re, nll in res: 
    if nll>=0. and nll<5:
        obs.SetPoint(obs.GetN(),re,nll)
graphs.append(obs)
m,m1 = findQuantile(res,0);
l,h  = findQuantile(res,1);
l2,h2  = findQuantile(res,4);


xmin = m
eplus = h-m
eminus = m-l
eplus2 = h2-m
eminus2 = m-l2

print "BestFit : %4.4f +%4.4g -%4.4g" % ( xmin, eplus , eminus )
#if True: smoothNLL(obs,res)

if opts.nosmooth:
    obs.Draw("L")
    #obs2= smoothNLL_v2(obs,res)
    #obs2.SetLineColor(ROOT.kRed)
    #obs2.Draw("C SAME")

else:
    obs2= smoothNLL_v2(obs,res)
    #obs2.SetLineColor(ROOT.kRed)
    obs2.Draw("C SAME")


# draw fit value
l = ROOT.TLatex()
l.SetNDC()
l.SetTextFont(42)

l.SetTextAlign(23)
l.SetTextSize(0.045)
l.DrawLatex(0.8,0.88,"H#rightarrow#mu#mu")

l.SetTextSize(0.045)
l.SetTextAlign(23)
l.DrawLatex(0.8,0.88-.1,"#hat{#mu}_{SM} = %4.1f ^{#font[122]{+}%4.1f}_{#font[122]{-}%4.1f}"%(xmin,eplus,eminus))

l.SetTextFont(42)
l.SetTextSize(0.055)
l.SetTextAlign(13)
if opts.paper:
    l.DrawLatex(0.18,.88,"#bf{CMS}")
else:
    l.DrawLatex(0.18,.88,"#bf{CMS} #scale[0.75]{#it{Preliminary}}")
l.SetTextSize(0.035)
l.SetTextAlign(31)

if opts.run12:
    l.DrawLatex(0.90,.91,"5.0 fb^{-1} (7 TeV) + 19.8 fb^{-1} (8 TeV) + 35.9 fb^{-1} (13 TeV)")
else:
    l.DrawLatex(0.90,.91,"35.9 fb^{-1} (13 TeV)")


c.Update()

raw_input("ok?")

doTable=True
if doTable:
    try:
        tex=open(opts.outname +".tex","w")
        #l.DrawLatex(0.5,0.85,"#hat{#mu}_{SM} = %4.2f ^{#font[122]{+}%4.2f}_{#font[122]{-}%4.2f}"%(fit,eplus,eminus))
        print>>tex, "\\begin{align}"
        print>>tex, "\\hat{\\mu}_\\textup{SM}=%4.2f^{+%4.2f}_-%4.2f"%(xmin,eplus,eminus)
        print>>tex, "\\end{align}"
        tex.close()
    except IOError:
        print "Unable to open tex file"

c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")

