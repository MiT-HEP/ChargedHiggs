#!env python
import sys, os
import re
from array import array
import math
from optparse import OptionParser,OptionGroup

sys.path.insert(0, 'script')
import FwBinning as FwRebin
import SystematicSmoother as SS
maxStat=0.15

# original binning is 10GeV
nRebinHT=1
#nRebinHT=5
LikeBins=10
#LikeBins=1

doRebin = False
applyLikelihoodRebinBin = True

doSyst = True
likelihoodBinning = FwRebin.RebinLikelihood(LikeBins)

doSChannel = False

parser= OptionParser()

##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/June12.root")
##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/hysteriaMeow.root")
##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ohho.root")
##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ssolstice.root")

##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ldoJune.root")
##parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/bebetter.root")

####
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/insaneM.root")
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/SoHot.root")
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/midAug.root")
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/lateAug.root")
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/updlimit.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/updlimit.root")

#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/crazylimit0306.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/crazylimit0306.root")

#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/pipilimitnew.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/pipilimitnew.root")

#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze2limit.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze2limit.root")

#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze1limit.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze1limit.root")

#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze00limit.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze00limit.root")

## this is the one used for the freezed note
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze00systlimit.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/freze00systlimit.root")

# new fixed (STAT only)
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/post0limit.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/post0limit.root")

# new fixed (SYST)
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/post0systlimit.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/post0systlimit.root")

# new fixed (minor SYST only)
### USED MIT meeting
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/post0systlimit_ALL.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/post0systlimit_ALL.root")

### NEW TO BE USED
parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/preapp0syst.root")
parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/preapp0syst.root")

## fixed mass window for 400-500 + lowered the bjet PT 40->30 
#parser.add_option("","--input",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/finaltry.root")
#parser.add_option("","--inputBin",type='string',help="Input ROOT file. [%default]", default="/afs/cern.ch/user/h/hum/work/public/CMSSW_8_0_26_patch1/src/ChargedHiggs/ntufile/finaltry.root")

if doSyst:
	parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_SYST.root")
	parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_SYST.txt")
else:
	parser.add_option("-o","--output",type='string',help="Output ROOT file. [%default]", default="workspace_STAT.root")
	parser.add_option("-d","--datCardName",type='string',help="Output txt file. [%default]", default="cms_datacard_topbottom_STAT.txt")

parser.add_option("-l","--lumi",type='float',help="Luminosity. [%default]", default=35867)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-k","--kMass",type='string',help = "Which mass point. [%default]", default=1000)
extra.add_option("-c","--kCat",type='int',help = "Which category. [%default]", default=0)
#extra.add_option("-r","--rebin",type='int',help = "Rebin Histogram. [%default]", default=-1)
#
#parser.add_option_group(extra)

opts, args = parser.parse_args()

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

g=[] ## garbage un-collector

def WorkspaceSubstitution(string):
	res = re.sub('JES','CMS_scale_j',string)	
	res = re.sub('JER','CMS_res_j',res)	
	res = re.sub('BTAGB','CMS_eff_b',res)	
	res = re.sub('BTAGL','CMS_fake_b',res)	
	res = re.sub('BTAG','CMS_eff_b',res)	
	res = re.sub('TAUANTIE','CMS_eff_t_ele',res)	
	res = re.sub('TAUSCALE','CMS_scale_t',res)	
	res = re.sub('TAUHIGHPT','CMS_eff_t_highpt',res)	
	res = re.sub('TAU','CMS_eff_t',res)	
	res = re.sub('TRIGMET','CMS_trig_met',res)	
	res = re.sub('TRIG1p','CMS_trg_t_1p',res)	
	res = re.sub('TRIG3p','CMS_trg_t_3p',res)	
	res = re.sub('TRIG','CMS_trg_t',res)	
	res = re.sub('RFAC1p','CMS_fake_t_1p',res)	
	res = re.sub('RFAC3p','CMS_fake_t_3p',res)	
	res = re.sub('RFAC','CMS_fake_t',res)	
	res = re.sub('ELEVETO','CMS_eff_e_veto',res)	
	res = re.sub('MUVETO','CMS_eff_m_veto',res)	
	res = re.sub("UNCLUSTER","CMS_scale_uncluster",res)
	res = re.sub("PU","CMS_pileup",res)
	res = re.sub("muTRG","CMS_eff_m_trigger",res)
	res = re.sub("muID","CMS_eff_m_id",res)
	res = re.sub("muISO","CMS_eff_m_iso",res)
	res = re.sub("muRECO","CMS_eff_m_reco",res)
	res = re.sub("TOPRW","CMS_topreweight",res)
	res = re.sub('ELE','CMS_eff_e',res)	
	res = re.sub('MU','CMS_eff_m',res)	
	res = re.sub('TAU','CMS_eff_t',res)	
	return res

def RebinN(h,nBin):
	''' Rebin with un-even bins '''
	h1=h.Rebin(nBin)
	return h1

fIn = ROOT.TFile.Open(opts.input,"READ")
fInBin = ROOT.TFile.Open(opts.inputBin,"READ")

if fIn == None: 
	print "ERROR: file",opts.input,"doesn't exist"
	exit(1)
if fInBin == None:
	print "ERROR: file",opts.inputBin,"doesn't exist"
	exit(1)

basecat = []
channel = []

######### workspace conventions
# SR:
# OneBOneFat_X_Y_Z
# CR1l:
# OneBOneFat1l_X_Y_Z
# CRmirror: OneBOneMirrorFat_X_Y_Z
# among X,  which number of bjets Y
# (one, two, three) number of remaining jets Z
# (highj, lowj) Signal category (t0b,t1b,wbb,wbj)

#HT_3000_in_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8

#########


def Merge3B(tfile,hname):

	hname_1=hname.replace("_two", "_three")
	hTmp=tfile.Get(hname)
	hTmp1=tfile.Get(hname_1)
	if hTmp!=None and hTmp1!=None: hTmp.Add(hTmp1)

	return hTmp


def MergeAbove(tfile,hname):

	hname_1=hname.replace("_below", "_above")
	hTmp=tfile.Get(hname)
	hTmp1=tfile.Get(hname_1)
	if hTmp!=None and hTmp1!=None: hTmp.Add(hTmp1)

	return hTmp


def MergeAbove3B(tfile,hname):

	## histos has "below" and "two"

	hname_=hname.replace("_below", "_above")
	hname_1=hname_.replace("_two", "_three")
	# original
	hTmp=tfile.Get(hname)
	# changed
	hTmp1=tfile.Get(hname_1)
	if hTmp!=None and hTmp1!=None: hTmp.Add(hTmp1)

	return hTmp


def mergeCategory(tfile,toget):

	if doSChannel:

		if "_two" in toget and not "_below" in toget:
			hTmp = Merge3B(tfile,toget)
		elif "_below" in toget and not "_two" in toget:
			hTmp = MergeAbove(tfile,toget)
		elif "_below" and "_two" in toget:
			hTmp = MergeAbove3B(tfile,toget)

		else:
			hTmp = tfile.Get(toget)
	else:
		if "_below" in toget:
			hTmp = MergeAbove(tfile,toget)
		else:
			hTmp = tfile.Get(toget)


	if hTmp!=None: hTmp.Rebin(nRebinHT)

	return hTmp


### TEST conf
#channel = ["t0b"]
#channel = ["t1b"]
#channel = ["wbb"]
#channel = ["wbj"]
#basecat = ["OneBOneFat_two_highj"]

#basecat = ["OneBOneFat_one_highj","OneBOneFat_two_highj", "OneBOneFat_one_lowj","OneBOneFat_two_lowj"
##	   ,"OneBOneFat1l_one","OneBOneFat1l_two"
#	   ,"OneBOneMirrorFat_one_highj","OneBOneMirrorFat_two_highj", "OneBOneMirrorFat_one_lowj","OneBOneMirrorFat_two_lowj"
#	   ]

### FINAL conf
if doSChannel:
	basecat = [ "OneBOneFat_one_highj","OneBOneFat_two_highj", "OneBOneFat_one_lowj","OneBOneFat_two_lowj"
		   ,"OneBOneFat1l_one","OneBOneFat1l_two","OneBOneFat1l_three"
		   ,"OneBOneMirrorFat_one_highj","OneBOneMirrorFat_two_highj", "OneBOneMirrorFat_one_lowj","OneBOneMirrorFat_two_lowj"
		   ]
else:
	basecat = [ "OneBOneFat_one_highj","OneBOneFat_two_highj","OneBOneFat_three_highj", "OneBOneFat_one_lowj","OneBOneFat_two_lowj","OneBOneFat_three_lowj"
		    ,"OneBOneFat1l_one","OneBOneFat1l_two","OneBOneFat1l_three"
		    ,"OneBOneMirrorFat_one_highj","OneBOneMirrorFat_two_highj","OneBOneMirrorFat_three_highj", "OneBOneMirrorFat_one_lowj","OneBOneMirrorFat_two_lowj","OneBOneMirrorFat_three_lowj"
		    ]

if doSChannel:
	if opts.kCat==0:
		channel = ["t0b","t1b","wbb","wbj"]
		label="sChan_"
	if opts.kCat==1:
		channel = ["t0b"]
		label="t0b_sChan_"
	if opts.kCat==2:
		channel = ["t1b"]
		label="t1b_sChan_"
	if opts.kCat==3:
		channel = ["wbb"]
		label="wbb_sChan_"
	if opts.kCat==4:
		channel = ["wbj"]
		label="wbj_sChan_"

##	channel = ["wbj"]
##	channel = ["t1b"]
##	label="sChan_"
##	label="sChan_sronly_"
##	label="sChan_inonly_"
##	label="sChan_"

else:

	if opts.kCat==0:
		channel = ["t0b","t1b","wbb","wbj"]
#		channel = ["t0b","t1b","wbb"]
		label="fullHad_"
	if opts.kCat==1:
		channel = ["t0b"]
		label="t0b_fullHad_"
	if opts.kCat==2:
		channel = ["t1b"]
		label="t1b_fullHad_"
	if opts.kCat==3:
		channel = ["wbb"]
		label="wbb_fullHad_"
	if opts.kCat==4:
		channel = ["wbj"]
		label="wbj_fullHad_"
#	label="counting_"
#	label="sronly_"
#	label="inonly_"

catStore = { } ## name -> {"file", extra options for syst}, hasSignal
statStore = {} ## used to store th1d for stat uncertainties

VarTest=""

#if opts.kTest > 10:
#	doRebin = False
	
for y in channel:
	for x in basecat:

##		region = ["_in","_above","_below"]
		region = ["_in","_below"]

#counting
#		if True:
#			region = ["_in"]
#			if "OneBOneFat1l" in x: continue
#			if "OneBOneMirrorFat" in x:continue

		if "OneBOneFat1l" in x:
			region = ["_all"]

		for reg in region:

			masses = [ opts.kMass ]
			for mas in masses:

				if "OneBOneFat1l" in x:
					name = x+ "_" + y
				else:
					name = x+ "_" + y + mas + reg

	#		catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["all"],"var":"invariantExt"}


				# UNDEFINED
				if "OneBOneFat_one" in x and y=="wbb": continue
				if "OneBOneMirrorFat_one" in x and y=="wbb": continue
				if "OneBOneFat1l_one" in x and y=="wbb": continue

				## generally BKG very rare
				if "_three_lowj" in x and y=="wbj": continue
				if "OneBOneFat1l_three" in x and y=="wbj": continue
				if "_three_highj" in x and y=="wbj": continue
				if "OneBOneFat1l_two" in x and y=="wbj": continue

##				if doSChannel and "OneBOneFat1l_three" in x: continue

				## very rare
				if opts.kMass=="400":
					## wbj
					if "_three_highj" in x and y=="wbj": continue
					if "_two_lowj" in x and y=="wbj": continue
					## wbb
					if "_two_lowj" in x and y=="wbb": continue
					## t0b and t1b
					if "_two_highj" in x and y=="t0b" and "below" in reg: continue
					if "_three_lowj" in x and y=="t0b" and "in" in reg: continue
					if "_one_highj" in x and (y=="t1b" or y=="t0b") and "in" in reg: continue
					if "_one_lowj" in x and (y=="t1b") and "below" in reg: continue
					if "_three" in x and y=="t0b" and "below" in reg: continue

                                if opts.kMass=="3000":
##					if "_two_highj" in x and y=="wbb": continue
##					if "_three_lowj" in x and (y=="t0b") and "above" in reg: continue
					if "_three_lowj" in x and (y=="t1b") and "in" in reg: continue

                                if opts.kMass=="2500":
					if "_three_lowj" in x and (y=="t0b") and "in" in reg: continue

                                ## below are for associated only
#				if not doSChannel and (opts.kMass=="500" or opts.kMass=="400") and "in" in reg or "below" in reg:
				if not doSChannel and (opts.kMass=="500" or opts.kMass=="400") and "below" in reg or "in" in reg:
					if "_three_lowj" in x and y=="wbb": continue
					if "_one_lowj" in x and y=="wbj": continue

				if doSChannel and (opts.kMass=="800" or opts.kMass=="900" or opts.kMass=="1000" or opts.kMass=="1500" or opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000") and "in" in reg:
					if "_two_highj" in x and y=="wbb": continue
					if "_one_highj" in x and y=="wbj": continue

				if doSChannel and (opts.kMass=="900" or opts.kMass=="1000" or opts.kMass=="1500" or opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000") and "in" in reg:
					if "_two_lowj" in x and y=="wbj": continue
					if "_two_highj" in x and y=="wbj": continue

				if doSChannel and (opts.kMass=="1000" or opts.kMass=="1500" or opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000") and "in" in reg:
					if y=="wbb": continue
					if y=="wbj": continue

				if not doSChannel and (opts.kMass=="1500" or opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000") and "in" in reg:
					if "_two_lowj" in x and y=="wbj": continue
					if "_three_highj" in x and y=="wbj": continue

                                if not doSChannel and (opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000") and "in" in reg:
					if "_one_highj" in x and y=="wbj": continue
					if "_two_highj" in x and y=="wbj": continue
					if "_three_lowj" in x and y=="wbb": continue
					if "_three_highj" in x and y=="wbb": continue

                                if not doSChannel and (opts.kMass=="2500" or opts.kMass=="3000") and "in" in reg:
					if "_two_lowj" in x and y=="wbb": continue
					if "_three_lowj" in x and y=="wbb": continue
					if "_three_highj" in x and y=="wbb": continue
					if "_one_lowj" in x and y=="wbj": continue


#				if opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000":
##				if opts.kMass=="1500" or opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000":
#					if y=="wbj": continue
#					if "_three" in x and y=="wbb": continue
#					if "_two" in x and y=="wbj": continue
#					if "_two" in x and y=="wbb" and opts.kMass=="3000": continue
#					if "_three" in x and y=="t0b": continue


#				if opts.kMass=="2500" or opts.kMass=="3000":
					#				# for the 2000,2500,3000: (They are all in the below bin)
#					if "OneBOneMirrorFat" in x and y=="wbb" and ("above" in reg or "in" in reg): continue
#					if "OneBOneMirrorFat" in x and y=="wbj" and ("above" in reg or "in" in reg): continue
#					if "OneBOneFat" in x and y=="wbb" and ("above" in reg or "in" in reg): continue
#					if "OneBOneFat" in x and y=="wbj" and ("above" in reg or "in" in reg): continue


##				if "OneBOneMirrorFat_three_lowj" in x and y=="t0b": continue 
#				if "_three" in x and ("above" in reg) and (y=="t0b"): continue 
#				if "_three" in x and ("above" in reg) and (y=="t1b"): continue 
#				if "_two" in x and ("above" in reg) and (y=="t0b"): continue 
#				if "_two" in x and ("above" in reg) and (y=="t1b"): continue 
#				if "_three" in x and (y=="wbb"): continue 
#				if "_three" in x and (y=="wbj"): continue

#				if doSChannel and opts.kMass=="3000":
#					if ("in" in reg or "above" in reg): continue

#				if doSChannel and opts.kMass=="3000":				
#					if "OneBOneFat_two_lowj" in x and y=="t1b" and "in" in reg: continue 
#					if "OneBOneMirrorFat_two_highj" in x and y=="t1b" and ("above" in reg) : continue
#					if "OneBOneFat_two_highj" in x and y=="t1b" and ("in" in reg) : continue
#					if "OneBOneMirrorFat_two_highj" in x and y=="t1b" and ("in" in reg) : continue

#				if "OneBOneFat_one_lowj" in x and y=="wbb": continue 
#				if "OneBOneMirrorFat_one_highj" in x and y=="wbb": continue

#				if opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000":
#					#				# for the 2000,2500,3000: (They are all in the below bin)
#					if "OneBOneMirrorFat" in x and y=="wbb" and ("above" in reg): continue
#					if "OneBOneMirrorFat" in x and y=="wbj" and ("above" in reg): continue
#					if "OneBOneFat" in x and y=="wbb" and ("above" in reg): continue
#					if "OneBOneFat" in x and y=="wbj" and ("above" in reg): continue

				# THIS IS planB
#				if "_lowj" in x and y=="wbb": continue
#				if "_lowj" in x and y=="wbj": continue


				# UNDEFINED
				# they should not exist but some are filled
#				if "OneBOneFat_one" in x and y=="t1b": continue
#				if "OneBOneMirrorFat_one" in x and y=="t1b": continue
#				if "OneBOneFat1l_one" in x and y=="t1b": continue

#				if "OneBOneFat_three" in x and y=="wbj": continue
#				if "OneBOneMirrorFat_three" in x and y=="wbj": continue
#				if "OneBOneFat1l_three" in x and y=="wbj": continue

#				if opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000":
#					#				# for the 2000,2500,3000: (They are all in the below bin)
#					if "OneBOneMirrorFat" in x and y=="wbb" and ("above" in reg or "in" in reg): continue
#					if "OneBOneMirrorFat" in x and y=="wbj" and ("above" in reg or "in" in reg): continue
#					#				## recovered the wbb - in
#					if "OneBOneFat" in x and y=="wbb" and ("above" in reg ): continue
#					if "OneBOneFat" in x and y=="wbj" and ("above" in reg or "in" in reg): continue

#				# t0b/t1b & 3b NO STAT very rare situation
#				if "OneBOneMirrorFat_three_lowj" in x and y=="t1b": continue
#				if "OneBOneMirrorFat_three_highj" in x and y=="t1b": continue

#				if doSChannel or (opts.kMass=="2000" or opts.kMass=="2500" or opts.kMass=="3000"):
#					if "OneBOneFat_three" in x and y=="t0b": continue
#					if "OneBOneMirrorFat_three" in x and y=="t0b": continue

#				## NO STAT
#				if "OneBOneFat1l_one" in x and (y=="t0b" or y=="wbj"): continue
				##
##				if "OneBOneFat1l_two" in x and ( y=="wbb" or y=="wbj" or y=="t0b" or y=="t1b"): continue
#				if "OneBOneFat1l_two" in x and ( y=="wbb" or y=="wbj" or y=="t0b" or y=="t1b"): continue
				##
##				if "OneBOneFat1l_three" in x and ( y=="wbb" or y=="t1b" or y=="t0b"): continue
#				if "OneBOneFat1l_three" in x and ( y=="wbb" or y=="t1b" or y=="t0b"): continue

				if "OneBOneFat1l" in x:
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["ttbar"],"var":"HT"+reg}
					name = x+ "_" + y
				else:
					catStore [ name ] = { "name": name,"dir": x+ "_" + y,"file": None, "hasMC":["ttbar"],"var":"HT_"+mas+reg}

				catStore[name]['file'] = fIn

### QCD correlated
#				catStore[name]["hasMC"]=["qcd","top","Hptb"]

### QCD uncorrelated
				if ( y=="wbb" or y=="wbj") and "_one" in x: catStore[name]["hasMC"]=["qcd_wx_one","ttbar","Hptb","top","ewk"]
				if ( y=="wbb" or y=="wbj") and "_two" in x: catStore[name]["hasMC"]=["qcd_wx_two","ttbar","Hptb","top","ewk"]
				if ( y=="wbb" or y=="wbj") and "_three" in x: catStore[name]["hasMC"]=["qcd_wx_three","ttbar","Hptb","top","ewk"]
				if ( y=="t0b" or y=="t1b") and "_one" in x: catStore[name]["hasMC"]=["qcd_tx_one","ttbar","Hptb","top","ewk"]
				if ( y=="t0b" or y=="t1b") and "_two" in x: catStore[name]["hasMC"]=["qcd_tx_two","ttbar","Hptb","top","ewk"]
				if ( y=="t0b" or y=="t1b") and "_three" in x: catStore[name]["hasMC"]=["qcd_tx_three","ttbar","Hptb","top","ewk"]

                        if doSChannel:
				mcStore={
					"Hptb":{"name":"Hptb", "hist":["WprimeToTB_TToHad_M-%d"], "num":0 },
##					"qcd":{"name":"qcd", "hist":["QCD_HT"], "num":1 },
					"qcd_wx_one":{"name":"qcd_wx_one", "hist":["QCD_HT"], "num":1 },
					"qcd_wx_two":{"name":"qcd_wx_two", "hist":["QCD_HT"], "num":2 },
					"qcd_wx_three":{"name":"qcd_wx_three", "hist":["QCD_HT"], "num":3 },
					"qcd_tx_one":{"name":"qcd_tx_one", "hist":["QCD_HT"], "num":4 },
					"qcd_tx_two":{"name":"qcd_tx_two", "hist":["QCD_HT"], "num":5 },
					"qcd_tx_three":{"name":"qcd_tx_three", "hist":["QCD_HT"], "num":6 },
					"ttbar":{ "name":"ttbar","hist":["TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":7},
					"top":{ "name":"top","hist":["ST"],"num":8},
#					"ewk":{ "name":"ewk","hist":["WJets","ZJets","DYJets"],"num":9}
					"ewk":{ "name":"ewk","hist":["WJets"],"num":9}
					}
			else:
				mcStore={
					"Hptb":{"name":"Hptb", "hist":["ChargedHiggs_HplusTB_HplusToTB_M-%d_13TeV_amcatnlo_pythia8"], "num":0 },
##					"qcd":{"name":"qcd", "hist":["QCD_HT"], "num":1 },
					"qcd_wx_one":{"name":"qcd_wx_one", "hist":["QCD_HT"], "num":1 },
					"qcd_wx_two":{"name":"qcd_wx_two", "hist":["QCD_HT"], "num":2 },
					"qcd_wx_three":{"name":"qcd_wx_three", "hist":["QCD_HT"], "num":3 },
					"qcd_tx_one":{"name":"qcd_tx_one", "hist":["QCD_HT"], "num":4 },
					"qcd_tx_two":{"name":"qcd_tx_two", "hist":["QCD_HT"], "num":5 },
					"qcd_tx_three":{"name":"qcd_tx_three", "hist":["QCD_HT"], "num":6 },
					"ttbar":{ "name":"ttbar","hist":["TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"],"num":7},
					"top":{ "name":"top","hist":["ST"],"num":8},
#					"ewk":{ "name":"ewk","hist":["WJets","ZJets","DYJets"],"num":9}
					"ewk":{ "name":"ewk","hist":["WJets"],"num":9}
					}

                        if doSyst:
                                systStore={
                                        "None":None,
                                        "lumi_13TeV":{"type":"lnN", "value":["1.025"] ,"proc":[".*"],"wsname":"lumi_13TeV","name":"XXX"},
					### lepton veto
                                        "CMS_eff_l":{"type":"lnN", "value":["0.96"] ,"proc":[".*"],"wsname":"CMS_eff_l","name":"XXX"}, ## name used for shape
                                        "CMS_eff_t":{"type":"lnN", "value":["1.03"] ,"proc":[".*"],"wsname":"CMS_eff_t","name":"XXX"}, ## name used for shape
					### trigger efficiency
                                        "CMS_eff_trigger":{"type":"lnN", "value":["1.05"] ,"proc":[".*"],"wsname":"CMS_eff_triggger","name":"XXX"},
					### Theory modeling
                                        "CMS_topreweight":{"type":"shape", "wsname":"CMS_topreweight","name":"TOPRW","proc":["ttbar"]},
                                        "muRF_ttbar":{"type":"shape", "wsname":"muRF_ttbar","name":"Scale","proc":["ttbar"]},
                                        "muRF_Hptb":{"type":"shape", "wsname":"muRF_Hptb","name":"Scale","proc":["Hptb"]},
					### MET-Jets-PU
                                        "CMS_pileup":{"type":"shape", "wsname":"CMS_pileup","name":"PU","proc":["qcd_wx_one","qcd_wx_two","qcd_wx_three","qcd_tx_one","qcd_tx_two","qcd_tx_three","ttbar","top","Hptb"]},
                                        "CMS_scale_uncluster":{"type":"shape", "wsname":"CMS_scale_uncluster","name":"UNCLUSTER","proc":["qcd_wx_one","qcd_wx_two","qcd_wx_three","qcd_tx_one","qcd_tx_two","qcd_tx_three","ttbar","top","Hptb"]},
                                        "CMS_scale_SDMass":{"type":"shape", "wsname":"CMS_scale_SDMass","name":"SDMassSCALE","proc":["qcd_wx_one","qcd_wx_two","qcd_wx_three","qcd_tx_one","qcd_tx_two","qcd_tx_three","ttbar","top","Hptb"]},
                                        "CMS_eff_Tau21":{"type":"lnN", "value":["1.08","1.08"] ,"proc":["ttbar","Hptb"],"wsname":"CMS_eff_Tau21","name":"XXX"}, ## name used for shape
                                        "CMS_eff_Tau32":{"type":"lnN", "value":["1.02","1.02"] ,"proc":["ttbar","Hptb"],"wsname":"CMS_eff_Tau32","name":"XXX"}, ## name used for shape
#                                        "CMS_res_j":{"type":"shape", "wsname":"CMS_res_j","name":"JER","proc":[".*"]},
                                        "CMS_scale_j_qcd":{"type":"shape", "wsname":"CMS_scale_j_qcd","name":"JESANDCSV","proc":["qcd_wx_one","qcd_wx_two","qcd_wx_three","qcd_tx_one","qcd_tx_two","qcd_tx_three"]},
                                        "CMS_scale_j":{"type":"shape", "wsname":"CMS_scale_j","name":"JESANDCSV","proc":["Hptb","ttbar","top","ewk"]},
                                        ##Light jets Heavy flavor contamination
					"CMS_btag_HF_qcd":{"type":"shape", "wsname":"CMS_btag_HF_qcd","name":"CSVRHF","proc":["qcd_wx_one","qcd_wx_two","qcd_wx_three","qcd_tx_one","qcd_tx_two","qcd_tx_three"]},
					"CMS_btag_HF":{"type":"shape", "wsname":"CMS_btag_HF","name":"CSVRHF","proc":["ttbar","top","Hptb"]},
                                        ##Heavy jets light flavor contamination
                                        "CMS_btag_LF":{"type":"shape", "wsname":"CMS_btag_LF","name":"CSVRLF","proc":["qcd_wx_one","qcd_wx_two","qcd_wx_three","qcd_tx_one","qcd_tx_two","qcd_tx_three","Hptb","ttbar","top"]},
                                        ##Linear and quadratic uncertainties
					# note decouple the qcd
					"CMS_btag_HFstat1":{"type":"shape", "wsname":"CMS_btag_HFstat1","name":"CSVRHFSTAT1","proc":[".*"]},
					"CMS_btag_HFstat2":{"type":"shape", "wsname":"CMS_btag_HFstat2","name":"CSVRHFSTAT2","proc":[".*"]},
                                        "CMS_btag_LFstat1":{"type":"shape", "wsname":"CMS_btag_LFstat1","name":"CSVRLFSTAT1","proc":[".*"]},
                                        "CMS_btag_LFstat2":{"type":"shape", "wsname":"CMS_btag_LFstat2","name":"CSVRLFSTAT2","proc":[".*"]},
                                        "CMS_btag_CFerr1":{"type":"shape", "wsname":"CMS_btag_CFerr1","name":"CSVRCERR1","proc":[".*"]},
                                        "CMS_btag_CFerr2":{"type":"shape", "wsname":"CMS_btag_CFerr2","name":"CSVRCERR2","proc":[".*"]}
                                        ####
                                        }
                        else:
				systStore={
                                        "None":None
                                        }
	
print "--------- CAT STORE IS --------"
for cat in catStore:
	print "* ",cat,":",catStore[cat]
print "---------------------- --------"

fileTmp="MIAO_JUNE22/"+label+VarTest+opts.kMass+"_"+opts.output

w = ROOT.RooWorkspace("w","w")
datNameTmp = opts.datCardName
datName = "MIAO_JUNE22/"+label+ VarTest+opts.kMass+"_" + datNameTmp

datacard=open(datName,"w")
datacard.write("-------------------------------------\n")
datacard.write("imax *\n")
datacard.write("jmax *\n")
datacard.write("kmax *\n")
datacard.write("-------------------------------------\n")

##exit(1)

w.factory("ht[800,6000]"); # RooRealVar
ht=w.var("ht")

arglist_obs = ROOT.RooArgList(ht)
argset_obs = ROOT.RooArgSet(ht)

for x in catStore:
	if applyLikelihoodRebinBin:
		w.factory("ht_"+x+"[0,1]"); # RooRealVar
	else:
		w.factory("ht_"+x+"[800,6000]"); # RooRealVar
        htx=w.var("ht_"+x)
        htx.setBins(LikeBins)
        arglist_obs.add(htx)


def skip(cat,mc):
	if cat["hasMC"][0] == "all": return False
	for x in cat["hasMC"]:
		if mc["name"] == x : return False
	return True


## write shapes in workspace
#if False: # data
#	datacard.write("shapes data_obs *\t" + fileTmp +"\t")
#	datacard.write("w:data_obs_$CHANNEL")
#	datacard.write("\n")
#if True: # Sig
#	datacard.write("shapes Hptb *\t" + fileTmp +"\t")
#	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL\t")
#	datacard.write("w:pdf_$PROCESS_M-$MASS_$CHANNEL_$SYSTEMATIC")
#	datacard.write("\n")
#if True: #bkg
#	datacard.write("shapes * * %s\t"%fileTmp +"\t")
#	datacard.write("w:pdf_$PROCESS_$CHANNEL\t")
#	datacard.write("w:pdf_$PROCESS_$CHANNEL_$SYSTEMATIC\n")
#datacard.write("-------------------------------------\n")
#datacard.write("## Observation\n")

## write shapes in histograms
if True: # data
# for MARIA
        datacard.write("shapes data_obs *\t" + fileTmp +"\t")
# for JAN
#        datacard.write("shapes data_obs *\t" + fileTmp.split('/')[1] +"\t")
        datacard.write("data_obs_$CHANNEL")
        datacard.write("\n")
if True: # Sig
# for MARIA
        datacard.write("shapes Hptb *\t" + fileTmp +"\t")
# for JAN
#        datacard.write("shapes Hptb *\t" + fileTmp.split('/')[1] +"\t")
        datacard.write("pdf_$PROCESS_$CHANNEL\t")
        datacard.write("pdf_$PROCESS_$CHANNEL_$SYSTEMATIC")
        datacard.write("\n")
if True: # bkg
# for MARIA
        datacard.write("shapes * * %s\t"%fileTmp +"\t")
# for JAN
#        datacard.write("shapes * * %s\t"%fileTmp.split('/')[1] +"\t")
        datacard.write("pdf_$PROCESS_$CHANNEL\t")
        datacard.write("pdf_$PROCESS_$CHANNEL_$SYSTEMATIC\n")
datacard.write("-------------------------------------\n")
datacard.write("## Observation\n")

# write observation
datacard.write("bin\t")
for cat in catStore:
	datacard.write("%s\t"%cat)
datacard.write("\n")
datacard.write("observation\t")
for cat in catStore:
	datacard.write("-1\t")
datacard.write("\n")

datacard.write("-------------------------------------\n")
# write expectations
datacard.write("bin\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("%s\t"%cat)
datacard.write("\n")

datacard.write("process\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("%s\t"%mc)
datacard.write("\n")

datacard.write("process\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("%d\t"%mcStore[mc]["num"])
datacard.write("\n")

datacard.write("rate\t")
for cat in catStore:
    for mc in mcStore:
         if skip(catStore[cat],mcStore[mc]): continue
         datacard.write("-1\t")
datacard.write("\n")
datacard.write("-------------------------------------\n")
# write systematics

## 
def writeNormSystLU(name="lumi",valueL=["1.027","1.026"], regexpL=["TT","ST",""]):
	datacard.write(name+"\tlnU")
	invert=False

        for cat in catStore:
            for proc in mcStore:
                if skip(catStore[cat],mcStore[proc]): continue
		idx=-1
		invert=False
		for i,regexp in enumerate(regexpL):
			match=re.search(regexp,proc)
			if regexp != "" and regexp[0] == '!':
				invert=True
				regexp=regexp[1:]
			if (match and not invert) or (not match and invert):
				idx=i
				break

		if (idx>=0):
		   datacard.write("\t"+valueL[idx])
		else:
		   datacard.write("\t-")
	datacard.write("\n")

def writeNormSyst(name="lumi",valueL=["1.027","1.026"], regexpL=["TT","ST",""],regexpCat=[]):
        datacard.write(name+"\tlnN")
        invert=False

	for cat in catStore:
		print cat
		for proc in mcStore:
			print proc
			if skip(catStore[cat],mcStore[proc]): continue
			idx=-1
			invert=False
			for i,regexp in enumerate(regexpL):
				print 'regexp',regexp,' proc',proc
				match=re.search(regexp,proc)
				if regexp != "" and regexp[0] == '!':
					invert=True
					regexp=regexp[1:]
				if (match and not invert) or (not match and invert):
					idx=i
					break
			idx2=-1
			invert=False
			for i,regexp in enumerate(regexpCat):
				print 'regexp',regexp,' proc',cat
				match=re.search(regexp,cat)
				if regexp != "" and regexp[0] == '!':
					invert=True
					regexp=regexp[1:]
				if (match and not invert) or (not match and invert):
					idx2=i
					break

			print cat, proc
			if (idx>=0 and (idx2>=0 or len(regexpCat)==0 )):
				print 'idx',idx,' valueL[idx]=' ,valueL
				print 'idx2',idx2, ' valueL[idx]=',valueL
				datacard.write("\t"+valueL[idx])
			else:
				datacard.write("\t-")
	datacard.write("\n")

def writeSystShape(syst,regexpL=[],regexpCat=None):
	# this function put a bunch of 1
	name=syst["wsname"]
	datacard.write(name+"\tshape")

        for cat in catStore:
	    matchCat=True
	    if regexpCat != None:
		matchCat=re.search(regexpCat,cat)

            for proc in mcStore:
                if skip(catStore[cat],mcStore[proc]): continue
		idx=-1
		invert=False
		for i,regexp in enumerate(regexpL):
			match=re.search(regexp,proc)
			if regexp != "" and regexp[0] == '!':
				invert=True
				regexp=regexp[1:]
			if (match and matchCat and not invert) or (not match and matchCat and invert):
				idx=i
				break

		if (idx>=0):
		   datacard.write("\t1")
		else:
		   datacard.write("\t-")
	datacard.write("\n")

def importStat():
	for statName in statStore:
	   stat= statStore[statName]
 	   h=stat["hist"] ## all MC Sum up correctly
	   h0=stat["hist0"] ## nominal
	   minError=-1
 	   for iBin in range(1,h.GetNbinsX()+1):
 	      c= h.GetBinContent(iBin)
 	      if c<=0: continue;
 	      e=h.GetBinError(iBin)
 	      if e>0 and (e<minError or minError<0):
		      minError=e
 	   for iBin in range(1,h.GetNbinsX()+1):
 	      c= h.GetBinContent(iBin)
 	      if c>0: continue
 	      h.SetBinError(iBin,minError)
	   for i in range(0,h.GetNbinsX()):
	      statsyst = { "wsname": statName + "_Bin%d"%(i+1) +"_STAT" , "name": statName + "_Bin%d"%(i+1) + "_STAT"}
	      hupbin=h0.Clone(h.GetName() +"_STATUp")
	      hdnbin=h0.Clone(h.GetName() +"_STATDown")
	      cont=h0.GetBinContent(i+1)
	      err = h . GetBinError(i+1) ## err is referred to the sum
	      c = h.GetBinContent(i+1) ## this is to check the magnitude, not to apply it
#	      if c > 0 and err/c <.01: continue ## don't write less than 1%
	      if c > 0 and err/c <.10: continue ## don't write less than 1% // TEMPORARY
	      hupbin.SetBinContent(i+1,cont+err)
	      hdnbin.SetBinContent(i+1,cont-err)

	      if hupbin.GetBinContent(i+1) <0 : hupbin.SetBinContent(i+1,cont)
	      if hdnbin.GetBinContent(i+1) <0 : hdnbin.SetBinContent(i+1,cont)

	      target = stat["target"]
	      cat = catStore[stat["cat"] ]
	      print "++++++++++++++++++++target",target,' cate',cat

	      al=arglist_obs
#	      al=arglist_obs_bdt
#	      if "bdt2D" in cat["var"]: al = arglist_obs_bdt2D
#	      if cat["var"] == "HT" or cat["var"] == "HT_SR1" or cat["var"] == "HT_SR2" or cat["var"] == "HT_SR3" or cat["var"] == "HT_SR4" or cat["var"] == "HT_SR13" or cat["var"] == "HT_SR24": al = arglist_obs

 	      roo_mc_binup = ROOT.RooDataHist(target+"_"+statsyst["wsname"]+"Up",target + "STAT",al,hupbin)
 	      pdf_mc_binup = roo_mc_binup
 	      roo_mc_bindn = ROOT.RooDataHist(target+"_"+statsyst["wsname"]+"Down",target + "STAT",al,hdnbin)
 	      pdf_mc_bindn = roo_mc_bindn
 	      getattr(w,'import')(pdf_mc_binup,ROOT.RooCmdArg())
 	      getattr(w,'import')(pdf_mc_bindn,ROOT.RooCmdArg())
 	      g.extend([hupbin,roo_mc_binup,pdf_mc_binup])
 	      g.extend([hdnbin,roo_mc_bindn,pdf_mc_bindn])
	
	      ## attribute it only to one MC, since is summed
	      #print "*** DEBUG *** writing matching for",stat["mc"]+"_","pdfname=",target+"_"+statsyst["wsname"]+"Up",
	      #print "--- statsyst",statsyst
	      writeSystShape(statsyst,[stat["mc"]+"$"],stat["cat"])

################### WRITE SYST ################
for syst in systStore:
	if syst == "None": continue
	if systStore[syst]["type"] == "lnN":
                if "CMS_eff_Tau21" in systStore[syst]["wsname"]:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["wbb","wbj"])
                elif "CMS_eff_Tau32" in systStore[syst]["wsname"]:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"],["t0b","t1b"])
		else:
			writeNormSyst(syst,systStore[syst]["value"],systStore[syst]["proc"])
	if systStore[syst]["type"] == "shape":
                if "muRF" in systStore[syst]["wsname"]:
			if not doSChannel: writeSystShape(systStore[syst],systStore[syst]["proc"])
		else:
			writeSystShape(systStore[syst],systStore[syst]["proc"])

if doSyst: writeNormSyst("QCDscale_ttbar",["0.965/1.024"],["ttbar"])
if doSyst: writeNormSyst("pdf_gg",["1.042"],["ttbar"])
if doSyst: writeNormSyst("CMS_mass_top",["1.027"],["ttbar"])

## "top" get the value od the single top
if doSyst: writeNormSyst("QCDscale_top",["0.977/1.028"],["top"])
if doSyst: writeNormSyst("pdf_top",["1.026"],["top"])
if doSyst: writeNormSyst("CMS_mass_top",["1.022"],["top"])

#if doSyst: writeNormSyst("CMS_HPTB_QCDnonclosure_",["1.20","1.20","1.20"],["qcd_wx_one","qcd_wx_two","qcd_wx_three"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDnonclosure_",["1.20","1.20","1.20"],["qcd_tx_one","qcd_tx_two","qcd_tx_three"])
if doSyst: writeNormSystLU("CMS_HPTB_QCDnonclosure",["2.","2.","2."],["qcd_wx_one","qcd_wx_two","qcd_wx_three"])
if doSyst: writeNormSystLU("CMS_HPTB_QCDnonclosure",["2.","2.","2."],["qcd_tx_one","qcd_tx_two","qcd_tx_three"])

## "ewk" get the envelop of the DY and W+jets
if doSyst: writeNormSyst("QCDscale_ewk",["0.996/1.008"],["ewk"])
if doSyst: writeNormSyst("pdf_qqbar",["1.04"],["ewk"])

#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttlf",["1.50"],["ttlf"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttcc",["1.50"],["ttcc"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttb",["1.50"],["ttb"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_ttbb",["1.50"],["ttbb"])
#if doSyst: writeNormSyst("CMS_HPTB_QCDscale_tt2b",["1.50"],["tt2b"])


#if doSyst: writeNormSyst("QCDscale_ewk",["0.98/1.02"],["ewk"])
#if doSyst: writeNormSyst("pdf_qqbar",["1.04"],["ewk"])

#if doSyst: writeNormSyst("QCDscale_ttbar",["0.977/1.028"],["ST"])
#if doSyst: writeNormSyst("pdf_ttbar",["1.026"],["ST"])
#if doSyst: writeNormSyst("CMS_mass_ttbar",["1.022"],["ST"])

##https://twiki.cern.ch/twiki/bin/view/LHCPhysics/LHCHXSWGTTH
#if doSyst: writeNormSyst("QCDscale_ttZ",["0.887/1.096"],["TTZ"])
#if doSyst: writeNormSyst("pdf_ttZ",["1.028"],["TTZ"])
#if doSyst: writeNormSyst("CMS_mass_ttZ",["1.028"],["TTZ"])

#if doSyst: writeNormSyst("QCDscale_ttH",["0.908/1.058"],["ttH"])
#if doSyst: writeNormSyst("pdf_ttH",["1.036"],["ttH"])
#if doSyst: writeNormSyst("CMS_mass_ttH",["1.036"],["ttH"])

##writeNormSyst("QCDscale_VV",["1.025","1.032","1.031"],["WWTo","WZTo","ZZTo"])
##writeNormSyst("pdf_VV",["1.022","1.044","1.037"],["WWTo","WZTo","ZZTo"])

##writeNormSyst("QCDscale_V",["0.9963/1.0065","0.996/1.008"],["DYJetsToLL_M","WJetsToLNu"])
##writeNormSyst("pdf_V",["1.037","1.0375"],["DYJetsToLL_M","WJetsToLNu"])

################### IMPORT ################
## def importStat(h,target,syst="STAT"):
## 	minError=-1
## 	for iBin in range(1,h.GetNbinsX()+1):
## 		c= h.GetBinContent(iBin)
## 		if c<=0: continue;
## 		e=h.GetBinError(iBin)
## 		if e>0 and (e<minError or minError<0):
## 			minError=e
## 	for iBin in range(1,h.GetNbinsX()+1):
## 		c= h.GetBinContent(iBin)
## 		if c<=0: continue;
## 		h.SetBinError(iBin,minError)
##
## 	hup=h.Clone(h.GetName() +"_STATUp")
## 	hdn=h.Clone(h.GetName() +"_STATDown")
## 	for i in range(0,h.GetNbinsX()):
## 		hupbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
## 		hdnbin=h.Clone(h.GetName()+ "_Bin%d"%(i+1)+"_STATUp")
## 		cont=h.GetBinContent(i+1)
## 		# 0 is for sure wrong in case of mc, fixed in GetHisto
## 		hupbin.SetBinContent(i+1, h.GetBinContent(i+1) + h.GetBinError(i+1) )
## 		hdnbin.SetBinContent(i+1, h.GetBinContent(i+1) - h.GetBinError(i+1) )
## 		roo_mc_binup = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Up",target,arglist_obs,hupbin)
## 		pdf_mc_binup = roo_mc_binup
## 		roo_mc_bindn = ROOT.RooDataHist(target+"_Bin%d"%(i+1)+"_"+syst+"Down",target,arglist_obs,hdnbin)
## 		pdf_mc_bindn = roo_mc_bindn
## 		getattr(w,'import')(pdf_mc_binup,ROOT.RooCmdArg())
## 		getattr(w,'import')(pdf_mc_bindn,ROOT.RooCmdArg())
## 		g.extend([hupbin,roo_mc_binup,pdf_mc_binup])
## 		g.extend([hdnbin,roo_mc_bindn,pdf_mc_bindn])


def SmoothAndMergeSyst(tfile,togetNom,togetSyst,s):

        hTmpNom=mergeCategory(tfile,togetNom)
        hTmpUp=mergeCategory(tfile,togetSyst+'Up')
        hTmpDown=mergeCategory(tfile,togetSyst+'Down')

        if hTmpUp==None: return hTmpNom
        if hTmpDown==None: return hTmpNom

        for iBin in range(1,hTmpNom.GetNbinsX()+1):
#               c= hTmpNom.SetBinError(iBin,0)
                c= hTmpUp.SetBinError(iBin,0)
                c= hTmpDown.SetBinError(iBin,0)

	if LikeBins==1:

		hUp = hTmpUp
		hDown = hTmpDown

	else:

		if ('CSV' in togetSyst and not "CSVRHF" in togetSyst) or "PU" in togetSyst or "JER" in togetSyst or "SDMassScale" in togetSyst:
			hUp, hDown = SS.SystematicSmoother(hTmpNom, hTmpUp, hTmpDown,"~/www/forMIAO/JUNE14/SYST").smooth()
		else:
			hUp = hTmpUp
			hDown = hTmpDown

        if 'Up' in s:
                return hUp

        if 'Down' in s:
                return hDown


def envelop(tfile,togetClone, s) :

        hname=togetClone+'RF'+s
        hTmp=mergeCategory(tfile,hname)

#	if hTmp!=None: hTmp.Rebin(nRebinHT)

        h=hTmp.Clone()

        if hTmp==None: return

        for w in [ 'R','F','RF']:
                for var in [ 'Up','Down']:

                        hnameClone=togetClone+w+var

                        hTmp=mergeCategory(tfile,hnameClone)

#			if hTmp!=None: hTmp.Rebin(nRebinHT)

                        for iBin in range(1,h.GetNbinsX()+1):
                                c= h.GetBinContent(iBin)

                        ## take the maximum for the Up and Down                                                                                                                           
                                if 'Up' in s and hTmp.GetBinContent(iBin)>c:
                                        h.SetBinContent(iBin,hTmp.GetBinContent(iBin))

                        ## take the minimum for the Down                                                                                                                                  
                                if 'Down' in s and hTmp.GetBinContent(iBin)<c:
                                        h.SetBinContent(iBin,hTmp.GetBinContent(iBin))
        return h

## import Everything in ws TODO
def importPdfFromTH1(cat,mc,myBin,LikelihoodMapping,syst=None):

	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	if doSChannel and mc["name"]=="Hptb":masses=[800,900,1000,1500,2000,2500,3000]
	elif mc["name"]=="Hptb":masses=[400,500,650,800,1000,1500,2000,2500,3000]
##	if mc["name"]=="Hptb":masses=[400,500,650,800,900,1000,1500,2000,2500,3000]
##	if mc["name"]=="Hptb":masses=[180,200,220,250,300,350,400,500,650,800,1000,1500,2000,2500,3000]
	else: masses=[0]

	if syst == None: shifts=["x"]
	else: shifts=["Up","Down"]

	skip=False
	if syst != None:
		  skip=True

		  invert=False
		  regexpL = syst["proc"]
		  print '==================='
		  print '====',syst
		  print '====',syst["proc"],'  ',mc["name"]
		  print '==================='
		  for i,regexp in enumerate(regexpL):
			  if regexp != "" and regexp[0] == '!':
				  invert=True
				  regexp=regexp[1:]
			  match=re.search(regexp,mc["name"])
			  if (match  and not invert) or (not match and invert):
				  idx=i
				  skip=False
				  break

	if skip: return


	h=None
	for s in shifts:

		hTmpSum=None
		for m in masses:

#			print '==========> '
#			print '==========> cat["name"] ===> ', cat["name"]
#			print '==========> mc["name"]  ===> ', mc["name"]
#			print '==========> m  ===> ', m
#			print '==========> '

			if not "OneBOneFat1l" in cat["name"]:
				#	  print  '1/3 mass now is =', str(m)
##				if m==500 and "1500" in cat["name"]: continue
##				if m==500 and "2500" in cat["name"]: continue
##				if str(m) not in cat["name"]: continue

##				if mc["name"]=="Hptb":
##					toget=toget%m

				myTMPmass = "%d_"%m
				if m==500 and "1500" in cat["name"]: continue
				if m==500 and "2500" in cat["name"]: continue
				if myTMPmass not in cat["name"]: continue


			target = "pdf_" + mc["name"] +"_"+ cat["name"]
			if m >10 :
				target = "pdf_" + mc["name"] + "_" + cat["name"]
#				target = "pdf_" + mc["name"] +"_M-%d"%m+"_"+ cat["name"]

			if syst != None:
				target += "_" + syst["wsname"] + s

			for hname in mc["hist"]:
				toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

				hTmp=None

				if mc["name"]=="Hptb":
					toget=toget%m

                                if mc["name"]=="Hptb":
					if "OneBOneFat1l" in cat["name"]:

						stringToParce="_M-"+opts.kMass

                                                if stringToParce in toget:
                                                        myTMPmass = "%d_"%m
                                                        print 'here is the one I want', stringToParce, ' to check inside',toget
                                                else:
                                                        continue

#				print '==========> '
#				print '==========> toget  ===> ', toget
#				print '==========> hname ===> ', hname
#				print '==========> cat["name"] ===> ', cat["name"]
#				print '==========> mc["name"]  ===> ', mc["name"]
#				print '==========> '
#				print 'here is the one I want', opts.kMass

				togetNom = toget
                                togetSyst = ''
				if syst != None:
					togetSyst = toget + "_" + syst["name"]
					toget += "_" + syst["name"] + s

				if syst != None and "muRF" in syst["wsname"] and not doSChannel:
					if mc["name"]=="Hptb" or mc["name"]=="ttbar":
						hTmp = envelop(tfile,togetSyst,s)
#						toget = togetClone+'RF'+s

				elif syst != None:

                                        hTmp = SmoothAndMergeSyst(tfile,togetNom,togetSyst,s)
##                                      hTmp = MergeCategory(tfile,togetSyst+s)

				else:
                                        hTmp = mergeCategory(tfile,togetNom)

##				if hTmp!=None: hTmp.Rebin(nRebinHT)
				if hTmp!=None: print "<*> Reading Hist '"+toget+"'",hTmp.Integral()

				if hTmp == None:
					print "<*> Hist '"+toget+"' doesn't exist"
					raise IOError

				## foor QCD we do not apply the tauVeto and trigger
				if mc["name"]=="qcd" and hTmp:
					hTmp.Scale(0.93)
				## top and W scale factor (for now only in ttbar later also signal)
				## https://twiki.cern.ch/twiki/bin/view/CMS/JetTopTagging#13_TeV_working_points_CMSSW_8_0
				## https://twiki.cern.ch/twiki/bin/view/CMS/JetWtagging
				if ( mc["name"]=="ttbar" or mc["name"]=="top") and ("t0b" in toget or "t1b" in toget) and hTmp:
					hTmp.Scale(1.06)
					# uncertainties +0.08 -0.04
				if ( mc["name"]=="ttbar" or mc["name"]=="top") and ("wbb" in toget or "wbj" in toget) and hTmp:
					hTmp.Scale(1.11)
					## need to have an uncetainty of 0.08

### -- MC --
#			print 'xxxxxxxxx hname=',hname,' base=',base,'cat["dir"]',cat["dir"]
#####1L

				if applyLikelihoodRebinBin:
					hTmp = likelihoodBinning.applyMapping(LikelihoodMapping, hTmp)
				if hTmpSum==None:hTmpSum = hTmp
				else:
					hTmpSum.Add(hTmp)

				h=hTmpSum

		if h!= None: print "<*> Read Hist '"+toget+"'",h.Integral(),' nBin=',h.GetNbinsX(), 'underflow=',h.GetBinContent(0), 'overflow=',h.GetBinContent(h.GetNbinsX()+1),' entries=',h.GetEntries()

	#clean h

		if h: h.SetBinContent(0,0) ##underflow
		if h: h.SetBinContent(h.GetNbinsX()+1,0) #overflow

		if h: ##negative yield
			for b in range(1,h.GetNbinsX()+1):
				if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

		if h:
			if h.Integral() <= 0 :
				print "ERROR histogram", h.GetName(),"has null norm"
				raise IOError
				raise ValueError

	  #save RooDataHist

		h.Scale(opts.lumi)

		print "* Importing ",target

		for i in range(0,len(arglist_obs)):
			if cat["name"] in arglist_obs.at(i).GetName():
				al = arglist_obs.at(i)
##	  print "-> with var", al[0]

		roo_mc = ROOT.RooDataHist(target,target,ROOT.RooArgList(al),h)
		pdf_mc = roo_mc

#		hBIS = roo_mc.createHistogram("test", al)

		getattr(w,'import')(pdf_mc,ROOT.RooCmdArg())
		g.extend([h,roo_mc,pdf_mc])
#		w.writeToFile(fileTmp)

### WRITE HISTOGRAMS
                f = ROOT.TFile.Open(fileTmp,"update")
                h.SetName(target)
                h.Write()
                f.Write()
                f.Close()


		if syst==None and m< 10 : # not for signal,
			print "DEBUG calling stat with target",target
			histName=cat["name"]
			if histName not in statStore:
				statStore[histName] = { "hist":h.Clone(histName + "_STAT"),
							"mc":mc["name"],
							"cat":cat["name"],
							"target":target,
							"hist0":h.Clone(histName + "_STAT0")
							}
			else:
				statStore[histName]["hist"] . Add (h )
## ::: FIXME ::: TEMPORARY: the top is not laways the MAIN BKG
				if mc["name"] == 'qcd':  ## put TT and reference
					statStore[histName]["mc"]= mc["name"]
					statStore[histName]["target"]= target
					statStore[histName]["hist0"] . Delete()
					statStore[histName]["hist0"] = h.Clone(histName + "_STAT0")
	return



def importPdfFromTH1SumBKG(cat,mc,syst=None,do1Third=False):

	if do1Third:
		tfile = fInBin
	else:
		tfile = fIn
        tfile = cat["file"]
        if tfile == None:
                print "<*> File not exists"
                raise IOError
        base="ChargedHiggsTopBottom"
	if doSChannel and mc["name"]=="Hptb":masses=[800,900,1000,1500,2000,2500,3000]
	elif mc["name"]=="Hptb":masses=[400,500,650,800,1000,1500,2000,2500,3000]
##	if mc["name"]=="Hptb":masses=[180,200,220,250,300,350,400,500,650,800,1000,1500,2000,2500,3000]
        else: masses=[0]

        if syst == None: shifts=["x"]
        else: shifts=["Up","Down"]

        skip=False
        if syst != None:
                  skip=True

                  invert=False
                  regexpL = syst["proc"]
                  print '==================='
                  print '====',syst
                  print '====',syst["proc"],'  ',mc["name"]
                  print '==================='
                  for i,regexp in enumerate(regexpL):
                        if regexp != "" and regexp[0] == '!':
                                invert=True
                                regexp=regexp[1:]
                        match=re.search(regexp,mc["name"])
                        if (match  and not invert) or (not match and invert):
                                idx=i
                                skip=False
                                break

        if skip: return

####
####

	hSum=None
	hRef=None
	hSig=None

        for m in masses:
		for s in shifts:
			for hname in mc["hist"]:
#				print hname, 'mc["name"]=', mc["name"],	'hRef, hSum, hSig',hRef, hSum, hSig
				toget=base + "/" +cat["dir"] + "/" +  cat["var"] + "_" + hname

				if "OneBOneFat1l" in cat["name"]:
					if mc["name"]=="Hptb":
						toget=toget%m

						stringToParce="_M-"+opts.kMass
						
                                                if stringToParce in toget:
                                                        myTMPmass = "%d_"%m
                                                        print '1/3 here is the one I want', stringToParce, ' to check inside',toget
                                                else:
                                                        continue


				else:
					#	  print  '1/3 mass now is =', str(m)
					if m==500 and "1500" in cat["name"]: continue
					if m==500 and "2500" in cat["name"]: continue
					if str(m) not in cat["name"]: continue

					if mc["name"]=="Hptb":
						toget=toget%m

				if syst != None:
##					togetClone = toget + "_" + syst["name"]
					toget += "_" + syst["name"] + s

#				hTmp=tfile.Get(toget)
#				if hTmp!= None: hTmp.Rebin(nRebinHT)
				hTmp=mergeCategory(tfile,toget)

				if hTmp!= None: print "<*> 1/3 Reading Hist '"+toget+"'",hTmp.Integral(),' nBin=',hTmp.GetNbinsX()

				if hTmp == None:
					print "<*> Hist '"+toget+"' doesn't exist"
	                ## raise error only if is on the 2Third and for the syst central value
					if not do1Third:
						if syst == None:
							raise IOError


				if mc["name"]!="Hptb" and hTmp!=None:
					if hSum==None: hSum = hTmp
					else:
						#  print 'going to add = ',hTmp.GetName()
						hSum.Add(hTmp)

				if mc["name"]=="ttbar" and hTmp!=None: hRef=hTmp.Clone()
				if mc["name"]=="Hptb" and hTmp!= None:
					hSig=hTmp.Clone()

	if hRef: hRef.SetBinContent(0,0) ##underflow
	if hRef: hRef.SetBinContent(hRef.GetNbinsX()+1,0) #overflow

	if hSum: hSum.SetBinContent(0,0) ##underflow
	if hSum: hSum.SetBinContent(hSum.GetNbinsX()+1,0) #overflow

	if hSig: hSig.SetBinContent(0,0) ##underflow
	if hSig: hSig.SetBinContent(hSig.GetNbinsX()+1,0) #overflow


	if hRef:
		if hRef.Integral() <= 0 :
			print "ERROR histogram", hRef.GetName(),"has null norm"
			raise ValueError

	if hSum:
		if hSum.Integral() <= 0 :
			print "ERROR histogram", hSum.GetName(),"has null norm"
			raise ValueError

	if hSig:
		if hSig.Integral() <= 0 :
			print "ERROR histogram", hSig.GetName(),"has null norm"
			raise ValueError

	if hSum!=None:
		hSum.Scale(opts.lumi)
#		print "hSum.Integral",hSum.Integral()

	if hRef!=None:
		hRef.Scale(opts.lumi)
#		print "hRef.Integral",hRef.Integral()

	if hSig!=None:
		hSig.Scale(opts.lumi)
#		print "hSig.Integral",hSig.Integral()

	return hSum,hRef,hSig


###-----------------------------------------------------

for cat in catStore:

        hSumAll1Third=None
        hSumAll2Third=None
        hRef1Third=None
        hRef2Third=None
        hSig1Third=None
        hSig2Third=None

        myBin=1

        ## use the 1/3 of the sample

        for proc in mcStore:
                if skip(catStore[cat],mcStore[proc]): continue
                hSumTMP,hRef1Third,sig=importPdfFromTH1SumBKG(catStore[cat],mcStore[proc],None,True)
                mc=mcStore[proc]
                if mc["name"]=="Hptb":
                        hSig1Third=sig
		if hSumTMP!=None:
			if hSumAll1Third==None:
				hSumAll1Third=hSumTMP
			else:
				hSumAll1Third.Add(hSumTMP)


#	print '====================='
#	print '====================='
#	print '====================='
#	print 'I have hRef, hSum, hSig'
#	print "hSumAll1Third=",hSumAll1Third.Integral(),"hSig1Third=",hSig1Third.Integral()
#	print '====================='
#	print '====================='
#	print '====================='

        LikelihoodMapping = likelihoodBinning.createMapping(hSumAll1Third, hSig1Third)

	for proc in mcStore:
		if skip(catStore[cat],mcStore[proc]): continue
		for syst in systStore:
			print "->2/3 calling import pdf with cat=",cat,"proc=",proc,"syst=",syst
			print " * cat:",catStore[cat]
			print " * syst:", systStore[syst]
			if systStore[syst] == None or systStore[syst]["type"] == "shape" :
				importPdfFromTH1(catStore[cat],mcStore[proc],myBin,LikelihoodMapping,systStore[syst])


###-----------------------------------------------------

#import data
#for c in catStore:
	cat=catStore[cat]
	tfile = cat["file"]
	if tfile == None:
		print "<*> File not exists"
		raise IOError
	base="ChargedHiggsTopBottom"
	target = "data_obs_"+ cat["name"] 
	toget=base + "/" +cat["dir"] + "/" +  cat["var"]  +"_Data"

	h=mergeCategory(tfile,toget)

	if h == None:
		print "<*> Hist do not exists ",toget
		raise IOError

	if h != None:
                h = likelihoodBinning.applyMapping(LikelihoodMapping, h)


	if h != None:
		h.SetBinContent(0,0) ##underflow
		h.SetBinContent(h.GetNbinsX()+1,0) #overflow

		for b in range(1,h.GetNbinsX()+1): ##negative yield
			if h.GetBinContent(b) <0 : h.SetBinContent(b,0)

		if h.Integral() <= 0 :
			print "ERROR histogram", h.GetName(),"has null norm"
			raise ValueError


	for i in range(0,len(arglist_obs)):
		if cat["name"] in arglist_obs.at(i).GetName():
			al = arglist_obs.at(i)

	if h != None:
# comment the roofit part
#		roo_data= ROOT.RooDataHist("data_obs_%s"%c,"Mass",ROOT.RooArgList(al),h)
#		getattr(w,'import')(roo_data,ROOT.RooCmdArg()) ## import is a reserved word in python :(, the cmdArg is there to solve a disambiguate issue
#		g.extend([h,roo_data])
		f = ROOT.TFile.Open(fileTmp,"update")
		h.SetName(target)
		h.Write()
                f.Write()
                f.Close()


## import and write statistical uncertainties
#if doSyst: importStat()
#importStat()
'* autoMCStats 0'
datacard.write("* autoMCStats 0")

w.writeToFile(fileTmp,False)
print "--------------------" 
print "datacard=",datName
print "ws=",fileTmp
print " --- DONE --- "

