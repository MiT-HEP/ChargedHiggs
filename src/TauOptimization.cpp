#include "interface/TauOptimization.hpp"

void TauOptimization::Init(){
    cout<<"[TauOptimization]::[Init]::[INFO] Booking Histo "<<endl;

    
    for ( string& l : AllLabel()  ) 
    {
        Book("TauOpt/Iso_Match_"+l,"tau Iso",1000,0.,20.);
        Book("TauOpt/Iso_Lead_"+l,"tau Iso",1000,0.,20.);
        Book("TauOpt/IsLeadMatched_"+l,"Leading Matched",10,-4.5,5.5);

        Book("TauOpt/Pt_Match_"+l,"tau Pt",1000,0.,1000.);
        Book("TauOpt/Pt_Lead_"+l,"tau Pt",1000,0.,1000.);
    }
}

int TauOptimization::analyze(Event* e, string systname)
{
    if(systname != "" and systname !="NONE" ) return 0;

    if ( e-> Ntaus() == 0 ) return 0;

    string label = GetLabel(e);

    // Require matching with GEN objects
    e->MatchTaus();


    Tau * t = e->LeadTau();

    Fill("TauOpt/Iso_Lead_"+label,systname, t->iso ,e ->weight() );
    Fill("TauOpt/Pt_Lead_"+label,systname, t->Pt() ,e ->weight() );
    Fill("TauOpt/IsLeadMatched_"+label,systname, t->IsMatch() ,e ->weight() );
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
        Fill("TauOpt/Iso_Match_"+label,systname, tM->iso ,e ->weight() );
        Fill("TauOpt/Pt_Match_"+label,systname, tM->iso ,e ->weight() );
    }//end LeadTauMatched
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
