#include "interface/AnalysisGG.hpp"

void GGAnalysis::Init(){

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
    }

    InitTree("ggTree");
    Branch("ggTree","mgg",'F');
    Branch("ggTree","weight",'F');

}

int GGAnalysis::analyze(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");

    Photon *g1 = e->GetPhoton(0);
    Photon *g2 = e->GetPhoton(1);

    if (g1 == NULL or g2 == NULL ) return 0;

    SetTreeVar("mgg", g1->InvMass( *g2) );
    SetTreeVar("weight", e->weight() ) ;
    FillTree("ggTree");

    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
