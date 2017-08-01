import ROOT 
import os,sys
from array import array
import re,math
from optparse import OptionParser, OptionGroup
print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
	if "ChargedHiggs" in os.path.basename(mypath):
		basepath = os.path.abspath(mypath)
	mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")
from hmm import hmm,hmmAutoCat,hmmWithTTH, Stack
parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Background.root")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmmWithTTH")
parser.add_option("","--unblind",dest="blind",action='store_false',help="Un/Blind plots [%default]",default=True)
parser.add_option("-p","--plot",type='string',help="Plot Dir [%default]", default="Hmumu/Plot/bkg")
opts,args= parser.parse_args()


fIn = ROOT.TFile.Open(opts.input)
if fIn == None:
    print "<*> Error,",opts.input,"no such file or directory"

w = fIn.Get("w")
if w == None:
    print "<*> Error, unable to locate workspace w in input file"

config= eval(opts.hmm)
config.Print()

if len(config.bkg_functions) == 0 : 
    print "<*> Error, no bkg functions in configuration"

try:
    os.mkdir(opts.plot)
except OSError:
    pass

obj=[]
for icat,cat in enumerate(config.categories):
    print "* Doing cat",cat
    x = w.var("mmm")
    c=ROOT.TCanvas(cat,cat,800,800)
    p=x.frame()
    data=w.data("dataObs_"+cat)
    if opts.blind:
        data.plotOn(p,ROOT.RooFit.Name("data"), ROOT.RooFit.CutRange("unblindReg_1"));
        data.plotOn(p,ROOT.RooFit.Name("data"), ROOT.RooFit.CutRange("unblindReg_2"));
        data.plotOn(p,ROOT.RooFit.Name("data"), ROOT.RooFit.Invisible());
        p.SetMinimum(0.1);
    else:
        data.plotOn(p,ROOT.RooFit.Name("data"));
    pdf=w.pdf( config.bkg_functions[icat])
    pdf.plotOn(p)

    obj.extend([data,c,p,pdf])
    p.Draw()
    c.SaveAs(opts.plot +"/final_bkg_"+ cat+ ".pdf"  );
    c.SaveAs(opts.plot +"/final_bkg_"+ cat+ ".png"  );
    #raw_input("ok?")



