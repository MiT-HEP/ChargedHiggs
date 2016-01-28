#include "interface/GeneralFunctions.hpp"
#include "TMath.h"

float ChargedHiggs::deltaPhi(const float phi1,const float phi2)
{
    float dphi = phi1 - phi2;
    while (dphi > TMath::Pi() ) dphi -= TMath::TwoPi(); 
    while (dphi < -TMath::Pi() ) dphi += TMath::TwoPi();
    return dphi;
}

float ChargedHiggs::mt( const float pt1,  const float pt2, const  float phi1, const float phi2)
{
    return TMath::Sqrt( 2* pt1 * pt2 * ( 1.-TMath::Cos(ChargedHiggs::deltaPhi(phi1,phi2)) ) );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
