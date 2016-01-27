#include "interface/TmvaTrainer.hpp"
#include "interface/GeneralFunctions.hpp"

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

int TmvaTrainer::analyzeInvIso(Event*e,string systname)
{
    Jet* j1 = e->LeadJet(); 
    Jet * bj1 = e->LeadBjet();
    Tau *t1 = e->GetTauInvIso(0);

    if (e->Nleps() >0 ) return 0;
    if (t1 == NULL ) return 0;

    //Trigger
    if (t1->Pt() < 51 ) return 0;
    if (e->GetMet().Pt() <130 ) return 0;

    SetTreeVar("sig",0);
    SetTreeVar("mc",-200); // data driven bkg

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
    double DPhiEtMissJet1=-1 ; if (j1 != NULL ) DPhiEtMissJet1 = fabs(e->GetMet().DeltaPhi(j1));
    double DPhiEtMissTau=fabs(e->GetMet().DeltaPhi(t1));

     // compute with the inv tau
    SetTreeVar("rbb",e->RbbMin(3, t1));
    SetTreeVar("rcoll",e->RCollMin(3,t1));
    SetTreeVar("rsr",e->RsrMax(3,t1));

    SetTreeVar("DPhiEtMissJet1",DPhiEtMissJet1);
    SetTreeVar("DPhiEtMissTau",DPhiEtMissTau);

    SetTreeVar("pTt1oMet", t1->Pt() / e->GetMet().Pt() );

    FillTree("tmva_tree");
    return 0;
}

int TmvaTrainer::analyze(Event*e, string systname)
{
    if(VERBOSE>2) cout<<"[TmvaTrainer]::[analyze]::[DEBUG]::[2] Getting Event:"<<e->eventNum()<<endl;

    if( systname !="" and systname !="NONE" ) return 0;

    string label = GetLabel(e);

    if (label=="Data" ) return analyzeInvIso(e,systname); // TAU INV ISO FOR DATA -> QCD

    if (e->Ntaus() <=0 ) return 0;
    if (e->Nleps() >0 ) return 0;


    Jet* j1 = e->LeadJet(); 
    Jet * bj1 = e->LeadBjet();
    Tau* t1 = e->LeadTau();

    //Trigger
    if (t1->Pt() < 51 ) return 0;
    if (e->GetMet().Pt() <130 ) return 0;

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

    int mc=0;
    if (label.find("amcatnlo") != string::npos) 
    {
        mc= 200;
        if (label.find("M-180") !=string::npos) mc += 1;
        if (label.find("M-200") !=string::npos) mc += 2;
        if (label.find("M-220") !=string::npos) mc += 3;
        if (label.find("M-250") !=string::npos) mc += 4;
        if (label.find("M-300") !=string::npos) mc += 5;
        if (label.find("M-400") !=string::npos) mc += 6;
    }
    else if (label.find("Hplus") !=string::npos) 
    {
        mc=100 ;
        if (label.find("M200") !=string::npos) mc += 1;
        if (label.find("M250") !=string::npos) mc += 2;
        if (label.find("M500") !=string::npos) mc += 3;
        if (label.find("M900") !=string::npos) mc += 4;
    }
    else  // bkg
    {
        mc = -100;
        if(label.find("QCD") != string::npos) mc -=1 ;
        if(label.find("DY") != string::npos) mc -=2 ;
        if(label.find("TTJets") != string::npos) mc -=3 ;
        if(label.find("WJets") != string::npos) mc -=4 ;
        if(label.find("WW") != string::npos) mc -=5 ;
        if(label.find("WZ") != string::npos) mc -=6 ;
        if(label.find("ZZ") != string::npos) mc -=7 ;
    }
    SetTreeVar("mc",mc);

    // angular variables
    double DPhiEtMissJet1=-1 ; if (e->GetJet(0) != NULL ) DPhiEtMissJet1 = fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(0))->Phi()));
    //double DPhiEtMissJet2=-1 ; if (e->GetJet(1) != NULL ) DPhiEtMissJet2 = fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(1))->Phi()));
    //double DPhiEtMissJet3=-1 ; if (e->GetJet(2) != NULL ) DPhiEtMissJet3 = fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),(e->GetJet(2))->Phi()));
    double DPhiEtMissTau=fabs(ChargedHiggs::deltaPhi(e->GetMet().Phi(),t1->Phi()));

    SetTreeVar("rbb",e->RbbMin());
    SetTreeVar("rcoll",e->RCollMin());
    SetTreeVar("rsr",e->RsrMax());

    SetTreeVar("DPhiEtMissJet1",DPhiEtMissJet1);
    SetTreeVar("DPhiEtMissTau",DPhiEtMissTau);

    SetTreeVar("pTt1oMet", t1->Pt() / e->GetMet().Pt() );

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
    // -- Angular Variables
    AddVariable("rbb",'F',0,3.1415*2);
    AddVariable("rcoll",'F',0,3.1415*2);
    AddVariable("rsr",'F',0,3.1415*2);
    AddVariable("DPhiEtMissJet1",'F',0,3.1415);
    AddVariable("DPhiEtMissTau",'F',0,3.1415);

    AddVariable("pTt1oMet",'F',0,10);

    // tell tmva about sig and bkg

    Branch("tmva_tree","sig",'I');
    Branch("tmva_tree","mc",'I'); // to distinguish between the different mc
    // tell tmva about weight
    Branch("tmva_tree","weight",'D');
    if(train)factory_->SetWeightExpression("weight");

}

void TmvaTrainer::End(){

    // if I just want to save the tree
    if (not train) return;
    //TODO, outdated, update with the offline one
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
