import os,sys
from array import array
import re,math

from optparse import OptionParser, OptionGroup
parser=OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. If comma separated 2016,2017,2018[%default]", default="Hmumu.root")
parser.add_option("","--binput",type='string',help="Backgorund Model Input ROOT file. If none skip this part. [%default]", default="")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="MC.root")
parser.add_option("-v","--var",dest='var',type="string",help="variable [%default]",default="Mmm")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmm")
parser.add_option("-s","--smooth",dest="smooth",action="store_true",help="Smooth systematic histograms [%default]",default=False)
#
parser.add_option("-n","--doNorm",dest="doNorm",action="store_true",help="Do Normalization terms [%default]",default=True)
parser.add_option("-x","--noNorm",dest="doNorm",action="store_false",help="Do NOT Normalization terms")
#
parser.add_option("","--fit",dest="fit",action="store_true",help="Smooth systematic histograms [%default]",default=False)
parser.add_option("","--plot",dest="plot",action="store_true",help="Produce plots [%default]",default=False)
parser.add_option("","--batch",dest="batch",action="store_true",help="Force batch in plot mode [%default]",default=False)
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
if not opts.plot or opts.batch:ROOT.gROOT.SetBatch()

### general functions
def Import(w,obj):
    #getattr(w,'import')(obj,ROOT.RooCmdArg())
    getattr(w,'import')(obj,ROOT.RooFit.RecycleConflictNodes())

## Global variables
#MonteCarlo=["DYJetsToLL_M-105To160","TT",'TTTT','TTW','TTZ','TTG','TT','ST','WWW','WWZ','WZZ','ZZZ','WW','WZ','ZZ','EWK_LLJJ' ]
MonteCarlo=["DYJetsToLL_M-105To160","TT",'TTTT','TTW','TTZ','TTG','ST','WWW','WWZ','WZZ','ZZZ','WW','WZ','ZZ' ]
MonteCarlo=["DYJetsToLL_M-105To160","TT",'ST']
Systs=["JES","PU","ScaleRF:DY","ScaleR:DY","ScaleF:DY","ScaleR:TT","ScaleF:TT","ScaleRF:TT" ] #"Linear:0.2"]
Systs=["Linear:0.2"]
#Systs=["ScaleRF:DY"] 
config= eval(opts.hmm)
config.Print()
w=ROOT.RooWorkspace("w","w")
fIn=ROOT.TFile.Open(opts.input)
fInYears=[]
lumis=[]
c2 = HmmConfig() ## for lumi and years
if fIn==None: 
    if ',' in opts.input:
        for f in opts.input.split(','):
            fInYears.append(ROOT.TFile.Open(f))
            if fInYears[-1] == None:
                raise IOError("Input file '"+f+"' does not exist")
            if '2016' in f: 
                c2.year=2016
                lumis.append(c2.lumi())
            if '2017' in f: 
                c2.year=2017
                lumis.append(c2.lumi())
            if '2018' in f: 
                c2.year=2018
                lumis.append(c2.lumi())
    else:
        raise IOError("Input file '"+opts.input+"' does not exist")

print "lumis are:",lumis,len(fInYears)

x=ROOT.RooRealVar("mmm","mmm",config.xmin,config.xmax)
arglist_obs=ROOT.RooArgList(x)

#####
import re
def GetMC(cat,nuis="",match=""):
    print "->Getting MC:",cat,nuis,match
    h=None
    for idx,mc in enumerate(MonteCarlo):
        useNuis=True
        if match != "":
            useNuis=False
            if re.search(match,mc): useNuis = True
        nuis_str = ""
        if useNuis: nuis_str = nuis[:]

        if fIn!=None:
            hTmp=fIn.Get("HmumuAnalysis/Vars/Mmm_"+cat+"_"+mc+nuis_str)
            h.Scale(config.lumi())

        else:
            hTmp=fInYears[0].Get("HmumuAnalysis/Vars/Mmm_"+cat+"_"+mc+nuis_str)
            if hTmp==None:
                print "Unable to find MC", "Mmm_"+cat+"_"+mc+nuis_str
            else:
                hTmp.Scale(lumis[0])
                for ifile in range(1,len(fInYears)):
                    hTmp2=fInYears[ifile].Get("HmumuAnalysis/Vars/Mmm_"+cat+"_"+mc+nuis_str)
                    if hTmp2 != None:
                        hTmp2.Scale(lumis[ifile])
                        hTmp.Add(hTmp2)


        if hTmp == None:
            if idx>1: ## make sure that DY and TT are there
                print "[WARNING]: Unable to get mc",mc,"for cat",cat
            else:
                print "[ERROR]: Unable to get mc",mc,"for cat",cat
                raise IOError

        if h == None: h=hTmp.Clone("simulation_"+cat)
        elif hTmp != None: h.Add(hTmp)

    bin0 = h.FindBin(config.xmin)
    bin1 = h.FindBin(config.xmax)
    for ib in range(0,bin0) + range(bin1+1,h.GetNbinsX()+2): ## excluded bin0, bin1
        h.SetBinContent(ib,0)
        h.SetBinError(ib,0)

    return h

histos=[]
statscaling={}
for cat in config.categories:
    print "-> Producing MC targets (RooDataHist)"
    h=GetMC(cat)    
    bin0 = h.FindBin(config.xmin)
    bin1 = h.FindBin(config.xmax)
    #scale h to effective lumi
    devents_a=array('d',[.0])
    events=h.IntegralAndError(bin0,bin1,devents_a)
    devents=devents_a[0]
    ## devents S = sqrt( S * events) 
    ## I want that devents' = sqrt(events')
    #S =  math.sqrt(events)/devents
    S =  events/(devents*devents)
    statscaling[cat]=S

    print "-> Stat scaling for Cat",cat, "is",S
    scale=ROOT.RooRealVar("S_"+cat,"stat scaling",S) ## this variable account for the statistics, should be constant. Statistical fluctuation will be put in the Likelihood
    scale.setConstant()
    Import(w,scale)

    h.Scale(S) ##??
    roo_mc= ROOT.RooDataHist("mc_"+cat,"mc_"+cat,arglist_obs,h)
    Import(w,roo_mc)
    histos.append(h)

objs=[]
if opts.binput != "":
    print "->Loading",os.environ['CMSSW_BASE']+"/lib/"+os.environ['SCRAM_ARCH']+"/libHiggsAnalysisCombinedLimit.so"
    ROOT.gSystem.Load(os.environ['CMSSW_BASE']+"/lib/"+os.environ['SCRAM_ARCH']+"/libHiggsAnalysisCombinedLimit.so")
    print "->Load LibBare"
    ROOT.gSystem.Load( "../NeroProducer/Core/bin/libBare.so")
    print "->Loading ChargedHiggs"
    ROOT.gSystem.Load("bin/libChargedHiggs.so")

    bIn=ROOT.TFile.Open(opts.binput)
    if bIn == None: raise IOError("Unable to open file:" + opts.binput)
    wb=bIn.Get("w")

    for icat,cat in enumerate(config.categories):
        name= config.bkg_functions[icat]
        h=GetMC(cat)    
        bfunc = wb.pdf(name)
        if bfunc == None: raise IOError("Unable to get func" + name)
        name2="bkg_func_binned_"+cat
        b2= ROOT.RooBinnedUncertainties(name2,name,x,bfunc, bin1-bin0,config.xmin,config.xmax)

        ## uncertainties reflected in the rate
        nuisRateStr="1."
        nuisRateAL = ROOT.RooArgList()
        nuisRateCount=0

        for syst_str in Systs:
            if ':' in syst_str: 
                s=syst_str.split(':')[0]
                match=syst_str.split(':')[1]
            else: 
                s= syst_str
                match=""
            if s == "Linear": ## DEBUG
                #raise ValueError("Do you really want me?")
                print "Do you really want me?"
                hup=h.Clone("LinearUp")
                hdown=h.Clone("LinearDown")
                alpha=float(match)
                for ib in range(bin0,bin1+1): ## excluded bin0, bin1
                    #print "Linear debug: Multiplying for",(1.+alpha*float(ib-bin0)/float(bin1-bin0) )
                    hup.SetBinContent(ib,h.GetBinContent(ib) * (1.+alpha*float(ib-bin0)/float(bin1-bin0) ))
                    hdown.SetBinContent(ib,h.GetBinContent(ib) * (1.-alpha*float(ib-bin0)/float(bin1-bin0) ))
            else:
                hup=GetMC(cat,"_"+s+"Up",match)    
                hdown=GetMC(cat,"_"+s+"Down",match)    
            
            ############### changing to deltas
            hup.Add(h,-1)
            hdown.Add(h,-1)

            up=ROOT.std.vector('float')()
            dn=ROOT.std.vector('float')()
            for i in range (0,bin1-bin0):
                try: up.push_back(hup.GetBinContent( bin0+i) / h.GetBinContent(bin0+i)  )
                except ZeroDivisionError: up.push_back(0.)
                try: dn.push_back(- hdown.GetBinContent( bin0+i) / h.GetBinContent(bin0+i) )# - because is going to be multiplied by -1 at -1sigma
                except ZeroDivisionError: dn.push_back(0.)

            if opts.smooth: 
                sigma=100 
                up2=ROOT.std.vector('float')()
                dn2=ROOT.std.vector('float')()
                for ibin in range(0,up.size()):
                    numUp=0.0
                    numDn=0.0
                    den=0.0
                    for jbin in range(0,up.size()):
                        weight=math.exp( -(ibin-jbin)**2/(2.*sigma))
                        den+=weight
                        numUp+=up[jbin] *weight
                        numDn+=dn[jbin] *weight
                    up2.push_back(numUp/den)
                    dn2.push_back(numDn/den)
                up=up2
                dn=dn2

            if opts.fit:

                upSmooth=ROOT.std.vector('float')(up)
                dnSmooth=ROOT.std.vector('float')(dn)  ## save for plotting

                htmpUp=ROOT.TH1D("tmpU","tmp",up.size(),0,up.size())
                htmpDn=ROOT.TH1D("tmpD","tmp",up.size(),0,up.size())
                for ibin in range(0,up.size()):
                    htmpUp.SetBinContent(ibin+1,up[ibin])
                    htmpDn.SetBinContent(ibin+1,dn[ibin])

                prev=0.
                func=None
                for n  in range(1,6):
                    myfunc=None
                    if n==1: myfunc=ROOT.TF1("myfunc1","[0]+x*[1]",0,up.size())
                    elif n==2:myfunc=ROOT.TF1("myfunc2","[0]+x*[1]+x*x*[2]",0,up.size())
                    elif n==3:myfunc=ROOT.TF1("myfunc3","[0]+x*[1]+x*x*[2]+[3]*x*x*x",0,up.size())
                    elif n==4:myfunc=ROOT.TF1("myfunc4","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x",0,up.size())
                    elif n==5:myfunc=ROOT.TF1("myfunc5","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x+[5]*x*x*x*x*x",0,up.size())
                    htmpUp.Fit("myfunc%d"%n,"WQN")
                    htmpUp.Fit("myfunc%d"%n,"WQNM")
                    chi2=myfunc.GetChisquare()
                    ftest= (prev-chi2)/1. / (chi2/(up.size()-n))
                    prob = 1.-ROOT.TMath.FDistI(ftest,1,up.size()-n)
                    prev=chi2
                    if n==5: func=myfunc
                    if n>1:print "-> fTest prob for degree",n,"prob=",prob
                    if (prob >0.05 and n > 1) or n==5:
                        # exit strategy
                        print "-> fTest choose degree",n,"for up variation","prob=",prob
                        for ibin in range(0,up.size()):
                            up[ibin] = func.Eval(htmpUp.GetBinCenter(ibin))
                        break
                    func=myfunc

                prev=0.
                func=None
                for n  in range(1,6):
                    myfunc=None
                    if n==1: myfunc=ROOT.TF1("myfunc1","[0]+x*[1]",0,dn.size())
                    elif n==2:myfunc=ROOT.TF1("myfunc2","[0]+x*[1]+x*x*[2]",0,dn.size())
                    elif n==3:myfunc=ROOT.TF1("myfunc3","[0]+x*[1]+x*x*[2]+[3]*x*x*x",0,dn.size())
                    elif n==4:myfunc=ROOT.TF1("myfunc4","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x",0,dn.size())
                    elif n==5:myfunc=ROOT.TF1("myfunc5","[0]+x*[1]+x*x*[2]+[3]*x*x*x+[4]*x*x*x*x+[5]*x*x*x*x*x",0,dn.size())
                    htmpDn.Fit("myfunc%d"%n,"WQN")
                    htmpDn.Fit("myfunc%d"%n,"WQNM")
                    chi2=myfunc.GetChisquare()
                    ftest= (prev-chi2)/1. / (chi2/(dn.size()-n))
                    prob = 1.-ROOT.TMath.FDistI(ftest,1,dn.size()-n)
                    prev=chi2
                    if n==5: func=myfunc
                    if n>1:print "-> fTest prob for degree",n,"prob=",prob
                    if (prob >0.05 and n > 1) or n==5:
                        # exit strategy
                        print "-> fTest choose degree",n,"for dn variation","prob=",prob
                        for ibin in range(0,dn.size()):
                            dn[ibin] = func.Eval(htmpDn.GetBinCenter(ibin))
                        break
                    func=myfunc

            nuisName = s
            if match != "": nuisName += "_" + match
            n = ROOT.RooRealVar(nuisName,"systematic: "+s +" " + match,-4.,4)
            objs.append(n)
            print "--> Adding uncertainty",s
            b2.addUncertainty(n, up, dn)
            print "--> Adding to the rate:" 
            nuisRateAL.add( n)

            I_nominal = h.Integral()
            DeltaIup=hup.Integral() / I_nominal
            DeltaIdown= - hdown.Integral() / I_nominal
            nuisRateStr += " + ( %f*@%d*(@%d>=0) ) + ( %f*@%d*(@%d<0) )"%(DeltaIup,nuisRateCount,nuisRateCount,DeltaIdown,nuisRateCount,nuisRateCount)
            nuisRateCount +=1

            if opts.plot:
                print "-> Plotting Syst",syst_str
                c=ROOT.TCanvas("c","c",800,800)
                hSystUp=ROOT.TH1D("hSystUp","hSystUp",up.size(),0,up.size())
                hSystDn=ROOT.TH1D("hSystDn","hSystDn",dn.size(),0,dn.size())
                for ibin in range(0,up.size()):
                    hSystUp.SetBinContent(ibin+1,+up[ibin])
                    hSystDn.SetBinContent(ibin+1,-dn[ibin])
                hSystUp.SetLineColor(38)
                hSystUp.SetFillColor(38)
                hSystUp.SetFillStyle(3004)
                hSystUp.SetLineWidth(2)
                hSystDn.SetLineColor(46)
                hSystDn.SetFillColor(46)
                hSystDn.SetFillStyle(3005)
                hSystDn.SetLineWidth(2)
                
                hSystUp.SetMaximum( max(hSystUp.GetMaximum(),hSystDn.GetMaximum(),0.045) *1.2)
                hSystUp.SetMinimum( min(hSystUp.GetMinimum(),hSystDn.GetMinimum(),-0.045) *1.2)
                hSystUp.Draw("H")
                hSystDn.Draw("H SAME")

                if opts.fit or opts.smooth:
                    hSystOrigUp=ROOT.TH1D("hSystOUp","hSystUp",up.size(),0,up.size())
                    hSystOrigDn=ROOT.TH1D("hSystODn","hSystDn",dn.size(),0,dn.size())
                    for ibin in range(0,up.size()):
                        try:hSystOrigUp.SetBinContent(ibin+1,hup.GetBinContent( bin0+ibin) / h.GetBinContent(bin0+ibin) )
                        except ZeroDivisionError: hSystOrigUp.SetBinContent(ibin+1,0)
                        try: hSystOrigDn.SetBinContent(ibin+1,-hdown.GetBinContent( bin0+ibin) / h.GetBinContent(bin0+ibin) )
                        except ZeroDivisionError: hSystOrigDn.SetBinContent(ibin+1,0)
                    hSystOrigUp.SetMarkerColor(ROOT.kRed)
                    hSystOrigDn.SetMarkerColor(ROOT.kBlue)

                    hSystOrigUp.SetMarkerStyle(20)
                    hSystOrigDn.SetMarkerStyle(21)
                    hSystOrigUp.SetMarkerSize(0.5)
                    hSystOrigDn.SetMarkerSize(0.5)

                    hSystOrigUp.Draw("P SAME")
                    hSystOrigDn.Draw("P SAME")

                if opts.fit and opts.smooth: ## Draw SmoothLine
                    hSystSmoothUp=ROOT.TH1D("hSystSmoothUp","hSystSmoothUp",up.size(),0,up.size())
                    hSystSmoothDn=ROOT.TH1D("hSystSmoothDn","hSystSmoothDn",dn.size(),0,dn.size())
                    for ibin in range(0,up.size()):
                        hSystSmoothUp.SetBinContent(ibin+1,+upSmooth[ibin])
                        hSystSmoothDn.SetBinContent(ibin+1,-dnSmooth[ibin])
                    hSystSmoothUp.SetLineColor(ROOT.kRed+2)
                    hSystSmoothUp.SetLineWidth(2)

                    hSystSmoothDn.SetLineColor(ROOT.kBlue+2)
                    hSystSmoothDn.SetLineWidth(2)

                    hSystSmoothDn.Draw("HIST SAME")
                    hSystSmoothUp.Draw("HIST SAME")


                ltx=ROOT.TLatex()
                ltx.SetNDC()
                ltx.SetTextFont(42)
                ltx.SetTextSize(0.03)
                ltx.SetTextAlign(11)
                ltx.DrawLatex(0.15,0.12,"Integral = %f/%f"%(DeltaIup,DeltaIdown))
                ltx.DrawLatex(0.15,0.16,"%s"%(syst_str))
                ltx.SetTextSize(0.04)
                ltx.DrawLatex(0.15,0.21,"#bf{%s}"%(cat))
                
                c.Modified()
                c.Update()
                if not opts.batch: raw_input("Ok?")
                c.SaveAs("bkgmc/"+cat+"_"+re.sub(':',"_",syst_str) +".pdf" )
    
        b2.info()
        Import(w,b2)
        objs.append(b2)

        nuisRate=ROOT.RooFormulaVar("bkg_func_binned_norm_nuis_"+cat,nuisRateStr,nuisRateAL)
        print "-> TotalNuisRate is:",nuisRateStr
        Import(w,nuisRate)
        objs.append(nuisRateAL)
        objs.append(nuisRate)
        
        ## to have the tot rate uncorrelated. Not scale by S, because the statscale is in the datacard
        #totRate=ROOT.RooRealVar("bkg_func_binned_"+cat+"_norm","rate",h.Integral()*statscaling[cat],0.5*h.Integral()*statscaling[cat],2*h.Integral()*statscaling[cat])
        if opts.doNorm:
            totRate=ROOT.RooRealVar("bkg_func_binned_"+cat+"_norm","rate",h.Integral(),0.5*h.Integral(),2*h.Integral())
            Import(w,totRate)

print "-> Writing"
w.writeToFile(opts.output)

print "->Done"
del w
