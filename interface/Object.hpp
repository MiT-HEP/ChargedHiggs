#ifndef OBJECT_H
#define OBJECT_H

#include "TLorentzVector.h"
#include "interface/GeneralFunctions.hpp"
#include <iostream>

class Object
{
    protected:
        // this class provide the really basic information used at analysis level
        TLorentzVector p4;
    public:
        //---
        Object(){};
        ~Object(){};
        //---
        virtual inline void  clearSyst(){}; // reset smearing
        virtual inline float Pt(){return p4.Pt() ;}
        virtual inline float Eta(){ return p4.Eta(); } 
        virtual inline float Phi(){ return p4.Phi(); }
        virtual inline float E(){ return p4.E(); }
        virtual inline float M(){ return p4.M(); }
        virtual inline int   IsObject(){return 0;}
        virtual inline float DeltaR(Object &o){ return p4.DeltaR(o.GetP4()); }
        virtual inline float DeltaEta(Object &o){return fabs(p4.Eta()-(o.GetP4()).Eta()); }
        virtual inline float DeltaPhi(Object &o){return fabs( ChargedHiggs::deltaPhi( Phi(), o.Phi()) ); }
        virtual inline TLorentzVector & GetP4(){ return p4;}
        virtual inline void SetP4(TLorentzVector &x){p4 = x;}

        float InvMass(Object &o);

        // pointer versions
        virtual inline float DeltaR(Object *o){ return DeltaR(*o) ;}
        virtual inline float DeltaEta(Object *o){return DeltaEta(*o); }
        virtual inline float DeltaPhi(Object *o){return DeltaPhi(*o); }

        // ---
        // copy constructor
        Object( const Object &x ) { p4 = x.p4 ; } 
        //assignment
        Object& operator=(const Object &x) { p4 = x.p4 ; return *this;} 
        Object& operator+=(const Object &x) { p4 += x.p4 ; return *this;} 

        // binary 
        const Object operator+(const Object &x) const { Object y(*this); y+=x; return y;} ;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
