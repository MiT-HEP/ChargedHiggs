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
parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true")
parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
parser.add_option("-u","--unblind",dest="unblind",default=False,action="store_true",help="Draw observation")
parser.add_option("-x","--xSec",dest="xsec",help="Print limit vs xSec instead of mu",default=False,action="store_true")
parser.add_option("","--xSecName",dest="xsecname",help="extra string to be match in the mc_database",default="ChargedHiggs")
parser.add_option("","--mcdb",dest="mcdb",help="the mc_database",default="aux/mcdb.txt")
parser.add_option(""  ,"--yaxis",help="Y axis range Y1,Y2 [%default]",default="")
parser.add_option(""  ,"--xaxis",help="X axis range X1,X2 [%default]",default="")
parser.add_option("","--exclude",dest="exclude",help="Exclude mh points MH1,MH2,.. [%default]",default="")
parser.add_option("","--run12",dest="run12",default=False,action="store_true")
parser.add_option("","--paper",dest="paper",default=False,action="store_true")
parser.add_option("","--supplementary",dest="supplementary",default=False,action="store_true")
parser.add_option("","--M125",dest="M125",default=False,help="The second file is the MH=125 asimov",action="store_true")
parser.add_option("-n","--newlegend",dest="newleg",action='store_true',help="NewLegend [%default]",default=False);
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

def EigthTeVGraph():
	''' Produce the 8TeV graph for exclusion limits in pb. CMS-PAS HIG-14-020. exp or obs'''
	median = ROOT.TGraph()
	median.SetName("median8TeV")
	obs = ROOT.TGraph()
	obs.SetName("obs8TeV")

	median.SetPoint( median.GetN() , 180 , 0.408 ) 
	obs.SetPoint(    median.GetN() , 180 , 0.384 ) 

	median.SetPoint( median.GetN() , 190 , 0.358 ) 
	obs.SetPoint(    median.GetN() , 190 , 0.377 ) 

	median.SetPoint( median.GetN() , 200 , 0.291 ) 
	obs.SetPoint(    median.GetN() , 200 , 0.362 ) 

	median.SetPoint( median.GetN() , 220 , 0.221 ) 
	obs.SetPoint(    median.GetN() , 220 , 0.331 ) 

	median.SetPoint( median.GetN() , 250 , 0.159 ) 
	obs.SetPoint(    median.GetN() , 250 , 0.266 ) 

	median.SetPoint( median.GetN() , 300 , 0.096 ) 
	obs.SetPoint(    median.GetN() , 300 , 0.153 ) 

	median.SetPoint( median.GetN() , 400 , 0.050 ) 
	obs.SetPoint(    median.GetN() , 400 , 0.054 ) 

	median.SetPoint( median.GetN() , 500 , 0.036 ) 
	obs.SetPoint(    median.GetN() , 500 , 0.033 ) 

	median.SetPoint( median.GetN() , 600 , 0.030 ) 
	obs.SetPoint(    median.GetN() , 600 , 0.026 ) 

	return (median,obs) 


def GetLimitFromTree(inputFile,unblind=False):
	''' Get Limit Histo from Combine tree'''

	## Make Limit plot
	fInput= ROOT.TFile.Open(inputFile)

	if fInput == None:
		print "**** ERROR ****"
		print " file ",f,"does not exist"
		print "***************"

	limitTree = fInput.Get("limit")
	if limitTree == None:
		print "**** ERROR ****"
		print " limit tree not present in file",f
		print "***************"
	
	#Expected
	obs=ROOT.TGraphAsymmErrors()
	obs.SetName("obs")
	
	exp=ROOT.TGraphAsymmErrors()
	exp.SetName("exp")
	
	oneSigma=ROOT.TGraphAsymmErrors()
	oneSigma.SetName("oneSigma")
	
	twoSigma=ROOT.TGraphAsymmErrors()
	twoSigma.SetName("twoSigma")
	
	data    = []
	median 	= []
	Up 	= []
	Up2 	= []
	Down 	= []
	Down2 	= []
	
	## loop over the entries and figure out if they are median, 1s/2s or observed	
	for iEntry in range(0,limitTree.GetEntries()):
		limitTree.GetEntry(iEntry)
		mh = limitTree.mh
		l  = limitTree.limit
		q  = limitTree.quantileExpected
		type= 0
	
		if abs(q-0.5)<1.e-5 : 
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
		if q <-.5: # -1
			data.append( (mh,l) ) 
	
	if len(Up2) != len(Down2) :print "[ERROR] Count 2s"
	if len(Up) != len(Down) :print "[ERROR] Count 1s"
	
	## sort median, Up Up2 Down Down2 data with mh
	median.sort()
	Up.sort()
	Up2.sort()
	Down.sort()
	Down2.sort()
	data.sort()
	## merge the list into TGraphs.
	for i in range(0,len(median)):
		count= exp.GetN()
	
		mh= median[i][0]
		if mh != Up[i][0] : print "[ERROR]: MH mismatch"
		if mh != Up2[i][0] : print "[ERROR]: MH mismatch"
		if mh != Down[i][0] : print "[ERROR]: MH mismatch"
		if mh != Down2[i][0] : print "[ERROR]: MH mismatch"
		if unblind and mh != data[i][0]: print "[ERROR]: MH mismatch"

		isToExclude=False
		if opts.exclude != "":
			for mtest in opts.exclude.split(','):
				if float(mtest) == mh:
					isToExclude = True
		if isToExclude: 
			print "Excluding MH=",mh,"from the plot"
			continue
		
		exp.SetPoint( count, median[i][0] ,median[i][1])

		oneSigma.SetPoint(count, mh, median[i][1] ) 
		oneSigma.SetPointError(count, 0, 0 , median[i][1] - Down[i][1], Up[i][1]-median[i][1] ) 
		twoSigma.SetPoint(count, mh , median[i][1] ) 
		twoSigma.SetPointError(count, 0, 0 , median[i][1] - Down2[i][1], Up2[i][1]-median[i][1] ) 

		if unblind:obs.SetPoint(count,mh,data[i][1])

	return obs,exp,oneSigma,twoSigma

def GetLimitFromTreeObsOnly(inputFile):
	''' Get Limit Histo from Combine tree'''

	## Make Limit plot
	fInput= ROOT.TFile.Open(inputFile)

	if fInput == None:
		print "**** ERROR ****"
		print " file ",f,"does not exist"
		print "***************"

	limitTree = fInput.Get("limit")
	if limitTree == None:
		print "**** ERROR ****"
		print " limit tree not present in file",f
		print "***************"
	
	#Expected
	obs=ROOT.TGraphAsymmErrors()
	obs.SetName("obs")
	
	data    = []
	
	## loop over the entries and figure out if they are median, 1s/2s or observed	
	for iEntry in range(0,limitTree.GetEntries()):
		limitTree.GetEntry(iEntry)
		mh = limitTree.mh
		l  = limitTree.limit
		q  = limitTree.quantileExpected
		type= 0
	
		if q <-.5: # -1
			data.append( (mh,l) ) 
	
	data.sort()
	## merge the list into TGraphs.
	for i in range(0,len(data)):
		count= obs.GetN()
	
		mh= data[i][0]

		isToExclude=False
		if opts.exclude != "":
			for mtest in opts.exclude.split(','):
				if float(mtest) == mh:
					isToExclude = True
		if isToExclude: 
			print "Excluding MH=",mh,"from the plot"
			continue
		print "Setting point",count,mh,data[i][1]	
		obs.SetPoint(count,mh,data[i][1])

	return obs

list_data = []
list_exp = []
list_oneSigma=[]
list_twoSigma=[]

colors=[ROOT.kBlack,ROOT.kRed,ROOT.kBlue,ROOT.kMagenta,ROOT.kViolet]

for idx,f in enumerate(opts.file.split(',')):
	obs,exp,oneSigma,twoSigma = GetLimitFromTree(f,opts.unblind)
	if exp.GetN() ==0 and opts.unblind:
	    print "Getting obs only:",f
	    obs = GetLimitFromTreeObsOnly(f)

	col = colors[idx]
	if idx == 0 :
		obs.SetMarkerStyle(21)
		obs.SetMarkerSize(1.0)
		obs.SetLineColor(1)
		obs.SetLineWidth(2)
		obs.SetFillStyle(0)
		obs.SetMarkerColor(col)
		obs.SetLineColor(col)
		
		exp.SetLineColor(1)
		exp.SetLineStyle(2)
		exp.SetFillStyle(0)
		exp.SetLineWidth(2)
		
		oneSigma.SetLineStyle(2)
		twoSigma.SetLineStyle(2)
		
		oneSigma.SetFillColor(ROOT.kGreen+1)
		twoSigma.SetFillColor(ROOT.kYellow)

		### PRINT MORE ##
		if opts.unblind:
			print "**** RESULTS **** "
			for point in range(0,obs.GetN()):
				print "MH=",obs.GetX()[point],"y=",obs.GetY()[point]
				print "exp=",exp.GetY()[point]
			print "***************** "

	else:
		obs.SetMarkerStyle(20+idx)
		obs.SetMarkerSize(0.3)
		obs.SetMarkerColor(col)
		obs.SetLineColor(col)
		obs.SetLineWidth(2)
		obs.SetFillStyle(0)
	
		exp.SetLineWidth(2)
		exp.SetLineStyle(7)
		exp.SetFillStyle(0)
		exp.SetLineColor(col)

		oneSigma.SetLineStyle(3)
		twoSigma.SetLineStyle(3)
		
		oneSigma.SetLineColor(ROOT.kGreen+2)
		twoSigma.SetLineColor(ROOT.kOrange)

		oneSigma.SetFillStyle(0)
		twoSigma.SetFillStyle(0)


	list_data.append(obs)

	if not (opts.M125 and idx> 0):
	    list_exp.append(exp)
	    list_oneSigma.append(oneSigma)
	    list_twoSigma.append(twoSigma)


## Start Drawing
c=ROOT.TCanvas()
c.SetCanvasSize(700,500)
c.SetBottomMargin(0.15)
c.SetLeftMargin(0.15)
c.SetTopMargin(0.10)
c.SetRightMargin(0.05)

ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)

if opts.xaxis != "":
	dummy = ROOT.TH1D("dummy","dummy",1000, float(opts.xaxis.split(',')[0]), float(opts.xaxis.split(',')[1]))
else:
	dummy = ROOT.TH1D("dummy","dummy",1000, 0, 3000)
	dummy.GetXaxis().SetRangeUser(200,3000)

dummy.GetYaxis().SetRangeUser(1e2,1e8)

dummy.GetXaxis().SetTitle("m_{H^{+}} [GeV]")
dummy.GetYaxis().SetTitle("#sigma/#sigma_{MSSM}")

if opts.xaxis != "" and float(opts.xaxis.split(',')[1]) <135: ## SM?
	dummy.GetXaxis().SetTitle("m_{H} [GeV]")
	dummy.GetXaxis().SetTitleSize(0.05)
	dummy.GetYaxis().SetTitleSize(0.05)
	dummy.GetXaxis().SetLabelSize(0.045)
	dummy.GetYaxis().SetLabelSize(0.045)
	dummy.GetXaxis().SetLabelOffset(0.02)
	dummy.GetYaxis().SetLabelOffset(0.02)
	dummy.GetXaxis().SetTitleOffset(1.2)
	dummy.GetYaxis().SetTitleOffset(1.1)
	dummy.GetYaxis().SetTitle("95% CL Limit on #sigma/#sigma_{SM}")
else:
    c.SetLogy()

if opts.xsec:
	dummy.GetYaxis().SetTitle("#sigma [pb]")
	dummy.GetXaxis().SetRangeUser(200,3000)
	dummy.GetYaxis().SetRangeUser(1e-2,1e2)

if opts.yaxis != "":
	dummy.GetYaxis().SetRangeUser( float(opts.yaxis.split(',')[0]), float(opts.yaxis.split(',')[1]) )
if opts.xaxis != "":
	dummy.GetXaxis().SetRangeUser( float(opts.xaxis.split(',')[0]), float(opts.xaxis.split(',')[1]) )

dummy.Draw("AXIS")

line = ROOT.TGraph()
line.SetLineColor(ROOT.kGray)
line.SetPoint(0,0,1)
line.SetPoint(1,1000,1)


#mg = ROOT.TMultiGraph()

for idx in range(0,len(list_exp) ):
	if idx==0:
		#mg.Add(list_twoSigma[idx])
		#mg.Add(list_oneSigma[idx])
		list_twoSigma[idx].Draw("PE3 SAME")
		list_oneSigma[idx].Draw("PE3 SAME")
	#mg.Add(list_exp[idx])
	list_exp[idx].Draw("L SAME")
	if opts.unblind: list_data[idx].Draw("LP SAME")
if opts.M125:
	list_data[1].SetLineColor(ROOT.kRed)
	list_data[1].SetLineWidth(2)
	list_data[1].SetLineStyle(7)
	list_data[1].Draw("L SAME")

#mg.Draw("3")
#mg.Draw("LPX")

line.Draw("L SAME")

#if opts.xsec:
#	exp8TeV.Draw("L SAME")
#	obs8TeV.Draw("PL SAME")
#	l2= ROOT.TLatex()
#	l2.SetTextSize(0.03)
#	l2.SetTextColor(ROOT.kRed+2)
#	l2.SetTextAlign(22)
#	l2.DrawLatex(600,0.08,"19.7 fb^{-1} (8 TeV)")


dummy.Draw("AXIS SAME")
#dummy.Draw("AXIS X+ Y+ SAME")
dummy.Draw("AXIG SAME")

# draw latex
l = ROOT.TLatex()
l.SetNDC()
l.SetTextSize(0.06)
l.SetTextFont(42)
#l.SetTextAlign(11) #outside
#xcms,ycms= 0.18,.91 # outside
l.SetTextAlign(13) #inside
xcms,ycms= 0.18,.88 # inside
if opts.unblind:
    if opts.paper:
        #l.DrawLatex(0.13,.88,"#bf{CMS}, #scale[0.75]{#it{Preliminary}}")
        if opts.supplementary:
            l.DrawLatex(xcms,ycms,"#bf{CMS} #scale[0.75]{#it{Supplementary}}")
        else:
            l.DrawLatex(xcms,ycms,"#bf{CMS}")
    else:
        if opts.supplementary:
            l.DrawLatex(xcms,ycms,"#bf{CMS} #scale[0.75]{#it{Preliminary Supplementary}}")
        else:
            l.DrawLatex(xcms,ycms,"#bf{CMS} #scale[0.75]{#it{Preliminary}}")
else:
    l.DrawLatex(xcms,ycms,"#bf{CMS}, #scale[0.75]{#it{Simulation}}")
l.SetTextSize(0.04)
l.SetTextAlign(31)
if opts.run12:
    l.DrawLatex(0.89+0.05,.91,"5.0 fb^{-1} (7 TeV) + 19.8 fb^{-1} (8 TeV) + 35.9 fb^{-1} (13 TeV)")
else:
    l.DrawLatex(0.89+0.05,.91,"35.9 fb^{-1} (13 TeV)")

#draw legend
enlarge = 0.0
if opts.M125: enlarge=0.08
leg = ROOT.TLegend(0.70-enlarge,.55,.88+0.05,.88)
leg.SetFillStyle(0)
leg.SetBorderSize(0)

for idx in range(0, len(list_exp) ) :
	label = ""
	if opts.label !="":
		label = opts.label.split(',')[idx]

	if opts.unblind:
		leg.AddEntry(list_data[idx],"Observed"+label,"LP")
	leg.AddEntry(list_exp[idx], "Expected" + label, "L" )

	if opts.M125:
		leg.AddEntry(list_data[1],"Expected (SM m_{H}=125 GeV)"+label,"L")
	
	if idx==0:
		leg.AddEntry(list_oneSigma[idx], "#pm 1 #sigma","FL") 
		leg.AddEntry(list_twoSigma[idx], "#pm 2 #sigma","FL") 

##### PRINT TABLE LATEX
doTable=True
if doTable:
	tex=open(opts.outname +".tex","w")
	print>>tex, "\\begin{tabular}{lcccccc}"
	print>>tex, "\\hlinewd{1.2pt}"
	print>>tex, "\\multirow{2}{*}{$m_\\textup{H}$ [GeV]} & \\multicolumn{5}{c}{Expected Limits} & \multirow{2}{*}{Observed limit} \\\\"
	print>>tex, "\\cline{2-6}"
	print>>tex,"& $-2\\sigma$ & $-1\\sigma$ & median  & $1\\sigma$ & $2\\sigma$ & \\\\"
	print>>tex, "\\hline"
	for i in range(0,list_exp[0].GetN()):
		print>>tex, "%.0f"%list_exp[0].GetX()[i],"&",
		print>>tex, "%.2f"%(list_twoSigma[0].GetY()[i]-list_twoSigma[0].GetEYlow()[i]),
		print>>tex, "&",
		print>>tex, "%.2f"%(list_oneSigma[0].GetY()[i]-list_oneSigma[0].GetEYlow()[i]),
		print>>tex, "&",
		print>>tex, "%.2f"%list_exp[0].GetY()[i], ## median
		print>>tex, "&",
		print>>tex, "%.2f"%(list_oneSigma[0].GetY()[i]+list_oneSigma[0].GetEYhigh()[i]),
		print>>tex, "&",
		print>>tex, "%.2f"%(list_twoSigma[0].GetY()[i]+list_twoSigma[0].GetEYhigh()[i]),
		print>>tex, "&",
		if opts.unblind:
			print>>tex, "%.2f"%list_data[0].GetY()[i], ## data
		else:
			print>>tex, "-", ## nothing
		print>>tex, "\\\\"
	print>>tex, "\\hlinewd{1.2pt}"
	print>>tex, "\\end{tabular}"
########

#if opts.xsec:
#	leg.AddEntry(exp8TeV, "expected (8TeV)","L")
#	leg.AddEntry(obs8TeV, "observed (8TeV)","PL")
obj=[]
if opts.newleg:
    print "-> Adding NEW Legend"
    ltx = ROOT.TLatex()
    obj.append(ltx)
    ltx . SetNDC()
    ltx . SetTextSize(0.05)
    ltx . SetTextFont(42)
    ltx . SetTextAlign(12)
    xmin = 0.4
    ymax = .85
    textSep = 0.05
    delta = 0.045
    entryDelta = 0.07
    dataPoint =  ROOT.TMarker(xmin,ymax,20)
    dataPoint.SetMarkerColor(ROOT.kBlack)
    dataPoint.SetMarkerStyle(list_data[0].GetMarkerStyle())
    dataPoint.SetMarkerSize(list_data[0].GetMarkerSize())
    dataPoint.SetNDC()
    dataLine =  ROOT.TLine(xmin-delta/2., ymax ,xmin + delta/2, ymax)
    dataLine.SetNDC()
    dataLine.SetLineColor(ROOT.kBlack)
    dataLine.SetLineWidth(1)
    obj += [dataPoint,dataLine]
    ## Draw data
    dataPoint.Draw("SAME")
    dataLine.Draw("SAME")
    ltx.DrawLatex(xmin+ textSep,ymax,"Observed")
    
    ## draw median and error
    y_exp = ymax - entryDelta
    vertical=False
    if vertical:
        l_exp = ROOT.TLine(xmin,y_exp -delta/2., xmin,y_exp+delta/2.)
        l_exp.SetNDC()
        l_exp.SetLineColor(ROOT.kBlack)
        l_exp.SetLineWidth(2)
        l_exp.SetLineColor(1)
        l_exp.SetLineStyle(7)
        oneSigma = ROOT.TPave(xmin-delta/3.,y_exp-delta/2.,xmin+delta/3.,y_exp+delta/2.,0,"NDC")
        twoSigma = ROOT.TPave(xmin-delta*2/3.,y_exp-delta/2.,xmin+delta*2/3.,y_exp+delta/2.,0,"NDC")
        obj . extend([l_exp,oneSigma,twoSigma])
    else:
        l_exp = ROOT.TLine(xmin-delta/2.,y_exp, xmin + delta/2.,y_exp)
        l_exp.SetNDC()
        l_exp.SetLineColor(ROOT.kBlack)
        l_exp.SetLineWidth(3)
        l_exp.SetLineColor(1)
        l_exp.SetLineStyle(2)
        oneSigma = ROOT.TPave(xmin-delta/2.,y_exp-delta/3.,xmin+delta/2.,y_exp+delta/3.,0,"NDC")
        twoSigma = ROOT.TPave(xmin-delta/2.,y_exp-delta*2/3.,xmin+delta/2.,y_exp+delta*2/3.,0,"NDC")
        obj . extend([l_exp,oneSigma,twoSigma])
    oneSigma.SetFillColor(ROOT.kGreen+1)
    twoSigma.SetFillColor(ROOT.kYellow)
    twoSigma.Draw("SAME")
    oneSigma.Draw("SAME")
    l_exp.Draw("SAME")
    ltx.DrawLatex(xmin +textSep,y_exp,"Expected (#scale[0.7]{background, 68% CL, 95% CL})")

    if opts.M125:
        y_exp125 = ymax - 2*entryDelta
        l_exp125 = ROOT.TLine(xmin-delta/2.,y_exp125 , xmin+delta/2.,y_exp125)
        l_exp125.SetNDC()
        l_exp125.SetLineColor(ROOT.kRed)
        l_exp125.SetLineStyle(2)
        l_exp125.SetLineWidth(3)
        l_exp125.Draw("SAME")
        ltx.DrawLatex(xmin +textSep,y_exp125,"Expected (SM #scale[0.7]{m_{H} = 125 GeV})")
else:
    leg.Draw()

dummy.Draw("AXIS SAME")
c.RedrawAxis()

c.Modified()
c.Update()
raw_input("Looks ok?")

c.SaveAs(opts.outname + ".pdf")
c.SaveAs(opts.outname + ".png")
c.SaveAs(opts.outname + ".root")
