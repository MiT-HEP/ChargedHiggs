#!env python

import re
import os,sys
import math
from optparse import OptionParser
from subprocess import call, check_output

parser = OptionParser(usage = "usage\n Example: $python python/mcDatabaseNano.py -i dat/config.nano.dat");

parser.add_option("-i","--dat",dest="dat",type="string",help="Input dat to scout. Either this is '' or dataset is ''",default="");
parser.add_option("-x","--xsec",dest="xsec",type="float",help="Use external cross-section",default=-1);

parser.add_option("-d","--dataset",dest="dataset",type="string",help="dataset spec",default="");
parser.add_option("-l","--label",dest="label",type="string",help="MC label",default="DYamcatnlo");
parser.add_option("-f","--file",dest="file",type="string",help="mc_database file name",default="dat/mc_database.txt");
parser.add_option("-p","--pileup",dest="pu",type="string",help="Pileup File. All together! Keep them together.")
parser.add_option("","--precise",action='store_true',help="Use Kahan accumulator (requires combine).",default=False)
parser.add_option("","--noruntree",action='store_true',help="Don't use run tree",default=False)

(opts,args)=parser.parse_args()

from ParseDat import *
useRunTree=not opts.noruntree

if not useRunTree:
    print "I will loop over the Event tree"
else:
    print "I will use the Run Tree to speed up things"

print "[DEBUG] useRunTree=",useRunTree

#EOS = "/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select"
#EOS = "/usr/bin/eos"
#if '/eos/user' in opts.eos: EOS += " root://eosuser"

if opts.dat != "":
    if opts.dataset != "":
        print "* ignoring dataset specification"
    cfg = ParseDat(opts.dat)

    print "* reading already parsed mcdb"

    try:
        mcdb= ReadMCDB(cfg['MCDB'])
    except KeyError:
        mcdb={}

    for f in cfg['Files']:
        if f=="": continue

        print "DEBUG, parsing file",f
        if '.root' in f : continue

        dirs=f.split('/')

        if '/store/mc' in f: 
            ### this is a nanoaod path central
            label=dirs[3] ## store mc RunII DY 
        elif '/store/user' in f:
            ###
            for x in dirs:
                if 'powheg' in x or \
                   'madgraph' in x or \
                   '13TeV' in x or \
                   'herwig' in x or \
                   'pythia' in x : label=x
        elif '/store/group/phys_higgs/cmshmm/amarini' in f:
            label=dirs[6] # default ?
#            raise ValueError("to check label finding")
        elif '/store/group/phys_higgs/HiggsExo/dalfonso/NANOV8' in f:
            label=dirs[8] # default ?
#            raise ValueError("to check label finding")
        elif '/eos/user/' in f: 
            ### this is a nanoaod path
            label=dirs[3] ## store mc RunII DY 
            raise ValueError("to check label finding")
        if 'NANOAOD' in f and (dirs[-1] == 'NANOAODSIM' or dirs[-1] == 'NANOAOD'): 
            ## this is a dataset
            label=dirs[1]

        print "DEBUG, label is",label
        ## find label: directory not containing only numbers, hyphens, underscore, or empty
        
        if label == 'Tau': continue # exclude data
        if label == 'MET': continue # exclude data
        if label == 'JetHT': continue # exclude data
        if label == 'SingleMuon': continue # exclude data
        if label == 'SingleElectron': continue # exclude data
        if label == 'DoubleMuon': continue # exclude data
        if label == 'DoubleElectron': continue # exclude data
        if label == 'BTagCSV': continue # exclude data

        cmd = "python %s -d '%s' -x %f -l '%s' -f %s -p %s"%(sys.argv[0],f,opts.xsec,label,cfg['MCDB'],cfg['pileup'])
        if opts.precise: cmd+=" --precise"
        if opts.noruntree: cmd+=" --noruntree"

        if label in mcdb:
            print "* label", "'"+label+"'", "already parsed in mcdb ("+cfg['MCDB']+"). Cmd was:"
            print "  ",cmd
            continue
        elif label != '':
            print "going to execute",cmd
            call(cmd,shell=True)
    exit(0)


if '/store/' in opts.dataset or '/eos/user' in opts.dataset: 
    print "DEBUG, Calling FindEOS"
    fileList =  FindEOS(opts.dataset)
elif 'NANOAOD' in opts.dataset: 
    print "DEBUG, Calling FindDataset"
    fileList =  FindDataset(opts.dataset)
else:
    raise ValueError("not supported nano spec: "+opts.dataset)

print "DEBUG, DATASET is",opts.dataset,"FILE LIST",fileList 

sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()

if opts.precise:
    print "-> Loading combine library for accumulators" #FIXME -> probably accumalators are not exported
    r.gSystem.Load("libHiggsAnalysisCombinedLimit.so")

## TODO count mcWeights, nEntries
n=0
myTmp= r.TFile.Open("/tmp/" + os.environ["USER"] + "/mytmp.root","RECREATE")
myTmp.cd()
sum=r.TH1D("SumWeights","Sum of mcWeights",1,0,2)
sum2=r.TH1D("SumWeights","Sum of mcWeights^2",1,0,2)
scales=r.TH1D("SumWeights_Scales","Sum of mcWeights",10,0,10)
nPdfs=100
pdfs=r.TH1D("SumWeights_Pdfs","Sum of mcWeights",nPdfs,0,nPdfs)

nAQGCs=len(aqgc_names) ## in ParseDat
aqgc=r.TH1D("SumWeights_aQGCs","Sum of mcWeights",nAQGCs,0,nAQGCs)

if opts.precise:
    sum=r.KahanAccumulator(double)(0.)

fOutput=r.TFile.Open(opts.pu,"UPDATE")

nBins = 1000
xMin = -0.5
xMax = 1000-0.5

simple = True ## NOT RD

## RUN DEPENDENT NEVER TESTED
if simple:  ## 1D not Run dependent
    name = "PU-%s"%opts.label
    h = fOutput.Get(name)
    if h != None : print "Overwriting",name
    puHist=[r.TH1F(name,"PU Distribution of %s"%opts.label,nBins,xMin,xMax)]
else:
    raise ValueError("Copied, not tested")
    ## RD
    puHist=[]
    for idx in range(0,len(opts.run.split(',')) -1):
        name = "RD-%s_%d_%d"%( opts.label, int(opts.run.split(',')[idx]), int(opts.run.split(',')[idx+1]) )
        h=fOutput.Get(name)
        if  h != None: print "Overwriting",name
        puHist.append(  r.TH1F(name,"RD PU Distribution of %s"%opts.label,nBins,xMin,xMax)  )


if opts.precise and useRunTree:
    raise ValueError("Cannot be precise with run tree")

for idx,fName in enumerate(fileList):
    print "processing file:",idx,"/",len(fileList)," : ", fName
    fROOT = r.TFile.Open( fName )
    if fROOT == None:
        print "Unable to open, trying AAA: ",re.sub('eoscms','xrootd-cms.infn.it',fName)
        fROOT = r.TFile.Open(re.sub('eoscms','xrootd-cms.infn.it',fName))

    t = fROOT.Get("Events")
    runs= fROOT.Get("Runs")
    runs.GetEntry(0)

    if (runs.nLHEPdfSumw != nPdfs) : 
        print "[WARNING] ",runs.nLHEPdfSumw," pdf weights in the NANOAOD. Using only",nPdfs, "(must be <)"

    print "[DEBUG] useRunTree=",useRunTree
    if not useRunTree:
        if opts.precise:
            t.Draw("Generator_weight","","goff") ##>>+ doesn't work
            for i in range(0,t.GetSelectedRows()) :
                sum += t.GetV1()[i]
        else:
            mysum=r.TH1D("mysum","Sum of mcWeights",1,0,2)
            t.Draw("1>>mysum","Generator_weight","goff") ##>>+ doesn't work
            sum.Add(mysum)

        mysum2=r.TH1D("mysum2","Sum of mcWeights^2",1,0,2)
        t.Draw("1>>mysum2","Generator_weight*Generator_weight","goff") ##>>+ doesn't work
        sum2.Add(mysum2)

        for i in range(0,9):
            myscale=r.TH1D("myscale","Sum of weights",1,0,2)
            t.Draw("1>>myscale","LHEScaleWeight[%d]*Generator_weight"%i,"goff") ##>>+ doesn't work
            scales.Fill(i, myscale.GetBinContent(1))
            myscale.Delete()

        for i in range(0,nPdfs):
            mypdf=r.TH1D("mypdf","Sum of weights",1,0,2)
            t.Draw("1>>mypdf","LHEPdfSumw[%d]*Generator_weight"%i,"goff") ##>>+ doesn't work
            pdfs.Fill(i, mypdf.GetBinContent(1))
            mypdf.Delete()

    else:
        sum.Fill(1,runs.genEventSumw)
        sum2.Fill(1,runs.genEventSumw2)

        ## scales
        for i in range(0,9):
            scales.Fill(i, runs.LHEScaleSumw[i] * runs.genEventSumw)
        ##pdfs
        for i in range(0,nPdfs):
            pdfs.Fill(i, runs.LHEPdfSumw[i] * runs.genEventSumw)

        ##aqgc
        if runs.GetLeaf("sum_"+aqgc_names[0]):
            print "Using RunTree for AQGCs","aqgc.Fill(i, runs.sum_"+aqgc_names[0]+" )"
            for i in range(0,nAQGCs):
                eval("aqgc.Fill(i, runs.sum_"+aqgc_names[i]+" )")

    n += t.GetEntries()

    if simple: #1D
        mypu=r.TH1F("mypu","mypu",nBins,xMin,xMax)
        t.Draw("Pileup_nTrueInt>>mypu","Generator_weight") ##>>+ doesn't work
        puHist[0].Add(mypu)
        mypu.Delete()
    else: ##RD
        for idx in range(0,len(opts.run.split(','))-1):
            mypu=r.TH1F("mypu","mypu",nBins,xMin,xMax)
            selection = "Generator_weight*(%d <=runNum && runNum<%d)" % ( int(opts.run.split(',')[idx]) , int(opts.run.split(',')[idx+1]) )
            t.Draw("Pileup_nTrueInt>>mypu",selection) ##>>+ doesn't work
            puHist[idx].Add(mypu)
            mypu.Delete()
    fROOT.Close()


print "---------------------------------------------"
print "SumWeights = ", sum.GetBinContent(1) if not opts.precise else sum()
print "Tot Entries = ", n
print "SumWeights2 = ", sum2.GetBinContent(1)
if opts.precise:
    print "Effective Events = ", sum()**2 / sum2.GetBinContent(1), "Ratio=",(sum())**2 / sum2.GetBinContent(1) /n
else:
    print "Effective Events = ", (sum.GetBinContent(1))**2 / sum2.GetBinContent(1), "Ratio=",(sum.GetBinContent(1))**2 / sum2.GetBinContent(1) /n
print "---------------------------------------------"

### LABEL dir Entries xSec
f = open (opts.file,"a")
print>>f, opts.label,opts.dataset, sum.GetBinContent(1) if not opts.precise else sum() ,

if opts.xsec >0 : 
    print>>f, opts.xsec,
else: 
    ## AUTOMAGIC CROSS SECTIONS -- NO INTERNAL XS :(
    xsec=0
    ## DY
    ## GenXsecAnalyzer
    if 'DYJetsToLL_M-50_HT-0to70'       in opts.label: xsec=5317.864951
    elif 'DYJetsToLL_M-50_HT-70to100'   in opts.label: xsec=206.184
    elif 'DYJetsToLL_M-50_HT-100to200'  in opts.label: xsec=181.302
    elif 'DYJetsToLL_M-50_HT-200to400'  in opts.label: xsec=50.4177
    elif 'DYJetsToLL_M-50_HT-400to600'  in opts.label: xsec=6.98394
    elif 'DYJetsToLL_M-50_HT-600to800'  in opts.label: xsec=1.68141
    elif 'DYJetsToLL_M-50_HT-800to1200' in opts.label: xsec=0.775392
    elif 'DYJetsToLL_M-50_HT-1200to2500' in opts.label: xsec=0.186222
    elif 'DYJetsToLL_M-50_HT-2500toInf' in opts.label: xsec=0.004385
    elif 'DYJetsToLL_M-10to50' in opts.label:xsec=18610
    ## amc@nlo sum=5991.7
    elif 'DYToLL_0J_13TeV' in opts.label: xsec=4754
    elif 'DYToLL_1J_13TeV' in opts.label: xsec=888.9 
    elif 'DYToLL_2J_13TeV' in opts.label: xsec=348.8 
    elif 'DYJetsToLL_M-105To160' in opts.label: xsec=46.958 #NNLO (QCD)+NLO (EW)
    elif 'DYJetsToLL_0J' in opts.label: xsec = 4620.70 #xsec=5313.0 ## from XSECDB + QCDNNL OF amcatnlo
    elif 'DYJetsToLL_1J' in opts.label: xsec = 831.26 #xsec=955.8 ## from XSECDB
    elif 'DYJetsToLL_2J' in opts.label: xsec = 313.44 #xsec=360.4 ## from XSECDB
    elif 'DYJetsToLL' in opts.label or 'DY' in opts.label: xsec=5765.4 # nnpdf 3.0
    #elif 'DYJets' in opts.label or 'DY' in opts.label: xsec=6025.
    ## SIG
    elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-200' in opts.label: xsec=1
    elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-300' in opts.label: xsec=1
    elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-350' in opts.label: xsec=1
    elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-400' in opts.label: xsec=1
    elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-500' in opts.label: xsec=1
    elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-220' in opts.label: xsec=1
    elif 'ChargedHiggs_HplusTB_HplusToTauNu_M-250' in opts.label: xsec=1
    ## SIG VBS
    elif 'ZNuNuWPMJJjj_EWK_LO' in opts.label: xsec=1
    elif 'ZNuNuWPMJJjj_QCD_LO' in opts.label: xsec=1
    elif 'ZNuNuWPMJJjj_EWK_QCD_LO' in opts.label: xsec=1
    elif 'ZBBWPMJJjj_EWK_LO' in opts.label: xsec=1
    elif 'ZBBWPMJJjj_QCD_LO' in opts.label: xsec=1
    elif 'ZBBWPMJJjj_EWK_QCD_LO' in opts.label: xsec=1
    elif 'ZJJZJJjj_EWK_LO' in opts.label: xsec=1
    elif 'ZJJZJJjj_EWK_QCD_LO' in opts.label: xsec=1
    elif 'ZJJZJJjj_QCD_LO' in opts.label: xsec=1
    elif 'WPMJJWPMJJjj_EWK_LO' in opts.label: xsec=1
    elif 'WPMJJWPMJJjj_QCD_LO' in opts.label: xsec=1
    elif 'WPMJJWPMJJjj_EWK_QCD_LO' in opts.label: xsec=1
    elif 'WPJJWMJJjj_EWK_LO' in opts.label: xsec=1
    elif 'WPJJWMJJjj_QCD_LO' in opts.label: xsec=1
    elif 'WPJJWMJJjj_EWK_QCD_LO' in opts.label: xsec=1
    elif 'WPJJZJJjj_EWK_LO' in opts.label: xsec=1
    elif 'WPJJZJJjj_EWK_LO' in opts.label: xsec=1
    elif 'WPJJZJJjj_EWK_QCD_LO' in opts.label: xsec=1

    ## SIG ChargedHiggsToBoson
    ## cross section 1/pb normalization scaled for charge coniugation (x2) and BR(WW->jjjj) 45
    elif 'DoublyChargedHiggsGMmodel_HWW_M1000_13TeV-madgraph' in opts.label: xsec=0.225
    elif 'DoublyChargedHiggsGMmodel_HWW_M1500_13TeV-madgraph' in opts.label: xsec=0.225
    elif 'DoublyChargedHiggsGMmodel_HWW_M2000_13TeV-madgraph' in opts.label: xsec=0.225
    elif 'DoublyChargedHiggsGMmodel_HWW_M3000_13TeV-madgraph' in opts.label: xsec=0.225
    ## cross section 1/pb normalization scaled for charge coniugation (x2) and BR(WW->jj e/mu/t) 22
    elif 'DoublyChargedHiggsGMmodel_HWW_semilep_M1000_13TeV-madgraph' in opts.label: xsec=0.11
    elif 'DoublyChargedHiggsGMmodel_HWW_semilep_M2000_13TeV-madgraph' in opts.label: xsec=0.11
    ## cross section 1/pb normalization scaled for charge coniugation (x2) and BR(Z->nn Wjj) 13.4
    elif 'SinglyChargedHiggsGMmodel_HWZ_Znn_M1000_13TeV-madgraph' in opts.label: xsec=0.067
    elif 'SinglyChargedHiggsGMmodel_HWZ_Znn_M1500_13TeV-madgraph' in opts.label: xsec=0.067
    elif 'SinglyChargedHiggsGMmodel_HWZ_Znn_M2000_13TeV-madgraph' in opts.label: xsec=0.067
    elif 'SinglyChargedHiggsGMmodel_HWZ_Znn_M3000_13TeV-madgraph' in opts.label: xsec=0.067
    ## cross section 1/pb normalization scaled for charge coniugation (x2) and BR(Z->bb Wjj) 10.
    elif 'SinglyChargedHiggsGMmodel_HWZ_Zbb_M1000_13TeV-madgraph' in opts.label: xsec=0.05
    elif 'SinglyChargedHiggsGMmodel_HWZ_Zbb_M1500_13TeV-madgraph' in opts.label: xsec=0.05
    elif 'SinglyChargedHiggsGMmodel_HWZ_Zbb_M2000_13TeV-madgraph' in opts.label: xsec=0.05
    elif 'SinglyChargedHiggsGMmodel_HWZ_Zbb_M3000_13TeV-madgraph' in opts.label: xsec=0.05
    ### QCD from https://cms-gen-dev.cern.ch/xsdb/
    elif 'QCD_HT50to100_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8'    in opts.label: xsec = 185300000.0
    elif 'QCD_HT100to200_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8'   in opts.label: xsec = 23590000.0
    elif 'QCD_HT200to300_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8'   in opts.label: xsec = 1551000.0
    elif 'QCD_HT300to500_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8'   in opts.label: xsec = 323400.0
    elif 'QCD_HT500to700_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8'   in opts.label: xsec = 30140.0
    elif 'QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8'  in opts.label: xsec = 6344.0
    elif 'QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia'  in opts.label: xsec = 1092.0
    elif 'QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8' in opts.label: xsec = 99.76
    elif 'QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8'  in opts.label: xsec = 20.35
    ## from https://cms-gen-dev.cern.ch/xsdb/  SUS-RunIIFall17MiniAODv2 campaign (LO from sxdb * 1.23)
    elif 'ZJetsToNuNu_HT-100To200'   in opts.label: xsec = 372.444
    elif 'ZJetsToNuNu_HT-200To400'   in opts.label: xsec = 113.8857
    elif 'ZJetsToNuNu_HT-400To600'   in opts.label: xsec = 16.2114
    elif 'ZJetsToNuNu_HT-600To800'   in opts.label: xsec = 4.00611
    elif 'ZJetsToNuNu_HT-800To1200'  in opts.label: xsec = 1.8327
    elif 'ZJetsToNuNu_HT-1200To2500' in opts.label: xsec = 0.420537
    elif 'ZJetsToNuNu_HT-2500ToInf'  in opts.label: xsec = 0.00632958
    ## from https://cms-gen-dev.cern.ch/xsdb/
    elif 'EWKWPlus2Jets'     in opts.label: xsec = 39.05
    elif 'EWKWMinus2Jets'    in opts.label: xsec = 32.05
    elif 'EWKZ2Jets_ZToNuNu' in opts.label: xsec = 10.66
    elif 'EWKZ2Jets_ZToLL'   in opts.label: xsec = 6.215
    ### H-> mumu
    elif 'HToMuMu' in opts.label: xsec=1
    elif 'HToZG' in opts.label: xsec=1
    ##TTX
    elif 'TTZToQQ' in opts.label: xsec=0.5297
    elif 'TTZToLLNuNu' in opts.label: xsec=0.2439
    elif 'TTWJetsToQQ' in opts.label:xsec=0.4377
    elif 'TTWJetsToLNu' in opts.label:xsec=0.2161
    elif 'ttHJetToNonbb_M125'  in opts.label:xsec=0.5066
    elif 'ttHJetTobb_M125'  in opts.label:xsec=0.5023
    elif 'TTGJets' in opts.label:xsec=3.697
    elif 'TTTT' in opts.label:xsec=0.009103
    ### TT
    elif 'TTTo2L2Nu' in opts.label: xsec=88.20
    elif 'TTToSemiLeptonic' in opts.label: xsec=365.3452
    elif 'TTToHadronic' in opts.label: xsec=377.9607
    elif 'TT_Mtt-700to1000_TuneCP5' in opts.label: xsec=80.85
    elif 'TT_Mtt-1000toInf_TuneCP5' in opts.label: xsec=19.81
    elif 'TT' in opts.label: xsec=831
    ## ST
    elif 'ST_s-channel_4f_InclusiveDecays' in opts.label: xsec=10.32
    elif 'ST_s-channel_4f_leptonDecays' in opts.label: xsec=3.365
    elif 'ST_t-channel_antitop_4f' in opts.label: xsec=80.95
    elif 'ST_t-channel_top_4f' in opts.label: xsec=136.02
    elif 'ST_tW_antitop_5f' in opts.label: xsec=30.09
    elif 'ST_tW_top_5f' in opts.label: xsec=30.11
    ## from https://cms-gen-dev.cern.ch/xsdb/ (LO from sxdb * 1.21)
    elif 'WJetsToLNu_HT-70To100'    in opts.label: xsec=1563.32
    elif 'WJetsToLNu_HT-100To200'   in opts.label: xsec=1687.95
    elif 'WJetsToLNu_HT-200To400'   in opts.label: xsec=493.559
    elif 'WJetsToLNu_HT-400To600'   in opts.label: xsec=69.5508
    elif 'WJetsToLNu_HT-600To800'   in opts.label: xsec=15.5727
    elif 'WJetsToLNu_HT-800To1200'  in opts.label: xsec=6.49286
    elif 'WJetsToLNu_HT-1200To2500' in opts.label: xsec=1.29954
    elif 'WJetsToLNu_HT-2500ToInf'  in opts.label: xsec=0.00968121
#    ### WJETS 20508.9 * 3
#    elif 'WJetsToLNu_HT-0To70'      in opts.label: xsec=57641.658055
##    elif 'WJets' in opts.label: xsec=61526.7
    elif 'W0' in opts.label: xsec=34273.632815
    elif 'W1' in opts.label: xsec=18455.979619
    elif 'W2' in opts.label: xsec=6035.904629
    elif 'W3' in opts.label: xsec=1821.46719
    elif 'W4' in opts.label: xsec=939.684984
    ##VV
    elif 'WWTo2L2Nu' in opts.label: xsec=12.46
    elif 'WZTo1L1Nu2Q' in opts.label: xsec=10.71
    elif 'WZTo1L3Nu' in opts.label: xsec=3.033
    elif 'WZTo2L2Q' in opts.label: xsec=5.595
    elif 'WZTo3LNu' in opts.label: xsec=4.42965
    elif 'ZZTo2L2Nu' in opts.label: xsec=0.5644
    elif 'ZZTo2L2Q' in opts.label: xsec=3.22
    elif 'ZZTo4L' in opts.label: xsec=1.256
    elif 'WWToLNuQQ' in opts.label: xsec=45.68
    elif 'WWTo1L1NuQQ' in opts.label: xsec=45.68
    #VVV from https://cms-gen-dev.cern.ch/xsdb/
    elif 'WWW' in opts.label: xsec=0.2158
    elif 'WWZ' in opts.label: xsec=0.1707
    elif 'WZZ' in opts.label: xsec=0.05709
    elif 'ZZZ' in opts.label: xsec=0.01476
    ### EWK EWK_LLJJ_MLL-50_MJJ-120
    elif 'EWK_LLJJ' in opts.label: xsec=1.630
    ##
    #elif '' in opts.label: xsec=
    ## INTERNAL
    print>>f,  xsec,

    ## 
    if scales.GetBinContent(1) > 0:
        print>>f, "SCALES", #r1f2=0,r1f5,r2f1,r2f2,r5f1,r5f5
        print>>f, sum.GetBinContent(1)/scales.GetBinContent(5 + 1)  , ## offset by one in filling
        print>>f, sum.GetBinContent(1)/scales.GetBinContent(3 + 1)  ,
        print>>f, sum.GetBinContent(1)/scales.GetBinContent(7 + 1)  ,
        print>>f, sum.GetBinContent(1)/scales.GetBinContent(8 + 1)  ,
        print>>f, sum.GetBinContent(1)/scales.GetBinContent(1 + 1)  ,
        print>>f, sum.GetBinContent(1)/scales.GetBinContent(0 + 1)  ,
    
    if pdfs.GetBinContent(1) > 0:
        print>>f, "PDFS",
        for i in range(0,100): ## this number is also in python/ParseDat
            print>>f, sum.GetBinContent(1)/pdfs.GetBinContent(i+1), 

    if aqgc.GetBinContent(1) >0:
        print >>f,"AQGC",
        ##aqgc
        for i in range(0,nAQGCs):
            print>>f, sum.GetBinContent(1)/aqgc.GetBinContent(i+1), 

    ## INTERNAL
    print >>f

print "---------------------------------------------"

fOutput.cd();
for h in puHist:
    h.Write("",r.TObject.kOverwrite)


