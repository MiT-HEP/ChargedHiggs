import os, sys

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-o","--outname",dest="outname",help="Name of output pdf/png/C",default="")
#parser.add_option("","--nosmooth",dest="nosmooth",default=False,action="store_true",help="No Smooth")
(opts,args)=parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

if  True:
    print "-> Looking for basepath"
    basepath = ""
    #mypath = os.path.abspath(os.getcwd())
    mypath = os.path.abspath(os.environ['PWD'])
    while mypath != "" and mypath != "/":
        if "ChargedHiggs" in os.path.basename(mypath):
            basepath = os.path.abspath(mypath)
        mypath = os.path.dirname(mypath)
    print "-> Base Path is " + basepath
    status=sys.path.insert(0,basepath)
    status=sys.path.insert(0,basepath+'/python')
    status=sys.path.insert(0,basepath+'/script')

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

def FindBF(fname,poi="r"):
    fIn=ROOT.TFile.Open(fname)
    if fIn == None:
        print "<*> ERROR: '"+fname+"': no such file or directory"
        raise IOError
    tree = fIn.Get('limit')
    if tree == None:
        print "<*> ERROR: '"+fname+"': no tree inside"
        raise IOError

    res=[]
    for i in range(tree.GetEntries()):
      tree.GetEntry(i)
      xv = getattr(tree,poi)
      if tree.deltaNLL<0 : print "Warning, found -ve deltaNLL = ",  tree.deltaNLL, " at ", xv 
      if 2*tree.deltaNLL < 100:
        res.append([xv,2*tree.deltaNLL])
    res.sort()
    ## clean spikes
    res = cleanSpikes1D(res)
    minNLL = min([re[1] for re in res])
    
    ##obs=ROOT.TGraph()
    ##for re, nll in res: 
    ##    if nll>=0. and nll<5:
    ##        obs.SetPoint(obs.GetN(),re,nll)
    ##graphs.append(obs)
    m,m1 = findQuantile(res,0);
    l,h  = findQuantile(res,1);
    l2,h2  = findQuantile(res,4);
    
    xmin = m
    eplus = h-m
    eminus = m-l
    eplus2 = h2-m
    eminus2 = m-l2
    
    print fname,"| BestFit: %4.4f +%4.4g -%4.4g" % ( xmin, eplus , eminus )
    return (xmin,eplus,eminus,eplus2,eminus2)



graphs=[]


def CreateGraph(name="bias",poi="r"):
    g= ROOT .TGraph()
    lumis=["35.867","50","100","200","300","500","750","1000","1500","2000","2500","3000"]
    for l in lumis:
        try:
            (xmin,ep,em,e2p,e2m)=FindBF( "higgsCombine"+name+"_"+l+"fb.MultiDimFit.mH125.root",poi)
        except IOError: 
            continue
        x = float(l)
        y = float( (ep+em) /2.0)
        g.SetPoint(g.GetN(),x,y)
    return g 


#fixed = CreateGraph( "fixed")
bias = CreateGraph( "bias")
#bias_betterres = CreateGraph( "bias_betterres")
statonly = CreateGraph( "StatOnly")
#statonlybias = CreateGraph( "StatOnlyBias")
#iced = CreateGraph( "Iced")
#rv = CreateGraph( "rv","RV")
#rf = CreateGraph( "rf","RF")

#sqrtn = ROOT.TF1("sqrtn","[0]*TMath::Sqrt([1])/TMath::Sqrt(x)",0,5000)
#sqrtn.SetParameter(0,fixed.GetY()[0])
#sqrtn.SetParameter(1,fixed.GetX()[0])

## draw canvas
c = ROOT . TCanvas("c","c",800,800)
c.SetTopMargin(0.05)
c.SetRightMargin(0.05)
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
c.SetLogx()


dummy= ROOT .TH1D("dummy","dummy",1000,0, 5000)
if False:
    dummy.GetXaxis().SetTitle("")
    m = ROOT.TMathText()
    m.SetTextAlign(31)
    m.SetTextSize(dummy.GetXaxis().GetTitleSize())
    m.DrawMathText(0.02,.95,"\\mathcal{L}\\text{ [fb^{-1}]}")
else:
    dummy.GetXaxis().SetTitle("Luminosity #scale[1.1]{[}#scale[0.2]{ }fb^{-1}#scale[0.2]{ }#scale[1.1]{]}")
dummy.GetYaxis().SetTitle("Uncertainty")
dummy.Draw("AXIS")
dummy.GetXaxis().SetRangeUser(35,3000)
dummy.GetXaxis().SetMoreLogLabels()
dummy.GetXaxis().SetNoExponent()
dummy.GetYaxis().SetRangeUser(0,1)
dummy.GetXaxis().SetTitleOffset(1.4)
dummy.GetYaxis().SetTitleOffset(1.4)


if True:
    ## draw 33% and 20% error lines
    g33=ROOT.TGraph()
    g33.SetPoint(0,0,.33)
    g33.SetPoint(1,5000,.33)
    
    g20=ROOT.TGraph()
    g20.SetPoint(0,0,.20)
    g20.SetPoint(1,5000,.20)
    
    g33.SetLineColor(ROOT.kGray)
    g20.SetLineColor(ROOT.kGray)
    g33.SetLineStyle(3)
    g20.SetLineStyle(3)
    
    g33.Draw("L SAME")
    g20.Draw("L SAME")

## set color and styles
#iced.SetLineColor(ROOT.kCyan)
#iced.SetLineWidth(1)
#iced.SetLineStyle(7)
#iced.Draw("L SAME")

statonly.SetLineColor(38)
statonly.SetLineWidth(2)
statonly.SetLineStyle(7)
statonly.Draw("L SAME")

#statonlybias.SetLineColor(ROOT.kBlue)
#statonlybias.SetLineWidth(2)
#statonlybias.SetLineStyle(1)
#statonlybias.Draw("L SAME")

#bias_betterres.SetLineColor(ROOT.kGreen+2)
#bias_betterres.SetLineWidth(1)
#bias_betterres.SetLineStyle(1)
#bias_betterres.Draw("L SAME")

#fixed.SetLineColor(46)
#fixed.SetLineWidth(2)
#fixed.SetLineStyle(7)
#fixed.Draw("L SAME")

#sqrtn.SetLineColor(ROOT.kBlue)
#sqrtn.SetLineWidth(1)
#sqrtn.Draw("L SAME")

bias.SetLineColor(ROOT.kBlack)
bias.SetLineWidth(3)
bias.SetLineStyle(1)
bias.Draw("L SAME")

## rv.SetLineColor(38)
## rv.SetLineWidth(2)
## rv.SetLineStyle(1)
## rv.Draw("L SAME")
## 
#rf.SetLineColor(30)
#rf.SetLineWidth(2)
#rf.SetLineStyle(1)
#rf.Draw("L SAME")

### legend
#leg = ROOT . TLegend(.16,.16,.56,.56)
leg = ROOT . TLegend(.20,.17,.46,.44)
leg.AddEntry(bias,"Projection","L")
#leg.AddEntry(fixed,"- no bias uncert","L")
leg.AddEntry(statonly,"- stat. only.","L")
#leg.AddEntry(sqrtn,"- #sqrt{N}","L")
#leg.AddEntry(bias_betterres,"- w/ 10% improvement in resolution","L")
#leg.AddEntry(statonlybias,"- stat. only. + bias","L")
#leg.AddEntry(iced,"- w/o fit uncert.","L")
#leg.AddEntry(rv,"RV","L")
#leg.AddEntry(rf,"RF","L")

leg.SetBorderSize(0)
#leg.SetFillStyle(0)
leg.Draw()


l = ROOT . TLatex()
l.SetNDC()
l.SetTextFont(42)

l.SetTextAlign(33)
l.SetTextSize(0.045)
l.DrawLatex(0.85,0.85,"H#rightarrow#mu#mu")
l.SetTextSize(0.055)
l.SetTextAlign(33)
l.DrawLatex(0.93,.93,"#bf{CMS} #scale[0.75]{#it{Projection}}")

## 
if True:
    l.SetTextColor(ROOT.kGray)
    l.SetTextAlign(12)
    l.SetTextSize(0.02)
    c.Update()
    y= (0.33-c.GetY1() ) / (c.GetY2()-c.GetY1())  #+ c.GetBottomMargin()
    l.DrawLatex(0.96,y,"33%")
    y= (0.20-c.GetY1() ) / (c.GetY2()-c.GetY1())  #+ c.GetBottomMargin()
    l.DrawLatex(0.96,y,"20%")

c.Modified()
c.Update()

if opts.outname=="":
    raw_input("ok?")
else:
    c.SaveAs(opts.outname + ".pdf")
    c.SaveAs(opts.outname + ".png")
    c.SaveAs(opts.outname + ".root")

