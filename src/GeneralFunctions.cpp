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

// where is Form?
#include "TH1D.h"
// --------------- BINS ------------
int Binning::findBin(const std::vector<float> &v, float x)
{
    int R=-1; // Can be binary search
    for( int i=0; i<v.size() -1 ; ++i){
        if (v[i]>=x and v[i+1]<x) { R=i; break;}
    }
    return R;
}

std::string Binning::findBinStr(const std::vector<float> &v, float x, const char* precision)
{
    int bin= findBin(v,x);
    if (bin<0) return "NotFound";
    return Form(precision, v[bin],v[bin+1]);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
