import os,sys,re

from array import array
from optparse import OptionParser

### tree
#Branch(treename, "runNum",'I');
#Branch(treename, "lumiSec",'I');
#Branch(treename, "evtNum", 'I');
#Branch(treename, "npv",'I');
#Branch(treename, "pass",'I');
#Branch(treename, "npu",'F');
#Branch(treename, "scale1fb",'F');
#Branch(treename, "mass", 'F'); // invariant mass
#Branch(treename, "qtag", 'I'); // invariant mass
#Branch(treename, "qprobe", 'I'); // invariant mass
#Branch(treename, "tag", 'T');
#Branch(treename, "probe", 'T');
#Branch(treename, "truth_tag",'I');
#Branch(treename, "truth_probe", 'I');


#ptBinList=  [0,51,60,70,80,90] + range(100,250)[::50] + [500]
ptBinList=  [0,51,90,140,1000]
ptBin = array('f', ptBinList)

for idx,b in enumerate(ptBinList):
	if idx >0 and b <= ptBinList[idx-1]: 
		print "ERROR, not increasing order"

usage= ''' Do the Tag And probe with the t&p trees.
	Fit the Z Line shapes and extract efficiencies
	'''

parser = OptionParser(usage=usage)
parser.add_option("-p","--plot" ,dest='plot',type='string',help="PlotDir [Default=%default]",default="plot/tp")
parser.add_option("-b","--batch" ,dest='batch',action='store_true',help="Batch [Default=%default]",default=False)
parser.add_option("-f","--file" ,dest='file',type='string',help="InputFile [Default=%default]",default="TagAndProbe.root")

opts,args = parser.parse_args()

sys.argv=[]
import ROOT
#ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)

## mk plot dir
if opts.plot!="": 
    try:
        os.mkdir(opts.plot)
    except OSError: pass

## load ChHiggs Library
print "-> Load ChargedHiggs library"
ROOT.gSystem.Load( "./bin/libChargedHiggs.so")
ROOT.gSystem.Load( "libHiggsAnalysisCombinedLimit.so")

if opts.batch:
	ROOT.gROOT.SetBatch(1)

fROOT = ROOT.TFile.Open(opts.file)
if fROOT == None: print opts.file+": no such file"
tree = fROOT.Get("tagprobe")
if tree == None: print "tagprobe: no such tree"

### to store histograms
hists={}
def HistName( type, mc ,pt , Pass=True):
	## find pt bin	
	idx=-1
	pt0=-1
	pt1=-1
	for i in range(0,len(ptBin) -1 ):
		if pt >= ptBin[i] and pt< ptBin[i+1]:
			idx = i
			pt0 = ptBin[i]
			pt1 = ptBin[i+1]
			break
	#print "DEBUG Pt=",pt,"idx=",idx,"pt=[",pt0,",",pt1,"]"
	if mc == 0 : mcStr="Data"
	elif mc == 2 : mcStr = "MC"
	else: mcStr="Other"

	name = "invm_%s_%s_bin%d"%(type,mcStr,idx)
	if Pass : name += "_pass"
	else : name += "_fail"

	if name not in hists:
		hists[name] = ROOT.TH1D(name,name + "_Pt_%.0f_%.0f"%(pt0,pt1),200,50,150)
		print "-> Creating histo with name",name,"hist~",len(hists)
	return name

################### LOOP ######################
for iEntry in range(0,tree.GetEntries()):
	if iEntry % 1000 == 0 :
		print "\r Doing entry",iEntry,"of",tree.GetEntries(),": %.1f"%(float(iEntry)/tree.GetEntries()*100.),"%",
		sys.stdout.flush()
	tree.GetEntry(iEntry)
	### SELECTION ###
	if not tree.isTagTrigger: continue ## HLT_ISOMu20
	if abs(tree.probe.Eta()) >2.1 : continue
	################
	if tree.isMC:
		name = HistName("MCTruth", tree.isMC, tree.probe.Pt(), tree.passMCTruth) ###
		hists[name].Fill(tree.mass)
	###
	name = HistName("HLT", tree.isMC, tree.probe.Pt(), tree.passTrigger) ###
	hists[name].Fill(tree.mass)
	###
	name = HistName("HLTEvent", tree.isMC, tree.probe.Pt(), tree.passEventTrigger) ###
	hists[name].Fill(tree.mass)

	name = HistName("HLTNone", tree.isMC, tree.probe.Pt(), tree.passTriggerNone) ###
	hists[name].Fill(tree.mass)

	if not tree.passTrigger: continue
	name = HistName("Iso", tree.isMC, tree.probe.Pt(), tree.passIso) ###
	hists[name].Fill(tree.mass)

print  "\r * DONE"

if opts.plot!="":
	for name in hists:
		c=ROOT.TCanvas("c","c")
		hists[name].Draw("PE ")
		c.SaveAs( opts.plot + "/" + name + ".pdf")
	## FIT
	x=ROOT.RooRealVar("mll","mll",60,120);
	## bkg
	## Balpha = ROOT.RooRealVar("Balpha","alpha",0,100)
	## Bbeta = ROOT.RooRealVar("Bbeta","beta",0,100)
	## Bgamma = ROOT.RooRealVar("Bgamma","gamma",0,100)
	## Bpeak = ROOT.RooRealVar("Bpeak","peak",0,100)
	Bbern0 = ROOT.RooRealVar("bern0","bern0",0,100);
	Bbern1 = ROOT.RooRealVar("bern1","bern1",0,100);
	Bbern2 = ROOT.RooRealVar("bern2","bern2",0,100);
	## sig
	Zmass= ROOT.RooRealVar("Zmass","Zmass",91.1876)
	Zwidth = ROOT.RooRealVar("Zwidth","Zwidth",27.995)
	Zmass.setConstant()
	Zwidth.setConstant()
	Salpha = ROOT.RooRealVar("Salpha","alpha",10,0,100)
	Ssigma = ROOT.RooRealVar("Ssigma","sigma",10,0,100)
	Sn = ROOT.RooRealVar("Sn","n",2,0,100)

	f = ROOT.RooRealVar("f","f",0,1)
	for name in hists:
		c2=ROOT.TCanvas("c2","c")
		data = ROOT.RooDataHist("h","h",ROOT.RooArgList(x),hists[name])
		#bkg = ROOT.RooCMSShape("cms-shape","cms-shape",x,Balpha,Bbeta,Bgamma,Bpeak)
		bkg = ROOT.RooBerneinFast(2)("bern","bern",x,ROOT.RooArgList(Bbern0,Bbern1,Bbern2))
		# RooVoigtianShape::RooVoigtianShape(const char* name, const char* title, RooAbsReal& _m, RooAbsReal& _m0, RooAbsReal& _sigma, RooAbsReal& _alpha, RooAbsReal& _n, RooAbsReal& _width, bool doFast) =>
		sig = ROOT.RooVoigtianShape("voigtian","voigtian", x,Zmass, Ssigma, Salpha,Sn,Zwidth,True)
		f.setVal(.9)
		model = ROOT.RooAddPdf("model","model",ROOT.RooArgList(sig,bkg), ROOT.RooArgList(f))
		model.fitTo(data,ROOT.RooFit.Save(),ROOT.RooFit.PrintEvalErrors(-1),ROOT.RooFit.PrintLevel(-1),ROOT.RooFit.Warnings(0))

		plot=x.frame()
		data.plotOn(plot)
		model.plotOn(plot)
		plot.Draw()
		c2.SaveAs( opts.plot + "/" + name + "_fit.pdf")
	## END FIT

### DONE ###
