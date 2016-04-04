#include "interface/AnalysisGG.hpp"

void GGAnalysis::Init(){

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
        Book(    name() + "/CutFlow/CutFlow_"+ l  , ("CutFlow "+ l).c_str(),100,-.5,100-.5);
    }

    InitTree("ggTree");
    Branch("ggTree","mgg",'F');
    Branch("ggTree","weight",'F');
    Branch("ggTree","pt",'F'); 
    Branch("ggTree","y",'F');
    Branch("ggTree","costheta",'F');
    Branch("ggTree","njets",'F');
    Branch("ggTree","njetsextended",'F');
    // jet0
    Branch("ggTree","ptjet0",'F');
    Branch("ggTree","qgljet0",'F');
    Branch("ggTree","btagjet0",'F');
    // jet1
    Branch("ggTree","mjj",'F');
}

int GGAnalysis::analyze(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;
    
    Fill( name() + "/CutFlow/CutFlow_"+label , systname, Total, e->weight() );
    

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");

    Photon *g1 = e->GetPhoton(0);
    Photon *g2 = e->GetPhoton(1);

    if ( g1 !=NULL )  Fill( name() + "/CutFlow/CutFlow_"+label , systname, Photon1, e->weight() ) ;

    if (g1 == NULL or g2 == NULL ) return 0;

    Fill( name() + "/CutFlow/CutFlow_"+label , systname, Photon2, e->weight() ) ;

    float mgg = g1->InvMass( *g2);
    if (mgg > 200 ) cut.SetCutBit(Mass); // invariant mass cut 
    else return 0; // TODO USE CUTS --- 

    Fill( name() + "/CutFlow/CutFlow_"+label , systname, Mass, e->weight() ) ;

    Object *gg; *gg = *g1 + *g2;

    SetTreeVar("mgg", g1->InvMass( *g2) );
    SetTreeVar("weight", e->weight() ) ;


    SetTreeVar("pt",gg->Pt());
    SetTreeVar("y",gg->Rapidity());
    SetTreeVar("costheta",ChargedHiggs::CosThetaCS(&g1->GetP4(), &g2->GetP4(),13));
    SetTreeVar("njets", e->NcentralJets());
    SetTreeVar("njetsextended", e->Njets());

    Jet *j = e->GetJet(0);
    if (j != NULL)
    {
        SetTreeVar("ptjet0", j->Pt());
        SetTreeVar("qgljet0",j->QGL());
        SetTreeVar("btagjet0",j->Btag());

        Jet *j2= e->GetJet(1);
        if (j2 != NULL) { SetTreeVar("mjj", j->InvMass( *j2) ); }
        else            { SetTreeVar("mjj", 0.0); }
        
    }
    else
    {
        SetTreeVar("ptjet0", 0);
        SetTreeVar("qgljet0",-1);
        SetTreeVar("btagjet0",-1);
    }

    // ---
    FillTree("ggTree");
    //---
    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
