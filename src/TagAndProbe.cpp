#include "interface/TagAndProbe.hpp"

//#define VERBOSE 1
//

void TagAndProbe::Init(){
	if ( doTree){
		cout<<"[TagAndProbe]::[Init]::[INFO] doTree: "<<treename<<endl;
		InitTree(treename);
		Branch(treename, "m", 'F'); // invariant mass
		Branch(treename, "ptProbe", 'F'); // pt probe
		Branch(treename, "etaProbe", 'F'); // eta probe
		//Branch(treename, "ptTag", 'F'); // pt probe
		//Branch(treename, "etaTag", 'F'); // eta probe
        
        Branch(treename,"passIso",'I');  // pass isolation
        Branch(treename,"passId",'I');  // pass id
        Branch(treename,"passIdEle",'I');  // pass id ele
        Branch(treename,"passIdMu",'I');  // pass id muon
        Branch(treename,"passTrigger",'I');  // pass trigger -- need trigger matching !

        Branch(treename,"iTag",'I');
        Branch(treename,"iProbe",'I');
        Branch(treename,"isMC",'I');
        //PrintTree(treename);
	}
}



int TagAndProbe::analyze(Event*e,string systname){

    bool isTrigger=false; 
    if (e->IsRealData() ) isTrigger =   e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v");

    if (not isTrigger) return EVENT_NOT_USED; // common base

    string label = GetLabel(e);

    Tau *tProbe = NULL;

    for(int iTag =0; iTag <=1 ; ++iTag)
    {
        Tau *tTag = e->GetTau(iTag);

        if (tTag == NULL ) continue; // no tag taus

        for( auto& t : e->taus_) 
            if (tTag != t  and tTag -> DeltaR ( *t ) > .05  ) { tProbe = t; break;} // take the first probe // pt ordered

        if (tProbe == NULL ) break;

        SetTreeVar("m", tTag->InvMass( *tProbe ) );
        SetTreeVar("ptProbe", tProbe->Pt() ) ;
        SetTreeVar("etaProbe",tProbe->Eta() ) ;

        //SetTreeVar("passIso", tProbe -> iso2 < tProbe->isocut_ );
        SetTreeVar("passIso", tProbe -> iso2 < 1.5 );
        SetTreeVar("passId", tProbe -> id );
        SetTreeVar("passIdEle", tProbe -> id_ele );
        SetTreeVar("passIdMu", tProbe -> id_mu );
        SetTreeVar("passTrigger", e->IsTriggered( "HLT_LooseIsoPFTau50_Trk30_eta2p1_v", tProbe )  );

        int _itag_ = -1, _iprobe_ = -1 ;

        for( int i = 0;i < e->taus_.size() ; ++i) if (tTag == e->taus_[i] ) _itag_ = i;
        for( int i = 0 ; i< e->taus_.size() ;++i) if (tProbe == e->taus_[i] ) _iprobe_= i;

        SetTreeVar("iTag", _itag_);
        SetTreeVar("iProbe",_iprobe_);

        //PrintTreeVar(treename);
        if(e->IsRealData() ) SetTreeVar("isMC",0);
        else if (label.find("Higgs") != string::npos ) SetTreeVar("isMC",1);
        else SetTreeVar("isMC",2);

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
