#ifndef LEPTON_H
#define LEPTON_H

#include "interface/Object.hpp"
#include "interface/Trigger.hpp"

class Lepton : virtual public Object,
    virtual public Trigger
{
    friend class TagAndProbe;

    protected:
        float isocut_;
        float ptcut_;
        float etacut_;
        float isorelcut_ {-1};
    public:
        inline void SetIsoCut(float x){isocut_=x;}
        inline void SetPtCut( float x){ptcut_=x;}
        inline void SetIsoRelCut( float x){isorelcut_=x;}
        inline void SetEtaCut( float x){etacut_=x;}

        inline float GetIsoCut()const {return isocut_;}
        inline float GetPtCut() const { return ptcut_;}
        inline float GetEtaCut() const { return etacut_;}
        inline float GetIsoRelCut() const { return isorelcut_;}
        inline int Charge() const { return charge; }

        Lepton() ;

        float iso; // isolation 
        int charge; // charge +1 -1
        int type;// abspdgid 11 o 13 

        virtual inline int IsLep() const { 
            if ( etacut_ >=0 and abs(Eta()) > etacut_) return 0;
            if ( isocut_ >=0 and iso > isocut_) return 0;
            if ( isorelcut_ >=0 and iso/Pt() > isorelcut_) return 0;
            if ( Pt() < ptcut_ ) return 0;

            return 1;
        }
        virtual inline bool IsElectron() const { return IsLep() and (type == 11); }
        virtual inline bool IsMuon() const { return IsLep() and (type == 13); }
        virtual inline int   IsObject(){return IsLep();}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
