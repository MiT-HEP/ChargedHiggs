import sys
import math
from optparse import OptionParser, OptionGroup
usage="Perform the wald test on the distribution"
parser=OptionParser(usage=usage)
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="Hmumu/2017_06_02/BackgroundModel.root ")
parser.add_option("-c","--cat",dest='cat',type="string",help="do cat xxx for bkg [%default]",default="cat0")
parser.add_option("-p","--pdf",dest='pdf',type="string",help="pdg: exp, zmod (ord hard-coded) [%default]",default="exp")
opts,args= parser.parse_args()

########### IMPORT ROOT #############
oldArgv=sys.argv[:]
sys.argv=[]
import ROOT

sys.argv=oldArgv[:]

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
#if opts.outdir != "":
if True:
	ROOT.gROOT.SetBatch()

## IMPORT COMBINE
ROOT.gSystem.Load("libHiggsAnalysisCombinedLimit.so")

## get files
fIn = ROOT.TFile.Open(opts.input,"READ")
if fIn == None: 
    print "<*> NO File '%s'"%opts.input
    raise IOError

w=fIn.Get("w")
#RooRealVar *x=(RooRealVar*)w->var("mmm")
x=w.var("mmm")
data=w.data("dataObs_"+opts.cat)

for ord in range(0,10):
    pdf = w.pdf(opts.pdf+"_"+opts.cat+"_ord%d"%ord)
    if pdf != None:
        print "Using pdf=",opts.pdf+"_"+opts.cat+"_ord%d"%ord
        break
if pdf == None:
    print "<*> Unable to find pdf of type",opts.pdf

# make a dummy plot and plot the pdf on top -> normalization ?
# pdf.getVal()
# pdf.getNorm()
# RooArgSet obs(x) ;
# Double_t val = gauss.getVal(&obs) 
norm = data.sumEntries()
x0=w.var("mmm")
#cdf=pdf.createCdf(ROOT.RooArgSet(x0))
Np=0
Nm=0
Nr=0

chi2=0.0

last=0
lastP=0
for i in range(0,data.numEntries()):
    x=data.get(i).getRealValue("mmm")
    y=data.weight()
    w=data.binVolume()
    x0.setVal(x)
    y0 = pdf.getVal(ROOT.RooArgSet(x0)) *norm *w
    x0.setVal(x+w/2.)
    #I0 = cdf.getVal(ROOT.RooArgSet(x0))
    #Y0=(I0-last)*norm
    #print "x=",x,"y=",y,"y0=",y0,"w=",w,"integral=",Y0
    # the correct thing is to use Y0. y0=median*width is a good approximation
    #
    #if (y>=Y0) :
    if (y>=y0) :
        Np +=1
        if lastP<0: Nr+=1
        lastP=1
    else : 
        Nm +=1
        if lastP>0: Nr+=1
        lastP=-1
    chi2 += (y-y0)**2/max(y0,1) ## pearson

    #last=I0


N=Np+Nm
mu=2*Np*Nm/N+1
sigma=math.sqrt((mu-1)*(mu-2)/(N-1))

try:
    z=(Nr-mu)/sigma
    if z>0:
        pOne=1.-1./2.*(1+ROOT.TMath.Erf(z/math.sqrt(2)))
        pTwo=2*pOne
    else:
        pOne=1./2.*(1+ROOT.TMath.Erf(z/math.sqrt(2)))
        pTwo=2*pOne
except ZeroDivisionError:
    z=-1
    pOne = -1
    pTwo = -1

npar=pdf.getVariables().getSize() ## x=1 -1; +1 = norm
prob=ROOT.TMath.Prob(chi2,N-npar)
print "Np=",Np,"Nm=",Nm,"Nr=",Nr,"mu=",mu,"sigma=",sigma,"z=",z,"p-value (one sided)=",pOne, "p-value (two)",pTwo,"chi2=",chi2,"npar=",npar,"prob=",prob

