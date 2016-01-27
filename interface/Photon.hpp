#ifndef PHOTON_H
#define PHOTON_H

#include "interface/Object.hpp"
#include "interface/Trigger.hpp"

class Photon : virtual public Object,
    virtual public Trigger
{
    friend class TagAndProbe;

    protected:
        float isocut_;
        float ptcut_;
    public:
        inline void SetPtCut(float x) {ptcut_ = x;}
        inline void SetIsoCut(float x) {isocut_ = x;}
        Photon() ; // in the cpp

        float iso; // isolation 
        int id; // 

        virtual inline int IsPho() const { 
            if ( iso > isocut_) return 0;
            if ( Pt() < ptcut_ ) return 0;

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
