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

files=open("files.txt","read")
h = ROOT.TH1D("ht","ht",1300,0,13000)

try:
   for f in files:
	# open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
	print "->Opening file",f.split()[0]
	events = Events("root://eoscms//"+f.split()[0])
	lhe,lheLabel = Handle("LHEEventProduct"),"externalLHEProducer"
	for iev,event in enumerate(events):
	        #if verbose:
	        #   print "\nEvent %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
	        #   #if iev > 10: break
	        event.getByLabel(lheLabel, lhe)
		hepeup = lhe.product().hepeup()
		ht=0.0
		Q=ROOT.TLorentzVector()
		for i in range(0,hepeup.IDUP.size() ):
			if hepeup.ISTUP[i] != 1: continue;
			x=ROOT.TLorentzVector()
	        	x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) 
			if ((abs(hepeup.IDUP[i]) <=6)  or hepeup.IDUP[i] ==21): ht+= x.Pt();
			if ((abs(hepeup.IDUP[i]) >=11) and (abs(hepeup.IDUP[i]) <=16)): Q += x;
		h.Fill(ht,lhe.product().weights()[0].wgt)
except KeyboardInterrupt:
	pass
print "DONE"
h.SaveAs("ht.root")
