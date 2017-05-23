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

void train( string fileName="Hmumu/Hmumu_2017_04_26_Sync/ChHiggs*.root",string bkg="TT", string sig="ttH") 
{
    TChain *t=new TChain("hmm");
    int n=t->Add(fileName.c_str());
	//TFile *fInput=TFile::Open(fileName.c_str() ) ;
	if (n==0) cout<<"[train]::[ERROR] No such file or directory '"<<fileName<<"'"<<endl;
    else cout<<"[train]::[INFO] Adding "<<n<<" files to the chain"<<endl;
	
	TMVA::Tools::Instance();

	TFile *out=new TFile("output.root","RECREATE");
	TMVA::Factory *factory_ = new TMVA::Factory("TMVAClassification", out,  "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Classification");
   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");

	dataloader->AddVariable("pt1rel := pt1/mass",'F',0,1);
	dataloader->AddVariable("pt2rel := pt2/mass",'F',0,1);
	dataloader->AddVariable("eta1",'F',-2.5,2.5);
	dataloader->AddVariable("eta2",'F',-2.5,2.5);
	dataloader->AddVariable("phi1",'F',0,6.2830);
	dataloader->AddVariable("phi2",'F',0.,6.2830);
	dataloader->AddVariable("Hpt",'F',0,1000.);
	dataloader->AddVariable("nbjets",'I',0,10);
	dataloader->AddVariable("njets",'I',0,10);
	dataloader->AddVariable("jet0_pt := Alt$(jet_pt[0],0)",'F',0,1000);
	dataloader->AddVariable("mjj_1",'F',0,1000);
	dataloader->AddVariable("mjj_2",'F',0,1000);
	dataloader->AddVariable("detajj_1",'F',0,10);
	dataloader->AddVariable("detajj_2",'F',0,10);
	dataloader->AddVariable("deltaeta",'F',0,10);

    Double_t signalWeight     = 1.0;
    Double_t backgroundWeight = 1.0;

    dataloader -> AddSignalTree(    t, signalWeight );
    dataloader -> AddBackgroundTree(    t, backgroundWeight );

    dataloader->SetBackgroundWeightExpression( "mcWeight*puWeight" );
    dataloader->SetSignalWeightExpression( "mcWeight*puWeight" );
	
    //TTree *t =(TTree*) fInput->Get("tmva_tree");


	int mcBkg =0;
	int mcSig =0;
    //120 -1, 125 -2 ,130 -3
	if (sig == "ttH") mcSig = -60;
	else if (sig == "ggH") mcSig = -10;
	else if (sig == "qqH") mcSig = -20;
	else if (sig == "ZH") mcSig = -30;
	else if (sig == "WMinusH") mcSig = -40;
	else if (sig == "WPlusH") mcSig = -50;
	else cout<<"ERROR: NO sig "<<sig<<endl;

	if(bkg =="DY") mcBkg = 10;
	else if(bkg=="TT") mcBkg =20;
	else if(bkg=="ST") mcBkg =21;
	else if(bkg=="VV") mcBkg =30; //31 32
	else cout<<"ERROR: No bkg "<< bkg<<endl;
	
	TCut sigCut ( Form("mc < 0 && mc <= %d && mc > %d",mcSig,mcSig-5)); // 201 = 180GeV, 202=200GeV; 206=400;
	TCut bgCut  ( Form("mc > 0 && mc >= %d && mc <%d",mcBkg,mcBkg+5)); // QCD= -101, DY = -102  TTJets = -103
	
	dataloader-> PrepareTrainingAndTestTree(sigCut,   bgCut, "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

	// -- ADABOOST	
	// -- factory_ ->BookMethod(TMVA::Types::kBDT, "BDT",
	// -- 		            "!H:!V:NTrees=850:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:Pray"
	// -- 			             );
	factory_ ->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
			            "!H:!V:NTrees=850:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2:Pray"
				             );
    TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:"
                                      "WeightInitialization=XAVIERUNIFORM");
    TString layoutString ("Layout=TANH|20,TANH|20,TANH|20,LINEAR");
      TString training0("LearningRate=1e-1,Momentum=0.9,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.5+0.5+0.5, Multithreading=True");
      TString training1("LearningRate=1e-2,Momentum=0.9,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
      TString training2("LearningRate=1e-3,Momentum=0.0,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
      TString trainingStrategyString ("TrainingStrategy=");
      trainingStrategyString += training0 + "|" + training1 + "|" + training2;
    dnnOptions.Append (":"); dnnOptions.Append (layoutString);
    dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);
    TString stdOptions = dnnOptions + ":Architecture=STANDARD";
    TString cpuOptions = dnnOptions + ":Architecture=CPU";
    //factory_->BookMethod(dataloader, TMVA::Types::kDNN, "DNN", cpuOptions);
	
	factory_ -> TrainAllMethods();
	factory_ -> TestAllMethods();
	factory_ -> EvaluateAllMethods();

   std::cout << "==> Wrote root file: " << out->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

	gSystem->Exec("mv output.root weights/;");
	gSystem->Exec(Form("mv weights weights_%s_%s;",bkg.c_str(),sig.c_str()));

   delete factory_;
   delete dataloader;
	return;
	
}
