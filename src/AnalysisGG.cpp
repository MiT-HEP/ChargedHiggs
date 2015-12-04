#include "interface/AnalysisGG.hpp"

void GGAnalysis::Init(){

    for ( string l : AllLabel()  ) {
	    Log(__FUNCTION__,"INFO","Booking Histo Mass");
    }

}

int GGAnalysis::analyze(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
