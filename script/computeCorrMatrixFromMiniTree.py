import sys,os
import math
import re
#argv

import ROOT

file0="root://eoscms///store/cmst3/user/dalfonso/chHiggsTB/FEB1/9b3618e5ba6ccd1df3aad92308e0da0797527d6a/1l.root"
file1="root://eoscms///store/cmst3/user/dalfonso/chHiggsTB/FEB1/andreaTEST/FEBjetsUpbis.root"

fIn0 = ROOT.TFile.Open(file0)
fIn1 = ROOT.TFile.Open(file1)

tree0 = fIn0.Get("tree_tb")
tree1 = fIn1.Get("tree_tb")

doCov=True
branches=["lep_category","run","!lep1_pt","lep1_phi","lep1_eta","lep1_charge","lep1_isolation","lep1_id","!lep2_pt","lep2_phi","lep2_eta","lep2_charge","lep2_isolation","lep2_id","NcentralJets","NJets","NBJets","!met_pt","met_phi","!ht","!st","DRl1b1","DRl2b1","DRbbmin","MassDRbbmin","MassDRlbmin","bdt1lh","bdt1lm","bdt1ll","bdt2lh","bdt2lm","bdt2ll","!mt","!mt2ll","!mt2bb","!mt2bb1l","!mtMin","!mtMax","!mtTot","!Cen","!HemiOut","!jet_pt[0]","!jet_pt[1]","jet_eta[0]","jet_eta[1]","jet_phi[0]","jet_phi[1]","jet_discr[0]","jet_discr[1]","!bjet_pt[0]","!bjet_pt[1]","bjet_eta[0]","bjet_eta[1]","bjet_phi[0]","bjet_phi[1]","bjet_discr[0]","bjet_discr[1]","nGenB","bFromH_pt","bFromH_phi","bFromH_eta","bFromTopH_pt","bFromTopH_phi","bFromTopH_eta","leptonFromTopH_pt","leptonFromTopH_phi","leptonFromTopH_eta"]


def PrintStatus(done,tot,n=30):
	if done % 10000 != 0  and done < tot-1: return
	#a=int(math.floor(float(done)*n/tot ))
	a=int(float(done)*n/tot )
	l="\r[" 
	if a==n: l += "="*n
	elif a> 0: l +=  "="*(a-1) + ">" + " "*(n-a)
	else: l += " "*n
	l+="] %.1f%%"%(float(done)*100./tot)
	if a==n: l+="\n"
	print l,
	sys.stdout.flush()

# create a dict structure to map nentries
def createDict(t):
	r={}
	# activate only mc,run,lumi,evt
	t.SetBranchStatus("*",0)
	t.SetBranchStatus("mc",1)
	t.SetBranchStatus("run",1)
	t.SetBranchStatus("lumi",1)
	t.SetBranchStatus("evt",1)
	n=t.GetEntries()
	for ientry in range(0,t.GetEntries()):
		PrintStatus(ientry,n)
		t.GetEntry(ientry)
		if t.mc != 333: continue
		r[ (t.mc,t.run,t.lumi,t.evt) ] = ientry
	return r

if doCov:
	#print "-> First Loop on ",file0
	#d0= createDict(tree0)
	print "-> First Loop on ",file1
	d1= createDict(tree1)
	print 
	#print "d1 is ", d1

	print "-> Join Loop"
	#branches=["lep1_pt","NJets","ht","st"]

	hCov = ROOT.TH2D("cov" ,"cov" ,len(branches),0,len(branches),len(branches),0,len(branches))
	hCovRel = ROOT.TH2D("covrel" ,"cov on chosen relative quantities" ,len(branches),0,len(branches),len(branches),0,len(branches))
	hNorm= ROOT.TH2D("norm","norm",len(branches),0,len(branches),len(branches),0,len(branches))
	
	tree0.SetBranchStatus("*",0)
	tree1.SetBranchStatus("*",0)
	spect = ["weight","run","lumi","evt","mc"]
	for s in spect:
		tree0.SetBranchStatus(s,1)
		tree1.SetBranchStatus(s,1)
	
	tree0.SetBranchStatus("*",1)
	tree1.SetBranchStatus("*",1)
	for idx,b in enumerate(branches):
		#tree0.SetBranchStatus(b,1)
		#tree1.SetBranchStatus(b,1)
		hCov.GetXaxis().SetBinLabel(idx+1,b)
		hCov.GetYaxis().SetBinLabel(idx+1,b)
		hCovRel.GetXaxis().SetBinLabel(idx+1,b)
		hCovRel.GetYaxis().SetBinLabel(idx+1,b)
	
	n=tree0.GetEntries()
	for ientry in range(0,tree0.GetEntries()):
		PrintStatus(ientry,n)
		tree0.GetEntry(ientry)
		if tree0.mc != 333: continue
		if (tree0.mc,tree0.run,tree0.lumi,tree0.evt) not in d1: continue;
		tree1.GetEntry(d1[ (tree0.mc,tree0.run,tree0.lumi,tree0.evt)] )
		for i,btmp in enumerate(branches):
		      ## i need to compute s= 1/N Sum |xUp-xNom| 
		      ### s2' ~=  1/N Sum (xUp-xNom)^2 ~ s2 + var(s), I don't think we want this
		      ## correlation is 1 because the source is 1. will be different when we will add different sources
		      if btmp[0] == "!": 
			      bi= btmp[1:]
			      doRel=True
		      else: 
			      bi = btmp[:]
			      doRel=False

		      if ientry == 0 : 
			      print "* Branch",bi,"doRel=",doRel
	
		      try:
		      	#valNom = eval("tree0."+bi)
		      	#valUp = eval("tree1."+bi)
			if '%' not in bi:
		      		valNom = float(eval("tree0."+bi))
		      		valUp = float(eval("tree1."+bi))
			else:
				valNom = float(eval(bi%{"tree":"tree0"}))
				valUp = float(eval(bi%{"tree":"tree1"}))
		      except IndexError: ## jet_pt[xx] not defined
			      continue
	
		      #print "valNom",valNom,"valUp",valUp,bi
		      rel = 0.0
		      try: rel = abs(valNom-valUp)/valNom
		      except ZeroDivisionError: pass
	
		      if valNom <-90 or valUp <-90 or rel >100 or valNom > 99998: 
			      continue

		      if ROOT.TMath.IsNaN(rel): continue

		      #print "ABS=",abs(valNom-valUp),"REL",rel,"W",tree0.weight,"BRANCH",bi
	
		      ##
		      c= hCov.GetBinContent(i+1,i+1)
		      c+= abs(valNom-valUp) * tree0.weight
		      norm= hNorm.GetBinContent(i+1,i+1)
		      norm += tree0.weight
		      hCov.SetBinContent(i+1,i+1,c)
		      if doRel:
			      crel= hCovRel.GetBinContent(i+1,i+1)
			      crel+= (rel*tree0.weight)
			      hCovRel.SetBinContent(i+1,i+1,crel)
		      else:
			      hCovRel.SetBinContent(i+1,i+1,c)
		 
		      hNorm.SetBinContent(i+1,i+1,norm)
	
	#normalize
	for i,bi in enumerate(branches):
		c= hCov.GetBinContent(i+1,i+1)
		crel= hCovRel.GetBinContent(i+1,i+1)
		norm= hNorm.GetBinContent(i+1,i+1)
		try:hCov.SetBinContent(i+1,i+1,(c/norm)**2)
		except ZeroDivisionError: pass
		try:hCovRel.SetBinContent(i+1,i+1,(crel/norm)**2)
		except ZeroDivisionError: pass
	
	## full corr
	for i,bi in enumerate(branches):
	   for j,bj in enumerate(branches):
		if i==j: continue
		ci=math.sqrt(hCov.GetBinContent(i+1,i+1))
		cj=math.sqrt(hCov.GetBinContent(j+1,j+1))
		hCov.SetBinContent(i+1,j+1,ci*cj)
		creli=math.sqrt(hCovRel.GetBinContent(i+1,i+1))
		crelj=math.sqrt(hCovRel.GetBinContent(j+1,j+1))
		hCovRel.SetBinContent(i+1,j+1,creli*crelj)
	
	fOut=ROOT.TFile.Open("cov.root","RECREATE")
	fOut.cd()
	hCov.Write()
	hCovRel.Write()
	fOut.Close()

####

# need to be rewritten in c++, eval and exec are too slow
doTree = False
sw=ROOT.TStopwatch()
sw.Reset()
#import random

if doTree:
	print "-> Doing Tree"
	fIn=ROOT.TFile.Open("cov.root")
	hCovRel= fIn.Get("covrel")
	fOut = ROOT.TFile("output.root","RECREATE")
	tOut = tree0.CloneTree(0)
	tOut.SetName("tb_tree_syst")
	n=tree0.GetEntries()
	for ientry in range(0,tree0.GetEntries()):
		PrintStatus(ientry,n)
		for itoy in range(0,10):
			tree0.GetEntry(ientry)
			#smear = random.gauss(0,1)
			smear = ROOT.gRandom.Gaus(0,1)
			for ib,btmp in enumerate(branches):
			      if btmp[0] == "!": 
				      bi= btmp[1:]
				      doRel=True
			      else: 
				      bi = btmp[:]
				      doRel=False
			      sigma = math.sqrt(hCovRel.GetBinContent(ib+1 ,ib+1))

			      #take nominal value
			      try:
			        if '%' not in bi:
			              valNom = float(eval("tree0."+bi))
			        else:
				      valNom = float(eval(bi%{"tree":"tree0"}))
			      except IndexError: ## array is not there, nothing to set
				      continue

			      # do shifts
			      if doRel:
				      valNom *= (1.+ smear*sigma)
			      else:
				      valNom += smear*sigma
			      # set branch value
			      if '%' not in bi:
				      exec("tree0."+bi +"= valNom")
			      else:
				      exec(bi%{"tree":"tree0"} +"= valNom")

			#Fill output tree
			tOut.Fill()
		#end toy loop
	#end entry loop
	tOut.Write()
	fOut.Close()
	print
	print "********"
	print "* DONE *"
	print "********"
