#include "interface/TmvaTrainer.hpp"

#define VERBOSE 2
#include <iostream>
using namespace std;
// ----------------------------- DATA STORE -----------------------
template<class T>
void DataStore::Set(string name, T value)
{
   if( valuesD_.find( name ) != valuesD_.end() ) 
            valuesD_[name] = double( value) ;
   if( valuesF_.find( name ) != valuesF_.end() ) 
            valuesF_[name] = float( value) ;
   if( valuesI_.find( name ) != valuesI_.end() ) 
            valuesI_[name] = int( value) ;
   return ;

}

bool DataStore::Exists(string name)
{
   if( valuesD_.find( name ) != valuesD_.end() ) return true;
   if( valuesF_.find( name ) != valuesF_.end() ) return true;
   if( valuesI_.find( name ) != valuesI_.end() ) return true;
   return false;
}
void DataStore::Add(string name, char type)
{
    if (Exists(name)) return;
    switch (type)
    {
        case 'F': valuesF_[name] = 0.0;break;
        case 'D': valuesD_[name] = 0.0;break;
        case 'I': valuesI_[name] = 0;break;
    }
    return;
}
void* DataStore::GetPointer(string name){
   if( valuesD_.find( name ) != valuesD_.end() ) return &valuesD_[ name ];
   if( valuesF_.find( name ) != valuesF_.end() ) return &valuesF_[ name ];
   if( valuesI_.find( name ) != valuesI_.end() ) return &valuesI_[ name ];
   return NULL;
}
// -------------------------------------------------------------------------------

TmvaTrainer::TmvaTrainer() : AnalysisBase(){
    factory_=NULL;
}

TmvaTrainer::~TmvaTrainer() {
    if(factory_!=NULL) delete factory_;
}


void TmvaTrainer::AddVariable(string name, char type ,double xmin,double xmax)
{
    cout<<"[TmvaTrainer]::[AddVariable]::[INFO] : "<<name<<" '"<<type<<"' "<<xmin<<" -- "<<xmax<<endl;
    factory_ -> AddVariable(name.c_str(),type,xmin,xmax); 
    tree_->Branch(name.c_str(), varsValues_.GetPointer(name), (name+"/" + type).c_str());

}

int TmvaTrainer::analyze(Event*e, string systname)
{
    if(VERBOSE>1) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[2] Getting Event:"<<e->eventNum()<<endl;

    if( systname !="" and systname !="NONE" ) return 0;

    string label = GetLabel(e);

    if (label=="Data" ) return 0;

    if(VERBOSE>1) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[2] Event is none of Syst and data"<<endl;

    if (e->Ntaus() <=0 ) return 0;
    if (e->Nleps() >0 ) return 0;

    if(VERBOSE>1) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[2] Event has a tau and no lep"<<endl;

    Jet* j1 = e->LeadJet(); 
    Jet * bj1 = e->LeadBjet();
    Tau* t1 = e->LeadTau();

    varsValues_.Set("NJets",e->Njets());
    varsValues_.Set("NBJets",e->Bjets());
    varsValues_.Set("etat1",t1->Eta());
    varsValues_.Set("phit1",t1->Phi());

    if (j1 != NULL ) varsValues_.Set("pTj1",j1->Pt());
              else  varsValues_.Set("pTj1",0);
    if (bj1 != NULL ) varsValues_.Set("pTb1",bj1->Pt());
              else  varsValues_.Set("pTb1",0);



    if(VERBOSE>1) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[2] Putting in the right place:"<<e->eventNum()<<endl;
    if (label.find("Hplus") )  // SIG
        {
            varsValues_.Set("sig",1);
        }
    else{ // BKG
            varsValues_.Set("sig",0);
        }
    tree_->Fill();
}

void TmvaTrainer::Init(){
    // output is already set
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[Init]::[DEBUG]::[1] InstanceTMVA "<<endl; 

    // -- Create a tree for TMVA
    GetOutputFile()->cd();
    tree_=new TTree("tmva_tree","tmva_tree");

    // book factory
    TMVA::Tools::Instance();
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[Init]::[DEBUG]::[1] Construct Factory "<<endl; 
    factory_ = new TMVA::Factory("TMVAClassification", GetOutputFile(),
            "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");

    if(VERBOSE>0) cout<<"[TmvaTrainer]::[Init]::[DEBUG]::[1] AddVariables "<<endl; 
   
   
    AddVariable("NJets",'I',0,10);    
    AddVariable("BJets",'I',0,10);    
    AddVariable("pTj1",'F',0,6500);
    AddVariable("pTb1",'F',0,6500);
    AddVariable("etat1",'F',-10,10);
    AddVariable("phit1",'F',-10,10);

    // tell tmva about sig and bkg
    varsValues_.Add("sig",'I');
    tree_->Branch("sig",varsValues_.GetPointer("sig"),"sig/I");

}

void TmvaTrainer::End(){

    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1] AddSignal and Bkg "<<endl; 

    factory_->AddSignalTree(tree_);
    factory_->AddBackgroundTree(tree_);
     
    TCut sigCut ("sig > 0.5");
    TCut bgCut  ("sig <= 0.5");
     
    factory_-> PrepareTrainingAndTestTree(sigCut,   bgCut,   
        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
    
    if(VERBOSE>0) cout<<"[TmvaTrainer]::[End]::[DEBUG]::[1] Book Methods "<<endl; 
    factory_ ->BookMethod(TMVA::Types::kBDT, "BDT",
            "!H:!V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning"
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
