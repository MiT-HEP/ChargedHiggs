#ifndef OBJECT_H
#define OBJECT_H

#include "TLorentzVector.h"
#include "interface/GeneralFunctions.hpp"
#include <iostream>

class CorrectorBase;

class Object
{
    friend class CorrectorBase;
    protected:
        // this class provide the really basic information used at analysis level
        TLorentzVector p4;

        TLorentzVector p4NoCorr;
        bool isCorrect=false;

        // --- Correctors should use this functions
        void Scale ( float value) { p4 *= value; isCorrect=true;}
        void Add (TLorentzVector&v, float c=1) { p4 += (c*v) ; isCorrect=true;}

    public:
        //---
        Object(){};
        ~Object(){};
        //---
        virtual inline void  clearSyst(){}; // reset smearing
        virtual inline float Pt() const {return p4.Pt() ;}
        inline float PtUncorr() const { return p4NoCorr.Pt() ;} // not overwrite
        virtual inline float Eta() const { return p4.Eta(); } 
        virtual inline float Phi() const { return p4.Phi(); }
        virtual inline float E() const { return p4.E(); }
        virtual inline float M() const { return p4.M(); }
        virtual inline int   IsObject() const {return 0;}
        virtual inline float DeltaR(Object &o) const { return p4.DeltaR(o.GetP4()); }
        virtual inline float DeltaEta(Object &o) const {return fabs(p4.Eta()-(o.GetP4()).Eta()); }
        virtual inline float DeltaPhi(Object &o) const {return fabs( ChargedHiggs::deltaPhi( Phi(), o.Phi()) ); }
        virtual inline TLorentzVector & GetP4(){ return p4;}
        virtual void SetP4(TLorentzVector &x);

        double InvMass(Object &o) const  ;

        // pointer versions
        virtual inline double InvMass(Object *o) const { return this->InvMass(*o);} 
        virtual inline float DeltaR(Object *o) const { return this->DeltaR(*o) ;}
        virtual inline float DeltaEta(Object *o) const {return this->DeltaEta(*o); }
        virtual inline float DeltaPhi(Object *o) const {return this->DeltaPhi(*o); }

        // ---
        // copy constructor
        Object( const Object &x ) { p4 = x.p4 ; p4NoCorr = x.p4NoCorr; isCorrect=x.isCorrect;} 
        //assignment
        Object& operator=(const Object &x) { p4 = x.p4 ; p4NoCorr=x.p4NoCorr ; isCorrect=x.isCorrect; return *this;} 
        Object& operator+=(const Object &x) { p4 += x.p4 ; p4NoCorr+=x.p4NoCorr;isCorrect = isCorrect || x.isCorrect; return *this;} 

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
