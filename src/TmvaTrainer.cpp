#include "interface/TmvaTrainer.hpp"

#define VERBOSE 0

#include <iostream>
using namespace std;


TmvaTrainer::TmvaTrainer() : AnalysisBase(){
    factory_=NULL;
    train = true;
}

TmvaTrainer::~TmvaTrainer() {
    if(factory_!=NULL) delete factory_;
}


void TmvaTrainer::AddVariable(string name, char type ,double xmin,double xmax)
{
    cout<<"[TmvaTrainer]::[AddVariable]::[INFO] : "<<name<<" '"<<type<<"' "<<xmin<<" -- "<<xmax<<endl;
    if (train) factory_ -> AddVariable(name.c_str(),type,xmin,xmax); 
    Branch("tmva_tree",name,type);

}

int TmvaTrainer::analyze(Event*e, string systname)
{
    if(VERBOSE>2) cout<<"[TmvaTrainer]::[analyze]::[DEBUG]::[2] Getting Event:"<<e->eventNum()<<endl;

    if( systname !="" and systname !="NONE" ) return 0;

    string label = GetLabel(e);

    if (label=="Data" ) return 0;

    if (e->Ntaus() <=0 ) return 0;
    if (e->Nleps() >0 ) return 0;


    Jet* j1 = e->LeadJet(); 
    Jet * bj1 = e->LeadBjet();
    Tau* t1 = e->LeadTau();

    SetTreeVar("NJets",e->Njets());
    SetTreeVar("NCJets",e->NcentralJets());
    SetTreeVar("BJets",e->Bjets());
    SetTreeVar("etat1",t1->Eta());
    SetTreeVar("phit1",t1->Phi());
    SetTreeVar("phimet",e->GetMet().Phi());
    SetTreeVar("ht",e->Ht());
    SetTreeVar("weight",e->weight());

    if (j1 != NULL ) SetTreeVar("pTj1",j1->Pt());
              else  SetTreeVar("pTj1",0);
    if (bj1 != NULL ) SetTreeVar("pTb1",bj1->Pt());
              else  SetTreeVar("pTb1",0);


    if (label.find("Hplus") !=string::npos)  // SIG
        {
            SetTreeVar("sig",1);
        }
    else{ // BKG
            SetTreeVar("sig",0);
        }

    if(VERBOSE>1) PrintTreeVar(); 
    FillTree("tmva_tree");
    return 0;
}

void TmvaTrainer::Init(){
    // output is already set
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[Init]::[DEBUG]::[1] InstanceTMVA "<<endl; 

    // -- Create a tree for TMVA
    InitTree("tmva_tree");

    // book factory
    if(train) TMVA::Tools::Instance();
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[Init]::[DEBUG]::[1] Construct Factory "<<endl; 
    if(train)factory_ = new TMVA::Factory("TMVAClassification", GetOutputFile(),
            //"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
            "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=Classification");

    if(VERBOSE>0) cout<<"[TmvaTrainer]::[Init]::[DEBUG]::[1] AddVariables "<<endl; 
   
   
    AddVariable("NJets",'I',0,10);    
    AddVariable("NCJets",'I',0,10);    
    AddVariable("BJets",'I',0,10);    
    AddVariable("pTj1",'F',0,6500);
    AddVariable("pTb1",'F',0,6500);
    AddVariable("etat1",'F',-10,10);
    AddVariable("phimet",'F',-10,10);
    AddVariable("phit1",'F',-10,10);
    AddVariable("ht",'F',0,10000);


    // tell tmva about sig and bkg

    Branch("tmva_tree","sig",'I');
    // tell tmva about weight
    Branch("tmva_tree","weight",'D');
    if(train)factory_->SetWeightExpression("weight");

}

void TmvaTrainer::End(){

    // if I just want to save the tree
    if (not train) return;

    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1] AddSignal and Bkg "<<endl; 
    if(VERBOSE>0) PrintTree("tmva_tree");

    factory_->AddSignalTree( GetTree("tmva_tree"));
    factory_->AddBackgroundTree( GetTree("tmva_tree"));
     
    TCut sigCut ("sig > 0.5");
    TCut bgCut  ("sig <= 0.5");
     
    factory_-> PrepareTrainingAndTestTree(sigCut,   bgCut,   
        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
    
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1] Book Methods "<<endl; 
    factory_ ->BookMethod(TMVA::Types::kBDT, "BDT",
            "!H:!V:NTrees=850:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning"
            );
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1] Train"<<endl;
    factory_ -> TrainAllMethods();
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1] Test"<<endl;
    factory_ -> TestAllMethods();
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1] Evaluate"<<endl;
    factory_ -> EvaluateAllMethods();
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1]  >-> DONE <-<"<<endl;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
