#include "interface/TagAndProbe.hpp"

//#define VERBOSE 1
//
void TagAndProbe::SetTauCuts(Tau *t){ 
    t->SetIsoCut(-1);  // default 1.5 -- inv iso >3  <20
    t->SetEtaCut(2.1); 
    t->SetPtCut(20);
    t->SetEleRej(false); // default true
    t->SetMuRej(false); // default true
    return ;
}

void TagAndProbe::Init(){
	if ( doTree){
		cout<<"[TagAndProbe]::[Init]::[INFO] doTree: "<<treename<<endl;
		InitTree(treename);
        Branch(treename, "runNum",'I');
        Branch(treename, "lumiSec",'I');
        Branch(treename, "evtNum", 'I');
        Branch(treename, "npv",'I');
        Branch(treename, "pass",'I');
        Branch(treename, "npu",'F');
        Branch(treename, "scale1fb",'F');
		Branch(treename, "mass", 'F'); // invariant mass
		Branch(treename, "qtag", 'I'); // invariant mass
		Branch(treename, "qprobe", 'I'); // invariant mass
        Branch(treename, "tag", 'T');
        Branch(treename, "probe", 'T');
        Branch(treename, "truth_tag",'I');
        Branch(treename, "truth_probe", 'I');
        //
		Branch(treename, "met", 'F'); // met
        
        Branch(treename,"passIso",'I');  // pass isolation
        Branch(treename,"passId",'I');  // pass id
        Branch(treename,"passIdEle",'I');  // pass id ele
        Branch(treename,"passIdMu",'I');  // pass id muon
        Branch(treename,"passEventTrigger",'I');  // pass trigger -- need trigger matching !
        Branch(treename,"passTrigger",'I');  // pass trigger -- need trigger matching !
        Branch(treename,"passTriggerNone",'I');  // pass trigger -- studying tr matching
        Branch(treename,"passTriggerMet",'I');  // pass trigger -- need trigger matching !
        Branch(treename,"passMCTruth",'I');  // pass Truth MC

        Branch(treename,"isTagTrigger",'I');  // pass trigger -- need trigger matching !
        Branch(treename,"isMC",'I');
        //PrintTree(treename);
	}
}



int TagAndProbe::analyze(Event*e,string systname){

    bool isTrigger=false; 
    //if (e->IsRealData() ) isTrigger =   e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v");
    if (e->IsRealData() ) isTrigger =   e->IsTriggered("HLT_IsoMu20"); // or 27

    if (not isTrigger and e->IsRealData()) return EVENT_NOT_USED; // common base ; //mc will pass 

    string label = GetLabel(e);
    SetTreeVar("runNum",e->runNum());
    SetTreeVar("lumiSec",e->lumiNum());
    SetTreeVar("evtNum",e->eventNum());
    SetTreeVar("scale1fb",e->weight() );
    SetTreeVar("npv",e->Npv() );
    SetTreeVar("npu",e-> GetWeight() -> GetPU() );

    Lepton *mTag = e->GetMuon(0);

    if (mTag == NULL ) return EVENT_NOT_USED; // no tag taus
    SetTreeVar("isTagTrigger", e->IsTriggered( "HLT_IsoMu20", mTag )  );

    // LEPTON VETO:
    if (e->GetMuon(1) != NULL) return EVENT_NOT_USED;
    if (e->GetElectron(0) != NULL ) return EVENT_NOT_USED;

    Tau *tProbe = NULL;

    for (int iT=0;iT<100 ;++iT)  // Fill one for each Tau Candidate
    {
    tProbe = e->GetTau(iT);

    if (tProbe == NULL ) return EVENT_NOT_USED;

    SetTreeVar("mass", mTag->InvMass( *tProbe ) );
    //SetTreeVar("mt", e->Mt(Event::MtMuon) );
    SetTreeVar("met", e->GetMet().Pt() );
    SetTreeVar("probe", tProbe->GetP4() ) ;
    SetTreeVar("tag",mTag->GetP4() ) ;
    SetTreeVar("pass",  tProbe -> iso2 < 1.5 && tProbe -> id  && tProbe -> id_ele && tProbe -> id_mu);
    SetTreeVar("qprobe", tProbe->Charge() ) ;
    SetTreeVar("qtag",mTag->Charge() ) ;

    SetTreeVar("passIso", tProbe -> iso2 < 1.5 );
    SetTreeVar("passId", tProbe -> id ); // 100% ?!?
    SetTreeVar("passIdEle", tProbe -> id_ele );
    SetTreeVar("passIdMu", tProbe -> id_mu );
    SetTreeVar("passEventTrigger", e->IsTriggered( "HLT_LooseIsoPFTau50_Trk30_eta2p1_v", NULL)  );
    SetTreeVar("passTrigger", e->IsTriggered( "HLT_LooseIsoPFTau50_Trk30_eta2p1_v", tProbe )  );
    SetTreeVar("passTriggerNone", e->IsTriggered( "HLT_LooseIsoPFTau50_Trk30_eta2p1_v", tProbe , true)  );
    SetTreeVar("passTriggerMet", e->IsTriggered( "HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80", tProbe )  );
    SetTreeVar("passMCTruth", tProbe -> IsMatch() );


    // ------------- SAVE EVENT INFO -------------
    if(e->IsRealData() ) SetTreeVar("isMC",0);
    else if (label.find("HplusToTauNu") != string::npos ) SetTreeVar("isMC",1);
    else if (label.find("DY") != string::npos) SetTreeVar("isMC",2);
    else SetTreeVar("isMC",3);
    //------------------------------------------

    //PrintTreeVar(treename);
	FillTree(treename);
    }

    return EVENT_NOT_USED;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
