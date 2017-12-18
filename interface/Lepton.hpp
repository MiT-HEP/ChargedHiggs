#ifndef LEPTON_H
#define LEPTON_H

#include "interface/Object.hpp"
#include "interface/Trigger.hpp"
#include "interface/Smearable.hpp"

class Lepton : virtual public Object,
    virtual public Trigger,
    virtual public SmearableBase
{
    friend class CorrectorBase;
        //Temporary class        
        TLorentzVector pp4; // temporary for syst -- will be overwritten
    protected:
        // these are the values for this lepton
        float iso {0}; // isolation 
        float miniIso {0}; // mini-isolation
        int charge {0}; // charge +1 -1
        int type {11};// abspdgid 11 o 13 
        int tightId {0};
        int mediumId {0};
        int mediumIdOrig {0}; // original definition of medium id, w/o 2016 modifications
        int looseId{0};
        int trackerMuon {0};
        int globalMuon {0};
        int nlayers{0};
        float mva{-1}; // mva

        // ele specific
        float pfPt_{0}; // w/o scale and smears for ele, pf pt for muons
        float r9_{0};
        float etaSC_{0};

        //  these are the cut we want to apply
        float isocut_;
        float ptcut_;
        float etacut_;
        bool mvaLoosecutEta_{false};
        float isorelcut_ {-1};
        float miniisorelcut_ {-1};
        bool  tightcut_ {0};  // ask for tight cut
        bool  mediumcut_ {0};
        bool  mediumorigcut_ {0};
        bool  loosecut_ {0};
        bool  trackermuoncut_ {0};
        bool  globalmuoncut_ {0};

    public:
        inline void SetR9(float x) { r9_=x;}
        inline void SetEtaSC(float x) { etaSC_=x;}
        inline void SetPfPt(float x) { pfPt_=x;}
        inline void SetIsoCut(float x){isocut_=x;}
        inline void SetPtCut( float x){ptcut_=x;}
        inline void SetIsoRelCut( float x){isorelcut_=x;}
        inline void SetMiniIsoRelCut( float x){miniisorelcut_=x;}
        inline void SetEtaCut( float x){etacut_=x;}
        inline void SetMvaLooseCut(bool x){mvaLoosecutEta_=x;}
        inline void SetVetoCut(){tightcut_=false;mediumcut_=false; loosecut_=false;mediumorigcut_=false;mvaLoosecutEta_=false;trackermuoncut_=false;globalmuoncut_=false;}
        inline void SetLooseCut(bool x){loosecut_=x;}
        inline void SetTightCut( bool x=true){tightcut_=x;}
        inline void SetMediumCut( bool x=true){mediumcut_=x;}
        inline void SetMediumOrigCut( bool x=true){mediumorigcut_=x;}
        inline void SetIso(float x=0.0){iso=x;}
        inline void SetType(int x){type=x;}
        inline void SetMiniIso(float x=0.0){miniIso=x;}
        inline void SetCharge(int x){charge=x;}
        inline void SetTightId( bool x=true){tightId=x;}
        inline void SetMediumId( bool x=true){mediumId=x;}
        inline void SetMediumIdOrig( bool x=true){mediumIdOrig=x;}
        inline void SetLooseId( bool x=true){looseId=x;}
        inline void SetMva(float x){mva=x;}
        inline void SetNLayers(int x){nlayers=x;}
        inline void SetTrackerMuon(bool x=true){trackerMuon=x;}
        inline void SetGlobalMuon(bool x=true){globalMuon=x;}
        inline void SetTrackerMuonCut(bool x=true){trackermuoncut_=x;}
        inline void SetGlobalMuonCut(bool x=true){globalmuoncut_=x;}

        inline float GetIso()const {return iso;}
        inline bool GetMediumId() const {return mediumId;}
        inline bool GetTightId() const {return tightId;}
        inline bool GetLooseId() const {return looseId;}
        inline float GetIsoCut()const {return isocut_;}
        inline float GetPtCut() const { return ptcut_;}
        inline float GetEtaCut() const { return etacut_;}
        inline float GetIsoRelCut() const { return isorelcut_;}
        inline float GetMiniIsoRelCut() const { return miniisorelcut_;}
        inline bool  GetTightCut() const { return tightcut_;}
        inline float GetPfPt() const {return pfPt_;}
        inline float GetR9() const {return r9_;}
        inline float GetEtaSC() const {return etaSC_;}
        inline int Charge() const { return charge; }
        inline int GetType() const { return type; } // bypass id
        inline int GetNLayers() const { return nlayers; } // bypass id
        inline bool GetTrackerMuon()const { return trackerMuon;}
        inline bool GetGlobalMuon()const { return globalMuon;}

        inline TLorentzVector & GetP4() override {
            if (syst == 0) return p4;
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

    //    Eta boundary0 -- 0.8, 0.8 -- 1.479, > 1.479
    //        Tight(0.674, 0.744, 0.170)
    //        Loose(-0.041, 0.383, -0.515)

        virtual inline int IsLep() const { 
            if ( std::isnan(Pt()) ) return 0; // put pt before eta
            if ( Pt() < ptcut_ ) return 0; // put pt before eta
            if ( etacut_ >=0 and abs(Eta()) > etacut_) return 0;
            if ( isocut_ >=0 and iso > isocut_) return 0;
            if ( isorelcut_ >=0 and iso/Pt() > isorelcut_) return 0;
            if ( miniisorelcut_ >=0 and miniIso > miniisorelcut_) return 0; //miniIso is already relative to Pt
            if ( type == 11 and mvaLoosecutEta_ and (
                                                     (Mva()<-0.041 and abs(Eta())<0.8) or
                                                     (Mva()<0.383 and abs(Eta())<=1.4442 and abs(Eta())>=0.8) or
                                                     (Mva()<-0.515 and abs(Eta())>1.566) or
                                                     ( abs(Eta())>1.4442 and abs(Eta())<=1.566)
                                                     )) return 0;

            if ( tightcut_ and not tightId) return 0;
            if ( mediumcut_ and not mediumId) return 0;
            if ( mediumorigcut_ and not mediumIdOrig) return 0;
            if ( loosecut_ and not looseId) return 0;
            if ( trackermuoncut_ and  IsMuonDirty() and not trackerMuon) return 0;
            if ( globalmuoncut_ and  IsMuonDirty() and not globalMuon) return 0;
            return 1;
        }

    virtual inline bool IsEleMvaTight() const { return IsElectron() and  (
                                                                          (Mva()>=0.674 and abs(Eta())<0.8) or
                                                                          (Mva()>=0.744 and abs(Eta())<=1.4442 and abs(Eta())>=0.8) or
                                                                          (Mva()>=0.170 and abs(Eta())>1.566) ) ; }

        virtual inline bool IsElectron() const { return IsLep() and (type == 11); }
        virtual inline bool IsMuon() const { return IsLep() and (type == 13); }
        virtual inline bool IsLoose() const { return IsLep() and looseId; }
        virtual inline bool IsMedium() const { return IsLep() and mediumId; }
        virtual inline bool IsTight() const { return IsLep() and tightId; }
        inline int   IsObject() const override {return IsLep();} // TODO, const, check that nothing broke
        virtual inline float Isolation() const { return iso; } 
        virtual inline float MiniIsolation() const { return miniIso; }
        inline float Mva() const { return mva; }

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
