#include "interface/Tau.hpp"

#include <iostream>
using namespace std;

Tau::Tau() : Lepton(){
    ptcut_ = 41;
    isocut_= 1.5; 
    idcut_ = 0.5; 

    match = -999; ;  // matching with generator
    iso2 = -999;  // Iso with Delta beta correction
    id_ele= -1; 
    id_mu=-1;
}

int Tau::IsTau(){
    if (id<idcut_ ) return 0;
    if (iso2 >= isocut_ ) return 0;
    return 1;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
