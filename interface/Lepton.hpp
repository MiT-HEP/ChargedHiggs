#ifndef LEPTON_H
#define LEPTON_H

#include "interface/Object.hpp"
#include "interface/Trigger.hpp"
#include "interface/Smearable.hpp"

class Lepton : virtual public Object,
    virtual public Trigger,
    virtual public SmearableBase
{
    friend class TagAndProbe;

        TLorentzVector pp4;
    protected:
        float isocut_;
        float ptcut_;
        float etacut_;
        float isorelcut_ {-1};
        float miniisorelcut_ {-1};
        bool   tightcut_ {0};  // ask for tight cut
    public:
        inline void SetIsoCut(float x){isocut_=x;}
        inline void SetPtCut( float x){ptcut_=x;}
        inline void SetIsoRelCut( float x){isorelcut_=x;}
        inline void SetMiniIsoRelCut( float x){miniisorelcut_=x;}
        inline void SetEtaCut( float x){etacut_=x;}
        inline void SetTightCut( bool x=true){tightcut_=x;}

        inline float GetIsoCut()const {return isocut_;}
        inline float GetPtCut() const { return ptcut_;}
        inline float GetEtaCut() const { return etacut_;}
        inline float GetIsoRelCut() const { return isorelcut_;}
        inline float GetMiniIsoRelCut() const { return miniisorelcut_;}
        inline bool  GetTightCut() const { return tightcut_;}
        inline int Charge() const { return charge; }

        inline TLorentzVector & GetP4() override {
            if (syst == 0) return pp4;
            if (syst!=0 ) {
                pp4=p4;
                if (p4.Pt()>0) {
                    return pp4 *= (Pt()/p4.Pt());
                } else {
                    return pp4;
                }
            }
        }

        Lepton() ;

        float iso; // isolation 
        float miniIso; // mini-isolation
        int charge; // charge +1 -1
        int type;// abspdgid 11 o 13 
        int tightId;

        virtual inline int IsLep() const { 
            if ( etacut_ >=0 and abs(Eta()) > etacut_) return 0;
            if ( isocut_ >=0 and iso > isocut_) return 0;
            if ( isorelcut_ >=0 and iso/Pt() > isorelcut_) return 0;
            if ( miniisorelcut_ >=0 and miniIso/Pt() > miniisorelcut_) return 0;
            if ( Pt() < ptcut_ ) return 0;
            if ( tightcut_ and not tightId) return 0;
            return 1;
        }

        virtual inline bool IsElectron() const { return IsLep() and (type == 11); }
        virtual inline bool IsMuon() const { return IsLep() and (type == 13); }
        virtual inline bool IsTight() const { return IsLep() and tightId; }
        inline int   IsObject() const override {return IsLep();} // TODO, const, check that nothing broke
        virtual inline float Isolation() const { return iso; } 
        virtual inline float MiniIsolation() const { return miniIso; }

        // these functions are used in order to avoid the lep checks
        virtual inline bool IsElectronDirty() const { return type == 11; }
        virtual inline bool IsMuonDirty() const { return type == 13; }
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
