import ROOT
import json
import sys
import math
from optparse import OptionParser
import numpy as np
from array import array

sys.argv = [ '-b-']
ROOT.gROOT.SetBatch(True)
ROOT.gSystem.Load("libHiggsAnalysisCombinedLimit")
from HiggsAnalysis.CombinedLimit.DatacardParser import *
from HiggsAnalysis.CombinedLimit.ShapeTools     import *

#card = "DATACARD/AUG4/cms_vbshad_2021_2_BDTnoBnoMET_BB_all.txt"
#card = "DATACARD/AUG4/cms_vbshad_2021_2_BDTbtag_BBtag_all.txt"
card = "DATACARD/AUG4/cms_vbshad_2021_2_BDTwithMET_RMET_all.txt"
#card = "DATACARD/AUG4/cms_vbshad_2021_2_BDTwithMET_BMET_all.txt"

### DC reader options
parser = OptionParser()

(options, args) = parser.parse_args()
options.mass = 0
options.process = None
options.dataname  = "data_obs"
options.grep = []
options.all = False
options.noshape = False
options.stat = False
options.bin = True # fake that is a binary output, so that we parse shape lines
options.out = "tmp.root"
options.fileName = card
options.cexpr = False
options.fixpars = False
options.libs = []
options.verbose = 0
options.poisson = 0
options.nuisancesToExclude = []
options.noJMax = True
options.allowNoSignal = True
options.modelparams = []

options.optimizeTemplateBins = None

file = open(options.fileName, "r")
DC = parseCard(file, options)


if not DC.hasShapes: DC.hasShapes = True
MB = ShapeBuilder(DC, options)
if not options.noshape: MB.prepareAllShapes()

def getSystIndex(syst):

    i = -1
    for (lsyst,nofloat,pdf,pdfargs,errline) in DC.systs:
        i += 1
        if syst == lsyst: return i
    return -1

def getPrefitUncertainty(systs, procs,name):

    variance = 0.0 # store the total variances here which is the sum of all the nuisance variances
    sumOfProcs = 0.0 # sum of all the processes
    delta = 0.0


    for b in DC.bins: # loop over all bins

        #print b
        for p in DC.exp[b].iterkeys():

            if p not in procs: continue

            objC = MB.getShape(b,p)
            sumOfProcs += objC.Integral() # nominal yield
            #print "Add", objC.Integral()
    sum = 0
    totUnc = 0.0
    totSum = 0.0
    variance = 0

    for k, syst in enumerate(systs):

        # for each proc and bin, add systematics linearly: sum all the nominal and uncertainties
        unc_ = 0.0 # total uncertainty
        sum_ = 0.0 # nominal sum

        if 'stats' in name:

            for b in DC.bins: # loop over all bins

                for p in DC.exp[b].iterkeys():
                    if p not in procs: continue

                    objC = MB.getShape(b,p)
                    first =1
                    last= objC.GetNbinsX()
                    eA=array('d',[0.])
                    a = objC.IntegralAndError(first, last,eA)
           
                    unc_ += eA[0]
                    sum_ += a
            delta += unc_
            variance += unc_*unc_ 
            continue                   

        index_ = getSystIndex(syst)
        if index_ == -1: continue
        lsyst, nofloat, pdf, pdfargs, errline = DC.systs[index_]
        #print errline
        #print lsyst, index_
        for b in DC.bins: # loop over all bins

            for p in DC.exp[b].iterkeys(): # loop over all processes

                if p not in procs: continue

                # get yield for sum of procs assigned to this syst
                objC = MB.getShape(b,p)
                valC = objC.Integral() # nominal yield

                if errline[b][p] == 0: # if no syst assigned for this proc, return nominal
                    continue


                #print "consider", lsyst, "for", p,b, sum_

                if "shape" in pdf and MB.isShapeSystematic(b,p,lsyst):
                    vals = []

                    systShapeName = lsyst
                    if (lsyst,b,p) in DC.systematicsShapeMap.keys(): systShapeName = DC.systematicsShapeMap[(lsyst,b,p)]

                    objU,objD,objC = MB.getShape(b,p,systShapeName+"Up"), MB.getShape(b,p,systShapeName+"Down"), MB.getShape(b,p)

                    if objC.InheritsFrom("TH1"): valU,valD,valC =  objU.Integral(), objD.Integral(), objC.Integral()
                    elif objC.InheritsFrom("RooDataHist"): valU,valD,valC =  objU.sumEntries(), objD.sumEntries(), objC.sumEntries()

                    if valC!=0: # else NaN

                        unc = max(abs(valU-valC), abs(valD-valC)) # this is the delta (maximum uncertainty)
                        #unc_ += (unc + valC) # this is the delta + nominal   
                        unc_ += (unc) # this is the delta + nominal   
                        sum_ += valC
                else:

                    if type(errline[b][p]) == list: unc = 1.0 + max(abs(errline[b][p][0]-1), abs(errline[b][p][1]-1))
                    else: unc = 1.0 + abs(errline[b][p]-1)
                    #unc_ += unc*valC # this is the delta + nominal
                    unc_ += unc*valC-valC # this is the delta + nominal
                    sum_ += valC

        # add this uncertainty in quadrature with the rest
        #print "Intermediate", lsyst, unc_, sum_, unc_+sum_, (unc_+sum_)/sum_
        #variance += (unc_-sum_)*(unc_-sum_)
        delta += unc_
        variance += unc_*unc_

    if sumOfProcs == 0 or delta == 0: return "-"
    else:
        ret = (100.*(delta/sumOfProcs))
        ret = (100.*(math.sqrt(variance)/sumOfProcs))
        if ret < 0.1: return "$<$ 0.1"
        else: return "%.1f" % ret


systGroups = {

    #"jets4"              : ["CMS_scale_j"],
    "jets4"              : ["CMS_jes_FlavorQCD", "CMS_jes_RelativeBal", "CMS_jes_HF", "CMS_jes_BBEC1", "CMS_jes_EC2", "CMS_jes_Absolute","CMS_jes_BBEC1_2016","CMS_jes_BBEC1_2017","CMS_jes_BBEC1_2018","CMS_jes_RelativeSample_2016","CMS_jes_RelativeSample_2017","CMS_jes_RelativeSample_2018","CMS_jes_EC2_2016","CMS_jes_EC2_2017","CMS_jes_EC2_2018","CMS_jes_HF_2016","CMS_jes_HF_2017","CMS_jes_HF_2018","CMS_jes_Absolute_2016","CMS_jes_Absolute_2017","CMS_jes_Absolute_2018"],
    "jets8"              : ["CMS_scale_AK8j"],
    #"jets"              : ["jes_FlavorQCD","jes_RelativeBal","jes_HF","jes_BBEC1","jes_EC2","jes_Absolute"],
    "lumi"              : ["lumi"], #   
    "pileup"            : ["CMS_pileUp"], # 
    "prefire"           : ["CMS_L1Prefire"],
    "resolvedV"         : ["CMS_eff_ResolvedDiscr"],
    "resolvedV_m"       : ["CMS_eff_ResolvedMass"],
    "boostedV"          : ["CMS_eff_Xqq","CMS_eff_Xbb", "CMS_eff_Xcc"],
    "boostedV_m"        : ["CMS_scale_PNetMass"],
    "th_xsec"           : ["QCDScale_ttbar"],
    "th_acc"            : ["CMS_scaleR_tt","CMS_scaleF_tt","CMS_scaleR_Zj","CMS_scaleF_Zj","CMS_scaleR_Wj","CMS_scaleF_Wj","CMS_scaleR_VQQ","CMS_scaleF_VQQ"],
    "trigger"           : ["CMS_eff_trigger"],
    "lepton"            : ["CMS_eff_l"],  
#    "btag"              : ['CMS_btag_CFERR1','CMS_btag_HF','CMS_btag_HFSTAT1','CMS_btag_HFSTAT2','CMS_btag_LFSTAT1','CMS_btag_LFSTAT2','CMS_btag_LF'],
#    "btag"              : ['CMS_btag_CFERR1'],
    "btag"              : ['CMS_btag_HF','CMS_btag_HFSTAT1','CMS_btag_HFSTAT2','CMS_btag_LFSTAT1','CMS_btag_LFSTAT2','CMS_btag_LF'],
    "met"               : ["CMS_scale_uncluster"],
    "qcd_s"               : ["CMS_QCDnonclosure_s_BB","CMS_QCDnonclosure_s_BBtag"],
    "qcd_n"               : ["CMS_QCDnonclosure_n_BB","CMS_QCDnonclosure_n_BBtag"],
    "stats"               : ["CMS_scale_j"]
}

systLabels = {

    "jets4"          : "AK4 JES and JER & $\\checkmark$",
    "jets8"          : "AK8 JES and JER & $\\checkmark$",
    "lumi"          : "Luminosity &",
    "pileup"        : "Pileup & $\\checkmark$",
    "prefire"       : "L1-prefiring inefficiency & $\\checkmark$",
    "trigger"       : "Trigger efficiency &",
    "lepton"        : "Lepton veto efficiency &",
#    "boostedV"      : "textsc{ParticleNet} discriminator scale &",
#    "boostedV_m"    : "textsc{ParticleNet} mass scale &",
    "boostedV"      : "ParticleNet discriminator scale &",
    "boostedV_m"    : "ParticleNet mass scale &",
    "resolvedV"     : "Resolved V discriminator scale &",
    "th_xsec"       : "Cross section (scale, PDF) &",
    "th_acc"        : "Acceptance (scale, PDF) & $\\checkmark$",
    "btag"          : "b-jet identification & $\\checkmark$",
    "met"           : "Unclustered \\MET energy scale & $\\checkmark$",
    "qcd_s"           : "QCD shape & $\\checkmark$",
    #"qcd_n"           : "QCD normalization &",
    "stats"           : "Limited MC sample size & $\\checkmark$ ",
}


procGroups = {

    #"Zjets"     : ["Zinv"],
    #"Wjets"     : ["Winv"],
    "VJets"	: ["Winv","Zinv","VQQ"],
    "EWKV"      : ["EWKV"],
    "qcd"       : ["QCD"],
    "ttbar"     : ["ttbar"],
    "VVEWK"     : ["VVEWK"],
    "VVQCD"     : ["VVOther"],
}

procLabels = {

    #"Zjets"    :"Zjets" ,
    #"Wjets"    :"Wjets",
    "VJets"    : "VJets",
    "ttbar"    : "\\ttbar",
    "qcd"      : "QCD",
    "EWKV"     : "EW V",
    "VVEWK"    : "EW VV",
    "VVQCD"    : "QCD VV"
}


systGroups_ordered = ["trigger","lepton","pileup","prefire","lumi","jets4","jets8","btag","met","resolvedV","boostedV","boostedV_m","qcd_s","th_xsec","th_acc","stats"]
#systGroups_ordered = ["trigger","lepton","pileup","prefire","lumi","jets4","jets8","btag","met","resolvedV","qcd_s","th_xsec","th_acc","stats"]
#systGroups_ordered = ["lumi","lepton","trigger","pileup","jets", "bjets", "met", "th_acc", "th_xsec", "tt_mass", "sdmass", "nsub", "qcdP0", "qcdP1"]
procGroups_ordered = ["VVEWK", "VVQCD", "qcd", "ttbar", "VJets","EWKV"]
#procGroups_ordered = ["ttbar"]


table_latex = "\\begin{tabular}{ l c c c c c c c c c}\n"
table_latex += "Source & Shape "
for p in procGroups_ordered: table_latex += " & " + procLabels[p]
table_latex += "\\\ \n\\hline\n"

for systGroup in systGroups_ordered:

    table_latex += systLabels[systGroup]
    for procGroup in procGroups_ordered:

        unc = getPrefitUncertainty(systGroups[systGroup], procGroups[procGroup],systGroup)
        table_latex += " & " + unc

    table_latex += " \\\ \n"

table_latex += "\\hline\n\\end{tabular} "


print table_latex
