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

void train( string fileName="reg_tree.root") 
{

	TFile *fInput=TFile::Open(fileName.c_str() ) ;
	if (fInput==NULL) cout<<"[train]::[ERROR] No such file or directory '"<<fileName<<"'"<<endl;
	
	TMVA::Tools::Instance();

	TFile *out=new TFile("reg_output.root","RECREATE");
	TMVA::Factory *factory_ = new TMVA::Factory("TMVARegression", out,  "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Regression");
	
	factory_->AddVariable("nvtx",'I');
	factory_->AddVariable("tauPt",'I');
	factory_->AddVariable("tauM",'I');
	factory_->AddVariable("tauQ",'F');
	factory_->AddVariable("tauEta",'F');
	factory_->AddVariable("tauIso",'F');
	factory_->AddVariable("tauIso2",'F');
	factory_->AddVariable("tauChargedIsoPtSum",'F');
	factory_->AddVariable("tauNeutralIsoPtSum",'F');
	factory_->AddVariable("jetPt",'F');
	factory_->AddVariable("jetEta",'F');

	//factory_->AddTarget("tauPartonPt/tauPt",'F');
	factory_->AddTarget("tauPartonPt/tauPt","Fraction","1",0.05,2.50);
	
	factory_->SetWeightExpression("weight");
	
	TTree *t =(TTree*) fInput->Get("tree");

	if (t==NULL) cout<<"[train]::[ERROR] No tree 'tmva_tree'"<<endl;
	
	factory_->SetInputTrees(t ,"","");

	
	factory_-> PrepareTrainingAndTestTree("",   "", "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
	
	factory_ ->BookMethod(TMVA::Types::kBDT, "BDT",
			            "!H:!V:nCuts=20:NTrees=300:MaxDepth=4:BoostType=AdaBoostR2:SeparationType=RegressionVariance"
				             );
	
	factory_ -> TrainAllMethods();
	factory_ -> TestAllMethods();
	factory_ -> EvaluateAllMethods();

	//gSystem->Exec("mv reg_output.root weights/;");
	//gSystem->Exec(Form("mv weights weights_%s_%s;",bkg.c_str(),sig.c_str()));
	return;
	
}
