import sys,os
import re
from array import array


print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import FindEOS

####################### PARSE OPTIONS ######################
from optparse import OptionParser

parser = OptionParser(usage="Produce small trees with matching suitable for input regression.")
parser.add_option("-e","--eos",type="string",help="eos path to consider",default="")
parser.add_option("-o","--output",type="string",help="Output File. Default=%default",default="output.root")
parser.add_option("-l","--limit",type="int",help="Limit in the number of entries to parse. Default=%default",default=-1)

opts,args= parser.parse_args()

###################### ADD FILES ###########################
sys.argv=[]
import ROOT

t = ROOT.TChain("nero/events")
fileList=FindEOS( opts.eos )
for f in fileList:
	t.Add(f)

###################### OPEN OUTPUT #########################

fOut= ROOT.TFile.Open(opts.output,"RECREATE")
tree = ROOT.TTree("tree","tree")
vars={}
for name in ['tauPt','tauEta','tauPhi','tauGenPt','tauGenPhi','tauIso','nvtx','jetPt','jetEta','jetPhi',
		'tauM','tauQ','tauIsoCh','tauIsoNh','tauIso2','tauNeutralIsoPtSum','tauChargedIsoPtSum',
		'tauPartonPt','weight', 'tauPartonEta', 'tauPartonPhi',
		]:
	vars[name]=array('f',[0.])
	tree.Branch(name,vars[name],name+"/F");


if True: ## deactivate branches
	t.SetBranchStatus("*",0)
	t.SetBranchStatus("npv",1)
	t.SetBranchStatus("tau*",1)
	t.SetBranchStatus("gen*",1) ## genP4 genPdgId genjetP4
	t.SetBranchStatus("jetP4",1)

for iEntry in range(0,t.GetEntries() ):
	if iEntry%1000 == 0 :
		print "\r Doing entry:",iEntry,"/",t.GetEntries(), ":", "%.1f %%"%(float(iEntry)*100./t.GetEntries()),
		sys.stdout.flush()
	if opts.limit>0 and iEntry >opts.limit: 
		print "\r LIMIT REACHED: ENDING"
		break
	t.GetEntry(iEntry)
	#find a tau particle
	leadTau =-1
	subleadTau= -1

	for i in range(0,t.genP4.GetEntries() ) :
		if abs(t.genPdgId[i]) == 15 :
			leadTau = i
			break

	if leadTau <0 : continue	

	for i in range(leadTau,t.genP4.GetEntries() ) :
		if abs(t.genPdgId[i]) == 15 :
		   if t.genP4[i].DeltaR( t.genP4[leadTau] ) > 0.1:
			subleadTau = i
			break

	for iGenTau in [i for i in [leadTau,subleadTau] if i>=0]:
		## gen jet

		iGenJet=-1	
		for i in range(0, t.genjetP4.GetEntries() ):
			if t.genjetP4[i].DeltaR( t.genP4[iGenTau])< 0.1:
				iGenJet=i
				break
		## FIXME WRT RECO TAU
		iJet=-1	
		for i in range(0, t.jetP4.GetEntries() ):
			if t.jetP4[i].DeltaR( t.genP4[iGenTau])< 0.1:
				iJet=i
				break
		iTau=-1	
		for i in range(0, t.tauP4.GetEntries() ):
			if t.tauP4[i].DeltaR( t.genP4[iGenTau])< 0.1:
				iTau=i
				break

		#if iTau < 0 or iJet < 0 or iGenJet <0:
		if iTau < 0 or iGenJet <0:
			continue

		vars['weight'][0]=1.0

		vars['nvtx'][0] = t.npv
		vars['tauPt'][0] = t.tauP4[iTau].Pt()
		vars['tauM'][0] = t.tauM[iTau]
		vars['tauQ'][0] = t.tauQ[iTau]
		vars['tauEta'][0] = t.tauP4[iTau].Eta()
		vars['tauPhi'][0] = t.tauP4[iTau].Phi()
		vars['tauIso'][0] = t.tauIso[iTau]
		vars['tauIso2'][0] = t.tauIsoDeltaBetaCorr[iTau]
		vars['tauChargedIsoPtSum'][0] = t.tauChargedIsoPtSum[iTau]
		vars['tauNeutralIsoPtSum'][0] = t.tauNeutralIsoPtSum[iTau]
		vars['tauGenPt'][0] = t.genjetP4[iGenJet].Pt()
		vars['tauPartonPt'][0] = t.genP4[iGenTau].Pt()
		vars['tauPartonEta'][0] = t.genP4[iGenTau].Eta()
		vars['tauPartonPhi'][0] = t.genP4[iGenTau].Phi()
		if iJet >=0:
			vars['jetPt'][0]  = t.jetP4[iJet].Pt()
			vars['jetEta'][0] = t.jetP4[iJet].Eta()
			vars['jetPhi'][0] = t.jetP4[iJet].Phi()
		else:
			vars['jetPt'][0]  = -10.
			vars['jetEta'][0] = -10.
			vars['jetPhi'][0] = -10.
		tree.Fill()
	## loop over entries

fOut.Write()
fOut.Close()
