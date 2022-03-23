from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import ROOT
import math

# PreVFP 116  PostVFP 216
import os,sys
year=18
try:
    year=int(sys.argv[1])
    print ("year is now",year)
except: 
    print ("Unable to set year. Current value is",year)

n=100

fname='/eos/user/d/dalfonso/AnalysisVBS/NANO/FEB26/UL20%d/HADanti/HADanti.root'%year
if year == 216: fname="/eos/user/d/dalfonso/AnalysisVBS/NANO/FEB26/UL2016/HADanti/HADanti.root"
if year == 116: fname="/eos/user/d/dalfonso/AnalysisVBS/NANO/FEB26/UL2016APV/HADanti/HADanti.root"

hname='VBShadAnalysis/Baseline/SF_FatJet_'
oname='datacard_SF_20%d.txt'%year
## structure is Data, QCD, Others
labels=["JetHT","QCD_HT","TT_TuneCP5"]

data=0
qcd =1

lumi=35920. if year==16 else 41530. if year==17 else  59740. if year == 18 else 0.
if year == 116: lumi=19523.
if year == 216: lumi=16803.

print ("DEBUG","lumi",lumi)
#return 35920
#return 41530
#return 59740 
##
#index = ((selectedFatJets[nFat]->Pt() < 600) and (fabs(selectedFatJets[nFat]->Eta()) < 1.3) )? 0 : -1 ; // pt1Eta1
#index = ((selectedFatJets[nFat]->Pt() >= 600) and (fabs(selectedFatJets[nFat]->Eta()) < 1.3) )? 1 : index ; // pt1Eta2
#index = ((selectedFatJets[nFat]->Pt() < 600) and (fabs(selectedFatJets[nFat]->Eta()) >= 1.3) )? 2 : index ; // pt2Eta1
#index = ((selectedFatJets[nFat]->Pt() >= 600) and (fabs(selectedFatJets[nFat]->Eta()) >= 1.3) )? 3 : index ; // pt2Eta2
###
#index = 4*index1 + index2


fIn=ROOT.TFile.Open(fname)
if fIn==None: print("ERROR fname",fname,"does not exists")
hists=[ fIn.Get(hname+l) for l in labels]
for i,(h,l) in enumerate(zip(hists,labels)) :
    if h==None: 
        print("ERROR","label",l,"does not have an hist")
        hists[i]=ROOT.TH1D("tmp","tmp",n,0,n)
        for ii in xrange(0,n): hists[i].SetBinContent(ii+1,0.001/lumi)
out=open(oname,"w")

valid= [ hists[qcd].GetBinContent(i+1)*lumi>0.01  for i in xrange(0,n)] ## check that prediction is non-zero.

print ("DEBUG","valid",' '.join( [ "%d"%x for x in valid]))
print ("DEBUG","data",' '.join(["%d"%int(hists[data].GetBinContent(i+1)) for i in xrange(0,n)]))
print ("DEBUG","qcd",' '.join(["%d"%int(hists[qcd].GetBinContent(i+1)*lumi) for i in xrange(0,n)]))

print("### Automatically created to derive SF",file=out)
print("ibin *",file=out)
print("jbin *",file=out)
print("kbin *",file=out)
print("shapes * * FAKE",file=out)
print("--------------",file=out)
print("bin "+ ' '.join([ "bin%d"%i for i in xrange(0,n) if valid[i]]),file=out)
print("observation " + ' '.join( ["%d"%int(hists[data].GetBinContent(i+1)) for i in xrange(0,n) if valid[i]]), file=out)
print("--------------",file=out)
print("bin " + ' '.join([ "bin%d"%i for i in xrange(0,n) if valid[i] for p in labels[1:] ]), file=out)
print("process "+ ' '.join([ p for i in xrange(0,n) if valid[i] for p in labels[1:]]),file=out)
print("process "+ ' '.join([ "0" if p =='QCD_HT' else "1" for i in xrange(0,n) if valid[i] for p in labels[1:]]),file=out)
print("rate "+ ' '.join([ "%.3f"%(lumi*hists[labels.index(p)].GetBinContent(i+1)) for i in xrange(0,n) if valid[i] for p in labels[1:]]),file=out)
print("--------------",file=out)

## now the magic begins
#name rateParam bin process initial_value [min,max]
n0=int(math.sqrt(n))
if n0*n0 != n: print("ERROR",n,"is not a square")
for j in xrange(0,n0):
    for i in xrange(0,n0):
        if valid[n0*j+i]: 
            print("w%d rateParam bin%d"%(i,n0*j+i),labels[qcd], "1.0",file=out)
            print("w%d rateParam bin%d"%(j,n0*j+i),labels[qcd],"1.0",file=out)
print("--------------",file=out)
print("### Run with",file=out)
print("### combine -M MultiDimFit --redefineSignalPOIs="+','.join(["w%d"%i for i in xrange(0,n0)])+" --setParameters r=1 --freezeParameters=r -d "+oname,file=out)
print("### or with", file=out)
print("### combine -M FitDiagnostics --saveShapes --saveWithUncertainties --skipBOnlyFit --redefineSignalPOIs="+','.join(["w%d"%i for i in xrange(0,n0)])+" --setParameters r=1 --freezeParameters=r -d "+oname, file=out)
print("",file=out)
print ("### interpret with",file=out)
print ("### w0 = pt1Eta1",file=out)
print ("### w1 = pt2Eta1",file=out)
print ("### w2 = pt1Eta2",file=out)
print ("### w3 = pt2Eta2",file=out)






