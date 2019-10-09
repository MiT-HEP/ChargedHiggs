### BUILD MODEL WITH KERAS

#import tensorflow as tf
#sess = tf.Session(config=tf.ConfigProto(log_device_placement=True))
import keras
print "keras version=",keras.__version__

from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout, Flatten
from keras.layers import Convolution2D, MaxPooling2D
from keras.regularizers import l2
#from keras import initializations
from keras.optimizers import SGD

# Define initialization -> to init
#def normal(shape, name=None):
#    return initializations.normal(shape, scale=0.05, name=name)

def build_model():
    print "-> building model"
    model = Sequential()
    l2_val=1e-2
    #model.add(Dense(100,kernel_regularizer=l2(l2_val), activity_regularizer=l2(l2_val), input_dim=29,activation='tanh') )
    #model.add(Dense(100, input_dim=29,activation='tanh') )
    model.add(Dense(100,kernel_regularizer=l2(l2_val), activity_regularizer=l2(l2_val), input_dim=29,activation='tanh') )
    model.add(Dropout(0.20))
    model.add(Dense(50,activity_regularizer=l2(l2_val),activation='tanh'))
    model.add(Dense(20,activation='tanh'))
    model.add(Dense(2,activation='tanh')) 
    
    model.compile(
            #loss='mean_squared_error',
            loss='categorical_crossentropy',
            #loss='binary_crossentropy',
            optimizer=SGD(lr=0.01),
            #optimizer='adam',
            metrics=['accuracy','precision',])
    return model

#model=build_model()
#model.save('model.h5')
#model.summary()
#print "-> skip model construction"
#try:
#    from keras.utils.visualize_util import plot
#    plot(model, to_file='model.png', show_shapes=True)
#except:
#    print('[INFO] Failed to make model plot')

########## USE TMVA FOR TRAINING ########
import ROOT

ROOT.TMVA.Tools.Instance()
# for KERAS TRAINING IS NEEDED
#ROOT.TMVA.PyMethodBase.PyInitialize()
#print "TMVA:",ROOT.TMVA_RELEASE

multiclass=False
if multiclass:
    print "-> Using multiclassification"

## BOOK Tree
t=ROOT.TChain("hmm")
#for n in range(0,100):
#    if n in [53,55,59,73,93,94]: continue #exclude running jobs
#    t.Add("/eos/user/a/amarini/Hmumu/fwk/2018_01_25_ExclusiveCategoriesAndTree/ChHiggs_%d.root"%n)
t.Add("/eos/user/a/amarini/Hmumu/fwk/2019_03_01_Hmm2018Sync_Feb15/ChHiggs*root")
print "TotEntries=",t.GetEntries()

outname="output.root" if not multiclass else "multiclass.root"
out=ROOT.TFile(outname,"RECREATE");

analysis="Classification" if not multiclass else "multiclass"
factory = ROOT.TMVA.Factory("factory",out,"!V:!Silent:Color:DrawProgressBar:Transformations=I;:AnalysisType=%s"%analysis)

dataset="dataset" if not multiclass else "datamulti"
dataloader=ROOT.TMVA.DataLoader(dataset);

#dataloader.AddVariable("pt1rel := pt1/mass");
#dataloader.AddVariable("pt2rel := pt2/mass");

features=[
        ## muons variables
        "Hpt","Heta","Hphi",
        "deltaphi","deltaeta",
        "eta1","eta2",
        ## jets variables
        "ncentjets","njets","htCent",
        ## additional leptons: to expand
        #"pass_leptonveto",
        ## vbf variables
        "mjj_1","mjj_2","detajj_1","detajj_2",
        #"softNjets1","softHt1","softHt5","softHt10",
        "(TMath::IsNaN(firstQGL)?-1:firstQGL)",
        "(TMath::IsNaN(secondQGL)?-1:secondQGL)",
        #"thirdQGL",
        ## bjets variables
        "nbjets","maxDeepB" #,"leadDeepB", #,"maxCSV",
        ## w/z variables
        #"mt1","mt2","met",
        ## photons
        #"Alt$(pho_pt[0],0.)",
        #"Alt$(pho_eta[0],0.)",
        #"Alt$(pho_dr1[0],0.)",
        #"Alt$(pho_dr2[0],0.)",
        ##"npho",
        ]

for x in features:
    dataloader.AddVariable(x)

dataloader.AddSpectator("bdt");
dataloader.AddSpectator("mass");

xsec = "(mc < -10 && mc >=-19 )*48.58* 0002176 + (mc< -20 && mc >= -29)*  3.782*0.0002176 + (mc < -30 && mc >= -39) * 0.8839*0.0002176 + (mc <-40 && mc >= -49)*0.5328*0.0002176 + (mc < -50 && mc>-59)*0.84*0.0002176 + (mc < -60 && mc >=-69) * 0.5071*0.0002176";
#print "FIX VBF in generator"
#xsec = "(mc < -10 && mc >=-19 )*48.58* 0002176 + (mc< -20 && mc >= -29 || (mc==0 && runNum==1))*  3.782*0.0002176 + (mc < -30 && mc >= -39) * 0.8839*0.0002176 + (mc <-40 && mc >= -49)*0.5328*0.0002176 + (mc < -50 && mc>-59)*0.84*0.0002176 + (mc < -60 && mc >=-69) * 0.5071*0.0002176";
dataloader.AddSpectator("xsec:= weight* ("+xsec +")");

if not multiclass:
    #print "FIXME TT"
    #ttfix = "(mc>0 && (mc<20||mc>30)) * 1 +(mc>=20 && mc<30)*88.20/831"; # bkg weight
    dataloader.SetBackgroundWeightExpression( "weight" );
    #dataloader.SetBackgroundWeightExpression( "weight" );
    dataloader.SetSignalWeightExpression( "weight * (" + xsec + ")" );
    dataloader . AddSignalTree(    t, 1.0 );
    dataloader . AddBackgroundTree(    t, 1.0 );
    #bb= " && abs(eta1)<0.9 && abs(eta2)<0.9";
    #oo = " && !(abs(eta1)<0.9 && abs(eta2)<0.9) && abs(eta1) <1.9 && abs (eta2)<1.9"
    #ee = " && !(abs(eta1)<1.9 && abs(eta2)<1.9)"
    sigCut = ROOT.TCut ( "pass_all && (mc < 0 && mc <= -10) && mass >110 && mass <150 && eventNum%2==0 " ); 
    #print "FIX VBF (2)"
    #sigCut = ROOT.TCut ( "pass_all && (mc <= 0 && mc <= -10 && runNum==1) && mass >110 && mass <150"); 
    bgCut = ROOT.TCut ( "pass_all && ( mc == 11 || mc == 20) && mass >110 && mass <150 && eventNum %2==0 " ); 
    dataloader. PrepareTrainingAndTestTree(sigCut,   bgCut, "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

if multiclass:
    dataloader.AddTree(t,"DY",1.0,ROOT.TCut("(mass>110 && mass<150) && pass_all && (mc ==11) && eventNum%2==0"))
    #dataloader.AddTree(t,"DY",1.0,ROOT.TCut("(mass>110 && mass<150) && pass_all && (mc>=10 && mc<20) && eventNum%2==0"))
    dataloader.AddTree(t,"TT",1.0,ROOT.TCut("(mass>110 && mass<150) && pass_all && (mc>=20 && mc<30) && eventNum%2==0"))
    dataloader.AddTree (t, "ggH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && ( mc<-10 && mc >-20) &&eventNum%2==0") );
    dataloader.AddTree (t, "qqH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && ( mc<-20 && mc >-30) && eventNum%2==0") );
    dataloader.AddTree (t,  "VH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && ( mc<-30 && mc >-60) && eventNum%2==0") );
    dataloader.AddTree (t, "ttH", 1.0, ROOT.TCut("(mass >110 && mass <150) && pass_all && ( mc<-60 && mc > -70) && eventNum%2==0") );
    # print "FIXME TT"
    #xsec += " + (mc>0 && (mc<20||mc>30)) * 1 +(mc>=20 && mc<30)*88.20/831"; # bkg weight
    xsec = "1"
    dataloader.SetWeightExpression("weight* (" +xsec+")")
    
    print "------------ NTRIES -------------------"
    print "DY",t.GetEntries("(mass>110 && mass<150) && pass_all && (mc>=10 && mc<20) && eventNum%2==0")
    print "TT",t.GetEntries("(mass>110 && mass<150) && pass_all && (mc>=20 && mc<30) && eventNum%2==0")
    print "ggH",t.GetEntries("(mass >110 && mass <150) && pass_all && ( mc<-10 && mc >-20) &&eventNum%2==0")
    print "qqH",t.GetEntries("(mass >110 && mass <150) && pass_all && (( mc<-20 && mc >-30)|| (mc==0 && runNum==1) ) && eventNum%2==0")
    print "VH",t.GetEntries("(mass >110 && mass <150) && pass_all && ( mc<-30 && mc >-60) && eventNum%2==0")
    print "ttH",t.GetEntries("(mass >110 && mass <150) && pass_all && ( mc<-60 && mc > -70) && eventNum%2==0")
    print "---------------------------------------"

    dataloader.PrepareTrainingAndTestTree( ROOT.TCut(""), "SplitMode=Random:NormMode=NumEvents:!V" ); 


print "-> BOOKING KERAS"
#factory.BookMethod(dataloader, ROOT.TMVA.Types.kPyKeras, "PyKeras", "H:!V:VarTransform=D,N:FileNameModel=model.h5:NumEpochs=50:BatchSize=128:SaveBestOnly=true:LearningRateSchedule=30,0.005;40,0.001;30,0.001")

print "-> BOOKING BDTG"
if not multiclass:
    factory . BookMethod(dataloader, ROOT.TMVA.Types.kBDT, "BDTG",
                "!H:!V:NTrees=1200:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.99:SeparationType=SDivSqrtSPlusB:Pray"
                );
else:
    #default
    factory . BookMethod(dataloader, ROOT.TMVA.Types.kBDT, ROOT.TString("BDTG"),
                ROOT.TString("!H:!V:NTrees=5000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.95:Pray")
                );
    #factory . BookMethod(dataloader, ROOT.TMVA.Types.kBDT, ROOT.TString("BDTG"),
    #            ROOT.TString("!H:!V:NTrees=4000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.95:Pray")
    #            );

factory . TrainAllMethods();
factory . TestAllMethods();
factory . EvaluateAllMethods();
