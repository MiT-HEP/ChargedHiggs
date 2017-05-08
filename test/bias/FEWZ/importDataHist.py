import sys
import ROOT
from array import array

w = ROOT.RooWorkspace("w","workspace") ;
x = ROOT.RooRealVar("mmm","mmm",110,150);

getattr(w,'import')(x)

fIn=ROOT.TFile.Open("test/Hmumu//Hmumu_2017_04_21_Bdt_ttH2/Hmumu_2017_04_21_Bdt_ttH2.root")
#fIn=ROOT.TFile.Open("test/Hmumu/Hmumu_2017_04_28_Bdt_amcatnlo/Hmumu_2017_04_28_Bdt_amcatnlo.root")
cat='cat0'

if len(sys.argv)> 1:
    cat=sys.argv[1]
    print "->Doing cat",cat
g=[]
g.extend([x,w])

h=fIn.Get("HmumuAnalysis/Vars/Mmm_"+cat+"_Data")
nevents=h.Integral(h.FindBin(110),h.FindBin(150))

#fewz=open("test/bias/FEWZ/h2mu-dsdm-13tev-xs-lux-1jet-nnlo-hp.dat")
fewz=open("test/bias/FEWZ/h2mu-dsdm-13tev-xs-lux-1jet-nnlo-hp.dat")
mass=[]
xsec=[]
tot=0.
for l in fewz:
    if len(l.split()) <2: continue
    m = float(l.split()[0])
    y = float(l.split()[1])
    if m < 109 or m > 151 : continue
    mass.append(m)
    xsec.append(y)
    tot += y

if mass[-1] <150:
    mass .append( 150 )
    xsec .append( (xsec[-2] - xsec[-1]) / (mass[-2]-mass[-1])*(150 - mass[-1]) + xsec[-1] ) 
if mass[0] > 110:
    massOld=mass[:]
    xsecOld=xsec[:]
    mass = [ 110 ] + massOld
    xsec = [ (xsecOld[1] - xsecOld[0]) / (massOld[1]-massOld[0])*(110 - massOld[0]) + xsecOld[0] ] + xsecOld

a_mass=array('f',mass)
a_xsec=array('f',xsec)
fewz.close()

spl = ROOT.RooSpline1D("spl","spl",x,len(mass), a_mass,a_xsec)
pdf = ROOT.RooGenericPdf("fewz","fewz","@0",ROOT.RooArgList(spl))
pdf_norm= ROOT.RooRealVar("fewz_norm","fewz_norm", nevents)
pdf_norm.setConstant()

getattr(w,'import')(pdf)
getattr(w,'import')(pdf_norm)

#dh = ROOT.RooDataHist( n+"_"+cat,n+"_"+cat,ROOT.RooArgList(x),h  )
#getattr(w,'import')(dh)
#g.extend([h,dh])

w.writeToFile(cat+"_fewz_2j.root")

print "DONE"

