# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
#sys.argv = [ '-b-' ]
import ROOT
#ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events, Runs

#files=open("files.txt","read")
#files=["/store/mc/RunIISummer16MiniAODv2/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/0AF0207B-EFBE-E611-B4BE-0CC47A7FC858.root"]

#root://eoscms// TO ADD
#files=["/store/mc/RunIIFall17MiniAOD/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/50000/BAF0844D-E7D9-E711-8888-FA163E4F4F50.root"]
#files=["/eos/cms/store/user//amarini/Sync/5AC9148F-9842-E811-892B-3417EBE535DA.root"]
#files=['/tmp/amarini/E24C7443-5B83-E811-9AEB-FA163E0C18B7.root']
files=['root://eoscms///store/group/phys_higgs/cmshmm/amarini/EWK_LLJJ_MLL_105-160_ptJ-0_SM_5f_LO_TuneEEC5_13TeV-madgraph-h7/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-MINIAODSIM/200122_095931/0000/step4_460.root']
files=['file:///eos/user/a/azecchin/public/sherpaSIM/SMP-RunIISummer15GS-00289.root']
files=['root://xrootd-cms.infn.it///store/user/salbrech/privateMCProduction/aQGC_ZJJZJJjj_EWK_LO_NPle1_mjj100_pTj10_13TeV_madgraphMLM_pythia8/FullSim_94X-MINIAODSIM/200424_094238/0000/step4_79.root']
files=['root://xrootd-cms.infn.it///store/user/salbrech/privateMCProduction/aQGC_ZJJZJJjj_EWK_LO_NPle1_mjj100_pTj10_13TeV_madgraphMLM_pythia8/FullSim_94X-MINIAODSIM/200424_094238/0000/step4_874.root']
#files=['root://eoscms///store/group/phys_higgs/cmshmm/amarini/EWK_LLJJ_MLL_105-160_ptJ-0_SM_5f_LO_TuneEEC5_13TeV-madgraph-herwigpp/RunIISummer16MiniAODv3-94X_mcRun2_asymptotic_v3-MINIAODSIM/200124_090527/0002/step4_2991.root']
h = ROOT.TH1D("ht","ht",1300,0,13000)
h2 = ROOT.TH1D("htsec","ht",1300,0,13000)


#edm::Handle<LHERunInfoProduct> run; 
#typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
# 
#iRun.getByLabel( "externalLHEProducer", run );
#LHERunInfoProduct myLHERunInfoProduct = *(run.product());
# 
#for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
#  std::cout << iter->tag() << std::endl;
#  std::vector<std::string> lines = iter->lines();
#  for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
#   std::cout << lines.at(iLine);
#  }
#}


try:
    for f in files:
        # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
        print "->Opening file",f.split()[0]
        lheR,lheRLabel = Handle("LHERunInfoProduct"), "externalLHEProducer"
        #genR=genRLabel = Handle("GenRunInfoProduct"), "generator"
        runs = Runs(f.split()[0])
        for (ir,run) in enumerate(runs):
            run.getByLabel(lheRLabel,lheR)
            for it in lheR.product():
                print "T",it.replace('\n','') #, lheR.product()[it].tag()
                #for l in lheR.product()[it].lines(): print "L",l
            #run.getByLabel(genRLabel,genR)
            
        events = Events(f.split()[0])
        lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
        gen,genLabel = Handle("GenEventInfoProduct"),"generator"
        #break
        for iev,event in enumerate(events):
            print "\nEvent %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
            if iev > 2: break
        
            try:
                event.getByLabel(lheLabel, lhe)
            except:
                continue
            hepeup = lhe.product().hepeup()
            ht=0.0
            print "\n\n---------"
            for i in range(0,lhe.product().weights().size() ):
                 print "W[",i,"]=",lhe.product().weights()[i].wgt,lhe.product().weights()[i].id
            try:
                event.getByLabel(genLabel, gen)
            except:
                continue

            for i in range(0,gen.product().weights().size() ):
                 print "G[",i,"]=",gen.product().weights()[i]

            print "\n\n---------"
            print "Weights Summary = ",gen.product().weights().size(), lhe.product().weights().size()
            print "\n\n---------"
except KeyboardInterrupt:
    pass

fOut=ROOT.TFile.Open("ht.root","RECREATE")
h.Write()
h2.Write()
fOut.Close()
print "DONE"
