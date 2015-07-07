#include "interface/Jet.hpp"

Jet::Jet() : Object(){
    syst = 0;
    unc =0; 
    bdiscr = 0; 
    bsyst = 0; 
    bunc=0; 
    isValid=1;
    pdgId=0;
    motherPdgId=0;
    grMotherPdgId=0;
    puId=0;
    // -- this cut should remain constant
    bcut_=0.814; // define bjets
    ptcut_=20.;
    etacut_=4,7;
    betacut_=2.5;
    puidcut_=0;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
