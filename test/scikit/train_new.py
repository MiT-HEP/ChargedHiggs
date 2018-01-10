### BUILD MODEL WITH KERAS

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
    model = Sequential()
    model.add(Dense(40, input_dim=17,activation='tanh') )
    model.add(Dropout(0.25))
    model.add(Dense(25,activation='tanh'))
    model.add(Dense(1,activation='sigmoid'))
    
    model.compile(
            #loss='mean_squared_error',
            #loss='categorical_crossentropy',
            loss='binary_crossentropy',
            #optimizer=SGD(lr=0.01),
            optimizer='adam',
            metrics=['accuracy'])
    return model

print "-> skip model construction"
#model=build_model()
#model.save('model.h5')

########## USE TMVA FOR TRAINING ########
import ROOT

ROOT.TMVA.Tools.Instance()
#ROOT.TMVA.PyMethodBase.PyInitialize()
#print "TMVA:",ROOT.TMVA_RELEASE

## BOOK Tree
t=ROOT.TChain("hmm")
for idx in range(0,100):
    t.Add("/eos/user/k/klute/Nero/2017_09_18_HmmTreeMoreVars_v4/ChHiggs_%d.root"%idx)
print "TotEntries=",t.GetEntries()

out=ROOT.TFile("output.root","RECREATE");
factory = ROOT.TMVA.Factory("factory",out,"!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Classification")
dataloader=ROOT.TMVA.DataLoader("dataset");

dataloader.AddVariable("pt1rel := pt1/mass");
dataloader.AddVariable("pt2rel := pt2/mass");
dataloader.AddVariable("eta1");
dataloader.AddVariable("eta2");
dataloader.AddVariable("phi1");
dataloader.AddVariable("phi2");
dataloader.AddVariable("Hpt");
dataloader.AddVariable("Heta");
dataloader.AddVariable("Hphi");
dataloader.AddVariable("mjj_1");
dataloader.AddVariable("mjj_2");
dataloader.AddVariable("detajj_1");
dataloader.AddVariable("detajj_2");
#
dataloader.AddVariable("nbjets");
dataloader.AddVariable("njets");

#dataloader.AddVariable("ncentjets");
dataloader.AddVariable("jet0_pt := Alt$(jet_pt[0],0.)");
#dataloader.AddVariable("jet1_pt := Alt$(jet_pt[1],0.)");
#dataloader.AddVariable("jet2_pt := Alt$(jet_pt[2],0.)");
#dataloader.AddVariable("jet3_pt := Alt$(jet_pt[3],0.)");
#dataloader.AddVariable("deltaeta");
dataloader.AddVariable("met");
#dataloader.AddVariable("aveQGLCent");
#dataloader.AddVariable("maxCSV");
#dataloader.AddVariable("aveCSV");
#dataloader.AddVariable("secondCSV");
#dataloader.AddVariable("thirdCSV");
#dataloader.AddVariable("htCent");
#dataloader.AddVariable("mt1");
#dataloader.AddVariable("mt2");
#dataloader.AddVariable("htmjj_1");
#dataloader.AddVariable("htmjj_2");
#dataloader.AddVariable("firstQGL");
#dataloader.AddVariable("secondQGL");
#dataloader.AddVariable("thirdQGL");

dataloader.AddSpectator("bdt");
dataloader.AddSpectator("mass");

xsec = "(mc < -10 && mc >=-19 )*48.58* 0002176 + (mc< -20 && mc >= -29)*  3.782*0.0002176 + (mc < -30 && mc >= -39) * 0.8839*0.0002176 + (mc <-40 && mc >= -49)*0.5328*0.0002176 + (mc < -50 && mc>-59)*0.84*0.0002176 + (mc < -60 && mc >=-69) * 0.5071*0.0002176";
dataloader.AddSpectator("xsec:= weight* ("+xsec +")");
dataloader.SetBackgroundWeightExpression( "weight" );
dataloader.SetSignalWeightExpression( "weight * (" + xsec + ")" );

dataloader . AddSignalTree(    t, 1.0 );
dataloader . AddBackgroundTree(    t, 1.0 );
sigCut = ROOT.TCut ( "pass_all && (mc < 0 && mc <= -10) && mass >110 && mass <150"); 
bgCut = ROOT.TCut ( "pass_all && ( mc == 10 || mc == 20) && mass >110 && mass <150"); 
dataloader. PrepareTrainingAndTestTree(sigCut,   bgCut, "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

#    if (multiclass){
#        cout<<"bkg events = "<< t->GetEntries("(mass >110 && mass <150) && pass_all && ( mc == 10 || mc == 20)")<<endl;
#        dataloader->AddTree (t, "bkg", 1.0, "(mass >110 && mass <150) && pass_all && ( mc == 10 || mc == 20)" );
#        dataloader->AddTree (t, "ggH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-10 && mc >-20)" );
#        dataloader->AddTree (t, "qqH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-20 && mc >-30)" );
#        dataloader->AddTree (t,  "VH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-30 && mc >-60)" );
#        dataloader->AddTree (t, "ttH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-60 && mc > -70)" );
#        dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" ); // multiclass
#    }


#print "-> BOOKING KERAS"
#factory.BookMethod(dataloader, ROOT.TMVA.Types.kPyKeras, "PyKeras", "H:!V:VarTransform=D,N:FileNameModel=model.h5:NumEpochs=50:BatchSize=128:SaveBestOnly=true:LearningRateSchedule=30,0.005;40,0.001;30,0.001")

print "-> BOOKING BDTG"
factory . BookMethod(dataloader, ROOT.TMVA.Types.kBDT, "BDTG",
            "!H:!V:NTrees=1200:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.99:SeparationType=SDivSqrtSPlusB:Pray"
            );

factory . TrainAllMethods();
factory . TestAllMethods();
factory . EvaluateAllMethods();
