#include "interface/Tau.hpp"

#include <iostream>
using namespace std;

int Tau::IsTau(){
    if (id<idcut_ ) return 0;
    if (iso>=isocut_ ) return 0;
    return 1;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
