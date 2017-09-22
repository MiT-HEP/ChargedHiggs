//root  test/mysub/Tmva/TmvaOutput.root
//for the gui use ROOT 6.0.4
// . /afs/cern.ch/sw/lcg/app/releases/ROOT/6.04.06/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh
// root 
// TMVA::TMVAGui("output.root")
#include <string>
#include "TH1F.h"
#include "TH1D.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TSystem.h"

using namespace std;
using namespace TMVA;

void train( string fileName="/eos/user/k/klute/Nero/2017_09_18_HmmTreeMoreVars_v4/ChHiggs*.root") 
{
    TChain *t=new TChain("hmm");
    int n=t->Add(fileName.c_str());
    //TFile *fInput=TFile::Open(fileName.c_str() ) ;
    if (n==0) cout<<"[train]::[ERROR] No such file or directory '"<<fileName<<"'"<<endl;
    else cout<<"[train]::[INFO] Adding "<<n<<" files to the chain"<<endl;

    TMVA::Tools::Instance();
    bool multiclass = true;

    TFile *out=new TFile("output.root","RECREATE");
    TMVA::Factory *factory_ ;
    if (multiclass)
     factory_= new TMVA::Factory("TMVAMulticlass", out,  "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=multiclass");
    else
     factory_= new TMVA::Factory("TMVAClassification", out,  "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Classification");
    //TMVA::Factory *factory_ = new TMVA::Factory("TMVAClassification", out,  "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");
    TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");

    dataloader->AddVariable("pt1rel := pt1/mass",'F',0,1);
    dataloader->AddVariable("pt2rel := pt2/mass",'F',0,1);
    dataloader->AddVariable("eta1",'F',-2.5,2.5);
    dataloader->AddVariable("eta2",'F',-2.5,2.5);
    dataloader->AddVariable("phi1",'F',0,6.2830);
    dataloader->AddVariable("phi2",'F',0.,6.2830);
    dataloader->AddVariable("Hpt",'F',0,4000.);
    dataloader->AddVariable("nbjets",'I',0,10);
    dataloader->AddVariable("njets",'I',0,10);
    dataloader->AddVariable("ncentjets",'I',0,10);
    dataloader->AddVariable("jet0_pt := Alt$(jet_pt[0],0)",'F',0,4000);
    dataloader->AddVariable("jet1_pt := Alt$(jet_pt[1],0)",'F',0,4000);
    dataloader->AddVariable("jet2_pt := Alt$(jet_pt[2],0)",'F',0,4000);
    dataloader->AddVariable("jet3_pt := Alt$(jet_pt[3],0)",'F',0,4000);
    dataloader->AddVariable("mjj_1",'F',0,4000);
    dataloader->AddVariable("mjj_2",'F',0,4000);
    dataloader->AddVariable("detajj_1",'F',0,10);
    dataloader->AddVariable("detajj_2",'F',0,10);
    dataloader->AddVariable("deltaeta",'F',0,10);
    dataloader->AddVariable("met",'F',0,1000.);
    dataloader->AddVariable("aveQGLCent",'F',0,10.);
    dataloader->AddVariable("maxCSV",'F',0,10.);
    dataloader->AddVariable("aveCSV",'F',0,10.);
    dataloader->AddVariable("secondCSV",'F',0,10.);
    dataloader->AddVariable("thirdCSV",'F',0,10.);
    dataloader->AddVariable("htCent",'F',0,10.);
    dataloader->AddVariable("mt1",'F',0,4000.);
    dataloader->AddVariable("mt2",'F',0,4000.);
    //
    // old bdt
    dataloader->AddSpectator("bdt",'F',-1,1);
    dataloader->AddSpectator("mass",'F',-1,1);

    Double_t signalWeight     = 1.0;
    Double_t backgroundWeight = 1.0;

    string xsec = "(mc < -10 && mc >=-19 )*48.58* 0002176 + (mc< -20 && mc >= -29)*  3.782*0.0002176 + (mc < -30 && mc >= -39) * 0.8839*0.0002176 + (mc <-40 && mc >= -49)*0.5328*0.0002176 + (mc < -50 && mc>-59)*0.84*0.0002176 + (mc < -60 && mc >=-69) * 0.5071*0.0002176";
    dataloader->SetBackgroundWeightExpression( "weight" );
    dataloader->SetSignalWeightExpression( "weight * (" + xsec + ")" );

    if (not multiclass)
    {
        dataloader -> AddSignalTree(    t, signalWeight );
        dataloader -> AddBackgroundTree(    t, backgroundWeight );
        TCut sigCut ( "pass_all && (mc < 0 && mc <= -10) && mass >110 && mass <150"); // 
        TCut bgCut  ( "pass_all && ( mc == 10 || mc == 20) && mass >110 && mass <150"); 
        dataloader-> PrepareTrainingAndTestTree(sigCut,   bgCut, "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
    }

    //string xsec = "(mc == -12)*48.58* 0002176 + (mc == -22)*  3.782*0.0002176 + (mc == -32) * 0.8839*0.0002176 + (mc ==-42)*0.5328*0.0002176 + (mc ==-52)*0.84*0.0002176 + (mc == -62) * 0.5071*0.0002176";

    int mcBkg =0;
    int mcSig =0;
    //120 -1, 125 -2 ,130 -3
    //if (sig == "ttH") mcSig = -60;
    //else if (sig == "ggH") mcSig = -10;
    //else if (sig == "qqH") mcSig = -20;
    //else if (sig == "ZH") mcSig = -30;
    //else if (sig == "WMinusH") mcSig = -40;
    //else if (sig == "WPlusH") mcSig = -50;
    //else cout<<"ERROR: NO sig "<<sig<<endl;
    //
    //125: BR 0.0002176
    //   : ggH 48.58 qqH 3.782 WPlusH 0.5328 WMinusH 0.84 ZH 0.8839 ttH 0.5071


    //if(bkg =="DY") mcBkg = 10;
    //else if(bkg=="TT") mcBkg =20;
    //else if(bkg=="ST") mcBkg =21;
    //else if(bkg=="VV") mcBkg =30; //31 32
    //else cout<<"ERROR: No bkg "<< bkg<<endl;

    // single cut

    if (multiclass){
        cout<<"bkg events = "<< t->GetEntries("(mass >110 && mass <150) && pass_all && ( mc == 10 || mc == 20)")<<endl;
        dataloader->AddTree (t, "bkg", 1.0, "(mass >110 && mass <150) && pass_all && ( mc == 10 || mc == 20)" );
        dataloader->AddTree (t, "ggH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-10 && mc >-20)" );
        dataloader->AddTree (t, "qqH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-20 && mc >-30)" );
        dataloader->AddTree (t,  "VH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-30 && mc >-60)" );
        dataloader->AddTree (t, "ttH", 1.0, "(mass >110 && mass <150) && pass_all && ( mc<-60 && mc > -70)" );
        dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" ); // multiclass
    }

    // -- ADABOOST	
    // -- factory_ ->BookMethod(TMVA::Types::kBDT, "BDT",
    // -- 		            "!H:!V:NTrees=850:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:Pray"
    // -- 			             );
    if ( multiclass)
    {
        //factory_ ->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
        //    "!H:!V:NTrees=1000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.99:SeparationType=SDivSqrtSPlusB:Pray"
        //    );
    }
    else 
    factory_ ->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
            //"!H:!V:NTrees=850:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2:Pray"
            "!H:!V:NTrees=1000:MinNodeSize=3%:BoostType=Grad:Shrinkage=0.10:nCuts=40:MaxDepth=5:NodePurityLimit=0.99:SeparationType=SDivSqrtSPlusB:Pray"
            );

    //MUTUALEXCLUSIVE  
    //TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:"
    TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:WeightInitialization=XAVIER:VarTransform=N");
    if (multiclass) dnnOptions = "!H:V:ErrorStrategy=CROSSENTROPY:WeightInitialization=XAVIER:VarTransform=N";
    TString layoutString ("Layout=TANH|(N+5)*2,TANH|N*2,TANH|N,LINEAR");
    TString training0("LearningRate=1e-1,Momentum=0.0,Repetitions=1,"
            "ConvergenceSteps=20,BatchSize=128,TestRepetitions=10,"
            "WeightDecay=1e-4,Regularization=L2,"
            "DropConfig=0.0+0.5+0.0,DropRepetitions=1, Multithreading=True");
    TString training1("LearningRate=1e-2,Momentum=0.5,Repetitions=1,"
            "ConvergenceSteps=20,BatchSize=128,TestRepetitions=10,"
            "WeightDecay=1e-4,Regularization=L2,"
            "DropConfig=0.0, Multithreading=True");
    TString training2("LearningRate=1e-3,Momentum=0.0,Repetitions=1,"
            "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
            "WeightDecay=1e-4,Regularization=L2,"
            "DropConfig=0.0, Multithreading=True");
    TString trainingStrategyString ("TrainingStrategy=");
    trainingStrategyString += training0 + "|" + training1 + "|" + training2;
    dnnOptions.Append (":"); dnnOptions.Append (layoutString);
    dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);
    TString stdOptions = dnnOptions + ":Architecture=STANDARD";
    TString cpuOptions = dnnOptions + ":Architecture=CPU";

    //if(not multiclass) 
    factory_->BookMethod(dataloader, TMVA::Types::kDNN, "DNN", stdOptions);

    factory_ -> TrainAllMethods();
    factory_ -> TestAllMethods();
    factory_ -> EvaluateAllMethods();

    std::cout << "==> Wrote root file: " << out->GetName() << std::endl;
    std::cout << "==> TMVAClassification is done!" << std::endl;

    out->Close();

    gSystem->Exec("mv output.root dataset/weights/ ;");
    //gSystem->Exec(Form("mv weights weights_hmm_%s_%s;",bkg.c_str(),sig.c_str()));
    gSystem->Exec(Form("mv dataset/weights dataset/weights_hmm ;"));

    delete factory_;
    delete dataloader;
    return;

}
