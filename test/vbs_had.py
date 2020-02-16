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

gdebug=0

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

indir='/store/group/phys_higgs/cmshmm/amarini/WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8/FullSim_94X-MINIAODSIM/191118_105222/0000'
cmd="eos find -f "+ indir
output=check_output(cmd,shell=True)
files = [ "root://eoscms/"+re.sub('/eos/cms','',x) for x in output.split('\n') if '/store/' in x ] 
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
   for ifile,f in enumerate(files):
    # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
    print "->Opening file",f.split()[0], ifile,"/",len(files)
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
        
        #event interpretation
        event={
                "idx":{
                    "q1":None,
                    "q2":None,
                    "j1":None,
                    "j2":None,
                    "j3":None,
                    "j4":None,
                },
                "p4":{
                    "q1":None,
                    "q2":None,
                    "j1":None,
                    "j2":None,
                    "j3":None,
                    "j4":None,
                    "w1":None,
                    "w2":None,
                    "qq":None
                    },
        }

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
                if m1>=0:m.SetPxPyPzE( hepeup.PUP[m1][0],hepeup.PUP[m1][1],hepeup.PUP[m1][2],hepeup.PUP[m1][3])
                if m1>=0 and hepeup.IDUP[m1]==24 and m2==m1:
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
                            if event["idx"]["j1"]==None: 
                                 event["idx"]["j1"]=i
                                 event["idx"]["j2"]=j
                                 event["p4"]["j1"]=x
                                 event["p4"]["j2"]=y
                                 event["p4"]["w1"]=m
                                 event["idx"]["w1"]=m1
                            elif event["idx"]["j3"]==None and event["idx"]["j1"]!=i and event["idx"]["j2"]!=i:
                                 event["idx"]["j3"]=i
                                 event["idx"]["j4"]=j
                                 event["p4"]["j3"]=x
                                 event["p4"]["j4"]=y
                                 event["p4"]["w2"]=m
                                 event["idx"]["w2"]=m1

                        if hepeup.ISTUP[j]==1 and abs(hepeup.IDUP[j])<6 and (hepeup.MOTHUP[j][0]-1) != m1: # can catch the other W
                            z=ROOT.TLorentzVector()
                            z.SetPxPyPzE( hepeup.PUP[j][0],hepeup.PUP[j][1],hepeup.PUP[j][2],hepeup.PUP[j][3]) 
                            if z.DeltaR(x)< minDR:
                                minDR=min(minDR,z.DeltaR(x))
                                noDoubleCounting = True if (i<j) else False

                    if noDoubleCounting:BookAndFill("DeltaRWProductAndOther",100,0,5,minDR,w)
                    if noDoubleCounting and m.Pt()> 800: BookAndFill("DeltaRWProductAndOther_ptW800",100,0,5,minDR,w)
                    if noDoubleCounting and m.Pt()> 200: BookAndFill("DeltaRWProductAndOther_ptW200",100,0,5,minDR,w)

                ## search for tagging jets
                if  m1<0 or (m1>= 0 and abs(hepeup.IDUP[m1])!=24):
                    #print "DEBUG:","Q jet is",i,x.Pt()
                    for j in range(i+1,hepeup.IDUP.size() ): 
                        y=ROOT.TLorentzVector()
                        y.SetPxPyPzE( hepeup.PUP[j][0],hepeup.PUP[j][1],hepeup.PUP[j][2],hepeup.PUP[j][3]) 
                        pdgid2=hepeup.IDUP[j]
                        status2=hepeup.ISTUP[j] 
                        mother2=hepeup.MOTHUP[j][0] -1
                        if status2==1 and abs(pdgid2)<6:
                            if mother2<0 or(mother2 >=0 and abs(hepeup.IDUP[mother2])!=24):
                                #print "DEBUG:","Q jet2 is",j,y.Pt()
                                ## x & y are the two tagging jets
                                qq=x+y
                                BookAndFill("MQQ",1000,0,5000,qq.M(),w)
                                BookAndFill("DeltaEtaQQ",1000,0,10,abs(x.Eta()-y.Eta()),w)
                                BookAndFill("DeltaPhiQQ",100,0,3.1416,abs(x.DeltaPhi(y)),w)
                                pt1=max(x.Pt(),y.Pt())
                                pt2=min(x.Pt(),y.Pt())
                                BookAndFill("QQ_pt1",100,0,100,pt1,w)
                                BookAndFill("QQ_pt2",100,0,100,pt2,w)
                                BoostedW=0
                                for k in range(0,hepeup.IDUP.size() ): 
                                    z.SetPxPyPzE( hepeup.PUP[k][0],hepeup.PUP[k][1],hepeup.PUP[k][2],hepeup.PUP[k][3]) 
                                    pdgid3=hepeup.IDUP[k]
                                    if abs(pdgid3)==24:
                                        if z.Pt() > 200: BoostedW+=1
                                extra = 'RR'
                                if BoostedW==1:extra='BR'
                                if BoostedW==2:extra='BB'
                                BookAndFill("MQQ_"+extra,1000,0,5000,qq.M(),w)
                                BookAndFill("DeltaEtaQQ_"+extra,1000,0,10,abs(x.Eta()-y.Eta()),w)
                                BookAndFill("DeltaPhiQQ_"+extra,100,0,3.1416,abs(x.DeltaPhi(y)),w)
                                BookAndFill("QQ_pt1_"+extra,100,0,100,pt1,w)
                                BookAndFill("QQ_pt2_"+extra,100,0,100,pt2,w)

                                if event["idx"]["q1"]==None:
                                    event["idx"]["q1"]=i
                                    event["idx"]["q2"]=j
                                    event["p4"]["q1"]=x
                                    event["p4"]["q2"]=y
                                    event["p4"]["qq"]=qq
                    
            #if hepeup.ISTUP[i] != 1: continue;
            #print " *)",i," pdgid=",hepeup.IDUP[i],"pt=",x.Pt(),"eta=",x.Eta(),"phi=",x.Phi(),"STATUS=",status, "mothers",hepeup.MOTHUP[i][0]-1,hepeup.MOTHUP[i][1]-1
            #std::vector< std::pair< int, int > >   MOTHUP

        ## Global event interpretation

        if event["idx"]["q1"]==None or \
           event["idx"]["q2"]==None or \
           event["idx"]["j1"]==None or \
           event["idx"]["j2"]==None or \
           event["idx"]["j3"]==None or \
           event["idx"]["j4"]==None:

            print "-> Failed to interpret event:",event
            for i in range(0,hepeup.IDUP.size() ):
                x=ROOT.TLorentzVector()
                x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) 
                pdgid=hepeup.IDUP[i]
                status=hepeup.ISTUP[i] # 1 -> FS for LHE
                m1=hepeup.MOTHUP[i][0] -1 ## starts from 1
                m2=hepeup.MOTHUP[i][1] -1
                print "  *",i,pdgid,status, x.Pt(),x.Eta(),x.Phi(),m1,m2
            continue
        elif gdebug<10:
            gdebug+=1
            print "---------DEBUG:--------"
            for a in ['q1','q2','j1','j2','j3','j4','w1','w2']:
                print event['p4'][a].Pt(),",",event['p4'][a].Eta(),",",event['p4'][a].Phi(),",",event['p4'][a].M()
            print "-----------------------"


        extra = 'RR'
        if event["p4"]["w1"].Pt() > 200 or event["p4"]["w2"].Pt()>200:
            extra='BR'
        if event["p4"]["w1"].Pt() > 200 and event["p4"]["w2"].Pt()>200:
            extra='BB'

        acc =          event["p4"]["j1"].Pt() >30 and abs(event["p4"]["j1"].Eta())< 4.7
        acc = acc and  event["p4"]["j2"].Pt() >30 and abs(event["p4"]["j2"].Eta())< 4.7
        acc = acc and  event["p4"]["j3"].Pt() >30 and abs(event["p4"]["j3"].Eta())< 4.7
        acc = acc and  event["p4"]["j4"].Pt() >30 and abs(event["p4"]["j4"].Eta())< 4.7
        acc = acc and  event["p4"]["q1"].Pt() >30 and abs(event["p4"]["q1"].Eta())< 4.7
        acc = acc and  event["p4"]["q2"].Pt() >30 and abs(event["p4"]["q2"].Eta())< 4.7

        acc2=True ## deltaR
        if extra=='RR':
            for a in ['j1','j2','j3','j4','q1','q2']:
                for b in ['j1','j2','j3','j4','q1','q2']:
                    if a==b: continue
                    acc2=acc2 and event["p4"][a].DeltaR(event["p4"][b]) > 0.4
        elif extra =='BB':
            acc2 = acc2 and event["p4"]["j1"].DeltaR(event["p4"]["j2"]) <0.8
            acc2 = acc2 and event["p4"]["j3"].DeltaR(event["p4"]["j4"]) <0.8
            for a in ['w2','w1','q1','q2']:
                for b in ['w1','w2','q1','q2']:
                    if a==b: continue
                    val=0.4
                    if 'w' in a or 'w' in b:val=0.8
                    acc2=acc2 and event["p4"][a].DeltaR(event["p4"][b]) > val
        elif event["p4"]["w1"].Pt()>200: ## BR
            acc2 = acc2 and event["p4"]["j1"].DeltaR(event["p4"]["j2"]) <0.8
            for a in ['w1','j3','j4','q1','q2']:
                for b in ['w1','j3','j4','q1','q2']:
                    if a==b: continue
                    val=0.4
                    if 'w' in a or 'w' in b:val=0.8
                    acc2=acc2 and event["p4"][a].DeltaR(event["p4"][b]) > val
        elif event["p4"]["w2"].Pt()>200: ## BR
            acc2 = acc2 and event["p4"]["j3"].DeltaR(event["p4"]["j4"]) <0.8
            for a in ['w2','j1','j2','q1','q2']:
                for b in ['w2','j1','j2','q1','q2']:
                    if a==b: continue
                    val=0.4
                    if 'w' in a or 'w' in b:val=0.8
                    acc2=acc2 and event["p4"][a].DeltaR(event["p4"][b]) > val

        if acc and acc2:
            BookAndFill("acc_MQQ",125,0,5000,event["p4"]["qq"].M(),w) 
            BookAndFill("acc_MQQ_"+extra,125,0,5000,event["p4"]["qq"].M(),w) 
            BookAndFill("acc_DeltaEtaQQ_"+extra,1000,0,10,abs(event["p4"]["q1"].Eta()-event["p4"]["q2"].Eta()),w)
            BookAndFill("acc_DeltaPhiQQ_"+extra,100,0,3.1416,abs(event["p4"]["q1"].DeltaPhi(event["p4"]["q2"])),w)
            BookAndFill2D("acc_PtWW",1000,0,1000,1000,0,1000,event["p4"]["w1"].Pt(),event["p4"]["w2"].Pt(),w)
            #print "DEBUG",event["p4"]["w1"].Pt(),event["p4"]["w2"].Pt()


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
