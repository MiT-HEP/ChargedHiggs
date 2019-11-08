# import ROOT in batch mode
import sys
import math
oldargv = sys.argv[:]
#sys.argv = [ '-b-' ]
import ROOT
#ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

from subprocess import check_output
import re

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

#indir='/store/group/phys_higgs/cmshmm/amarini/WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8/FullSim_94X-MINIAODSIM/191029_214616/0000/'
#cmd="eos find -f "+ indir
#output=check_output(cmd,shell=True)
#files = [ "root://eoscms/"+re.sub('/eos/cms','',x) for x in output.split('\n') if '/store/' in x ] 
#xsec=9.059e-02 
#outname="vbs_hadronic.root"
#
#from glob import glob
#indir='/afs/cern.ch/user/a/amarini//work/GenProduction/Fall2017/MCProduction/Nov2019/VBS/WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_HT1000/2016/out'
#flist=glob(indir+"/step1_100*.root")
#files=[ "file:"+x for x in flist]
#xsec=1.370e-03 
#outname="vbs_hadronic_ht1000.root"

from glob import glob
indir='/afs/cern.ch/user/a/amarini//work/GenProduction/Fall2017/MCProduction/Nov2019/VBS/WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10/2016/out'
flist=glob(indir+"/step1_100*.root")
files=[ "file:"+x for x in flist]
xsec=9.059e-02
outname="vbs_hadronic.root"

#onlyEvent=47651033
onlyEvent=None

fOut=ROOT.TFile.Open(outname,"RECREATE")
fOut.cd()
histos={}

def BookAndFill(name,nbins,xmin,xmax,value,weight=1.):
    global histos
    global fOut
    if name not in histos:
        print "creating histos with name",name
        fOut.cd()
        histos[name]=ROOT.TH1D(name,name,nbins,xmin,xmax)
    if histos[name]==None:
        print "ERROR","histo with name",name,"is Null Poitr"
    #print "filling histos",name,"with value",value,weight
    histos[name].Fill(value,weight)
    return

def BookAndFill2D(name,nbins,xmin,xmax,mbins,ymin,ymax,value,value2,weight=1.):
    global histos
    global fOut
    if name not in histos:
        print "creating histos with name",name
        fOut.cd()
        #histos[name]=ROOT.TH1D(name,name,nbins,xmin,xmax)
        histos[name]=ROOT.TH2D(name,name,nbins,xmin,xmax,mbins,ymin,ymax)
    if histos[name]==None:
        print "ERROR","histo with name",name,"is Null Poitr"
    #print "filling histos",name,"with value",value,weight
    histos[name].Fill(value,value2,weight)
    return

## counters events
allw=0.
alle=0
try:
   for f in files:
    # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
    print "->Opening file",f.split()[0]
    events = Events(f.split()[0])
    lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
    #vector<reco::GenParticle>             "prunedGenParticles"  
    handlePruned  = Handle ("std::vector<reco::GenParticle>")
    labelPruned = ("prunedGenParticles")
    for iev,event in enumerate(events):

        if onlyEvent != None and event.eventAuxiliary().event() != onlyEvent: continue
        
        if event.eventAuxiliary().event() % 100 ==1:
            print "\n-> Event %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
        event.getByLabel(lheLabel, lhe)
        hepeup = lhe.product().hepeup()
        w=lhe.product().weights()[0].wgt
        allw+=w
        alle+=1
        for i in range(0,hepeup.IDUP.size() ):
            x=ROOT.TLorentzVector()
            x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) 
            pdgid=hepeup.IDUP[i]
            status=hepeup.ISTUP[i] # 1 -> FS for LHE
            
            if abs(pdgid)==24:
                BookAndFill("WpEta",100,-5,5,x.Eta(),w)
                BookAndFill("WpPt",500,0,5000,x.Pt(),w)
                for j in range(i+1,hepeup.IDUP.size() ): ## from i+1 --> only one comb
                    if abs(hepeup.IDUP[j]) != 24 : continue
                    y=ROOT.TLorentzVector()
                    y.SetPxPyPzE( hepeup.PUP[j][0],hepeup.PUP[j][1],hepeup.PUP[j][2],hepeup.PUP[j][3]) 
                    BookAndFill("DeltaRWW",100,0,5,x.DeltaR(y),w)
                    BookAndFill("DeltaEtaWW",100,0,5,abs(x.Eta() - y.Eta()),w)
                    BookAndFill("DeltaPhiWW",100,0,5,abs(x.DeltaPhi(y)),w)
                    BookAndFill2D("PtWW",1000,0,1000,1000,0,1000,x.Pt(),y.Pt(),w)
                    BookAndFill2D("YWW",100,-5,5,100,-5,5,x.Rapidity(),y.Rapidity(),w)
                    BookAndFill2D("EtaWW",100,-5,5,100,-5,5,x.Eta(),y.Eta(),w)

                    if x.Pt()>800 or y.Pt()>800: 
                        BookAndFill("DeltaRWW_ptW800",100,0,5,x.DeltaR(y),w)
                        BookAndFill("DeltaEtaWW_ptW800",100,0,5,abs(x.Eta() - y.Eta()),w)
                        BookAndFill("DeltaPhiWW_ptW800",100,0,5,abs(x.DeltaPhi(y)),w)

            if status==1 and abs(pdgid)<6:
                BookAndFill("JetPt",100,0,5000,x.Pt(),w)
                m1=hepeup.MOTHUP[i][0] -1 ## starts from 1
                m2=hepeup.MOTHUP[i][1] -1
                m=ROOT.TLorentzVector()
                if m1>0:m.SetPxPyPzE( hepeup.PUP[m1][0],hepeup.PUP[m1][1],hepeup.PUP[m1][2],hepeup.PUP[m1][3])
                if m1>0 and hepeup.IDUP[m1]==24 and m2==m1:
                    # from a W decay
                    minDR=5
                    noDoubleCounting=True
                    #for j in range(i+1,hepeup.IDUP.size() ):
                    for j in range(0,hepeup.IDUP.size() ): ## run on all to keep track of doubleCounting
                        if i==j: continue
                        if (hepeup.MOTHUP[j][0]-1)==m1 and (hepeup.MOTHUP[j][1]-1)==m1 and hepeup.ISTUP[j]==1 and abs(hepeup.IDUP[j])<6:
                            # matched
                            y=ROOT.TLorentzVector()
                            y.SetPxPyPzE( hepeup.PUP[j][0],hepeup.PUP[j][1],hepeup.PUP[j][2],hepeup.PUP[j][3]) 
                            if i<j: BookAndFill("DeltaRFromWDecay",100,0,5,x.DeltaR(y),w)
                            if i<j and m.Pt()>800: BookAndFill("DeltaRFromWDecay_ptW800",100,0,5,x.DeltaR(y),w)
                            if i<j and m.Pt()>200: BookAndFill("DeltaRFromWDecay_ptW200",100,0,5,x.DeltaR(y),w)
                            #print "found a matching from W",j,i

                        if hepeup.ISTUP[j]==1 and abs(hepeup.IDUP[j])<6 and (hepeup.MOTHUP[j][0]-1) != m1: # can catch the other W
                            z=ROOT.TLorentzVector()
                            z.SetPxPyPzE( hepeup.PUP[j][0],hepeup.PUP[j][1],hepeup.PUP[j][2],hepeup.PUP[j][3]) 
                            if z.DeltaR(x)< minDR:
                                minDR=min(minDR,z.DeltaR(x))
                                noDoubleCounting = True if (i<j) else False

                    if noDoubleCounting:BookAndFill("DeltaRWProductAndOther",100,0,5,minDR,w)
                    if noDoubleCounting and m.Pt()> 800: BookAndFill("DeltaRWProductAndOther_ptW800",100,0,5,minDR,w)
                    if noDoubleCounting and m.Pt()> 200: BookAndFill("DeltaRWProductAndOther_ptW200",100,0,5,minDR,w)
                    
            #if hepeup.ISTUP[i] != 1: continue;
            #print " *)",i," pdgid=",hepeup.IDUP[i],"pt=",x.Pt(),"eta=",x.Eta(),"phi=",x.Phi(),"STATUS=",status, "mothers",hepeup.MOTHUP[i][0]-1,hepeup.MOTHUP[i][1]-1
            #std::vector< std::pair< int, int > >   MOTHUP


        ### GEN PARTICLES
        #print " ------------ "
        #event.getByLabel (labelPruned, handlePruned)
        #pruned = handlePruned.product()
        #for p in pruned:
        #    mother=p.mother(0)
        #    mpdg=0
        #    if mother: mpdg=mother.pdgId()
        #    print " *) PdgId : %s   pt : %s  eta : %s   phi : %s mother : %s" %(p.pdgId(),p.pt(),p.eta(),p.phi(),mpdg) 

except KeyboardInterrupt:
    pass

fOut.cd()
for hstr in histos:
    print "writing ",hstr,"scaled to xsec",xsec,histos[hstr].Integral(),
    histos[hstr].Scale(xsec/allw)
    print "->",histos[hstr].Integral()
    histos[hstr].Write()
fOut.Close()

print "DONE"
