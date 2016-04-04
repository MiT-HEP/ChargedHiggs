#ifndef GENERAL_FUNCT_H
#define GENERAL_FUNCT_H

#ifndef NULL
    #define NULL 0
#endif

#include <vector>
#include <string>

class TLorentzVector;

namespace ChargedHiggs{
    // ---
    float deltaPhi(const float phi1,const float phi2);
    // --- call delete and set to null
    template<typename T> 
    void Delete(T& x) { delete x; x=NULL; }

    float mt(const float pt1, const float pt2, const float phi1, const float phi2);

    double CosThetaCS(const TLorentzVector *v1, const TLorentzVector*v2, float sqrtS=13) ;
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
