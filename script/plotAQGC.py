#!/usr/bin/env python

# Usual reasons to love python imports
import os
import sys
import array 
import math

''' Modified version of makeNLL for AQGC parameters'''

from optparse import OptionParser
parser = OptionParser()
#parser.add_option("-f","--file",dest="files",default=[],action="append",help="Add a file")
##
parser.add_option("","--more",dest="more",default="",type="string",help="Input files (more file:label,file:label)")

parser.add_option("-l","--label",dest="label",default="",type="string",help="Labels, or none or one for input file")
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C")
#parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true")
parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
#parser.add_option("-u","--unblind",dest="unblind",default=False,action="store_true",help="Draw observation")
#parser.add_option("-p","--poi",dest="poi",default="r",type="string",help="POI [%default]")
parser.add_option("","--xtitle",dest="xtitle",default=None,type="string",help="xtitle [%default]")
parser.add_option("","--xrange",dest="xrange",default=None,type="string",help="xrange [%default]")
parser.add_option("","--yrange",dest="yrange",default=None,type="string",help="yrange [%default]")
parser.add_option("","--tobaseline",dest="tobaseline",default='r',type="string",help="assume that r has value 0/1 only and one is the interesting value. Value r. [%default]")
##
parser.add_option("","--paper",dest="paper",default=False,action="store_true")
parser.add_option("","--nosmooth",dest="nosmooth",default=False,action="store_true",help="No Smooth")
parser.add_option("","--debug",dest="debug",default=False,action="store_true")
parser.add_option("","--precision",dest="precision",default="4.1f",help="precision [%default]")
(opts,args)=parser.parse_args()

if opts.batch: sys.argv=["-b-"]
import ROOT
if opts.batch: ROOT.gROOT.SetBatch()

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

def shiftToMinimum(res):
    minNLL = min([re[1] for re in res])
    res2=[]
    for xv,nll in res:
        res2.append( (xv,nll - minNLL) ) 
    #print "->Found minimum at",xv,nll,"->0"
    return res2

def smoothNLL(gr,res):
  print "-> Smooth"
  minVal = min([re[0] for re in res])
  maxVal = max([re[0] for re in res])
  sp = ROOT.TSpline3('sp_%s'%gr.GetName(),gr,"",minVal,maxVal)
  gr = ROOT.TGraph()
  for p in range(100):
    x = minVal+p*((maxVal-minVal)/100.)
    y = sp.Eval(x)
    if y<5 :# and not ( abs(x)<0.001 and abs(y)<0.001):
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
        gr2.SetPoint(gr2.GetN(),x,y)
  return gr2

def smoothNLL_v3(gr,res,w=0.3,delta=0.05):
    print "-> Smooth v3 <-"
    gr2 = smoothNLL_v2(gr,res,w)
    print "-> ",gr.GetName(),"len2,=",gr2.GetN()
    gr3 = ROOT.TGraph()
    for x,y in res:
        y2 = gr2.Eval(x)
        if abs(y-y2) < delta:
            gr3.SetPoint(gr3.GetN(), x,y ) 
    print "-> ",gr.GetName(),"len3,=",gr3.GetN()
    gr4 = smoothNLL_v2(gr3,res,w)
    print "-> ",gr.GetName(),"len4,=",gr4.GetN()
    return gr4

def cleanSpikes1D(rfix):

 # cindex is where deltaNLL = 0 (pre anything)
 cindex=-1
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


def Draw(obs, label,xobs,eplus,eminus):
    c=ROOT.TCanvas()
    c.SetCanvasSize(700,500)
    c.SetBottomMargin(0.15)
    c.SetLeftMargin(0.10)
    c.SetTopMargin(0.10)

    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetOptStat(0)
    xmin=-1
    xmax=6
    if opts.xrange: 
        xmin= float(opts.xrange.split(',')[0])
        xmax= float(opts.xrange.split(',')[1])
    dummy = ROOT.TH1D("dummy","dummy",1000, xmin,xmax)
    dummy.GetXaxis().SetRangeUser(xmin,xmax)
    #dummy.GetYaxis().SetRangeUser(1.e-10,1)
    dummy.GetYaxis().SetRangeUser(0,6)
    if opts.yrange:
        ymin= float(opts.yrange.split(',')[0])
        ymax= float(opts.yrange.split(',')[1])
        dummy.GetYaxis().SetRangeUser(ymin,ymax)


    dummy.GetXaxis().SetTitle("#mu")
    if opts.xtitle:  dummy.GetXaxis().SetTitle(opts.xtitle)

    dummy.GetYaxis().SetTitle("-2 #Delta Ln L")

    dummy.GetXaxis().SetTitleSize(0.05)
    dummy.GetYaxis().SetTitleSize(0.05)
    dummy.GetXaxis().SetTitleOffset(1.2)
    dummy.GetYaxis().SetTitleOffset(1.0)
    dummy.GetXaxis().SetLabelSize(0.045)
    dummy.GetYaxis().SetLabelSize(0.045)
    dummy.GetXaxis().SetLabelOffset(0.01)
    dummy.GetYaxis().SetLabelOffset(0.01)

    dummy.Draw("AXIS")
    dummy.Draw("AXIG SAME")
    
    obs.SetMarkerStyle(20)
    obs.SetMarkerColor(ROOT.kBlack)
    obs.SetMarkerSize(0.8)
    obs.SetLineColor(ROOT.kBlack)
    obs.Draw("LP")

    l = ROOT.TLatex()
    l.SetNDC()
    l.SetTextFont(42)

    l.SetTextAlign(23)
    l.SetTextSize(0.045)
    l.DrawLatex(0.8,0.88,"VBS Hadronic")

    l.SetTextSize(0.045)
    l.SetTextAlign(23)

    l.DrawLatex(0.8,0.88-.1,(label + " = %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"}")%(xobs,eplus,eminus))

    l.SetTextFont(42)
    l.SetTextSize(0.06)
    l.SetTextAlign(13)
    if opts.paper:
        l.DrawLatex(0.18,.88,"#bf{CMS}")
    else:
        l.DrawLatex(0.18,.88,"#bf{CMS} #scale[0.75]{#it{Preliminary}}")
    l.SetTextSize(0.035)
    l.SetTextAlign(31)

    leg = ROOT.TLegend(0.6,0.3,.8,.5)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)

    l.DrawLatex(0.89,.91,"138 fb^{-1} (13 TeV)")

    c.Update()

    c.SaveAs(opts.outname + "_" + label+ ".pdf")
    c.SaveAs(opts.outname + "_" + label+ ".png")


    ##########################
objs=[]
graphs=[]

c=ROOT.TCanvas()
c.SetCanvasSize(700,500)
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.10)
c.SetTopMargin(0.10)

ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)
xmin=-1
xmax=6
if opts.xrange: 
    xmin= float(opts.xrange.split(',')[0])
    xmax= float(opts.xrange.split(',')[1])
dummy = ROOT.TH1D("dummy","dummy",1000, xmin,xmax)
dummy.GetXaxis().SetRangeUser(xmin,xmax)
#dummy.GetYaxis().SetRangeUser(1.e-10,1)
dummy.GetYaxis().SetRangeUser(0,6)
if opts.yrange:
    ymin= float(opts.yrange.split(',')[0])
    ymax= float(opts.yrange.split(',')[1])
    dummy.GetYaxis().SetRangeUser(ymin,ymax)


dummy.GetXaxis().SetTitle("#mu")
if opts.xtitle:  dummy.GetXaxis().SetTitle(opts.xtitle)

dummy.GetYaxis().SetTitle("-2 #Delta Ln L")

dummy.GetXaxis().SetTitleSize(0.05)
dummy.GetYaxis().SetTitleSize(0.05)
dummy.GetXaxis().SetTitleOffset(1.2)
dummy.GetYaxis().SetTitleOffset(1.0)
dummy.GetXaxis().SetLabelSize(0.045)
dummy.GetYaxis().SetLabelSize(0.045)
dummy.GetXaxis().SetLabelOffset(0.01)
dummy.GetYaxis().SetLabelOffset(0.01)

dummy.Draw("AXIS")
dummy.Draw("AXIG SAME")

def GetLimitFromFile(fname,poi):
    print "> Opening file",fname
    fIn=ROOT.TFile.Open(fname)
    if fIn ==None: print "No such file",fname
    tree = fIn.Get('limit')
    if tree ==None: print "No limit tree in",fname

    res=[]
    for i in range(tree.GetEntries()):
      tree.GetEntry(i)
      xv = getattr(tree,poi)
      if isinstance(xv,ROOT.std.string):
          xv = float( str(xv).replace('p','.').replace('m','-'))
      if opts.tobaseline:
          r = getattr(tree,opts.tobaseline)
          if abs(r-0.00) <0.001 : continue
          #print "BASELINE: ",xv, r, 2*tree.deltaNLL
      if tree.deltaNLL<0 and not opts.tobaseline : print "Warning, found -ve deltaNLL = ",  tree.deltaNLL, " at ", xv 
      if (2*tree.deltaNLL < 100) or opts.tobaseline: ## baseline may be arbitrary
        res.append([xv,2*tree.deltaNLL])

    res.sort()
    res=shiftToMinimum(res)
    ## clean spikes
    if not opts.tobaseline:  res = cleanSpikes1D(res) 
    print "> ",len(res), "good entries over",tree.GetEntries()/2 if opts.tobaseline else tree.GetEntries()
    return res

def GetGraph(res):
    minNLL = min([re[1] for re in res])
    print "minNLL is",minNLL
    obs=ROOT.TGraph()
    for re, nll in res: 
        if (nll>=0. and nll<5) or (opts.tobaseline and nll>=0 and nll <=20) :
            obs.SetPoint(obs.GetN(),re,nll)
    return obs

#res = GetLimitFromFile(opts.file,'VALUE')
#obs=GetGraph(res)
#graphs.append(obs)

## 68% and 95%
cl1=1
cl2=3.8414588

#m,m1 = findQuantile(res,0);
#l,h  = findQuantile(res,cl1);
#l2,h2  = findQuantile(res,cl2);
#
###########
#xmin = m
#eplus = h-m
#eminus = m-l
#############
#eplus2 = h2-m
#eminus2 = m-l2


#print "BestFit : %4.4f +%4.4g -%4.4g (95%% +%4.4g -%4.4g)" % ( xmin, eplus , eminus ,eplus2,eminus2)
#if True: smoothNLL(obs,res)
results ={}
#results[opts.label] = {"bf":xmin, "68":[eminus, eplus],"95":[eminus2,eplus2]}

graphsMore={}
textMore={}

if opts.more != "":
    for s in opts.more.split(','):
        if len(s.split(':')) == 2:
            filename,label=s.split(':')
        else:
            filename,label,sm=s.split(':')
        print "Doing label",label

        resM = GetLimitFromFile(filename,'VALUE')
        obsM = GetGraph(resM)

        m,m1 = findQuantile(resM,0);
        l,h  = findQuantile(resM,cl1);
        l2,h2  = findQuantile(resM,cl2);
        obsM.SetName(label)
        obsM.SetTitle(label)


        xminM = m
        eplusM = h-xminM
        eminusM = xminM-l
        eplusM2 = h2-xminM
        eminusM2 = xminM-l2

        graphsMore[label]=(obsM,resM)
        textMore[label]= (label+":" +"#hat{#mu}_{SM} = %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"}")%(xminM,eplusM,eminusM)
    
        print "BestFit (",label,")  : %4.4f +%4.4g -%4.4g (95%% +%4.4g -%4.4g  ) " % ( xminM, eplusM , eminusM, eplusM2,eminusM2 )
        results[label] = {"bf":xminM, "68":[eminusM, eplusM],"95":[eminusM2,eplusM2]}
        
        ## draw each canvas
        Draw( obsM,label,xminM,eplusM,eminusM)


################## Draw results
#obs.Draw("L")
idx=0
for label in graphsMore:
    idx+=1
    obsM,resM=graphsMore[label]
    obsM.SetLineWidth(3)
    if idx == 0 :obsM.Draw("L")
    else: obsM.Draw("L SAME")
    obsM.SetLineColor(ROOT.kBlue+idx)

# draw fit value
l = ROOT.TLatex()
l.SetNDC()
l.SetTextFont(42)

l.SetTextAlign(23)
l.SetTextSize(0.045)
l.DrawLatex(0.8,0.88,"VBS Hadronic")

l.SetTextSize(0.045)
l.SetTextAlign(23)

#l.DrawLatex(0.8,0.88-.1,("#hat{#mu}_{SM} = %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"}")%(xmin,eplus,eminus))

if opts.more !="":
    for idx,label in enumerate(textMore):
        l.DrawLatex(0.37,0.88-.1 -.1*idx,textMore[label])

l.SetTextFont(42)
l.SetTextSize(0.06)
l.SetTextAlign(13)
if opts.paper:
    l.DrawLatex(0.18,.88,"#bf{CMS}")
else:
    l.DrawLatex(0.18,.88,"#bf{CMS} #scale[0.75]{#it{Preliminary}}")
l.SetTextSize(0.035)
l.SetTextAlign(31)

leg = ROOT.TLegend(0.6,0.3,.8,.5)
leg.SetFillStyle(0)
leg.SetBorderSize(0)

if opts.more != "":
    for label in graphsMore:
        obsM,more = graphsMore[label]
        leg.AddEntry(obsM,label,"L")
    leg.Draw()

l.DrawLatex(0.89,.91,"138 fb^{-1} (13 TeV)")

c.Update()

#raw_input("ok?")
c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")
import json
with open(opts.outname + ".json","w") as fout: 
    jstring = json.dumps(results,sort_keys=True, indent=4)
    fout.write(jstring+"\n")
    fout.close()


