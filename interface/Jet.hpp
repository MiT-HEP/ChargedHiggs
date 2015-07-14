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
    float etacutcentral_;
    float bcut_; /// ** bcut on the bJets discr

    public:

    Jet() ; 

    int isValid; // rejected by DR

    float unc; // TOFILL
    int syst ;

    float bdiscr; // 
    float bunc; // TOFILL
    int bsyst ;

    // ---
    inline float Pt(){ if (syst ==0) return p4.Pt(); return p4.Pt() *(1.0  + unc*syst );}
    virtual inline void  clearSyst(){syst = 0;bsyst=0; isValid=1;}; // reset smearing
    // ---
    virtual inline int   IsObject(){return IsJet();}
    inline int IsJet() { if (not isValid) return 0 ; 
        if( Pt() < ptcut_ ) return 0; 
        if( fabs(Eta()) >= etacut_) return 0;
        return 1;
    }

    inline int IsCentralJet() {
        if ( not IsJet() ) return 0;
        if ( fabs(Eta()) >= etacutcentral_ ) return 0;
    }

    inline int IsBJet(){ if( bdiscr > bcut_ + bsyst*bunc and IsJet() )   return 1; return 0;}

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
