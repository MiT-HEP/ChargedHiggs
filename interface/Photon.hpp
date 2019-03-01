#ifndef PHOTON_H
#define PHOTON_H

#include "interface/Object.hpp"
#include "interface/Trigger.hpp"

class Photon : virtual public Object,
    virtual public Trigger
{
    protected:
        float isocut_;
        float etacut_ {-1}; // ** eta cut on the accepted photons
        float ptcut_;
        float isorelcut_{-1};
    public:
        inline void SetEtaCut(float x) {etacut_ = x;}
        inline void SetPtCut(float x) {ptcut_ = x;}
        inline void SetIsoCut(float x) {isocut_ = x;}
        inline void SetIsoRelCut( float x){isorelcut_=x;}
        inline float GetIsoRelCut() const { return isorelcut_;}
        Photon() ; // in the cpp

        float iso; // isolation 
        int id; // 

        virtual inline int IsPho() const { 
            if (not id ) return 0; // medium
            if ( isocut_ > 0 and iso > isocut_) return 0;
            if ( ptcut_ > 0 and Pt() < ptcut_ ) return 0;
            if ( etacut_ > 0 and std::abs(Eta()) > etacut_ ) return 0;
            if ( isorelcut_ >=0 and iso/Pt() > isorelcut_) return 0;

            return 1;
        }

        inline int  IsObject() const override {return IsPho();}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
