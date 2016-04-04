#include "interface/RegressionTrainer.hpp"

#define VERBOSE 0

#include <iostream>
using namespace std;


RegressionTrainer::RegressionTrainer() : AnalysisBase(){
    factory_=NULL;
    train = true;
}

RegressionTrainer::~RegressionTrainer() {
    if(factory_!=NULL) delete factory_;
}


void RegressionTrainer::AddVariable(string name, char type ,double xmin,double xmax)
{
    cout<<"[RegressionTrainer]::[AddVariable]::[INFO] : "<<name<<" '"<<type<<"' "<<xmin<<" -- "<<xmax<<endl;
    if (train) factory_ -> AddVariable(name.c_str(),type,xmin,xmax); 
    Branch("regression_tree",name,type);

}

int RegressionTrainer::analyze(Event*e, string systname)
{
    if(VERBOSE>2) cout<<"[RegressionTrainer]::[analyze]::[DEBUG]::[2] Getting Event:"<<e->eventNum()<<endl;

    if( systname !="" and systname !="NONE" ) return 0;

    string label = GetLabel(e);

    if (label=="Data" ) return 0;

    if (e->Ntaus() <=0 ) return 0;
    if (e->Nleps() >0 ) return 0;


    Jet* j1 = e->LeadJet(); 
    Jet * bj1 = e->LeadBjet();
    Tau* t1 = e->LeadTau();
    
    SetTreeVar("MetGEN",e->GetMet() .gen ) ;


    SetTreeVar("NJets",e->Njets());
    SetTreeVar("NCJets",e->NcentralJets());
    SetTreeVar("etat1",t1->Eta());
    SetTreeVar("phit1",t1->Phi());
    SetTreeVar("ptt1", t1->Pt() );
    SetTreeVar("phimet",e->GetMet().Phi());
    SetTreeVar("ht",e->Ht());
    SetTreeVar("weight",e->weight());

    if (j1 != NULL ) {
                    SetTreeVar("pTj1",j1->Pt());
                    SetTreeVar("etaj1", j1->Eta() );
                    SetTreeVar("phij1", j1->Phi() );
                    }
              else  {SetTreeVar("pTj1",0);
                    SetTreeVar("etaj1", 0 );
                    SetTreeVar("phij1", 0 );
                    }


    if (label.find("Hplus") !=string::npos)  // SIG
        {
            SetTreeVar("sig",1);
        }
    else{ // BKG
            SetTreeVar("sig",0);
        }

    if(VERBOSE>1) PrintTreeVar(); 
    FillTree("regression_tree");
    return 0;
}

void RegressionTrainer::Init(){
    // output is already set
    if(VERBOSE>0) cout<<"[RegressionTrainer]::[Init]::[DEBUG]::[1] InstanceTMVA "<<endl; 

    // -- Create a tree for TMVA
    InitTree("regression_tree");

    // book factory
    if(train) TMVA::Tools::Instance();
    if(VERBOSE>0) cout<<"[RegressionTrainer]::[Init]::[DEBUG]::[1] Construct Factory "<<endl; 
    if(train)factory_ = new TMVA::Factory("TMVAClassification", GetOutputFile(),
            //"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
            "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Classification");

    if(VERBOSE>0) cout<<"[RegressionTrainer]::[Init]::[DEBUG]::[1] AddVariables "<<endl; 
   
  
    AddVariable("MetGEN",'I',0,6500);    

    AddVariable("NJets",'I',0,10);    
    AddVariable("NCJets",'I',0,10);    
    AddVariable("pTj1",'F',0,6500);
    AddVariable("etaj1",'F',0,6500);
    AddVariable("phij1",'F',-5,5);
    AddVariable("ptt1",'F',0,6500);
    AddVariable("etat1",'F',-10,10);
    AddVariable("phit1",'F',-5,5);
    AddVariable("phimet",'F',-5,5);
    AddVariable("ht",'F',0,10000);


    // tell tmva about sig and bkg

    Branch("regression_tree","sig",'I');
    // tell tmva about weight
    Branch("regression_tree","weight",'D');

    if(train) factory_->AddTarget( "MetGEN" ); 
    if(train)factory_->SetWeightExpression("weight");

}

void RegressionTrainer::End(){

    // if I just want to save the tree
    if (not train) return;

    if(VERBOSE>0) cout<<"[RegressionTrainer]::[End]::[DEBUG]::[1] AddSignal and Bkg "<<endl; 
    if(VERBOSE>0) PrintTree("regression_tree");

    factory_->AddRegressionTree( GetTree("regression_tree"));
    
     
    TCut sigCut ("sig > 0.5");
    TCut bgCut  ("sig <= 0.5");
    TCut noneCut ("");
     
    factory_-> PrepareTrainingAndTestTree( noneCut , "nTrain_Regression=0:nTest_Regression=0:SplitMode=Random:NormMode=NumEvents:!V");
   
    factory_ -> BookMethod( TMVA::Types::kPDERS, "PDERS", 
                           "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=40:NEventsMax=60:VarTransform=None" );

    factory_ -> BookMethod( TMVA::Types::kBDT, "BDTG",
                           "!H:!V:NTrees=2000::BoostType=Grad:Shrinkage=0.1:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:MaxDepth=3:NNodesMax=15" );
 
    if(VERBOSE>0) cout<<"[RegressionTrainer]::[End]::[DEBUG]::[1] Train"<<endl;
    factory_ -> TrainAllMethods();
    if(VERBOSE>0) cout<<"[RegressionTrainer]::[End]::[DEBUG]::[1] Test"<<endl;
    factory_ -> TestAllMethods();
    if(VERBOSE>0) cout<<"[RegressionTrainer]::[End]::[DEBUG]::[1] Evaluate"<<endl;
    factory_ -> EvaluateAllMethods();
    if(VERBOSE>0) cout<<"[RegressionTrainer]::[End]::[DEBUG]::[1]  >-> DONE <-<"<<endl;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
