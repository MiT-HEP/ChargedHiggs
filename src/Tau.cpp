#include "interface/Tau.hpp"

#include <iostream>
using namespace std;

Tau::Tau() : Lepton(){
    etacut_= 2.1;
    ptcut_ = 20;
    isocut_= 1.5;  // inv iso may be different

    match = -999; ;  // matching with generator
    iso2 = -999;  // Iso with Delta beta correction
}

int Tau::IsTau(){
    if ( not id ) return 0;
    if ( not id_ele) return 0;
    if ( not id_mu) return 0;
    if (iso2 >= isocut_ ) return 0;
    if ( Pt() < ptcut_ ) return 0;
    if ( fabs(Eta() ) > etacut_) return 0;
    return 1;
}

int Tau::IsTauInvIso(){
    if ( not id ) return 0;
    if ( not id_ele) return 0;
    if ( not id_mu) return 0;
    if (iso2 < 3.0 ) return 0;
    if (iso2 > 20.0 ) return 0;
    if ( Pt() < ptcut_ ) return 0;
    return 1;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
