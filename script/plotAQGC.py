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
parser.add_option("-f","--file",dest="file",default="",type="string",help="Input file")
##
parser.add_option("","--more",dest="more",default="",type="string",help="Input files (more file:label,file:label)")

parser.add_option("-l","--label",dest="label",default="",type="string",help="Labels, or none or one for input file")
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C")
#parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true")
#parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
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

def GetLimitFromFile(fname,poi):
    fIn=ROOT.TFile.Open(fname)
    tree = fIn.Get('limit')

    res=[]
    for i in range(tree.GetEntries()):
      tree.GetEntry(i)
      xv = getattr(tree,poi)
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
    return res

def GetGraph(res):
    minNLL = min([re[1] for re in res])
    print "minNLL is",minNLL
    obs=ROOT.TGraph()
    for re, nll in res: 
        if (nll>=0. and nll<5) or (opts.tobaseline and nll>=0 and nll <=20) :
            obs.SetPoint(obs.GetN(),re,nll)
    return obs

res = GetLimitFromFile(opts.file,'VALUE')
obs=GetGraph(res)
graphs.append(obs)

## 68% and 95%
cl1=1
cl2=3.8414588

m,m1 = findQuantile(res,0);
l,h  = findQuantile(res,cl1);
l2,h2  = findQuantile(res,cl2);

##########
xmin = m
eplus = h-m
eminus = m-l
############
eplus2 = h2-m
eminus2 = m-l2


print "BestFit : %4.4f +%4.4g -%4.4g (95%% +%4.4g -%4.4g)" % ( xmin, eplus , eminus ,eplus2,eminus2)
#if True: smoothNLL(obs,res)
results ={}
results[opts.label] = {"bf":xmin, "68":[eminus, eplus],"95":[eminus2,eplus2]}

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


################## Draw results
obs.Draw("L")
idx=0
for label in graphsMore:
    idx+=1
    obsM,resM=graphsMore[label]
    obsM.SetLineWidth(3)
    obsM.Draw("L SAME")
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

l.DrawLatex(0.8,0.88-.1,("#hat{#mu}_{SM} = %"+opts.precision+" ^{#font[122]{+}%"+opts.precision+"}_{#font[122]{-}%"+opts.precision+"}")%(xmin,eplus,eminus))

if opts.more !="":
    for idx,label in enumerate(textMore):
        l.DrawLatex(0.37,0.88-.1 -.1*idx,textMore[label])

print
print "---------------------------"
print "Summary: %f + %f -%f ="%(xmin,eplus,eminus)
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

if opts.more != "":
    for label in graphsMore:
        obsM,more = graphsMore[label]
        leg.AddEntry(obsM,label,"L")
    leg.Draw()

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
        print>>tex, "%% ---------------------------"
        print>>tex, "\\begin{align}"
        print>>tex, "\\hat{\\mu}_\\textup{SM}&=%4.2f^{+%4.2f}_-%4.2f"%(xmin,eplus,eminus)
        print>>tex, "\\end{align}"
        tex.close()
    except IOError:
        print "Unable to open tex file"

c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")
import json
with open(opts.outname + ".json","w") as fout: 
    jstring = json.dumps(results,sort_keys=True, indent=4)
    fout.write(jstring+"\n")
    fout.close()

## Draw Panel. Use only results
doPanel=True
if doPanel:
    print "-> Looking for basepath"
    basepath = ""
    mypath = os.path.abspath(os.getcwd())
    while mypath != "" and mypath != "/":
        if "ChargedHiggs" in os.path.basename(mypath):
            basepath = os.path.abspath(mypath)
        mypath = os.path.dirname(mypath)
    print "-> Base Path is " + basepath
    sys.path.insert(0,basepath)
    sys.path.insert(0,basepath +"/python")

    from ParseDat import aqgc_names
    aqgc_parameters = sorted(list(set([ x.split('_')[0] for x in aqgc_names ]))) 

    ####################################### 
    nPoints=12
    xmin,xmax=-16.,16.
    ytick = 0.1
    bottom_margin=0.15
    top_margin=0.10
    right_margin=0.20
    left_margin=0.10
    ########################################

    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetOptStat(0)

    c=ROOT.TCanvas("c2","c2")
    c.SetCanvasSize(800,800)
    #c.SetBottomMargin(0.15)
    #c.SetLeftMargin(0.10)
    #c.SetTopMargin(0.10)

    p1 = ROOT.TPad("pad1","pad",0,0,0.25,1)
    p2 = ROOT.TPad("pad2","pad",0.5,0,.75,1)
    p3 = ROOT.TPad("pad1","pad",0.25,0,0.5,1)
    p4 = ROOT.TPad("pad2","pad",0.75,0,1,1)


    p1.SetBottomMargin(bottom_margin)
    p1.SetLeftMargin(left_margin)
    p1.SetTopMargin(top_margin)
    p1.SetRightMargin(0.00)

    p2.SetBottomMargin(bottom_margin)
    p2.SetLeftMargin(0.00)
    p2.SetTopMargin(top_margin)
    p2.SetRightMargin(0.00)

    p3.SetBottomMargin(bottom_margin)
    p3.SetLeftMargin(0.00)
    p3.SetTopMargin(top_margin)
    p3.SetRightMargin(0.00)

    p4.SetBottomMargin(bottom_margin)
    p4.SetLeftMargin(0.00)
    p4.SetTopMargin(top_margin)
    p4.SetRightMargin(right_margin)

    c.cd()
    p1.Draw()
    c.cd()
    p2.Draw()

    print "-> Drawing Canvas and pad"
    dummy = ROOT.TH2F("dummy","",1000,xmin,xmax,nPoints,0-0.5,nPoints-0.5);
    dummy.SetStats(0);
    ci = ROOT.TColor.GetColor("#00ff00");
    dummy.GetYaxis().SetBinLabel(1,"")
    dummy.GetXaxis().SetNdivisions(10)
    dummy.SetFillColor(ci);

    dummy2 = ROOT.TH2F("dummy2","",10,0.,10,nPoints,0-0.5,nPoints-0.5);
    dummy2.SetStats(0);
    ci = ROOT.TColor.GetColor("#00ff00");
    dummy2.SetFillColor(ci);
    dummy2.GetXaxis().SetBinLabel(1,"")
    dummy2.GetYaxis().SetBinLabel(1,"")
    dummy2.GetXaxis().SetTickSize(0.)
    dummy2.GetYaxis().SetTickSize(0.)

    p2.cd()
    dummy2.Draw("AXIS")
    p1.cd()
    dummy2.Draw("AXIS")
    c.cd()
    p3.Draw()
    p3.cd()
    dummy.Draw("AXIS")
    dummy.Draw("AXIS X+ Y+ SAME")
    c.cd()
    p4.Draw()
    p4.cd()
    dummy.Draw("AXIS")
    dummy.Draw("AXIS X+ Y+ SAME")

    gc=[]

    def draw_pad(y,xmin,xmax,side):
        return
        global dummy, gc,c
        c.cd()
        mydummy = dummy.Clone('dummy_%d'%len(gc))
        mydummy.GetXaxis().SetRangeUser(xmin,xmax)
        mydummy.GetYaxis().SetRangeUser(y-0.5,y+0.5)
        gc.append(mydummy)
        if side=='left': 
            padmin,padmax= 0.25,.5
        if side=='right': 
            padmin,padmax= 0.75,1.-right_margin

        padymin = (1.-bottom_margin-top_margin)/nPoints* (y-0.5) + bottom_margin
        padymax = (1.-bottom_margin-top_margin)/nPoints* (y+0.5) + bottom_margin
        p=ROOT.TPad("pad_%d"%len(gc),"",padmin,padymax,padmax,padymin)
        p.Draw()
        p.SetBottomMargin(0)
        p.SetTopMargin(0)
        p.SetRightMargin(0)
        p.SetLeftMargin(0)
        p.cd()
        mydummy.Draw("AXIS")
        mydummy.Draw("AXIS X+ Y+ SAME")
        mydummy.GetYaxis().SetTickSize(0)
        gc.append(p)

    def set_style(o,style):
        if o.InheritsFrom("TMarker") and style=='68':
            o.SetMarkerColor(ROOT.kBlack)
            o.SetMarkerStyle(20)
            o.SetMarkerSize(0.02)
        if o.InheritsFrom("TLine"):
            if style=='68':
                o.SetLineColor(ROOT.kBlack)
                o.SetLineWidth(3)
            if style=='95':
                o.SetLineColor(ROOT.kRed+2)
                o.SetLineWidth(2)
        if o.InheritsFrom("TArrow"):
            if style=='68':
                o.SetFillColor(ROOT.kBlack);
                o.SetAngle(40);
                o.SetArrowSize(0.02)
                o.SetOption(">")
            if style=='95':
                o.SetFillColor(ROOT.kRed+2);
                o.SetAngle(40);
                o.SetArrowSize(0.03)
                o.SetOption("|>")

    def draw_line( x, y, dxl,dxh, pad='left',style='68'):
        global gc
        global xmin,xmax
        if pad=='left': p3.cd()
        if pad=='right': p4.cd()
        print "DEBUG", "Drawing line @",x,y,dxl,dxh,pad,style
        
        xlow= x-dxl 
        xhigh= x+dxh
        
        openLeft=False 
        openRight=False 
        if xlow< xmin:
            openLeft=True
            xlow=xmin
        if xhigh> xmax:
            openRight=True
            xhigh=xmax
        
        line= ROOT.TLine(xlow,y,xhigh,y)
        set_style(line,style)
        line.Draw()
        gc.append(line)
        
        if style == '68' and xmin <x and x<xmax: 
            m= ROOT.TMarker(x,y,20)    
            set_style(m,style)
            m.Draw()
            gc.append(m)

        if not openLeft:
            t1 = ROOT.TLine(xlow,y -ytick,xlow ,y+ytick)
            set_style(t1,style)
            t1.Draw()
            gc.append(t1)
        else:
            a1 = ROOT.TArrow(xlow+ytick,y,xlow,y) #,0.02,"|>")### ytick is epsilon here
            set_style(a1,style)
            a1.Draw()
            gc.append(a1)

        if not openRight:
            t1 = ROOT.TLine(xhigh,y -ytick,xhigh ,y+ytick)
            set_style(t1,style)
            t1.Draw()
            gc.append(t1)
        else: 
            a1 = ROOT.TArrow(xhigh-ytick,y,xhigh,y) #,0.02,"|>")### ytick is epsilon here
            set_style(a1,style)
            a1.Draw()
            gc.append(a1)

    txt2=ROOT.TLatex()
    txt2.SetNDC(False)
    txt2.SetTextFont(42)
    txt2.SetTextSize(0.08)
    txt2.SetTextAlign(11)

    def draw_label(label, x,y, dxl, dxh, dxl2,dxh2,side):
        global gc
        global txt2
        if side=='left': p1.cd()
        if side=='right': p2.cd()
        #TODO #font[122]{+}
        txt2.DrawLatex(0.5,y,label + "   %.1f^{+%.1f}_{-%.1f}  ^{+%.1f}_{-%.1f}"%(x,dxh,dxl,dxh2,dxl2) )

    #fm0 fm1 fm2 fm3 fm4 fm5 fm7    fs0 fs1 
    #ft0 ft1 ft2 ft5 ft6 ft7 ft8 ft9    fs2

    left=['fm0','fm1','fm2','fm3','fm4','fm5','fm7','empty','fs0','fs1']
    right=['ft0','ft1','ft2','ft5','ft6','ft7','ft8','ft9','empty','fs2']
    
    ranges={ "fm0":[-10,10.],
            "fs0":[-16,16.],
            "default":[-16,16],
            }

    print "-> Drawing Lines and Labels"
    for label in results:
        print "Doing label",label
        if label not in left and label not in right:
            raise ValueError("label '"+label+"' not in: " +','.join(left)+','.join(right))
        side= 'left' if label in left else 'right' if label in right else None
        y = left.index(label) if label in left else right.index(label) if label in right else None
        draw_pad( y,ranges[label if label in ranges else "default"][0],ranges[label if label in ranges else "default"][1],side)
        draw_line( results[label]['bf'], y, results[label]['95'][0] ,results[label]['95'][1], side,'95')
        draw_line( results[label]['bf'], y, results[label]['68'][0] ,results[label]['68'][1], side,'68')
        draw_label( label, results[label]['bf'], y, results[label]['68'][0] ,results[label]['68'][1],  results[label]['95'][0] ,results[label]['95'][1], side)
    ### Draw the legend
    print "-> Drawing Text and co"
    c.cd()
    ### Draw text on Canvas
    c.cd()

    #eps=0.001
    #p5=ROOT.TPad("p5","p5",left_margin/4.+eps,1.-top_margin-0.1+eps,1.0-right_margin/4.-eps,1.-top_margin-eps) ##or until 0.5?
    #p5.SetBottomMargin(0)
    #p5.SetTopMargin(0)
    #p5.SetRightMargin(0)
    #p5.SetLeftMargin(0)
    #p5.SetBorderMode(1)
    #p5.SetBorderSize(1)
    #p5.SetBorderColor(ROOT.kBlack)
    #p5.Draw()
    #p5.cd()

    txt=ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextFont(43)
    txt.SetTextSize(20)
    txt.SetTextAlign(31)
    p2.cd()
    txt.DrawLatex(.90,.92,"138 fb^{-1} (13 TeV)")
    txt.SetTextSize(29)
    txt.SetTextAlign(13)
    p1.cd()
    if opts.paper:
        txt.DrawLatex(.21,.86,"#bf{CMS}")
    else:
        #0.21 0.86 on p1
        # 0.01 .95 on p5
        txt.DrawLatex(.21,.86,"#bf{CMS} #scale[0.7]{#it{Preliminary}}")
    print "-> Save"
    c.SaveAs(opts.outname +"_summary"+ ".pdf")
    c.SaveAs(opts.outname +"_summary"+ ".png")

    raw_input("ok?")




