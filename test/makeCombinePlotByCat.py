import os,sys
from array import array
import re,math
from subprocess import call
from optparse import OptionParser, OptionGroup

usage="program higss*"
parser=OptionParser()
parser.add_option("-b","--batch",dest="batch",default=False,action="store_true")
#parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Hmumu.root")
opts,args= parser.parse_args()

########### IMPORT ROOT #############
oldArgv=sys.argv[:]
sys.argv=[]
import ROOT
sys.argv=oldArgv[:]

garbage=[]

def GetLimitFromTree(inputFile,xsec=False):
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
	
		## TODO OBS
			
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

	return  median[0][1], Up[0][1], Down[0][1], Up2[0][1],Down2[0][1]

c=ROOT.TCanvas("c","canvas",800,800)

c.SetTopMargin(0.10);
c.SetBottomMargin(0.15);
c.SetLeftMargin(0.15);
c.SetRightMargin(0.05);


nCats=len(args)

maxL=20.
dummy = ROOT.TH2F("dummy","Expected Limits per Category",20,0.,maxL,nCats,1-0.5,nCats+0.5);
dummy.SetStats(0);
ci = ROOT.TColor.GetColor("#00ff00");
dummy.SetFillColor(ci);

for ic,cat in enumerate(args):
    dummy.GetYaxis().SetBinLabel(nCats-ic,"cat%d"%ic);
dummy.GetXaxis().SetTickLength(0.01);
dummy.GetYaxis().SetTickLength(0);
dummy.GetXaxis().SetTitle("Upper Limit (95%CL)");
dummy.GetXaxis().SetNdivisions(510);
dummy.GetXaxis().SetLabelFont(42);
dummy.GetXaxis().SetLabelSize(0.045);
dummy.GetXaxis().SetTitleSize(0.045);
dummy.GetXaxis().SetTitleOffset(0.95);
dummy.GetXaxis().SetTitleFont(42);
dummy.GetYaxis().SetNdivisions(510);
dummy.GetYaxis().SetLabelSize(0.035);
dummy.GetYaxis().SetTitleSize(0.045);
dummy.GetYaxis().SetTitleOffset(1.1);
dummy.GetYaxis().SetTitleFont(42);
dummy.GetZaxis().SetLabelFont(42);
dummy.GetZaxis().SetLabelSize(0.035);
dummy.GetZaxis().SetTitleSize(0.035);
dummy.GetZaxis().SetTitleFont(42);
dummy.Draw("");
#sigYields[(cat,proc)] = ea * config.lumi() * config.xsec(proc) *config.br() # proc+"_HToMuMu_M%d" 
width = 0.34

for ic,cat in enumerate(args):
    S=0.0
    print " ** Cat=",ic,"==",cat
    ybin = nCats-ic;
    ybinmin = ybin-width;
    ybinmax = ybin+width;

    exp,up,down,up2,down2 = GetLimitFromTree(cat)
    marker = ROOT.TGraph()
    marker.SetMarkerStyle(21)
    marker.SetMarkerColor(ROOT.kBlack)
    marker.SetPoint(0,exp,ybin)
    

    pave1 = ROOT.TPave(down,ybinmin,min(up,maxL),ybinmax);
    pave2 = ROOT.TPave(down2,ybinmin,min(up2,maxL),ybinmax);
    pave2.SetFillColor(ROOT.kOrange)
    pave1.SetFillColor(ROOT.kGreen)

    pavetext = ROOT.TPaveText(0.001,ybinmin,10,ybinmax);
    pavetext.AddText("median=%.1f"%exp)
    pavetext.SetTextColor(ROOT.kBlack);
    pavetext.SetTextFont(43);
    pavetext.SetTextSize(18);
    pavetext.SetTextAlign(11);
    pavetext.SetFillStyle(0);
    pavetext.SetFillColor(0);
    pavetext.SetLineColor(0);
    pavetext.SetBorderSize(0);

    pave2.Draw("SAME")
    pave1.Draw("SAME")
    marker.Draw("P SAME")
    pavetext.Draw("SAME")
    garbage.extend([marker,pave1,pave2,pavetext])


c.Modify()
c.Update()

raw_input("ok?")
