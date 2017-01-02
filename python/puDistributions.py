#!/bin/env python

import re
import os,sys
import math
from optparse import OptionParser
from subprocess import call, check_output

parser = OptionParser(usage = "usage");
parser.add_option("-e","--eos",dest="eos",type="string",help="eos directory to scout, will not read the files in the pSet",default="");
parser.add_option("-l","--label",dest="label",type="string",help="MC label",default="DYamcatnlo");
parser.add_option("-f","--file",dest="file",type="string",help="Pileup file (Output in UPDATE mode)",default="aux/pileup.root");
parser.add_option("","--run",dest="run",type="string",help="run list",default="");
parser.add_option("-r","--recursive", dest='rec', action= 'store_true', help="do same for each subdir", default =False);
parser.add_option("","--mcdb",dest="mcdb",type="string",help="dump from mcdb",default="");

(opts,args)=parser.parse_args()

EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
if '/eos/user' in opts.eos: EOS += " root://eosuser"

if opts.eos=="" and opts.mcdb!="":
	print "Inserting python in path"
	sys.path.insert(0,os.getcwd())
	sys.path.insert(0,os.getcwd()+"/python")
	from  ParseDat import ReadMCDB
	import ROOT
	ROOT.gROOT.SetBatch()
	mcdb=ReadMCDB(opts.mcdb)
	for key in mcdb:
		fIn=ROOT.TFile.Open(opts.file)
		h = fIn.Get("PU-" + key ) 		
		toRun=False ## make sure fIn is closed correctly
		if h==None:toRun=True
		fIn.Close()
		if toRun:
			cmd = "python %s -e %s -l %s -f %s --run '%s'"%(sys.argv[0],mcdb[key][0],key,opts.file,opts.run)
			print "going to execute",cmd
			call(cmd,shell=True)
	exit(0)	

if opts.rec:
	cmd = EOS +" find -d "
##	if '/eos/user' in opts.eos: cmd += " --childcount "
	cmd += " --childcount "
	cmd+=opts.eos
	list = check_output(cmd,shell=True);
	for line in list.split('\n'):
		if line =='': continue
		print "Line is '"+line+"'"
		dir = line.split() [0]
		ndir = line.split() [1]
		nfiles = line.split() [2]
		#
		nd = float(re.sub('ndir=','',ndir) )
		nf = float(re.sub('nfiles=','',nfiles) )
		dir = re.sub('/eos/cms','',dir)
		if dir[-1] == '/' : dir = dir[:-1] # remove trailing slash

		## find label: directory not containing only numbers, hyphens, underscore, or empty
		dirs=dir.split('/')
		idx=len(dirs) -1 
		while idx >=0 :
			label = dirs[idx]
			if re.match( '^[0-9_\-]*$',label): 
				idx -= 1
			else:
				break
		if idx <0: label = re.sub('.*/','',dir)

		if nd==0 and nf >0 and not re.match("Run2015",dir) : # exclude Run directories
			print "Found one directory:",dir
			cmd = "python %s -e %s -l %s -f %s --run '%s'"%(sys.argv[0],dir,label,opts.file,opts.run)
			print "going to execute",cmd
			call(cmd,shell=True)
	exit(0)

cmd = EOS+ " find -f " + opts.eos

outputList = check_output(cmd,shell=True)
fileList0 = outputList.split() ## change lines into list
if '/eos/user' in opts.eos:
	fileList = [ re.sub("/eos/user","root://eosuser///eos/user",f) for f in fileList0 if '/failed/' not in f ]
else:
	fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]

import ROOT as r
r.gROOT.SetBatch()

## TODO count mcWeights, nEntries
n=0

fOutput= r.TFile.Open(opts.file,"UPDATE")

nBins = 1000
xMin = -0.5
xMax = 1000-0.5

if opts.run =="" :
	## 1D
	name = "PU-%s"%opts.label
	h = fOutput.Get(name)
	if h != None : print "Overwriting",name

	puHist=[r.TH1F(name,"PU Distribution of %s"%opts.label,nBins,xMin,xMax)]
else:
	## RD
	puHist=[]
	for idx in range(0,len(opts.run.split(',')) -1):
		name = "RD-%s_%d_%d"%( opts.label, int(opts.run.split(',')[idx]), int(opts.run.split(',')[idx+1]) )
		h=fOutput.Get(name)
		if  h != None: print "Overwriting",name
		puHist.append(  r.TH1F(name,"RD PU Distribution of %s"%opts.label,nBins,xMin,xMax)  )



for idx,fName in enumerate(fileList):
	print "processing file:",idx,"/",len(fileList)," : ", fName
	fROOT = r.TFile.Open( fName )
	t = fROOT.Get("nero/all")

	if opts.run == "": #1D
		mypu=r.TH1F("mypu","mypu",nBins,xMin,xMax)
		t.Draw("puTrueInt>>mypu","mcWeight") ##>>+ doesn't work
		puHist[0].Add(mypu)
		mypu.Delete()
	else: ##RD
		for idx in range(0,len(opts.run.split(','))-1):
			mypu=r.TH1F("mypu","mypu",nBins,xMin,xMax)
			selection = "mcWeight*(%d <=runNum && runNum<%d)" % ( int(opts.run.split(',')[idx]) , int(opts.run.split(',')[idx+1]) )
			t.Draw("puTrueInt>>mypu",selection) ##>>+ doesn't work
			puHist[idx].Add(mypu)
			mypu.Delete()
	fROOT.Close()

fOutput.cd();
for h in puHist:
	h.Write("",r.TObject.kOverwrite)

print "-> ",opts.label,opts.run,"DONE"
