from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import ROOT
import math

year=16
#fname='/eos/user/d/dalfonso/AnalysisVBS/NANO/SEPT6/UL%d/HADanti/HADanti.root'%year
#n=16
fname='/eos/user/d/dalfonso/AnalysisVBS/NANO/SEPT10fix/UL%d/HADanti/HADanti.root'%year
n=36
hname='VBShadAnalysis/Baseline/SF_FatJet_'
oname='datacard_SF_20%d.txt'%year
## structure is Data, QCD, Others
labels=["JetHT","QCD_HT","TT_TuneCP5"]

data=0
qcd =1
lumi=35920. if year==16 else 41530 if year==17 else 59740

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






