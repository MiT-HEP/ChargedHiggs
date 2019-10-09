import ROOT
from optparse import OptionParser, OptionGroup
usage=''' 
    Converts a TH1 (typically from FitDiagnostics) to a RooDatahist named histo_out on the 'mmm' observables in the 'w' output workspace
'''
parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="fitDiagnostics.root")
parser.add_option("","--hist",type='string',help="Input hist file. [%default]", default="shapes_prefit/cat0/total")
#
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="Out.root")
parser.add_option("","--histout",type='string',help="Output dataset file. [%default]", default="shapes_prefit/cat0/total")

opts, args = parser.parse_args()

def Import(w,obj):
    #getattr(w,'import')(obj,ROOT.RooCmdArg())
    getattr(w,'import')(obj,ROOT.RooFit.RecycleConflictNodes())

fIn=ROOT.TFile.Open(opts.input)
h=fIn.Get(opts.hist)
print "Scaling by binwidth", h.GetBinWidth(10)
h.Scale(h.GetBinWidth(10))## assuming constant
x = ROOT.RooRealVar("mmm","mmm",110,150);
arglist_obs = ROOT.RooArgList(x)

w=ROOT.RooWorkspace("w","w")
roo_data= ROOT.RooDataHist(opts.histout,"myhist",arglist_obs,h)

Import(w,roo_data)
w.writeToFile(opts.output)
del w
