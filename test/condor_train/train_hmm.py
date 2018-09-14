########## USE TMVA FOR TRAINING ########
import sys,os
from optparse import OptionParser

parser=OptionParser()
parser.add_option("-m","--multiclass",action='store_true',help="Switch on Multiclass mode", default=False)
parser.add_option("-o","--outname",help="Outputname", default="output.root")
parser.add_option("-d","--dataset",help="Dataset name", default="dataset")
parser.add_option("-v","--varscheme",type='int',help="Variable scheme", default=0)
parser.add_option("-t","--trainscheme",type='int',help="Variable scheme", default=0)

opts,args=parser.parse_args()

print "-> running train_hmm with options:",' '.join(sys.argv)

import ROOT
ROOT.TMVA.Tools.Instance()
ROOT.gROOT.SetBatch()

multiclass=opts.multiclass
if multiclass: print "-> Using multiclassification"

## BOOK Tree
files=["root://eoscms///store/user/amarini/Hmumu/fwk/2018_08_10_SyncTree//ChHiggs_%d.root"%idx for idx in range(0,300) if idx !=1 and idx !=56]
files.extend(["root://eoscms///store/user/amarini/Hmumu/fwk/2018_08_23_SyncHH/ChHiggs_%d.root"%idx for idx in range(0,100)])

t=ROOT.TChain("hmm")
#t.Add("/scratch/amarini/2018_04_04_ExclusiveCategoriesAndTree/ChHiggs*root")
for f in files: t.Add(f)
#print "TotEntries=",t.GetEntries()
print "TotEntries(Fast)=",t.GetEntriesFast()

outname="output.root" if not multiclass else "multiclass.root"
out=ROOT.TFile(opts.outname,"RECREATE");

analysis="Classification" if not multiclass else "multiclass"
factory = ROOT.TMVA.Factory("factory",out,"!V:!Silent:Color:DrawProgressBar:Transformations=I;:AnalysisType=%s"%analysis)

#dataset="dataset" if not multiclass else "datamulti"
dataloader=ROOT.TMVA.DataLoader(opts.dataset);

#dataloader.AddVariable("pt1rel := pt1/mass");
#dataloader.AddVariable("pt2rel := pt2/mass");

##all variables
features=[
        ## muons variables
        "Hpt","Heta","Hphi",
        "deltaphi","deltaeta",
        "eta1","eta2",
        ## jets variables
        "ncentjets","njets","htCent",
        ## additional leptons: to expand
        "pass_leptonveto",
        ## vbf variables
        "mjj_1","mjj_2","detajj_1","detajj_2",
        "softNjets1","softHt1","softHt5","softHt10",
        "firstQGL","secondQGL","thirdQGL",
        ## bjets variables
        "nbjets","maxDeepB","leadDeepB","maxCSV",
        ## w/z variables
        "mt1","mt2","met",
        ]

features=[
        "pt1/mass","pt2/mass","eta1","eta2","phi1","phi2",
        "Hpt","Heta","Hphi","mjj_1","mjj_2","detajj_1","detajj_2",
        "maxDeepB","njets","Alt$(jet_pt[0],0)","met","softNjets1","softHt1",
        "firstQGL","secondQGL","thirdQGL","costhetastar"
        ]
# super reduced list
if opts.varscheme==1:
	features=["Hpt","deltaphi","deltaeta","ncentjets","njets","htCent","pass_leptonveto","mjj_1","detajj_1","softHt5","met",]
# super reduced list
elif opts.varscheme==2: # no qg
	features=[ "Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","pass_leptonveto","mjj_1","mjj_2","detajj_1","detajj_2","softNjets1","softHt1","softHt5","softHt10","nbjets","maxDeepB","leadDeepB","maxCSV","mt1","mt2","met",]
elif opts.varscheme==3: # no soft
	features=[ "Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","pass_leptonveto","mjj_1","mjj_2","detajj_1","detajj_2","firstQGL","secondQGL","thirdQGL","nbjets","maxDeepB","leadDeepB","maxCSV","mt1","mt2","met",]
elif opts.varscheme==4: # no mt and b info
	features=[ "Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","pass_leptonveto","mjj_1","mjj_2","detajj_1","detajj_2","softNjets1","softHt1","softHt5","softHt10","firstQGL","secondQGL","thirdQGL","nbjets","met",]
elif opts.varscheme==5: # no qg, + no soft, + no mt and b info
	features=[ "Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","pass_leptonveto","mjj_1","mjj_2","detajj_1","detajj_2","nbjets","met",]
elif opts.varscheme==6: # no qg, + no soft, + no mt and b info, + no lepton_veto
	features=[ "Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","mjj_1","mjj_2","detajj_1","detajj_2","nbjets","met",]
elif opts.varscheme==7: # all (to copy)
	features=[ "Hpt","Heta","Hphi","deltaphi","deltaeta","eta1","eta2","ncentjets","njets","htCent","pass_leptonveto","mjj_1","mjj_2","detajj_1","detajj_2","softNjets1","softHt1","softHt5","softHt10","firstQGL","secondQGL","thirdQGL","nbjets","maxDeepB","leadDeepB","maxCSV","mt1","mt2","met",]

for x in features:
    dataloader.AddVariable(x)

dataloader.AddSpectator("bdt");
dataloader.AddSpectator("mass");

xsec = "(mc < -10 && mc >=-19 )*48.58* 0002176 + (mc< -20 && mc >= -29)*  3.782*0.0002176 + (mc < -30 && mc >= -39) * 0.8839*0.0002176 + (mc <-40 && mc >= -49)*0.5328*0.0002176 + (mc < -50 && mc>-59)*0.84*0.0002176 + (mc < -60 && mc >=-69) * 0.5071*0.0002176";
#print "FIX VBF in generator"
#xsec = "(mc < -10 && mc >=-19 )*48.58* 0002176 + (mc< -20 && mc >= -29 || (mc==0 && runNum==1))*  3.782*0.0002176 + (mc < -30 && mc >= -39) * 0.8839*0.0002176 + (mc <-40 && mc >= -49)*0.5328*0.0002176 + (mc < -50 && mc>-59)*0.84*0.0002176 + (mc < -60 && mc >=-69) * 0.5071*0.0002176";
dataloader.AddSpectator("xsec:= weight* ("+xsec +")");
dataloader.SetBackgroundWeightExpression( "weight" );
dataloader.SetSignalWeightExpression( "weight * (" + xsec + ")" );

if not multiclass:
    dataloader . AddSignalTree(    t, 1.0 );
    dataloader . AddBackgroundTree(    t, 1.0 );
    sigCut = ROOT.TCut ( "pass_all && (mc < 0 && mc <= -10) && mass >110 && mass <150"); 
    #print "FIX VBF (2)"
    #sigCut = ROOT.TCut ( "pass_all && (mc <= 0 && mc <= -10 && runNum==1) && mass >110 && mass <150"); 
    bgCut = ROOT.TCut ( "pass_all && ( mc == 10 || mc == 20) && mass >110 && mass <150"); 
    dataloader. PrepareTrainingAndTestTree(sigCut,   bgCut, "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

if multiclass:
    dataloader.AddTree(t,"DY",1.0,ROOT.TCut("(mass>110 && mass<150) && pass_all && (mc>=10 && mc<20) && eventNum%2==0"))
    dataloader.AddTree(t,"TT",1.0,ROOT.TCut("(mass>110 && mass<150) && pass_all && (mc>=20 && mc<30) && eventNum%2==0"))
    dataloader.AddTree (t, "ggH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && ( mc<-10 && mc >-20) &&eventNum%2==0") );
    print "FIX VBF (2)"
    dataloader.AddTree (t, "qqH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && (( mc<-20 && mc >-30)|| (mc==0 && runNum==1) ) && eventNum%2==0") );
    dataloader.AddTree (t,  "VH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && ( mc<-30 && mc >-60) && eventNum%2==0") );
    dataloader.AddTree (t, "ttH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && ( mc<-60 && mc > -70) && eventNum%2==0") );
    
    print "------------ NTRIES -------------------"
    print "DY",t.GetEntries("(mass>110 && mass<150) && pass_all && (mc>=10 && mc<20) && eventNum%2==0")
    print "TT",t.GetEntries("(mass>110 && mass<150) && pass_all && (mc>=20 && mc<30) && eventNum%2==0")
    print "ggH",t.GetEntries("(mass >110 && mass <150) && pass_all && ( mc<-10 && mc >-20) &&eventNum%2==0")
    print "qqH",t.GetEntries("(mass >110 && mass <150) && pass_all && (( mc<-20 && mc >-30)|| (mc==0 && runNum==1) ) && eventNum%2==0")
    print "VH",t.GetEntries("(mass >110 && mass <150) && pass_all && ( mc<-30 && mc >-60) && eventNum%2==0")
    print "ttH",t.GetEntries("(mass >110 && mass <150) && pass_all && ( mc<-60 && mc > -70) && eventNum%2==0")
    print "---------------------------------------"

    dataloader.PrepareTrainingAndTestTree( ROOT.TCut(""), "SplitMode=Random:NormMode=NumEvents:!V" ); 


#print "-> BOOKING KERAS"
#factory.BookMethod(dataloader, ROOT.TMVA.Types.kPyKeras, "PyKeras", "H:!V:VarTransform=D,N:FileNameModel=model.h5:NumEpochs=50:BatchSize=128:SaveBestOnly=true:LearningRateSchedule=30,0.005;40,0.001;30,0.001")

print "-> BOOKING BDTG"
if not multiclass:
    factory . BookMethod(dataloader, ROOT.TMVA.Types.kBDT, "BDTG",
                "!H:!V:NTrees=1200:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.99:SeparationType=SDivSqrtSPlusB:Pray"
                );
else:
    scheme = ROOT.TString("!H:!V:NTrees=5000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.95:Pray")
    if opts.trainscheme==1: # NTrees
    	scheme = ROOT.TString("!H:!V:NTrees=4000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.95:Pray")
    if opts.trainscheme==2: #Node Size 
    	scheme = ROOT.TString("!H:!V:NTrees=5000:MinNodeSize=5%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.95:Pray")
    if opts.trainscheme==3:# Shrinkage
    	scheme = ROOT.TString("!H:!V:NTrees=5000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.15:nCuts=40:MaxDepth=5:NodePurityLimit=0.95:Pray")
    if opts.trainscheme==4:# Cuts
    	scheme = ROOT.TString("!H:!V:NTrees=5000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=50:MaxDepth=5:NodePurityLimit=0.95:Pray")
    if opts.trainscheme==5:  #Depth
    	scheme = ROOT.TString("!H:!V:NTrees=5000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=4:NodePurityLimit=0.95:Pray")
    if opts.trainscheme==6: # Purity
    	scheme = ROOT.TString("!H:!V:NTrees=5000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.90:Pray")
    factory . BookMethod(dataloader, ROOT.TMVA.Types.kBDT, ROOT.TString("BDTG"), scheme
                );

factory . TrainAllMethods();
factory . TestAllMethods();
factory . EvaluateAllMethods();
