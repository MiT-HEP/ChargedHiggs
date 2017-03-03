import sys,os
import math
import re
from optparse import OptionParser
from subprocess import call

parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="ChHiggs.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="ChHiggs_new.root")

opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

fIn=ROOT.TFile.Open(opts.input)
fOut=ROOT.TFile.Open(opts.output + ".tmp","RECREATE")
#n= ROOT.TIter(fIn.GetListOfKeys())
# * Writing HmumuAnalysis/Vars/MuonIso_Other
#  * Writing HmumuAnalysis/Vars/MuonIso_VBF_HToMuMu_M125
#   * Writing HmumuAnalysis/Vars/MuonIso_WMinusH_HToMuMu_M125
#    * Writing HmumuAnalysis/Vars/MuonIso_WPlusH_HToMuMu_M125
#     * Writing HmumuAnalysis/Vars/MuonIso_ZH_HToMuMu_M125
#      * Writing HmumuAnalysis/Vars/MuonIso_ttHToNonbb_M125
#       * Writing HmumuAnalysis/Vars/MuonIso_ttH_HToMuMu_M125

gb=[]
def ParseDir( dIn,fOut,d=""):
   for key in dIn.GetListOfKeys():
	#print "-> Considering",key.ReadObj().GetName()
	name = key.ReadObj().GetName()
	if key.ReadObj().InheritsFrom("TDirectory"):
		print "-> Entering Directory",key.ReadObj().GetName()
		fOut.mkdir(d + key.ReadObj().GetName() +"/" )
		ParseDir(key.ReadObj(),fOut,d + key.ReadObj().GetName() +"/")
	if 'Data' in name or \
		'Other' in name or \
		'MuonIso' in name or \
		'M130' in name or 'M120' in name or\
		'VBF1' in name or 'VBF0' in name or 'GF' in name or 'OneB' in name or \
		'_BE_' in name or '_BO_' in name or '_OO_' in name or '_OE_' in name or '_EE_' in name or\
		'VBF' in name or 'ttH' in name or 'ZH' in name or 'WMinusH' in name or 'WPlusH' in name : 
			continue
	if key.ReadObj().InheritsFrom("TH1") :
		print " * Writing",name
		fOut.cd(d)
		#h=fIn.Get(key.ReadObj().GetName()).Clone()
		h=key.ReadObj().Clone()
		h.Write(re.sub(".*/","",name),ROOT.TObject.kOverwrite)
		gb.append(h)
		fOut.Write()

ParseDir(fIn,fOut)
fOut.Close()

## recompress-> x10 in space
call("hadd -f "+ opts.output + " " + opts.output +".tmp",shell=True)
call("rm "+ opts.output +".tmp",shell=True)

##TIter next(inFile->GetListOfKeys());
##TKey *key;
##while ((key = (TKey*)next())){
##  if (string(key->ReadObj()->ClassName())=="TMacro") {
##    //cout << key->ReadObj()->ClassName() << " : " << key->GetName() << endl;
##    TMacro *macro = (TMacro*)inFile->Get(key->GetName());
##    outFile->cd();
##    macro->Write();
##  }
##}
