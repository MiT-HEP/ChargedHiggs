# import ROOT in batch mode
import sys
import math
oldargv = sys.argv[:]
#sys.argv = [ '-b-' ]
import ROOT
ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

from subprocess import check_output
import re
import math

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

## using pyjet
from pyjet import cluster, DTYPE_EP, DTYPE_PTEPM
from pyjet import ClusterSequence, ClusterSequenceArea, JetDefinition, PseudoJet, USING_EXTERNAL_FASTJET
import numpy as np

ROOT.gSystem.Load("ECON-script/analyze/bin/libFastjetInterface.so")
fi = ROOT.fastjet_interface();
#ROOT.gSystem.Load("/cvmfs/cms.cern.ch/slc6_amd64_gcc700//external/fastjet/3.3.0-omkpbe/lib/libfastjet.so")


gdebug=0

def deltaPhi(phi1, phi2):
    x = phi1 - phi2
    while (x >= math.pi): x -= 2*math.pi
    while (x < -math.pi): x += 2*math.pi
    return abs(x)

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

indir='/store/group/phys_higgs/cmshmm/amarini/WWjj_SS_ll_hadronic/FullSim_94X-2016-MINIAODSIM'
cmd="eos find -f "+ indir
output=check_output(cmd,shell=True)
filesLL = [ "root://eoscms/"+re.sub('/eos/cms','',x) for x in output.split('\n') if '/store/' in x ] 

indir='/store/group/phys_higgs/cmshmm/amarini/WWjj_SS_tt_hadronic/FullSim_94X-2016-MINIAODSIM'
cmd="eos find -f "+ indir
output=check_output(cmd,shell=True)
filesTT = [ "root://eoscms/"+re.sub('/eos/cms','',x) for x in output.split('\n') if '/store/' in x ] 

indir='/store/group/phys_higgs/cmshmm/amarini/WWjj_SS_lt_hadronic/FullSim_94X-2016-MINIAODSIM'
cmd="eos find -f "+ indir
output=check_output(cmd,shell=True)
filesLT = [ "root://eoscms/"+re.sub('/eos/cms','',x) for x in output.split('\n') if '/store/' in x ] 

files=filesLL+filesTT + filesLT

outname="pol_studies.root"

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
allw_ll=0.
allw_tt=0.

try:
   for ifile,f in enumerate(files):
    # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
    print "->Opening file",f.split()[0], ifile,"/",len(files)
    events = Events(f.split()[0])
    lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
    handlePruned  = Handle ("std::vector<reco::GenParticle>")
    labelPruned = ("prunedGenParticles")

    handleJet  = Handle ("std::vector<reco::GenJet>")
    labelJet = ("slimmedGenJetsAK8")

    handleJetReco  = Handle ("std::vector<pat::Jet>")
    labelJetReco = ("slimmedJetsAK8")

    if f in filesLL: 
        if f in filesTT: raise ValueError("both LL and TT")
        if f in filesLT: raise ValueError("both LL and LT")
        pol="LL"
    elif f in filesTT:
        if f in filesLT: raise ValueError("both LT and TT")
        pol="TT"
    elif f in filesLT:
        pol="LT"
    else:
        raise ValueError("neither LL, LT, nor TT")

    for iev,event in enumerate(events):

        if onlyEvent != None and event.eventAuxiliary().event() != onlyEvent: continue
        
        if event.eventAuxiliary().event() % 1000 ==1:
            print "-> Event %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
        event.getByLabel(lheLabel, lhe)
        hepeup = lhe.product().hepeup()
        w=lhe.product().weights()[0].wgt

        #allw+=w
        #alle+=1

        Ws=[] 
        for i in range(0,hepeup.IDUP.size() ):
            x=ROOT.TLorentzVector()
            x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) 
            pdgid=hepeup.IDUP[i]
            status=hepeup.ISTUP[i] # 1 -> FS for LHE

            
            if abs(pdgid)==24:
                BookAndFill("WpEta",100,-5,5,x.Eta(),w)
                BookAndFill("WpPt",500,0,5000,x.Pt(),w)
                Ws.append(x)
        if len(Ws) < 2: print "Less than 2 W"
        
        event.getByLabel(labelJet,handleJet)
        event.getByLabel(labelJetReco,handleJetReco)
        
        jetsGen=handleJet.product()
        jetsReco = handleJetReco.product()

        for label,jets in zip(['gen','reco'],[jetsGen,jetsReco]):
            for j in jets:
                ## deltaR matching
                isW=False
                W=None
                jp4=ROOT.TLorentzVector()
                jp4.SetPtEtaPhiM(j.pt(),j.eta(),j.phi(),j.mass())
                for myW in Ws: 
                    if jp4.DeltaR(myW)< 0.4: 
                        isW=True  ## matching should be pretty efficient
                        W=myW
                if not W: continue

                if W.Pt()<200: continue

                BookAndFill("jetpt_"+label+"_"+pol,100,0,1000,j.pt(),w)
                BookAndFill("jeteta_"+label+"_"+pol,100,-5,5,j.eta(),w)

                ## constituents variables --> do it with pf candidates
                vars={
                    'sum_weight' : 0.,
                    'sum_deta' : 0., 
                    'sum_dphi' : 0., 
                    'sum_deta2' : 0., 
                    'sum_dphi2' : 0., 
                    'sum_detadphi' : 0., 
                    'sum_pt' : 0.,
                    'skew1': 0.,
                    'skew2':0.,
                }
                vars['jeta'] = j.eta()
                vars['jphi'] = j.phi()
                for p in j.getJetConstituentsQuick():
                    vars['pt']=p.pt()
                    vars['eta']=p.eta()
                    vars['phi']=p.phi()
                    vars['deta'] = vars['eta'] - vars['jeta'];
                    vars['dphi'] = deltaPhi(vars['phi'], vars['jphi']);
                    vars['weight'] = vars['pt']**2;
                    vars['sum_weight'] += vars['weight'];
                    vars['sum_pt'] += vars['pt'];
                    vars['sum_deta'] += vars['deta']*vars['weight'];
                    vars['sum_dphi'] += vars['dphi']*vars['weight'];
                    vars['sum_deta2'] += vars['deta']*vars['deta']*vars['weight'];
                    vars['sum_detadphi'] += vars['deta']*vars['dphi']*vars['weight'];
                    vars['sum_dphi2'] += vars['dphi']*vars['dphi']*vars['weight'];

                #Calculate axis2 and ptD
                a = 0.
                b = 0.
                c = 0.
                ave_deta = 0. 
                ave_dphi = 0. 
                ave_deta2 = 0.
                ave_dphi2 = 0.
                if(vars['sum_weight'] > 0):
                    ave_deta = vars['sum_deta']/vars['sum_weight'];
                    ave_dphi = vars['sum_dphi']/vars['sum_weight'];
                    ave_deta2 = vars['sum_deta2']/vars['sum_weight'];
                    ave_dphi2 = vars['sum_dphi2']/vars['sum_weight'];
                    a = ave_deta2 - ave_deta*ave_deta;
                    b = ave_dphi2 - ave_dphi*ave_dphi;
                    c = -(vars['sum_detadphi']/vars['sum_weight'] - ave_deta*ave_dphi);
                delta = math.sqrt(abs((a-b)*(a-b)+4*c*c));
                vars['axis2'] = math.sqrt(0.5*(a+b-delta)) if a+b-delta>0 else 0
                vars['axis1'] = math.sqrt(0.5*(a+b+delta)) if a+b-delta>0 else 0
                vars['ptD']   = math.sqrt(vars['sum_weight'])/vars['sum_pt'] if vars['sum_weight']>0 else 0


                BookAndFill("axis1_"+label+"_"+pol,100,0,1,vars['axis1'],w)
                if vars['axis2']>2.e-9:
                    BookAndFill("axis2_"+label+"_"+pol,100,0,20,-math.log(vars['axis2']),w)
                else:
                    BookAndFill("axis2_"+label+"_"+pol,100,0,20,19.99,w)
                BookAndFill("ptD_"+label+"_"+pol,100,0,1,vars['axis2'],w)

                ## eigen vectors on the major axis
                if(vars['sum_weight'] > 0):
                    #- (-a+b-delta)/2*c, 1  # major
                    #- (-a+b+delta)/2*c, 1
                    norm1=math.sqrt( ( - (-a+b-delta)/2*c )**2 + 1)
                    norm2=math.sqrt( (- (-a+b+delta)/2*c)**2 + 1 )

                    axis1V=np.array([- (-a+b-delta)/2*c, 1])/norm1
                    axis2V=np.array([- (-a+b+delta)/2*c, 1])/norm2
                    #ave_deta = vars['sum_deta']/vars['sum_weight'];
                    #ave_dphi = vars['sum_dphi']/vars['sum_weight'];
                    ave=np.array([ave_deta,ave_dphi])
                    for p in j.getJetConstituentsQuick():
                        vars['pt']=p.pt()
                        vars['eta']=p.eta()
                        vars['deta'] = vars['eta'] - vars['jeta'];
                        vars['phi']=p.phi()
                        vars['dphi'] = deltaPhi(vars['phi'], vars['jphi']);
                        x=np.array([vars['deta'],vars['dphi']])
                        
                        #(x-mu)**3/sigma -> skewness
                        vars['weight'] = vars['pt']**2;
                        try:
                            vars["skew1"] += vars['weight']*( np.dot(x,axis1V) -np.dot(ave,axis1V))**3/vars["axis1"]**3 #/ vars['sum_weight']
                        except ZeroDivisionError:
                            pass
                        try:
                            vars["skew2"] += vars['weight']*( np.dot(x,axis2V) -np.dot(ave,axis2V))**3/vars["axis2"]**3 #/ vars['sum_weight']
                        except ZeroDivisionError:
                            pass
                    vars["skew1"] = abs(vars["skew1"]/vars['sum_weight'])
                    vars["skew2"] = abs(vars["skew2"]/vars['sum_weight'])
                
                #print "skew1/2",vars["skew1"],vars["skew2"]
                if vars['skew1']> 2e-9:
                    BookAndFill("skew1_"+label+"_"+pol,1000,-10,20,-math.log(vars['skew1']),w)
                else:
                    BookAndFill("skew1_"+label+"_"+pol,1000,-10,20,19.99,w)

                if vars['skew2']>2e-9:
                    BookAndFill("skew2_"+label+"_"+pol,1000,-10,20,-math.log(vars['skew2']),w)
                else:
                    BookAndFill("skew2_"+label+"_"+pol,1000,-10,20,19.99,w)

                #https://github.com/amarini/ECON-script/blob/master/analyze/substructure.py#L97
                #input_particles=[]
                #input_particles.append( (p.Pt(),p.Eta(),p.Phi(),p.M())  )
                #ip = np.array( input_particles, dtype=DTYPE_PTEPM)
                #sequence=cluster( ip, algo="antikt",ep=False,R=self.clusterR)
                input_particles=ROOT.std.vector(PseudoJet)()
                for p in j.getJetConstituentsQuick():
                    input_particles.push_back(ROOT.fastjet.PseudoJet(p.px(),p.py(),p.pz(),p.energy()))

                if input_particles.size() == 0: continue

                tau3=fi.getTaus(input_particles,3)
                tau2=fi.getTaus(input_particles,2)
                tau1=fi.getTaus(input_particles,1)
                BookAndFill("tau1_"+label+"_"+pol,100,0,1,tau1,w)
                BookAndFill("tau2_"+label+"_"+pol,100,0,1,tau2,w)
                BookAndFill("tau3_"+label+"_"+pol,100,0,1,tau3,w)

                if tau1>0 and tau2>0:
                    BookAndFill("tau1prod2_"+label+"_"+pol,500,0,20,-math.log(tau2)-math.log(tau1),w)
                try:
                    BookAndFill("tau21_"+label+"_"+pol,100,0,1,tau2/tau1,w)
                except ZeroDivisionError:
                    pass
                try:
                    BookAndFill("tau32_"+label+"_"+pol,100,0,1,tau3/tau2,w)
                except ZeroDivisionError:
                    pass

                input_particles=[]
                for p in j.getJetConstituentsQuick():
                    input_particles.append( (p.pt(),p.eta(),p.phi(),p.mass())  )
                ip = np.array( input_particles, dtype=DTYPE_PTEPM)
                sequence=cluster( ip, algo="kt",ep=False,R=0.8)
                ## similarly to tau2/tau1
                try: # one particle
                    jets1=sequence.exclusive_jets(1)
                    jets2=sequence.exclusive_jets(2)
                except ValueError:
                    continue
                #jets3=sequence.exclusive_jets(3)

                try:
                    BookAndFill("pt21_"+label+"_"+pol,100,0,2,jets2[1].pt/jets2[0].pt,w)
                    BookAndFill2D("axis1_vs_pt21_"+label+"_"+pol,100,0,1,100,0,2,vars['axis1'],jets2[1].pt/jets2[0].pt,w)
                except ZeroDivisionError:
                    pass

                DR=0.8
                tau0_charged=0.
                tau1_charged=0.
                tau2_charged=0.
                tau2p_charged=0.

                for p in j.getJetConstituentsQuick():
                    dr=0.8
                    dr2=0.8
                    q=p.charge()
                    tau0_charged+=p.pt()*DR*q
                    dr1=math.sqrt(deltaPhi(p.phi(),jets1[0].phi)**2 + (jets1[0].eta-p.eta())**2)*q
                    tau1_charged+=p.pt()*dr1*q
                    for idx,j in enumerate(jets2):
                        dr=min(dr,math.sqrt(deltaPhi(p.phi(),j.phi)**2 + (j.eta-p.eta())**2) )
                        if abs(dr2)<math.sqrt(deltaPhi(p.phi(),j.phi)**2 + (j.eta-p.eta())**2):
                            dr2=math.sqrt(deltaPhi(p.phi(),j.phi)**2 + (j.eta-p.eta())**2) *q *(-1)**idx
                    tau2_charged+=p.pt()*dr*q
                    tau2p_charged+=p.pt()*dr2
                
                if tau0_charged>0:
                    BookAndFill("tau1_charged_"+label+"_"+pol,200,-2,2,tau1_charged/tau0_charged,w)
                    BookAndFill("tau2_charged_"+label+"_"+pol,200,-2,2,tau2_charged/tau0_charged,w)
                    BookAndFill("tau2p_charged_"+label+"_"+pol,2000,-10,20,tau2p_charged/tau0_charged,w)

                try:
                    BookAndFill("tau21_charged_"+label+"_"+pol,200,-1,1,tau2_charged/tau1_charged,w)
                except ZeroDivisionError:
                    pass

                try:
                    BookAndFill("tau21p_charged_"+label+"_"+pol,2000,-10,10,tau2p_charged/tau1_charged,w)
                except ZeroDivisionError:
                    pass
                #print "DEBUG", "end jets",label,"cycle"
            #print "DEBUG", "end jets cycles"
            # end jets cycle        


except KeyboardInterrupt:
    pass

fOut.cd()
for hstr in histos:
    #print "writing ",hstr,"scaled to xsec",xsec,histos[hstr].Integral(),
    #histos[hstr].Scale(xsec/allw)
    print "->",histos[hstr].Integral()
    histos[hstr].Scale(1.0/histos[hstr].Integral())
    histos[hstr].Write()

for hstr in histos:
    if not re.search(".*_LL",hstr): continue
    name = re.sub('_LL','',hstr)
    c=ROOT.TCanvas("c_"+name,"c",800,800)

    histos[hstr].Draw("AXIS")
    histos[hstr].Draw("AXIS X+ Y+ SAME")
    leg=ROOT.TLegend(.6,.6,.9,.9)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    up=0.
    for pol in ['LL','LT','TT']:
        if pol=='LL': histos[name + "_" + pol].SetLineColor(38)
        if pol=='TT': histos[name + "_" + pol].SetLineColor(46)
        if pol=='LT': histos[name + "_" + pol].SetLineColor(8)
        histos[name + "_" + pol].SetLineWidth(2)
        histos[name + "_" + pol].Draw("HIST SAME")
        up = max(up, histos[name + "_" + pol].GetMaximum())
        leg.AddEntry(histos[name + "_" + pol],pol,"L")
    histos[hstr].SetMaximum(up*1.2)
    leg.Draw()
    c.Update()
    c.Write()

fOut.Close()

print "DONE"
