import os,sys,re

from array import array
from optparse import OptionParser


ptBinList=  [0,51,60,70,80,90] + range(100,250)[::50] + [500]
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
	return name

################### LOOP ######################
for iEntry in range(0,tree.GetEntries()):
	if iEntry %1000 == 0 :
		print "\r Doing entry",iEntry,"of",tree.GetEntries(),": %.1f"%(float(iEntry)/tree.GetEntries()*100.),"%",
		sys.stdout.flush()
	tree.GetEntry(iEntry)
	### SELECTION ###
	if not tree.isTagTrigger: continue ## HLT_ISOMu20
	if abs(tree.etaProbe) >2.1 : continue
	################
	if tree.isMC:
		name = HistName("MCTruth", tree.isMC, tree.ptProbe, tree.passMCTruth) ###
		hists[name].Fill(tree.m)
	###
	name = HistName("HLT", tree.isMC, tree.ptProbe, tree.passTrigger) ###
	hists[name].Fill(tree.m)
	###
	name = HistName("HLTEvent", tree.isMC, tree.ptProbe, tree.passEventTrigger) ###
	hists[name].Fill(tree.m)

	name = HistName("HLTNone", tree.isMC, tree.ptProbe, tree.passTriggerNone) ###
	hists[name].Fill(tree.m)

	if not tree.passTrigger: continue
	name = HistName("Iso", tree.isMC, tree.ptProbe, tree.passIso) ###
	hists[name].Fill(tree.m)

print  "\r * DONE"

if opts.plot!="":
	for name in hists:
		c=ROOT.TCanvas("c","c")
		hists[name].Draw("PE ")
		c.SaveAs( opts.plot + "/" + name + ".pdf")
### DONE ###
