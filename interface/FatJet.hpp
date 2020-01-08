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
    float tau21cut_ ; // To Set

    //    float corrprunedMasscut_ ;
    //    float prunedMasscut_ ;
    float softdropMasscut_ ;
    float nSubjetscut_ ;
    float hbbcut_;

    float hadFlavor_;

    float TvsQCDMDcut_;
    float WvsQCDMDcut_;
    float ZHbbvsQCDMDcut_;
    float ZHccvsQCDMDcut_;


    // FIXME: add puppi

    TLorentzVector pp4;

    public:

    void SetPtCut(float x){ptcut_= x;}
    void SetEtaCut(float x){etacut_ = x;}

    void SetTau1Cut(float x){tau1cut_ = x;}
    void SetTau2Cut(float x){tau2cut_ = x;}
    void SetTau3Cut(float x){tau3cut_ = x;}

    void SetTau21Cut(float x){tau21cut_ = x;}

    //    void SetCorrPrunedMassCut(float x){corrprunedMasscut_ = x;}
    //    void SetPrunedMassCut(float x){prunedMasscut_ = x;}
    void SetSDMassCut(float x){softdropMasscut_ = x;}
    void SetSubjetsCut(float x){nSubjetscut_ = x;}

    void SetDoubleBBCut(float x){hbbcut_ = x;}
    void SetHadFlavor(float x) {hadFlavor_=x;}

    FatJet() ; 
    float tau1; //
    float tau2; //
    float tau3; //
    float TvsQCDMD; //
    float WvsQCDMD; //
    float ZHbbvsQCDMD; //
    float ZHccvsQCDMD; //

    float sdtau1;
    float sdtau2;
    float sdtau3;
    float softdropMass;
    float CorrectedPrunedMass;
    int nSubjets;
    float subjet_btag;
    int hasSubJetBTag{0};
    int hasSubJetBTagLoose{0};

    int isValid;
    //Gen-level info

    // ---

    inline float Tau1() const { return tau1 ; }
    inline float Tau2() const { return tau2 ; }
    inline float Tau3() const { return tau3 ; }

    inline float SDTau1() const { return sdtau1 ; }
    inline float SDTau2() const { return sdtau2 ; }
    inline float SDTau3() const { return sdtau3 ; }

    inline float TvsQCD() const { return TvsQCDMD ; }
    inline float WvsQCD() const { return WvsQCDMD ; }
    inline float ZHbbvsQCD() const { return ZHbbvsQCDMD ; }
    inline float ZHccvsQCD() const { return ZHccvsQCDMD ; }
    //    inline float SDTau1() const { return sdtau1 ; }
    //    inline float SDTau2() const { return sdtau2 ; }
    //    inline float SDTau3() const { return sdtau3 ; }

    inline float SDMass() const { return softdropMass ; }
    inline float CorrPrunedMass() const { return CorrectedPrunedMass ; }

    inline int IsSubjetBTag() const { return hasSubJetBTag ; }
    inline int IsSubjetBTagLoose() const { return hasSubJetBTagLoose ; }

    inline int hadFlavor() const { return hadFlavor_;}

    inline int IsJetExceptValidity() const {
        if( std::isnan(Pt()) ) return 0;
        if( Pt() < ptcut_ ) return 0;
        if( fabs(Eta()) >= etacut_) return 0;
        if( fabs(SDMass()) < softdropMasscut_) return 0;
        return 1;
    }

    inline int IsFatJet() const {
        if (not isValid) return 0;
        return IsJetExceptValidity();
    }

    // for VBS
    //    inline int IsWJet() const { if( Pt() > 200. and softdropMass > 65. and softdropMass < 105. and tau2 > 0 and tau1 > 0 and tau2 < tau1*0.6 and IsFatJet() )   return 1; return 0;}
    inline int IsWJet() const { if( Pt() > 200. and softdropMass > 65. and softdropMass < 105. and tau2 > 0 and tau1 > 0 and tau2 < tau1*0.45 and IsFatJet() )   return 1; return 0;}

    /* // for ChargedHiggs
    // tipically 250 GeV
//    inline int IsWJet() const { if( Pt() > 200. and softdropMass > 65. and softdropMass < 105.  and tau2 < tau1*0.6  and IsJet() )   return 1; return 0;}
//    inline int IsWJetMirror() const { if( Pt() > 200. and softdropMass > 65. and softdropMass < 105.  and tau2 > tau1*0.6  and IsJet() )   return 1; return 0;}
    inline int IsWJet() const { if( Pt() > 200. and softdropMass > 65. and softdropMass < 105. and tau2 > 0 and tau1 > 0 and tau2 < tau1*0.6  and IsFatJet() and IsSubjetBTagLoose() == 0 )   return 1; return 0;}
    */
    inline int IsWJetMirror() const { if( Pt() > 200. and softdropMass > 65. and softdropMass < 105. and tau2 > 0 and tau1 > 0 and tau2 > tau1*0.6  and IsFatJet() and IsSubjetBTagLoose() == 0 )   return 1; return 0;}
    // tipically 400 GeV
//    inline int IsTopJet() const { if( softdropMass > 105. and softdropMass < 220. and tau3 < tau2*0.81  and IsFatJet() and IsSubjetBTag()>0)   return 1; return 0;}
    inline int IsTopJet() const { if( Pt() > 400. and softdropMass > 105. and softdropMass < 220. and tau3 > 0 and tau2 > 0 and tau3 < tau2*0.81  and IsFatJet() ) return 1; return 0;}
    inline int IsTopJetMirror() const { if( Pt() > 400. and softdropMass > 105. and softdropMass < 220. and tau3 > 0 and tau2 > 0 and tau3 > tau2*0.81  and IsFatJet() ) return 1; return 0;}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
