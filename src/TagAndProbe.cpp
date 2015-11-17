#include "interface/TagAndProbe.hpp"

//#define VERBOSE 1
//

void TagAndProbe::Init(){
	if ( doTree){
		cout<<"[TagAndProbe]::[Init]::[INFO] doTree: "<<treename<<endl;
		InitTree(treename);
		Branch(treename, "m", 'F'); // invariant mass
		Branch(treename, "mt", 'F'); // mt muon-t
		Branch(treename, "ptProbe", 'F'); // pt probe
		Branch(treename, "etaProbe", 'F'); // eta probe
		//Branch(treename, "ptTag", 'F'); // pt probe
		//Branch(treename, "etaTag", 'F'); // eta probe
        
        Branch(treename,"passIso",'I');  // pass isolation
        Branch(treename,"passId",'I');  // pass id
        Branch(treename,"passIdEle",'I');  // pass id ele
        Branch(treename,"passIdMu",'I');  // pass id muon
        Branch(treename,"passTrigger",'I');  // pass trigger -- need trigger matching !

        Branch(treename,"isTagTrigger",'I');  // pass trigger -- need trigger matching !
        Branch(treename,"isMC",'I');
        //PrintTree(treename);
	}
}



int TagAndProbe::analyze(Event*e,string systname){

    bool isTrigger=false; 
    //if (e->IsRealData() ) isTrigger =   e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v");
    if (e->IsRealData() ) isTrigger =   e->IsTriggered("HLT_IsoMu20"); // or 27

    if (not isTrigger) return EVENT_NOT_USED; // common base

    string label = GetLabel(e);

    Tau *tProbe = NULL;
    Jet *jProbe = NULL;

        Lepton *mTag = e->GetMuon(0);

        if (mTag == NULL ) return EVENT_NOT_USED; // no tag taus
        SetTreeVar("isTagTrigger", e->IsTriggered( "HLT_IsoMu20", mTag )  );

        for( auto& t : e->taus_) 
            if (tProbe != NULL) { tProbe = t; break;} // take the first probe // pt ordered, can't use GetTau

        // nothnig to probe FIXME, logic needs to change
        if (tProbe == NULL ) return EVENT_NOT_USED;

        for( auto& j : e->jets_) 
            if (j -> DeltaR( *tProbe ) <0.05) jProbe = j; 

        if (jProbe == NULL ){ 
            cout <<"WARNING: not bare jet associated to the tau"<<endl;
            return EVENT_NOT_USED;
            }

        SetTreeVar("m", mTag->InvMass( *tProbe ) );
        SetTreeVar("mt", e->Mt(Event::MtMuon) );
        SetTreeVar("ptProbe", tProbe->Pt() ) ;
        SetTreeVar("etaProbe",tProbe->Eta() ) ;

        //SetTreeVar("passIso", tProbe -> iso2 < tProbe->isocut_ );
        SetTreeVar("passIso", tProbe -> iso2 < 1.5 );
        SetTreeVar("passId", tProbe -> id );
        SetTreeVar("passIdEle", tProbe -> id_ele );
        SetTreeVar("passIdMu", tProbe -> id_mu );
        SetTreeVar("passTrigger", e->IsTriggered( "HLT_LooseIsoPFTau50_Trk30_eta2p1_v", tProbe )  );

        int _itag_ = -1, _iprobe_ = -1 ;


        //PrintTreeVar(treename);
        if(e->IsRealData() ) SetTreeVar("isMC",0);
        else if (label.find("HplusToTauNu") != string::npos ) SetTreeVar("isMC",1);
        else if (label.find("DY") != string::npos) SetTreeVar("isMC",2);
        else SetTreeVar("isMC",3);

	    FillTree(treename);

    return EVENT_NOT_USED;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
