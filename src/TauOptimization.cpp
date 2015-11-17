#include "interface/TauOptimization.hpp"

void TauOptimization::Init(){
    cout<<"[TauOptimization]::[Init]::[INFO] Booking Histo "<<endl;
    
    
    for ( string& l : AllLabel()  ) 
    {
        Book2D("TauOpt/Iso_Rho_" + l , "tau Iso delta Beta corrections",1000,0,200,1000,0,20);
        Book2D("TauOpt/Iso2_Rho_" + l , "tau Iso2 delta Beta corrections",1000,0,200,1000,0,20);

        Book("TauOpt/Iso_Match_"+l,"tau Iso",1000,0.,20.);
        Book("TauOpt/Iso_Lead_"+l,"tau Iso",1000,0.,20.);

        Book("TauOpt/IsLeadMatched_"+l,"Leading Matched",10,-4.5,5.5);

        Book("TauOpt/Pt_Match_"+l,"tau Pt",1000,0.,1000.);
        Book("TauOpt/Pt_Lead_"+l,"tau Pt",1000,0.,1000.);

        Book("TauOpt/Iso2_Match_"+l,"tau Iso Delta Beta",1500,-10,20.);
        Book("TauOpt/Iso2_Lead_"+l,"tau Iso Delta Beta",1500,-10,20.);
    }

    string treename = "TauOptTree";
    InitTree(treename);
    Branch(treename, "tau1Pt" ,'F');
    Branch(treename, "tau2Pt" ,'F');
    Branch(treename, "tau1Eta" ,'F');
    Branch(treename, "tau2Eta" ,'F');
    Branch(treename, "tau1Iso" ,'F');
    Branch(treename, "tau2Iso" ,'F');
    Branch(treename, "ttM" ,'F');

}

int TauOptimization::analyze(Event* e, string systname)
{
    if(systname != "" and systname !="NONE" ) return 0;

    if ( e-> Ntaus() == 0 ) return 0;

    string label = GetLabel(e);

    Tau * t = e->LeadTau();

    Fill2D("TauOpt/Iso_Rho_" + label,systname, e->Rho() ,t->iso, e->weight() ) ;
    Fill2D("TauOpt/Iso2_Rho_" + label,systname, e->Rho() ,t->iso2, e->weight() ) ;

    Fill("TauOpt/Iso_Lead_"+label,systname, t->iso ,e ->weight() );
    Fill("TauOpt/Iso2_Lead_"+label,systname, t->iso2 ,e ->weight() );
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
        Fill("TauOpt/Iso2_Match_"+label,systname, tM->iso2 ,e ->weight() );
        Fill("TauOpt/Pt_Match_"+label,systname, tM->iso ,e ->weight() );
    }//end LeadTauMatched

    // 
    if ( e->IsRealData() and not e->IsTriggered("HLT_LooseIsoPFTau50_Trk30_eta2p1_v") )
    {
        //SetTreeVar("mc",mc); // XXX
        //FillTree("tmva_tree");
    
    }
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
