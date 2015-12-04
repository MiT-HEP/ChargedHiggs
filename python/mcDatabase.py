#!env python

import re
import os,sys
import math
from optparse import OptionParser
from subprocess import call, check_output

parser = OptionParser(usage = "usage");
parser.add_option("-e","--eos",dest="eos",type="string",help="eos directory to scout, will not read the files in the pSet",default="");
parser.add_option("-x","--xsec",dest="xsec",type="float",help="Use external cross-section",default=-1);
parser.add_option("-l","--label",dest="label",type="string",help="MC label",default="DYamcatnlo");
parser.add_option("-f","--file",dest="file",type="string",help="mc_database file name",default="dat/mc_database.txt");
parser.add_option("-r","--recursive", dest='rec', action= 'store_true', help="do same for each subdir", default =False);
parser.add_option("-p","--pileup",dest="pu",type="string",help="TODO FIXME")

(opts,args)=parser.parse_args()

EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"

if opts.rec:
	cmd = EOS +" find -d "+opts.eos
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
		label = re.sub('.*/','',dir)
		if nd==0 and nf >0 and not re.match("Run2015",dir): ## remove data
			print "Found one directory:",dir
			cmd = "python %s -e %s -x %f -l %s -f %s"%(sys.argv[0],dir,opts.xsec,label,opts.file)
			print "going to execute",cmd
			call(cmd,shell=True)
	exit(0)

cmd = EOS+ " find -f " + opts.eos

outputList = check_output(cmd,shell=True)
fileList0 = outputList.split() ## change lines into list
fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]

import ROOT as r
r.gROOT.SetBatch()

## TODO count mcWeights, nEntries
n=0
myTmp= r.TFile.Open("/tmp/" + os.environ["USER"] + "/mytmp.root","RECREATE")
myTmp.cd()
sum=r.TH1D("SumWeights","Sum of mcWeights",1,0,2)

for idx,fName in enumerate(fileList):
	print "processing file:",idx,"/",len(fileList)," : ", fName
	fROOT = r.TFile.Open( fName )
	if idx == 0:
		myTmp.cd()
		h_xSec = fROOT.Get("nero/xSec").Clone("myxSec")
	else:
		h = fROOT.Get("nero/xSec")
		print "\txSec in file is ",h.GetBinContent(1)/h.GetBinContent(2),"+/-",math.sqrt(1./h.GetBinContent(2))
		h_xSec.Add( h )

	t = fROOT.Get("nero/all")
	mysum=r.TH1D("mysum","Sum of mcWeights",1,0,2)
	t.Draw("1>>mysum","mcWeight") ##>>+ doesn't work
	sum.Add(mysum)
	n += t.GetEntries()
	fROOT.Close()


print "---------------------------------------------"
print "internal xSec = ",h_xSec.GetBinContent(1)/h_xSec.GetBinContent(2), "+/-", math.sqrt(1./h_xSec.GetBinContent(2))
print "SumWeights = ", sum.GetBinContent(1)
print "Tot Entries = ", n
print "---------------------------------------------"
### LABEL dir Entries xSec
f = open (opts.file,"a")
print>>f, opts.label,opts.eos, sum.GetBinContent(1),
if opts.xsec >0 : print>>f, opts.xsec,
else: print>>f,  h_xSec.GetBinContent(1)/h_xSec.GetBinContent(2)
print "---------------------------------------------"

