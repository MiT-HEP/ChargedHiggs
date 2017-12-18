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
from DataFormats.FWLite import Handle, Events

#files=open("files.txt","read")
files=["/store/mc/RunIISummer16MiniAODv2/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/0AF0207B-EFBE-E611-B4BE-0CC47A7FC858.root"]
h = ROOT.TH1D("ht","ht",1300,0,13000)
h2 = ROOT.TH1D("htsec","ht",1300,0,13000)

try:
   for f in files:
	# open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
	print "->Opening file",f.split()[0]
	#Bin=0,70 f=0.936856
	#Bin=70,100 f=0.0275586
	#Bin=100,200 f=0.0270849
	#Bin=200,400 f=0.00719491
	#Bin=400,600 f=0.000958862
	#Bin=600,800 f=0.000230162
	#Bin=800,1200 f=9.69229e-05
	#Bin=1200,2500 f=1.99723e-05
	#Bin=2500,13000 f=1.76188e-07
	if   'WJetsToLNu_HT-0To70'      in f: xsec=57641.658055
	elif 'WJetsToLNu_HT-70To100'    in f: xsec=1695.589715
	elif 'WJetsToLNu_HT-100To200'   in f: xsec=1666.444517
	elif 'WJetsToLNu_HT-200To400'   in f: xsec=442.679069
	elif 'WJetsToLNu_HT-400To600'   in f: xsec=58.995615
	elif 'WJetsToLNu_HT-600To800'   in f: xsec=14.161108
	elif 'WJetsToLNu_HT-800To1200'  in f: xsec=5.963346
	elif 'WJetsToLNu_HT-1200To2500' in f: xsec=1.22883
	elif 'WJetsToLNu_HT-2500ToInf'  in f: xsec=0.01084
	elif 'WJets' in f: xsec=61526.7

	events = Events("root://eoscms//"+f.split()[0])
	lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
	for iev,event in enumerate(events):
	        #if verbose:
	        #   print "\nEvent %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
	        #   #if iev > 10: break
		try:
	        	event.getByLabel(lheLabel, lhe)
		except:
			continue
		hepeup = lhe.product().hepeup()
		ht=0.0
		print "\n\n---------"
		for i in range(0,lhe.product().weights().size() ):
			print "W=",lhe.product().weights()[i].wgt,lhe.product().weights()[i].id
except KeyboardInterrupt:
	pass

fOut=ROOT.TFile.Open("ht.root","RECREATE")
h.Write()
h2.Write()
fOut.Close()
print "DONE"
