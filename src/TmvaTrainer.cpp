#include "interface/TmvaTrainer.hpp"


TmvaTrainer::TmvaTrainer() : AnalysisBase(){
    factory_=NULL;
}

TmvaTrainer::~TmvaTrainer() {
    if(factory_!=NULL) delete factory_;
}


void TmvaTrainer::AddVariable(string name, char type ,double xmin,double xmax)
{
    factory_ -> AddVariable(name.c_str(),type,xmin,xmax); 
    varsNames_.push_back(name);

}

int TmvaTrainer::analyze(Event*e, string systname)
{
    if( systname !="" and systname !="NONE" ) return 0;

    string label = GetLabel(e);

    if (label=="Data" ) return 0;

    if (e->Ntaus() <=0 ) return 0;
    if (e->Nleps() >0 ) return 0;

    Jet* j1 = e->LeadJet(); 
    Jet * bj1 = e->LeadBjet();
    Tau* t1 = e->LeadTau();

    varsValues_["NJets"]=e->Njets();
    varsValues_["NBJets"]=e->Bjets();
    varsValues_["etat1"]=t1->Eta();
    varsValues_["phit1"]=t1->Phi();

    if (j1 != NULL ) varsValues_["pTj1"]=j1->Pt();
              else  varsValues_["pTj1"]=0;
    if (bj1 != NULL ) varsValues_["pTb1"]=bj1->Pt();
              else  varsValues_["pTb1"]=0;



    // keep track of ordering
    variables_.clear(); // I guess the variables are ordered
    for(auto&v: varsNames_)
        variables_.push_back(varsValues_[v]);

    if (label.find("Hplus") )  // SIG
        {
            if( e->eventNum() % 2 ) factory_->AddSignalTrainingEvent(variables_,e->weight());
                               else factory_->AddSignalTestEvent(variables_,e->weight() ) ;
        }
    else{ // BKG
            if( e->eventNum() %2 ) factory_->AddBackgroundTrainingEvent(variables_,e->weight());
                              else factory_->AddBackgroundTestEvent(variables_,e->weight());
        }
}

void TmvaTrainer::Init(){
    // output is already set
    TMVA::Tools::Instance();
    // book factory
    factory_ = new TMVA::Factory("TMVAClassification", GetOutputFile(),
            "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
    
    AddVariable("NJets",'I',0,10);    
    AddVariable("BJets",'I',0,10);    
    AddVariable("pTj1",'F',0,6500);
    AddVariable("pTb1",'F',0,6500);
    AddVariable("etat1",'F',-10,10);
    AddVariable("phit1",'F',-10,10);

    factory_ ->BookMethod(TMVA::Types::kBDT, "BDT",
            "!H:!V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning"
            );
}

void TmvaTrainer::End(){
    factory_ -> TrainAllMethods();
    factory_ -> TestAllMethods();
    factory_ -> EvaluateAllMethods();
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
