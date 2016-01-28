#include "interface/AnalysisDY.hpp"


void DYAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(25); 
    l->SetIsoRelCut(0.15);
    l->SetEtaCut(2.4);
}

void DYAnalysis::Init(){

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
	    Book ("DYAnalysis/Vars/Mee_"+ l ,"Mee;m^{ee} [GeV];Events"    , 100,50,200);
	    Book ("DYAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 100,50,200);
	    Book ("DYAnalysis/Vars/Mem_"+ l ,"Mem;m^{e#mu} [GeV];Events"  , 100,50,200);
	    // 
	    Book ("DYAnalysis/Vars/Ptee_"+ l ,"Ptee;p_{T}^{ee} [GeV];Events"    , 1000,0,1000);
	    Book ("DYAnalysis/Vars/Ptmm_"+ l ,"Ptmm;p_{T}^{#mu#mu} [GeV];Events", 1000,0,1000);
	    Book ("DYAnalysis/Vars/Ptem_"+ l ,"Ptem;p_{T}^{e#mu} [GeV];Events"  , 1000,0,1000);
	    Book ("DYAnalysis/Vars/PtemNoMCut_"+ l ,"Ptem;p_{T}^{e#mu} [GeV];Events"  , 1000,0,1000);
        // counting unweighted events
	    Book ("DYAnalysis/Vars/PtmmUW_"+ l ,"Ptmm;p_{T}^{#mu#mu} [GeV];Events", 1000,0,1000);
        // trilepton
	    Book ("DYAnalysis/Vars/Ptlll_"+ l ,"Ptlll;p_{T}^{ll} [GeV];Events"  , 100,0,1000);
        // eta
	    Book ("DYAnalysis/Vars/EtaMu1_"+ l ,"EtaMu1;#eta^{#mu1} ;Events", 1000,-5,5);
        //
	    Book ("DYAnalysis/Vars/NJee_"+ l ,"NJee;N_{jets}^{ee};Events"    , 10,0,10);
	    Book ("DYAnalysis/Vars/NJmm_"+ l ,"NJmm;N_{jets}^{#mu#mu};Events", 10,0,10);
	    Book ("DYAnalysis/Vars/NJem_"+ l ,"NJem;N_{jets}^{e#mu};Events"  , 10,0,10);
        //
	    Book ("DYAnalysis/Vars/Npvee_"+ l ,"Npvee", 50,0,50);
	    Book ("DYAnalysis/Vars/Npvmm_"+ l ,"Npvmm", 50,0,50);
	    Book ("DYAnalysis/Vars/Npvem_"+ l ,"Npvem", 50,0,50);
        //
	    Book ("DYAnalysis/Vars/MHighPtee_"+ l ,"MHighPtee", 1000,0,1000);
	    Book ("DYAnalysis/Vars/MHighPtmm_"+ l ,"MHighPtmm", 1000,0,1000);
	    Book ("DYAnalysis/Vars/MJHighPtee_"+ l ,"MJHighPtee", 1000,0,1000);
	    Book ("DYAnalysis/Vars/MJHighPtmm_"+ l ,"MJHighPtmm", 1000,0,1000);
        //
	    Book ("DYAnalysis/Vars/Pt20mm_"+ l ,"Ptmm20", 1000,0,1000);
    }

}

int DYAnalysis::analyzeMM(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    Lepton*mu0 = e->GetMuon(0);
    Lepton*mu1 = e->GetMuon(1);
    Jet *j0 = e->GetJet(0);
    Jet *j1 = e->GetJet(1);

    if (mu0 == NULL or mu1 == NULL) return 0;

    if ( mu1->Pt() >20)
    { // ------------------- 20 -------------------
        Object Z(*mu0);
        Z += *mu1;
        if (Z.M()> 60 && Z.M()<120){
            Fill("DYAnalysis/Vars/Pt20mm_"+ label,systname, Z.Pt(),e->weight()) ;
        }
    
    }

    // ---------------------- 25 -------------------
    if (mu1->Pt() >25)
    {
        cut.SetCutBit(Leptons);
        if (e->IsTriggered("HLT_IsoMu20") ) cut.SetCutBit(Trigger);

        Object Z(*mu0);
        Z += *mu1;
       	if (cut.passAllUpTo(Trigger) ){
            Fill("DYAnalysis/Vars/Mmm_"+ label,systname, Z.M(),e->weight()) ;
        }

        if (Z.M()> 60 && Z.M()<120) cut.SetCutBit(Mass);

       	if (cut.passAll() ){
            Fill("DYAnalysis/Vars/Ptmm_"+ label,systname, Z.Pt(),e->weight()) ;
            Fill("DYAnalysis/Vars/EtaMu1_"+ label,systname, mu0->Eta(),e->weight()) ;
       	    Fill("DYAnalysis/Vars/Npvmm_"+ label,systname, e->Npv(),e->weight()) ;
            Fill("DYAnalysis/Vars/NJmm_"+ label,systname, e->NcentralJets(),e->weight()) ;
            Fill("DYAnalysis/Vars/PtmmUW_"+ label,systname, Z.Pt(),1.0) ;
        }

        if ( Z.Pt() > 500) // NoSingleMuon Trigger -> ISO
        {
            Fill( "DYAnalysis/Vars/MHighPtmm_" + label,systname,Z.M() ,e->weight() ) ;
            if( j1 != NULL ) Fill( "DYAnalysis/Vars/MJHighPtmm_" + label,systname, j1->InvMass(j0) ,e->weight() ) ;
        }
    }
    return 0;
}


int DYAnalysis::analyzeEE(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");

    Lepton*e0 = e->GetElectron(0);
    Lepton*e1 = e->GetElectron(1);
    Jet *j0 = e->GetJet(0);
    Jet *j1 = e->GetJet(1);

    if (e0 == NULL or e1 == NULL) return 0;

    if ( e1->Pt() >25)
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
            Fill("DYAnalysis/Vars/NJee_"+ label,systname, e->NcentralJets(),e->weight()) ;
       	    Fill("DYAnalysis/Vars/Npvee_"+ label,systname, e->Npv(),e->weight()) ;
        }

        if ( Z.Pt() > 500)
        {
            Fill( "DYAnalysis/Vars/MHighPtee_" + label,systname,Z.M() ,e->weight() ) ;
            if( j1 != NULL ) Fill( "DYAnalysis/Vars/MJHighPtee_" + label,systname, j1->InvMass(j0) ,e->weight() ) ;
        }
    }

    return 0;
}

int DYAnalysis::analyzeEM(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    Lepton*mu0 = e->GetMuon(0);
    Lepton*e0 = e->GetElectron(0);
    Lepton*mu1 = e->GetMuon(1);
    Lepton*e1 = e->GetElectron(1);

    if (mu0==NULL or e0 == NULL) return 0;

    // e -- mu pair is not leading
    bool isEMuLeading= true;
    if ( mu1 != NULL  and mu1->Pt() > e0->Pt() ) isEMuLeading=false;
    if ( e1 != NULL  and e1->Pt() > mu0->Pt()  ) isEMuLeading=false;

    if ( not isEMuLeading ) return 0;

    /////////////             E MU 
    bool doNotDoubleCount = true;

    if ( e->IsRealData() and e->GetName().find("SingleElectron") != string::npos) doNotDoubleCount=false;

    if ( mu0->Pt() >25 and e0->Pt() >25 and doNotDoubleCount and isEMuLeading  )
    {
        
        if ( e->IsRealData() and e->GetName().find("SingleMuon") == string::npos) cout <<"WTF ?"<<endl;

        cut.SetCutBit(Leptons);
        // I don't have to double count between SingleE and SingleMu
        if (e->IsTriggered("HLT_IsoMu20") ) cut.SetCutBit(Trigger);

        Object Z(*mu0);
        Z += *e0;
       	if (cut.passAllUpTo(Trigger) ){
            Fill("DYAnalysis/Vars/Mem_"+ label,systname, Z.M(),e->weight()) ;
            Fill("DYAnalysis/Vars/PtemNoMCut_"+ label,systname, Z.Pt(),e->weight()) ;
        }

        if (Z.M()> 60 && Z.M()<120) cut.SetCutBit(Mass);

       	if (cut.passAll() ){
            Fill("DYAnalysis/Vars/Ptem_"+ label,systname, Z.Pt(),e->weight()) ;
            Fill("DYAnalysis/Vars/NJem_"+ label,systname, e->NcentralJets(),e->weight()) ;
       	    Fill("DYAnalysis/Vars/Npvem_"+ label,systname, e->Npv(),e->weight()) ;
        }

    }

}

int DYAnalysis::analyzeLLL(Event *e, string systname)
{
    //Log(__FUNCTION__,"DEBUG","Begin Function");
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    if (e->IsTriggered("HLT_IsoMu20") ) cut.SetCutBit(Trigger);

    Lepton*mu0 = e->GetMuon(0);
    Lepton*e0 = e->GetElectron(0);
    Lepton*mu1 = e->GetMuon(1);
    Lepton*e1 = e->GetElectron(1);

    if (mu0==NULL or e0 == NULL) return 0;

    bool doNotDoubleCount = true;
    if ( e->IsRealData() and e->GetName().find("SingleElectron") != string::npos) doNotDoubleCount=false;

    //Log(__FUNCTION__,"DEBUG","First Block emm");
    if (mu1 != NULL and mu1->Pt() > 25 and e0->Pt()> 25  and doNotDoubleCount){ //mme
        if (mu1->Charge() *mu0->Charge() == -1)cut.SetCutBit(Leptons);
        Object Z(*mu0);
        Z+= *mu1;
        if (Z.M()> 60 && Z.M()<120) cut.SetCutBit(Mass);
       	if (cut.passAll() ){
            Fill("DYAnalysis/Vars/Ptlll_"+ label,systname, Z.Pt(),e->weight()) ;
        }
    }
    //Log(__FUNCTION__,"DEBUG","Second Block eem");
    if (e1 != NULL and mu0->Pt() > 25 and e1->Pt()> 25  and doNotDoubleCount){ //emm
        if (e1->Charge() *e0->Charge() == -1)cut.SetCutBit(Leptons);
        Object Z(*e0);
        Z += *e1;
        if (Z.M()> 60 && Z.M()<120) cut.SetCutBit(Mass);
       	if (cut.passAll() ){
            Fill("DYAnalysis/Vars/Ptlll_"+ label,systname, Z.Pt(),e->weight()) ;
        }
    }
    //Log(__FUNCTION__,"DEBUG","End Function");
    return 0;

}

int DYAnalysis::analyze(Event *e, string systname)
{

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");
    analyzeMM(e,systname);
    analyzeEM(e,systname);
    analyzeEE(e,systname);
    analyzeLLL(e,systname);
    return 0;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
