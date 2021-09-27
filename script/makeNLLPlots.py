#!/usr/bin/env python

# Usual reasons to love python imports
import os
import sys
import array 
import math

from optparse import OptionParser
parser = OptionParser()
#parser.add_option("-f","--file",dest="files",default=[],action="append",help="Add a file")
parser.add_option("-f","--file",dest="file",default="",type="string",help="Input file")
parser.add_option("-s","--stat",dest="stat",default="",type="string",help="Input file Stat only")
##
parser.add_option("","--more",dest="more",default="",type="string",help="Input files (more file:label,file:label)")

#parser.add_option("-l","--label",dest="label",default="",type="string",help="Labels, or none or one for input file")
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C")
#parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true")
#parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
#parser.add_option("-u","--unblind",dest="unblind",default=False,action="store_true",help="Draw observation")
parser.add_option("-p","--poi",dest="poi",default="r",type="string",help="POI [%default]")
parser.add_option("","--xtitle",dest="xtitle",default=None,type="string",help="xtitle [%default]")
parser.add_option("","--xrange",dest="xrange",default=None,type="string",help="xrange [%default]")
parser.add_option("","--yrange",dest="yrange",default=None,type="string",help="yrange [%default]")
parser.add_option("","--tobaseline",dest="tobaseline",default=None,type="string",help="assume that r has value 0/1 only and one is the interesting value. Value r. [%default]")
##
#parser.add_option("","--addSM",dest="addSM",default=False,action="store_true",help="Add SM Diamond to 2D plot")
parser.add_option("","--run12",dest="run12",default=False,action="store_true")
parser.add_option("","--paper",dest="paper",default=False,action="store_true")
parser.add_option("","--nosmooth",dest="nosmooth",default=False,action="store_true",help="No Smooth")
parser.add_option("","--debug",dest="debug",default=False,action="store_true")
parser.add_option("","--precision",dest="precision",default="4.1f",help="precision [%default]")
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


fIn=ROOT.TFile.Open(opts.file)
tree = fIn.Get('limit')

res=[]
for i in range(tree.GetEntries()):
  tree.GetEntry(i)
  xv = getattr(tree,opts.poi)
  if isinstance(xv,ROOT.std.string):
      xv = float( str(xv).replace('p','.').replace('m','-'))
  if opts.tobaseline:
      r = getattr(tree,opts.tobaseline)
      if abs(r-0.00) <0.001 : continue
      print "BASELINE: ",xv, r, 2*tree.deltaNLL
  if tree.deltaNLL<0 and not opts.tobaseline : print "Warning, found -ve deltaNLL = ",  tree.deltaNLL, " at ", xv 
  if (2*tree.deltaNLL < 100) or opts.tobaseline: ## baseline may be arbitrary
    res.append([xv,2*tree.deltaNLL])

res.sort()
res=shiftToMinimum(res)
## clean spikes
if not opts.tobaseline: 
    res = cleanSpikes1D(res) 
minNLL = min([re[1] for re in res])
print "minNLL is",minNLL

obs=ROOT.TGraph()
for re, nll in res: 
    if (nll>=0. and nll<5) or (opts.tobaseline and nll>=0 and nll <=20) :
        obs.SetPoint(obs.GetN(),re,nll)
graphs.append(obs)
m,m1 = findQuantile(res,0);
l,h  = findQuantile(res,1);

xmin = m
eplus = h-m
eminus = m-l

print "BestFit : %4.4f +%4.4g -%4.4g" % ( xmin, eplus , eminus )
#if True: smoothNLL(obs,res)

stat=[]
obsStat=None
if opts.stat != "":
    fStat=ROOT.TFile.Open(opts.stat)
    if fStat == None: print "<*> ERROR: No such file or directory",opts.stat
    treeStat = fStat.Get('limit')
    if treeStat == None: print "<*> ERROR: No such tree: limit"
    for i in range(treeStat.GetEntries()):
      treeStat.GetEntry(i)
      xv = getattr(treeStat,opts.poi)
      if treeStat.deltaNLL<0 : print "Warning, found -ve deltaNLL = ",  treeStat.deltaNLL, " at ", xv 
      if 2*treeStat.deltaNLL < 100:
        stat.append([xv,2*treeStat.deltaNLL])
    stat.sort()
    stat=shiftToMinimum(stat)
    ## clean spikes
    stat = cleanSpikes1D(stat)
    minNLLStat = min([re[1] for re in stat])
    
    obsStat=ROOT.TGraph()
    for re, nll in stat: 
        if nll>=0. and nll<5:
            obsStat.SetPoint(obsStat.GetN(),re,nll)
    graphs.append(obsStat)
    m,m1 = findQuantile(stat,0);
    l,h  = findQuantile(stat,1);
    
    
    #xminStat = m
    #eplusStat = h-m
    #eminusStat = m-l
    xminStat = m
    eplusStat = h-xmin
    eminusStat = xmin-l
    
    print "BestFit (Stat)  : %4.4f +%4.4g -%4.4g (==%4.4g)" % ( xminStat, eplusStat , eminusStat,xmin )

graphsMore={}
textMore={}

if opts.more != "":
    for s in opts.more.split(','):
        more=[]
        if len(s.split(':')) == 2:
            filename,label=s.split(':')
            dosmooth=True
        else:
            filename,label,smoothstr=s.split(':')
            dosmooth=True
            if smoothstr=='false':
                dosmooth=False
                print "unsetting smoothing for",label
        print "Doing label",label
        obsM=None
        fIn=ROOT.TFile.Open(filename)
        if fIn == None: print "<*> ERROR: No such file or directory",filename
        treeM = fIn.Get('limit')
        if treeM == None: print "<*> ERROR: No such tree: limit for file:",filename
        for i in range(treeM.GetEntries()):
          treeM.GetEntry(i)
          xv = getattr(treeM,opts.poi)
          if treeM.deltaNLL<0 : print "Warning, found -ve deltaNLL = ",  treeM.deltaNLL, " at ", xv 
          if 2*treeM.deltaNLL < 100:
            more.append([xv,2*treeM.deltaNLL])
        more.sort()
        print "-> found",len(more),"entries"
        ## clean spikes
        more = cleanSpikes1D(more)
        minNLLM = min([re[1] for re in more])
    
        obsM=ROOT.TGraph()
        for re, nll in more: 
           if (nll>=0. and nll<5) or (opts.tobaseline):
               obsM.SetPoint(obsM.GetN(),re,nll)
        m,m1 = findQuantile(more,0);
        l,h  = findQuantile(more,1);
        obsM.SetName(label)
        obsM.SetTitle(label)

        graphsMore[label]=(obsM,more,dosmooth)
    
        xminM = m
        eplusM = h-xmin
        eminusM = xmin-l
        print "BestFit (",label,")  : %4.4f +%4.4g -%4.4g (==%4.4g)" % ( xminM, eplusM , eminusM,xmin )

if opts.nosmooth:
    obs.Draw("L")
    print "->DEBUG"
    if opts.debug:
        obs2= smoothNLL_v3(obs,res)
        obs2.SetLineColor(ROOT.kRed)
        obs2.Draw("C SAME")

    if opts.stat!="":
        obsStat.SetLineColor(ROOT.kBlue)
        obsStat . SetMarkerColor(ROOT.kBlue)
        obsStat . SetLineStyle(7)
        obsStat . Draw("L SAME")

else:
    obs2= smoothNLL_v3(obs,res)
    obs2.Draw("C SAME")

    if opts.stat!="":
        obsStat2 = smoothNLL_v3(obsStat,stat)
        obsStat2 . SetLineColor(ROOT.kBlue)
        obsStat2 . SetMarkerColor(ROOT.kBlue)
        obsStat2 . SetLineStyle(7)
        obsStat2 . Draw("C SAME")

    if opts.more !="":
        color = [ROOT.kRed,ROOT.kGreen,ROOT.kOrange,ROOT.kCyan]
        for idx,label in enumerate(graphsMore):
            obsM,more,dosmooth = graphsMore[label]
            print "processing (v2)",label, obsM.GetName(),len(more)
            if dosmooth:
                obsM2 = smoothNLL_v3(obsM,more)
            else:
                obsM2=obsM
            obsM2.SetLineColor( color[idx%len(color)] )
            obsM2.SetTitle(label)
            obsM2.Draw("C SAME")
            more2=[]
            for i in range(0,obsM2.GetN()):
                more2.append( [ obsM2.GetX()[i],obsM2.GetY()[i]]) 
            m,m1 = findQuantile(more2,0);
            l,h  = findQuantile(more2,1);
            xminSmooth = m
            eplusM = h-xmin
            eminusM = xmin-l
            print "BestFit (",label,"Smooth) : %4.4f +%4.4g -%4.4g" % ( xminSmooth, eplusM , eminusM )
            # override for legend
            obsM.SetLineColor(color[idx%len(color)] )
            graphsMore[label] = (obsM2,more,dosmooth)
            textMore[label]= (label+":" +"#hat{#mu}_{SM} = %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"}")%(xminSmooth,eplusM,eminusM)
            
            if opts.debug:
                obsM.SetLineWidth(3)
                obsM.Draw("L SAME")
                graphs.append(obsM)

    res_smooth = []
    for i in range(0,obs2.GetN()):
        res_smooth.append( [ obs2.GetX()[i],obs2.GetY()[i]]) 
    m,m1 = findQuantile(res_smooth,0);
    l,h  = findQuantile(res_smooth,1);
    xminSmooth = m
    #if not opts.run12:
    eplusSmooth = h-xmin
    eminusSmooth = xmin-l
    print "BestFit (Smooth) : %4.4f +%4.4g -%4.4g" % ( xminSmooth, eplusSmooth , eminusSmooth )
    ## OVERRIDE xmin, eplus eminus
    if True:
        xmin=xminSmooth
        eplus=eplusSmooth
        eminus=eminusSmooth

    if opts.stat!="":
        res_stat_smooth = []
        for i in range(0,obsStat2.GetN()):
            res_stat_smooth.append( [ obsStat2.GetX()[i],obsStat2.GetY()[i]]) 
        m,m1 = findQuantile(res_stat_smooth,0);
        l,h  = findQuantile(res_stat_smooth,1);
        xminStat = m
        if not opts.run12:
            eplusStat = h-xmin
            eminusStat = xmin-l
        print "BestFit (Smooth,Stat)  : %4.4f +%4.4g -%4.4g (==%4.4g)" % ( xminStat, eplusStat , eminusStat,xmin )

if opts.stat!="":
    try:
        eplusSyst= math.sqrt( eplus**2 - eplusStat**2)
    except:
        eplusSyst = 0.0
    try:
        eminusSyst= math.sqrt( eminus**2 - eminusStat**2)
    except:
        eminusSyst = 0.0
# draw fit value
l = ROOT.TLatex()
l.SetNDC()
l.SetTextFont(42)

l.SetTextAlign(23)
l.SetTextSize(0.045)
#l.DrawLatex(0.8,0.88,"H#rightarrow#mu#mu")

l.SetTextSize(0.045)
l.SetTextAlign(23)
if opts.stat == "":
    l.DrawLatex(0.8,0.88-.1,("#hat{#mu}_{SM} = %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"}")%(xmin,eplus,eminus))
    #if opts.run12:
    #else:
    #    l.DrawLatex(0.8,0.88-.1,"#hat{#mu}_{SM} = %4.1f ^{#font[122]{+}%4.1f}_{#font[122]{-}%4.1f}"%(xmin,eplus,eminus))
else:
    l.DrawLatex(0.37,0.88-.1,("#splitline{#hat{#mu}_{SM} = %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"}}{#scale[0.7]{ =  %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"} (stat) #oplus ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"} (syst)}}")%(xmin,eplus,eminus,xmin,eplusStat,eminusStat ,eplusSyst, eminusSyst ))

if opts.more !="":
    for idx,label in enumerate(textMore):
        l.DrawLatex(0.37,0.88-.1 -.1*idx,textMore[label])

print
print "---------------------------"
print "Summary: %f + %f -%f ="%(xmin,eplus,eminus)
if opts.stat != "":
    print "         = %f +%f -%f (stat) "%(xmin,eplusStat,eminusStat)
    print "        ++ +%f -%f (syst) "%( eplusSyst, eminusSyst )
print "---------------------------"
print

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
if opts.stat != "" and not opts.nosmooth:
   obs2.SetFillStyle(0)
    
   # DEBUG
   ### obsStat . SetLineStyle(7)
   ### obsStat.SetMarkerColor(ROOT.kRed)
   ### obsStat.SetLineColor(ROOT.kRed)
   ### obsStat.SetMarkerStyle(20)
   ### obsStat.Draw("PL SAME")

   obsStat2.SetFillStyle(0)
   leg . AddEntry (obs2,"Total uncertainty","L")
   leg . AddEntry (obsStat2,"Statistical only","L")
   leg . Draw()

if opts.more != "":
    for label in graphsMore:
        obsM,more,dosmooth = graphsMore[label]
        leg.AddEntry(obsM,label,"L")



if opts.run12:
    l.DrawLatex(0.89,.91,"5.0 fb^{-1} (7 TeV) + 19.8 fb^{-1} (8 TeV) + 138 fb^{-1} (13 TeV)")
else:
    l.DrawLatex(0.89,.91,"138 fb^{-1} (13 TeV)")


c.Update()

raw_input("ok?")

doTable=True
if doTable:
    try:
        tex=open(opts.outname +".tex","w")
        #l.DrawLatex(0.5,0.85,"#hat{#mu}_{SM} = %4.2f ^{#font[122]{+}%4.2f}_{#font[122]{-}%4.2f}"%(fit,eplus,eminus))
        print>>tex, "%% ---------------------------"
        print>>tex, "%%%% Summary: %f + %f -%f ="%(xmin,eplus,eminus)
        if opts.stat != "":
            try:
                eplusSyst= math.sqrt( eplus**2 - eplusStat**2)
            except:
                eplusSyst = 0.0
            try:
                eminusSyst= math.sqrt( eminus**2 - eminusStat**2)
            except:
                eminusSyst = 0.0
            print>>tex, "%%%%         = %f +%f -%f (stat) "%(xmin,eplusStat,eminusStat)
            print>>tex, "%%%%        ++ +%f -%f (syst) "%(eplusSyst, eminusSyst )
        print>>tex, "%% ---------------------------"
        print>>tex, "\\begin{align}"
        print>>tex, "\\hat{\\mu}_\\textup{SM}&=%4.2f^{+%4.2f}_-%4.2f"%(xmin,eplus,eminus)
        if opts.stat != "":
            print >>tex, "\\\\ &= %4.2f +%4.2f -%4.2f (stat) "%(xmin,eplusStat,eminusStat)
            print "        \\oplus +%4.2f -%4.2f (syst) "%(eplusSyst, eminusSyst )
        print>>tex, "\\end{align}"
        tex.close()
    except IOError:
        print "Unable to open tex file"

c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")

