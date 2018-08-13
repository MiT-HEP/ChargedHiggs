import ROOT
import re
from optparse import OptionParser

usage="%p input output"
parser=OptionParser()
parser.add_option("-x","--exclude",action='append',help="exclude regex. Can be specified more than once.",default=[])
parser.add_option("-v","--verbose",action='store_true',help="Verbose",default=False)
opts,args = parser.parse_args()

if opts.verbose: print "Opening",args[0]
fIn=ROOT.TFile.Open(args[0])
if opts.verbose: print "Output is",args[1]
fOut=ROOT.TFile.Open(args[1],"RECREATE")

def CopyDir(d):
    for key in d.GetListOfKeys():
        name = key.GetName()
        if opts.verbose: print "* Considering",name
        toExclude=False
        for match in opts.exclude:
            if re.search(match,name): toExclude=True
        if toExclude: continue
        print "  -> Writing"
        fOut.cd()
        d.Get(name).Clone().Write()
    return

CopyDir(fIn)

print "-> Done"
