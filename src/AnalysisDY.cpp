#include "interface/AnalysisDY.hpp"


void DYAnalysis::SetLeptonCuts(Lepton *l){ l->SetIsoCut(-1); l->SetPtCut(15); l->SetIsoRelCut(0.1);}

void DYAnalysis::Init(){

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
	    Book ("DYAnalysis/Vars/Mee_"+ l ,"Mee", 100,60,120);
	    Book ("DYAnalysis/Vars/Mmm_"+ l ,"Mmm", 100,60,120);
	    // 
	    Book ("DYAnalysis/Vars/Ptee_"+ l ,"Ptee", 1000,0,1000);
	    Book ("DYAnalysis/Vars/Ptmm_"+ l ,"Ptmm", 1000,0,1000);
        //
	    Book ("DYAnalysis/Vars/Npvee_"+ l ,"Npvee", 50,0,50);
	    Book ("DYAnalysis/Vars/Npvmm_"+ l ,"Npvmm", 50,0,50);
        //
	    Book ("DYAnalysis/Vars/MHighPtee_"+ l ,"MHighPtee", 1000,0,1000);
	    Book ("DYAnalysis/Vars/MHighPtmm_"+ l ,"MHighPtmm", 1000,0,1000);
        //
	    Book ("DYAnalysis/Vars/Pt20mm_"+ l ,"Ptmm", 1000,0,1000);
    }

}

int DYAnalysis::analyze(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");

    Lepton*mu0 = e->GetMuon(0);
    Lepton*mu1 = e->GetMuon(1);
    Lepton*e0 = e->GetElectron(0);
    Lepton*e1 = e->GetElectron(1);

    if (mu0 !=NULL and mu1 != NULL and mu1->Pt() >20)
    {
        Object Z(*mu0);
        Z += *mu1;
        if (Z.M()> 60 && Z.M()<120){
            Fill("DYAnalysis/Vars/Pt20mm_"+ label,systname, Z.Pt(),e->weight()) ;
        }
    
    }

    if (mu0 != NULL and mu1 != NULL and mu1->Pt() >25)
    {
        cut.SetCutBit(Leptons);
        if (e->IsTriggered("HLT_IsoMu20") ) cut.SetCutBit(Trigger);

        Object Z(*mu0);
        Z += *mu1;
       	if (cut.passAllUpTo(Trigger) ) Fill("DYAnalysis/Vars/Mmm_"+ label,systname, Z.M(),e->weight()) ;

        if (Z.M()> 60 && Z.M()<120) cut.SetCutBit(Mass);

       	if (cut.passAll() ){
            Fill("DYAnalysis/Vars/Ptmm_"+ label,systname, Z.Pt(),e->weight()) ;
       	    Fill("DYAnalysis/Vars/Npvmm_"+ label,systname, e->Npv(),e->weight()) ;
        }

        if ( cut.passAllUpTo(Trigger) and Z.Pt() > 500)
        {
            Fill( "DYAnalysis/Vars/MHighPtmm_" + label,systname,Z.M() ,e->weight() ) ;
        }
    }

    if (e0 != NULL and e1 != NULL and e1->Pt() >25)
    {
        cut.SetCutBit(Leptons);
        //if (e->IsTriggered("HLT_Ele27_eta2p1_WPLoose_Gsf") ) cut.SetCutBit(Trigger);
        cut.SetCutBit(Trigger);
        Object Z(*e0);
        Z += *e1;
       	if (cut.passAllUpTo(Trigger))Fill("DYAnalysis/Vars/Mee_"+ label,systname ,Z.M(),e->weight()) ;

        if (Z.M()> 60 && Z.M()<120) cut.SetCutBit(Mass);

        if (cut.passAll()){
       	    Fill("DYAnalysis/Vars/Ptee_"+ label,systname, Z.Pt(),e->weight()) ;
       	    Fill("DYAnalysis/Vars/Npvee_"+ label,systname, e->Npv(),e->weight()) ;
        }

        if ( cut.passAllUpTo(Trigger) and Z.Pt() > 500)
        {
            Fill( "DYAnalysis/Vars/MHighPtee_" + label,systname,Z.M() ,e->weight() ) ;
        }
    }
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
