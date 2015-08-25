import ROOT
import os,sys,re
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-f","--file",dest="file",type='string',help="Input File Name, Default=%default.",default="ChHiggsEWKEmbedding.root")
parser.add_option("-t","--tree",dest="tree",type='string',help="Input Tree Name, Default=%default.",default="embedding")
parser.add_option("-o","--output",dest="output",type='string',help="Output File, Default=%default.",default='aux/embedding.txt')
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)


(opts,args) = parser.parse_args()


fIn = ROOT.TFile.Open( opts.file)
if fIn == None:
	print "Error: '" + opts.file + "' no such file"
	exit(1)

tIn = fIn.Get(opts.tree)
if tIn == None:
	print "Error: '" + opts.tree + "' no such tree"
	exit(1)

if not tIn.InheritsFrom("TTree"):
	print "Error: '" + opts.tree + "' is not a tree"
	exit(1)

out = open( opts.output , "w" ) 
out.write("#run lumi event np pdgid pt eta phi\n")

for iEntry in range(0,tIn.GetEntries() ) :
	tIn.GetEntry(iEntry)
	out.write("%d %d %d 1 "%(tIn.run,tIn.lumi,tIn.event))
	out.write("15 %f %f %f"%(tIn.muPt,tIn.muEta,tIn.muPhi) )
	out.write("\n")

out.close()
