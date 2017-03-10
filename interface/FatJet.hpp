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
class FatJet : virtual public Object, virtual public SmearableComplex
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
    inline float Pt() const override { 
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<"Requested Pt for jet"<<endl;
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" syst=%d",syst)<<endl;
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" type=%d",type)<<endl;
            // --- cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" UncorrPt=%f",p4.Pt())<<endl;
            // --- if (syst>0) cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" NewPt=%f", ptUpSyst[type])<<endl;
            // --- if (syst<0) cout<<"[Jet]::[Pt]::[DEBUG]"<<Form(" NewPt=%f", ptDownSyst[type])<<endl;

            if (syst ==0) return p4.Pt(); 
            //return p4.Pt() *(1.0  + uncSyst[type]*syst );
            if (syst>0 ) return ptUpSyst[type];
            else return ptDownSyst[type];
    }
    inline float E() const override { 
        if (syst == 0) return p4.E(); 
        else return Pt()/p4.Pt() *p4.E();
    }

    inline TLorentzVector & GetP4() override {
        if (syst == 0) return p4;
        if (syst!=0 ) {
            pp4=p4;
            if (p4.Pt()>0) {
                return pp4 *= (Pt()/p4.Pt());
            } else {
                return pp4;
            }
        }
    }

    inline float GetUnc() const { return Pt()/p4.Pt(); }

    inline int IsJet() const {
        return 1;
        //        if (not isValid) return 0;
        //        return IsJetExceptValidity();
    }


};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
