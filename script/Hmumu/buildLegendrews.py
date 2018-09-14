import os,sys
from array import array
import re,math

from optparse import OptionParser, OptionGroup
parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/a/amarini/public/forSagar/Hmumu/2017_08_21_NewCat5_NoTTH/BackgroundModel.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="MC.root")
parser.add_option("-m","--mh",type='float',help="Higgs boson mass [%default]",default=125.)
parser.add_option("-c","--cat",type='int',help="Category [%default]",default=14)
parser.add_option("-w","--width",type='float',help="Higgs boson window [%default]",default=15.)
parser.add_option("-n","--ncore",type='int',help="Use ncore [%default]",default=-1)
opts,args= parser.parse_args()

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
from hmm import *

oldArgv=sys.argv[:]
sys.argv=[]
import ROOT

sys.argv=oldArgv[:]

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

### general functions
def Import(w,obj):
    #getattr(w,'import')(obj,ROOT.RooCmdArg())
    getattr(w,'import')(obj,ROOT.RooFit.RecycleConflictNodes())

def GoodnessOfFit(data,pdf,mass,name=""):  
    plot_chi2 = mass.frame();
    data.plotOn(plot_chi2,ROOT.RooFit.Binning(100),ROOT.RooFit.Name("data"));
    pdf.plotOn(plot_chi2,ROOT.RooFit.Name("pdf"));
    np = pdf.getParameters(data).getSize()
    chi2 = plot_chi2.chiSquare("pdf","data",np);
    if name !="":
        c=ROOT.TCanvas(name,name)
        plot_chi2.Draw()
        c.SaveAs(name)
    return ROOT.TMath.Prob(chi2,np)

def Saturated(data,pdf):
    hpdf = ROOT.RooHistPdf("%s_shape"% data.GetName(), "", data.get(), data);
    norm = ROOT.RooConstVar("%s_norm"% data.GetName(), "", data.sumEntries());
    saturatedPdf = ROOT.RooAddPdf("%s_saturated"% data.GetName(), "", ROOT.RooArgList(hpdf), ROOT.RooArgList(norm))
    
    saturated_nll=saturatedPdf.createNLL(data)
    nominal_nll = pdf.createNLL(data)

    nll_nominal = nominal_nll.getVal();
    nll_saturated = saturated_nll.getVal();

    limit = 2*(nll_nominal-nll_saturated);
    return limit

def runFit(data,model):
    ntry=0
    status=1
    while status !=0 and ntry<3:
        ntry+=1
        fitres=model.fitTo(data,
                ROOT.RooFit.Save(), 
                ROOT.RooFit.PrintEvalErrors(-1),
                ROOT.RooFit.PrintLevel(-1), 
                ROOT.RooFit.Warnings(0)
                )
        status= fitres.status();
    nll=fitres.minNll();
    return nll

import threading
import time
#from Queue import Queue
lock=threading.Lock()

def GoFSaturated(data,pdf,mass,ntoys=1000):
    global lock
    print "-> Running Saturated model"
    runFit(data,pdf)
    obs=Saturated(data,pdf)
    All=[]
    print "toy",
    threads=[]
    def runToy(itoy,All):
        global lock
        print itoy,
        nEvents=data.sumEntries() #-> Poisson?
        toy=pdf.generateBinned(ROOT.RooArgSet(mass),nEvents)
        runFit(toy,pdf)
        sat=Saturated(toy,pdf)
        lock.acquire()
        All.append(sat)
        lock.release()
    for itoy in range(0,ntoys):
        if opts.ncore<0:runToy(itoy)
        else: 
            while threading.activeCount() > opts.ncore: time.sleep(3)
            t=threading.Thread(target=runToy, args=(itoy,All,))
            t.start()
            threads.append(t)
    print 
    for t in threads:t.join()
    pruned=[x for x in All if x>=obs]
    runFit(data,pdf)
    return float(len(pruned))/len(All)


ROOT.RooMsgService.instance().setGlobalKillBelow(ROOT.RooFit.ERROR);

wOut=ROOT.RooWorkspace("w","w")

fIn=ROOT.TFile.Open(opts.input)
if fIn==None: raise IOError("Input file '"+opts.input+"' does not exist")

wIn=fIn.Get("w")
x=wIn.var("mmm")
pdfIn=wIn.pdf("zmod_cat%d_ord1"%opts.cat)
xmin=opts.mh-opts.width
xmax=opts.mh+opts.width
x.setRange(xmin,xmax)
data=wIn.data("dataObs_cat%d"%opts.cat)

rebinMass=ROOT.RooFormulaVar ("rebinMass","Rebinned Mass","2*(@0-%(xmin)f)/(%(xmax)f-%(xmin)f) - 1."%{"xmax":xmax,"xmin":xmin},ROOT.RooArgList(x));

maxOrder=10 ## even?
legendre=[ ROOT.RooLegendre("l_cat%d_l%d"%(opts.cat,i),"l%d"%i,rebinMass,i) for i in range(0,maxOrder+1)]

order=[0]
order.extend( [2*i+1 for i in range(0,maxOrder/2)])
order.extend( [2*i for i in range(0,maxOrder/2+1) ])
print "Order is",order

bestnll={} # n -> nll,included

storedPdfs =ROOT.RooArgList("stored");
gc=[]
for im in range(0,2**(maxOrder+1)):
    if im%2 == 0 : continue # force to have the constant term for mult
    modelStr="(1+"
    lastIdx=0
    modelName="leg_model%d"%im
    arglist=ROOT.RooArgList()
    gc.append(arglist)
    #arglist.add(pdfIn) #@0
    n=0
    print "Considering Model",im,":",
    for il in range(0,maxOrder+1):
        if il==0 : continue # already in
        if not ((im>>il)&1) : continue
        if lastIdx != 0 : modelStr+="+"
        coef=ROOT.RooRealVar("coef_cat%d_Model%d_il%d"%(opts.cat,im,order[il]),"legendre coeff",0,-0.05,0.05)
        modelStr += "@%d*@%d"%(lastIdx,lastIdx+1)
        gc.append(coef)
        arglist.add(legendre[order[il]])
        arglist.add(coef)
        lastIdx+=2
        print order[il],
        n+=1
    modelStr+=")"
    print " | Tot=",n," | ",modelStr
    if im!=1:
        modelL=ROOT.RooGenericPdf(modelName,modelName,modelStr,arglist)
        model= ROOT.RooProdPdf("model%d"%im,"model%d"%im,pdfIn,modelL)
        gc.append(model)
        gc.append(modelL)
    else:
        model=pdfIn
    print "--> FIT"
    ntry=0
    status=1
    while status !=0 and ntry<3:
        ntry+=1
        fitres=model.fitTo(data,
                ROOT.RooFit.Save(), 
                ROOT.RooFit.PrintEvalErrors(-1),
                ROOT.RooFit.PrintLevel(-1), 
                ROOT.RooFit.Warnings(0)
                )
        status= fitres.status();
    nll=fitres.minNll();
    #gof=GoodnessOfFit( data,model,x,"")#,"plotMW/powlawLog_cat%d.pdf"%cat)
    #gof=GoFSaturated(data,model,x)
    #print "GOF For model",im,"=",gof
    gof=0

    print "FitStatus=",status,"nll=",nll

    toinclude=True
    if n not in bestnll:
        bestnll[n] = (nll,gof)
    else: 
        if bestnll[n][0] < nll: bestnll[n]=(nll,gof)

    if n-1 in bestnll:
        deltanll = bestnll[n-1][0] -nll
        print "deltanll",bestnll[n-1][0],nll,deltanll
        if deltanll>0: # <0, means that some different configurations of polys
            chi2=2*deltanll
            prob=ROOT.TMath.Prob(chi2,1);
            print "ProbF=",prob
            if prob >0.05 : toinclude=False
        else: toinclude=False

        #if gof -bestnll[n-1][1] <0.05: toinclude=False
    else:
        print "-> This logL",nll


    if toinclude:print "-> Included. "
    else :print "-> Not Included. (STOP?)"

    storedPdfs.add(model)
    Import(wOut,model)
print "BESTNLL"
print bestnll
print "-----"
pdf_cat=ROOT.RooCategory("pdfindex_cat%d"%opts.cat,"c")
pdf_bkg=ROOT.RooMultiPdf("pdf_cat%d_bkg"%opts.cat,"multipdf",pdf_cat,storedPdfs)
pdf_norm=ROOT.RooRealVar("pdf_cat%d_bkg_norm"%opts.cat,"norm",data.sumEntries())
pdf_norm.setConstant(False)

Import(wOut,pdf_bkg)
Import(wOut,pdf_cat)
Import(wOut,pdf_norm)
print "-> Writing"
wOut.writeToFile(opts.output)

print "->Done"
del wOut
