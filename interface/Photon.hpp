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


        float iso; // isolation 
        int id; // 
        float r9{0.};
        float sieie{0.};
        float hoe{0.};
    public:
        inline void SetEtaCut(float x) {etacut_ = x;}
        inline void SetPtCut(float x) {ptcut_ = x;}
        inline void SetIsoCut(float x) {isocut_ = x;}
        inline void SetIsoRelCut( float x){isorelcut_=x;}
        inline float GetIsoRelCut() const { return isorelcut_;}
        Photon() ; // in the cpp


        inline void SetId(int x){id=x;}
        inline void SetIso(float x){iso=x;}
        inline void SetR9(float x){r9=x;}
        inline void SetHoE(float x){hoe=x;}
        inline void SetSieie(float x){sieie=x;}

        virtual inline int IsPho() const { 
            if (not id ) return 0; // medium
            if ( std::isnan(Pt()) ) return 0; // put pt before eta
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
