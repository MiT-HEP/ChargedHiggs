#ifndef JET_H
#define JET_H

#include "interface/Object.hpp"

// ---
class Jet : virtual public Object
{
    // This class take care of the jet definition in the analysis
    //
    float ptcut_; // ** pt cut on the accepted jets
    float etacut_ ; // ** eta cut on the accepted jets
    float betacut_ ; // ** eta cut on the accepted bjets
    float bcut_; /// ** bcut on the bJets discr
    float puidcut_;
    float etacutcentral_;
    public:

    Jet() ; 

    int isValid; // rejected by DR

    float unc; // TOFILL
    int syst ;

    float bdiscr; // 
    float bunc; // TOFILL
    int bsyst ;

    float puId;

    //Gen-level info
    int pdgId;
    int motherPdgId;
    int grMotherPdgId;

    // ---
    inline float Pt(){ if (syst ==0) return p4.Pt(); return p4.Pt() *(1.0  + unc*syst );}
    virtual inline void  clearSyst(){syst = 0;bsyst=0; isValid=1;}; // reset smearing
    // ---
    virtual inline int   IsObject(){return IsJet();}

    inline int IsJet() {
        if (Pt() < ptcut_ || fabs(Eta()) > etacut_) return 0; 
        else return 1;
    }

    inline int IsBJet(){
        if( bdiscr < bcut_ || !IsJet() || fabs(Eta()) > betacut_) return 0;
        else return 1;
    }

    inline int IsCentralJet() {
        if (not IsJet() || fabs(Eta()) >= etacutcentral_) return 0;
        else return 1;
    }

    inline void computeValidity( Object* o, float dR = 0.4)
    {
        if ( DeltaR (*o) < dR )  isValid = 0;
    }
    inline void resetValidity(){isValid=1;}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
