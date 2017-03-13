#ifndef FATJET_H
#define FATJET_H

#include "interface/Object.hpp"
#include "interface/Smearable.hpp"
#include <algorithm>
#include <string>
#include <map>

//#include <iostream>
//using namespace std;

// ---
class FatJet : virtual public Object, virtual public SmearableBase
{
    // This class take care of the jet definition in the analysis
    //
    float ptcut_; // ** pt cut on the accepted jets
    float etacut_ ; // ** eta cut on the accepted jets
    float tau1cut_ ; // To Set
    float tau2cut_ ; // To Set
    float tau3cut_ ; // To Set

    float corrprunedMasscut_ ;
    float prunedMasscut_ ;
    float softdropMasscut_ ;
    float nSubjetscut_ ;
    float hbbcut_;

    // FIXME: add puppi

    TLorentzVector pp4;

    public:

    void SetPtCut(float x){ptcut_= x;}
    void SetEtaCut(float x){etacut_ = x;}

    void SetTau1Cut(float x){tau1cut_ = x;}
    void SetTau2Cut(float x){tau2cut_ = x;}
    void SetTau3Cut(float x){tau3cut_ = x;}

    void SetCorrPrunedMassCut(float x){corrprunedMasscut_ = x;}
    void SetPrunedMassCut(float x){prunedMasscut_ = x;}
    void SetSDMassCut(float x){softdropMasscut_ = x;}
    void SetSubjetsCut(float x){nSubjetscut_ = x;}

    void SetDoubleBBCut(float x){hbbcut_ = x;}


    FatJet() ; 


    float bdiscr; // 

    //Gen-level info

    // ---

    inline int IsJet() const { return 1;}


};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
