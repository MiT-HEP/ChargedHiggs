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
#files=["/store/mc/RunIIFall17MiniAOD/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/50000/BAF0844D-E7D9-E711-8888-FA163E4F4F50.root"]
files=["/eos/cms/store/user//amarini/Sync/5AC9148F-9842-E811-892B-3417EBE535DA.root"]
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
        runs = Runs("root://eoscms//"+f.split()[0])
        for (ir,run) in enumerate(runs):
            run.getByLabel(lheRLabel,lheR)
            for it in lheR.product():
                print "T",it.replace('\n','') #, lheR.product()[it].tag()
                #for l in lheR.product()[it].lines(): print "L",l
            
        events = Events("root://eoscms//"+f.split()[0])
        lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
        #break
        for iev,event in enumerate(events):
            print "\nEvent %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
            if iev > 10: break
        
            try:
                event.getByLabel(lheLabel, lhe)
            except:
                continue
            hepeup = lhe.product().hepeup()
            ht=0.0
            print "\n\n---------"
            for i in range(0,lhe.product().weights().size() ):
                 print "W[",i,"]=",lhe.product().weights()[i].wgt,lhe.product().weights()[i].id
except KeyboardInterrupt:
    pass

fOut=ROOT.TFile.Open("ht.root","RECREATE")
h.Write()
h2.Write()
fOut.Close()
print "DONE"
