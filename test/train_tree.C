//root  test/mysub/Tmva/TmvaOutput.root
//for the gui use ROOT 6.0.4
// . /afs/cern.ch/sw/lcg/app/releases/ROOT/6.04.06/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh
// root 
// TMVA::TMVAGui("output.root")
#include <string>
#include "TH1F.h"
#include "TH1D.h"
#include "TTree.h"
#include "TFile.h"
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TSystem.h"

using namespace std;
using namespace TMVA;

void train( string fileName="test/mysub/Tmva/TmvaOutput.root",string bkg="QCD", string sig="200") 
{

	TFile *fInput=TFile::Open(fileName.c_str() ) ;
	if (fInput==NULL) cout<<"[train]::[ERROR] No such file or directory '"<<fileName<<"'"<<endl;
	
	TMVA::Tools::Instance();

	TFile *out=new TFile("output.root","RECREATE");
	//TMVA::Factory *factory_ = new TMVA::Factory("TMVAClassification", out,  "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Classification");
	TMVA::Factory *factory_ = new TMVA::Factory("TMVAClassification", out,  "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Classification");
	
	factory_->AddVariable("NJets",'I',0,10);
	factory_->AddVariable("NCJets",'I',0,10);
	factory_->AddVariable("BJets",'I',0,10);
	factory_->AddVariable("pTj1",'F',0,6500);
	factory_->AddVariable("pTb1",'F',0,6500);
	factory_->AddVariable("etat1",'F',-10,10);
	factory_->AddVariable("phimet",'F',-10,10);
	factory_->AddVariable("phit1",'F',-10,10);
	factory_->AddVariable("ht",'F',0,10000);
	factory_->AddVariable("rbb",'F',-10,10);
	factory_->AddVariable("rcoll",'F',-10,10);
	factory_->AddVariable("rsr",'F',-10,10);
	factory_->AddVariable("pTt1oMet",'F',0,10);
	
	factory_->SetWeightExpression("weight");
	
	TTree *t =(TTree*) fInput->Get("tmva_tree");

	if (t==NULL) cout<<"[train]::[ERROR] No tree 'tmva_tree'"<<endl;
	
	factory_->AddSignalTree( t );
	factory_->AddBackgroundTree( t );

	int mcBkg =0;
	int mcSig =0;

	if (sig == "200") mcSig = 202;
	else if (sig == "400") mcSig = 206;
	else if (sig == "180") mcSig = 201;
	else if (sig=="200LO") mcSig=101;
	else if (sig=="500LO") mcSig=103;
	else if (sig=="900LO") mcSig=104;
	else cout<<"ERROR: NO sig "<<sig<<endl;

	if(bkg =="QCD") mcBkg = -101;
	else if(bkg=="TTJets") mcBkg =-103;
	else if(bkg=="DY") mcBkg =-102;
	else if(bkg=="QCDData") mcBkg = -200;
	else cout<<"ERROR: No bkg "<< bkg<<endl;
	
	TCut sigCut ( Form("sig > 0.5 && mc == %d",mcSig)); // 201 = 180GeV, 202=200GeV; 206=400;
	TCut bgCut  ( Form("sig <= 0.5 && mc == %d ",mcBkg)); // QCD= -101, DY = -102  TTJets = -103
	
	factory_-> PrepareTrainingAndTestTree(sigCut,   bgCut, "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
	
	factory_ ->BookMethod(TMVA::Types::kBDT, "BDT",
			            "!H:!V:NTrees=850:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:Pray"
				             );
	
	factory_ -> TrainAllMethods();
	factory_ -> TestAllMethods();
	factory_ -> EvaluateAllMethods();

	gSystem->Exec("mv output.root weights/;");
	gSystem->Exec(Form("mv weights weights_%s_%s;",bkg.c_str(),sig.c_str()));
	return;
	
}
