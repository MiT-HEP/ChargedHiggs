#include "interface/TauOptimization.hpp"

void TauOptimization::Init(){
    cout<<"[TauOptimization]::[Init]::[INFO] Booking Histo "<<endl;


    Book("TauOpt/Iso_Match","tau Iso",1000,0.,20.);
    Book("TauOpt/Iso_Lead","tau Iso",1000,0.,20.);
    Book("TauOpt/IsLeadMatched","Leading Matched",10,-4.5,5.5);

    Book("TauOpt/Pt_Match","tau Pt",1000,0.,1000.);
    Book("TauOpt/Pt_Lead","tau Pt",1000,0.,1000.);
}

int TauOptimization::analyzeEvent(Event* e, string systname)
{
    if(systname != "" and systname !="NONE" ) return 0;
    if ( e-> Ntaus() == 0 ) return 0;

    string label = GetLabel(e);

    // Require matching with GEN objects
    e->MatchTaus();


    Tau * t = e->LeadTau();

    Fill("TauOpt/Iso_Lead",systname, t->iso ,e ->weight() );
    Fill("TauOpt/Pt_Lead",systname, t->Pt() ,e ->weight() );
    Fill("TauOpt/IsLeadMatched",systname, t->IsMatch() ,e ->weight() );
    // Find first match tau
    //
    int LeadTauMatched=-1;
    for(int iT =0 ;iT < e->Ntaus() ; ++iT )
    {
        if (e -> GetTau(iT) -> IsMatch() )
        {
            LeadTauMatched=iT;
            break;
        }
    }

    // 
    if ( LeadTauMatched >=0 ) 
    {
        Tau * tM = e->GetTau( LeadTauMatched ); 
        Fill("TauOpt/Iso_Match",systname, tM->iso ,e ->weight() );
        Fill("TauOpt/Pt_Match",systname, tM->iso ,e ->weight() );
    }//end LeadTauMatched
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
