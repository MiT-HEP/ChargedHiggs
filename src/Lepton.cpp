#include "interface/Lepton.hpp"


Lepton::Lepton() : Object() {
    iso =-1; 
    charge = 0 ;
    isocut_ = 10;
    ptcut_ = 15;
    type=0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
