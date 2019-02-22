import ROOT
import os,sys
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-v","--verbose",dest="verbose",action='store_true',help="Verbose",default=False)
parser.add_option("-o","--outfile",dest="outfile",type="string",help="OutputFile [%default]",default="QCDFit.root")
parser.add_option("","--xmin",dest="xmin",type='float',help="xmin",default=110)
parser.add_option("","--xmax",dest="xmax",type='float',help="xmax",default=150)
(opts,args) = parser.parse_args()

if opts.verbose: print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
    if "ChargedHiggs" in os.path.basename(mypath):
        basepath = os.path.abspath(mypath)
    mypath = os.path.dirname(mypath)
################ IMPORT ROOT  ################
if opts.verbose: print "-> Importing root",
sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()
if opts.verbose: print "DONE"

################ LOAD LIBRARY ###############
if opts.verbose: print "-> Load Bare library" ## probably not needed
r.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
if opts.verbose: print "-> Load ChargedHiggs library"
r.gSystem.Load( "./bin/libChargedHiggs.so")
if opts.verbose: print "DONE",

fitter = r.BackgroundFitter()
#fitter.outname= opts.outfile
fitter.xmin = opts.xmin
fitter.xmax = opts.xmax
fitter.init()
w = ROOT.RooWorkspace("w","w")
x=ROOT.RooRealVar("mmm","mmm",opts.xmin,opts.xmax);
modelBuilder = r.PdfModelBuilder();
modelBuilder.setObsVar(x);

#import
def Import(w,o):
    #getattr(w,"import")(o,ROOT.RooCmdArg())
    getattr(w,"import")(o,ROOT.RooFit.RecycleConflictNodes())

models={
        "fewz_full": "aux/fewz/h2mu-dsdm-13tev-xs-lux-full-nnlo-hp.dat",
        "fewz_new" : "aux/fewz/h2mu-13tev-xs-lux-full-nnlo-nlo.dat",
        }
l=[]
for modelname in models: 
    for order in [0,1,2,3,4]:
        p=modelBuilder.getPolyTimesFewz(modelname + "_ord%d"%order,order, models[modelname])
        l.append(p)
        Import(w,p)
        n=ROOT.RooRealVar(p.GetName()+"_norm","norm",100,0.,100000)
        Import(w,p)

w.writeToFile(opts.outfile)


