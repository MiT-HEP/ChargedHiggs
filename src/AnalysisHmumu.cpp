#include "interface/AnalysisHmumu.hpp"

//#warning Hmumu ANALYSIS NON ISO
void HmumuAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(25); 
    l->SetIsoRelCut(0.20);
    l->SetEtaCut(2.5);
    l->SetTightCut(false);
    l->SetMediumCut(false)
}

void HmumuAnalysis::Init(){

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
	    Book ("HmumuAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 100,50,200);
	    // 
	    Book ("HmumuAnalysis/Vars/MuonIso_"+ l ,"Muon Isolation;Iso^{#mu} [GeV];Events", 1000,0,0.1);
    }

}

int HmumuAnalysis::analyze(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    Lepton*mu0 = e->GetMuon(0);
    Lepton*mu1 = e->GetMuon(1);
    Jet *j0 = e->GetJet(0);
    Jet *j1 = e->GetJet(1);

    // Truth
    GenParticle *genmu0=NULL; GenParticle *genmu1=NULL;

    for( int iGen=0 ; /*empty*/ ; ++iGen)
    {
        GenParticle *g = e->GetGenParticle(iGen); 
        if (g==NULL ) break;  // end loop statement
        if (not g->IsDressed()) continue;
        if (not abs(g->GetPdgId())==13) continue;
        
        if (genmu0== NULL) genmu0=g;
        else if (genmu1==NULL) {genmu1=g; break;}
    }

    bool genMuons = genmu0!=NULL and genmu1 !=NULL;

    bool isGen=false;
    bool isReco=false;

    if (genMuons) // no requirement on pT
    {
        Object Ztruth(*genmu0); 
        Ztruth += *genmu1;
        if (Ztruth.M() > 60 and Ztruth.M()<120) isGen=true;
    }

    bool recoMuons= mu0 != NULL and mu1 !=NULL; 

    // ---------------------- 25 -------------------
    if ( recoMuons)
    {
        cut.SetCutBit(Leptons);
        if (e->IsTriggered("HLT_IsoMu20") ) cut.SetCutBit(Trigger);

        Object Z(*mu0);
        Z += *mu1;
       	if (cut.passAllUpTo(Trigger) ){
            Fill("HmumuAnalysis/Vars/Mmm_"+ label,systname, Z.M(),e->weight()) ;
        }
    }


    return 0;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
