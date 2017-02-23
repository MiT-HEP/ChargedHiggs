
## DO CMSENV FOR python2.7
cd work/Nero/CMSSW_8_0_26_patch1/src
cmsenv
export PATH=$HOME/.local/bin:/afs/cern.ch/cms/lumi/brilconda-1.1.7/bin:$PATH
PYTHONPATH=$HOME/.local/lib/python2.7/site-packages:$PYTHONPATH
python `which brilcalc` lumi --help
python `which brilcalc` lumi -i Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt -u '/pb' --normtag /afs/cern.ch/user/l/lumipro/public/normtag_file/normtag_DATACERT.json
python `which brilcalc` lumi -i Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt -u '/pb' --normtag /afs/cern.ch/user/l/lumipro/public/normtag_file/normtag_DATACERT.json --hltpath=HLT_LooseIsoPFTau50_Trk30_eta2p1_v*



### PILEUP ###
JSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco//Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
PUJSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt"
python `which brilcalc` lumi -i ${JSON} --normtag /afs/cern.ch/user/l/lumipro/public/normtag_file/normtag_DATACERT.json --hltpath=HLT_LooseIsoPFTau50_Trk30_eta2p1_v* --byls --output-style=csv -o HLT_LooseIsoPFTau50.csv
pileupReCalc_HLTpaths.py -i HLT_LooseIsoPFTau50.csv --inputLumiJSON ${PUJSON} -o My_HLT_corrected_PileupJSON.txt --runperiod Run2



########### NON BRIL #############
OUT="Pileup.txt"
JSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco//Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
PUJSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt"

#  KEY: TH1D     PU-pileup;1     pileup
#  KEY: TH1D     PU-pileupUp;1   pileup
#  KEY: TH1D     PU-pileupDown;1 pileup

echo "-> Doing nominal pileup with JSON=$JSON"
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  \
	--calcMode true \
	--minBiasXsec 69200 \
	--maxPileupBin 100 --numPileupBins 100  \
	--pileupHistName=PU-pileup \
	pileupNominal.root

echo "-> Doing nominal pileup Up"
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  \
	--calcMode true \
	--minBiasXsec 72660 \
	--maxPileupBin 100 --numPileupBins 100  \
	--pileupHistName=PU-pileupUp \
	pileupUp.root

echo "-> Doing nominal pileup Down"
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  \
	--calcMode true \
	--minBiasXsec 65740 \
	--maxPileupBin 100 --numPileupBins 100  \
	--pileupHistName=PU-pileupDown \
	pileupDown.root
echo "-> Merging"
hadd pileup.root pileupNominal.root pileupUp.root pileupDown.root
echo "-> DONE"
