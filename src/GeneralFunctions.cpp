#include "interface/GeneralFunctions.hpp"
#include "TMath.h"
#include <cmath>

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

#include "TLorentzVector.h"

double ChargedHiggs::CosThetaCS( const TLorentzVector *v1, const TLorentzVector *v2,float sqrtS){
    
    TLorentzVector b1,b2,v;
    b1.SetPx(0); b1.SetPy(0);
    b2.SetPx(0); b2.SetPy(0);
    double beamE = 500.*sqrtS; // 1/2 sqrtS in GeV
    b1.SetPz( beamE); b1.SetE(beamE);
    b2.SetPz(-beamE); b2.SetE(beamE);
    
    v=*v1+*v2;
    TVector3 boostToVFrame = -v.BoostVector();

    // Boost to higgs frame
    TLorentzVector refV_v1 = *v1; refV_v1.Boost(boostToVFrame);
    TLorentzVector refV_b1 = b1; refV_b1.Boost(boostToVFrame);
    TLorentzVector refV_b2 = b2; refV_b2.Boost(boostToVFrame);

    // Getting beam 3-vector from 4-vectors
    TVector3 refV_vb1_direction = refV_b1.Vect().Unit();
    TVector3 refV_vb2_direction = refV_b2.Vect().Unit();

    // Definition of zz directions
    TVector3 direction_cs = (refV_vb1_direction - refV_vb2_direction).Unit(); // CS direction

    return TMath::Cos(direction_cs.Angle(refV_v1.Vect()));
}


// where is Form?
#include "TH1D.h"
// --------------- BINS ------------
int Binning::findBin(const std::vector<float> &v, float x)
{
    int R=-1; // Can be binary search
    for( int i=0; i<v.size() -1 ; ++i){
        if (v[i]<=x and x<v[i+1]) { R=i; break;}
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
