#ifndef GENERAL_FUNCT_H
#define GENERAL_FUNCT_H

#ifndef NULL
    #define NULL 0
#endif

#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <TLorentzVector.h>

namespace ChargedHiggs{

    // --- delta phi between -pi and pi
    float deltaPhi(const float phi1,const float phi2);

    // --- call delete and set to null
    template<typename T> 
    void Delete(T& x) { delete x; x=NULL; }

    // compute mt for massive vectors
    float mtMassive( const TLorentzVector p1,  const TLorentzVector p2);
    // mt for massless particles
    float mt(const float pt1, const float pt2, const float phi1, const float phi2);

    // use this for massive objects
    template <typename T>
    inline float mt( const T& obj1, const T& obj2)
    {
        float constexpr pi = float(std::acos(-1));
        const float m1 = obj1.M();
        const float pt1= obj1.Pt();
        const float m2 = obj2.M();
        const float pt2= obj2.Pt();
        const float dphi = deltaPhi(obj1.Phi() , obj2.Phi() );

        const float pt12 = std::pow(pt1,2) + std::pow(pt2,2) - 2*pt1*pt2 * std::cos( pi  - dphi);

        const float et1 = std::sqrt( std::pow(m1,2) + std::pow(pt1,2) );
        const float et2 = std::sqrt( std::pow(m2,2) + std::pow(pt2,2) );
        return std::sqrt( std::pow(et1 + et2,2) - pt12);
    }

    double CosThetaCS(const TLorentzVector *v1, const TLorentzVector*v2, float sqrtS=13) ;

    constexpr float Mw = 80.385;
    constexpr float Mh = 125.;
    constexpr float Mtop = 173.34;

   template<typename T>
   std::string printBinary(const T& x){ std::bitset<sizeof(T)*8> b(x); return b.to_string() ; }

   template<typename T>
   std::string printBinary(const T& x,unsigned max){ std::bitset<sizeof(T)*8> b(x); std::string s= b.to_string(); return s.substr(b.size()-max) ;  }

};

namespace Binning{
    // can I template this ? 
    // bins, find the bin, return i if in [i,i+1)
    int findBin(const std::vector<float> &v, float x);
    // return the formmatted string "bin0_bin1"
    std::string findBinStr(const std::vector<float> &v, float x,const char* precision="%.0f_%.0f");
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
