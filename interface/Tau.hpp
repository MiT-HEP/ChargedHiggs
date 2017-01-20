#ifndef TAU_H
#define TAU_H

#include "interface/Lepton.hpp"
#include "interface/GenParticle.hpp"
#include "interface/Trigger.hpp"
#include "interface/Smearable.hpp"

class Event;

class Tau: virtual public Object,
    virtual public Lepton,
    virtual public Trigger,
    virtual public SmearableBase
{
    float etacut_; 
    bool doEleRej_;
    bool doMuRej_;
    int rematch_ {-1};
    float escale_{0.03};
    int nprong=0;
    int npizero = 0;
    float trackpt_{0};
    float trackptcut_{0};

    int selectprongs_{-1};
    bool oldId{0};

    int selectdecay_{-1};


    public:
    void inline SetOldId(const bool &x){oldId=x;}
    void inline SetTrackPt(const float& x){trackpt_=x;}
    void inline SetTrackPtCut(const float& x){trackptcut_=x;}
    void inline SetEtaCut(const float& x){etacut_=x;}
    void inline SetProngsCut(const int& x){selectprongs_=x;}
    void inline SetDecayMode(const int& x){selectdecay_=x;} // 0=NEW, 1=OLD

    void inline SetMuRej(bool x ) { doMuRej_ = x;}
    void inline SetEleRej(bool x ) { doEleRej_ = x;}
    void inline SetNProng(int n) {nprong=n;}
    void inline SetNPiZero(int n){npizero=n;}

    int inline GetNProng() const { return nprong;}
    int inline GetNPiZero() const { return npizero;}

    Tau() ;
    bool id;
    float iso2;
    bool id_ele =0;
    bool id_mu =0 ;
    bool id_iso = 0 ; // iso switch for isolation. In miniAOD they include more
    int match ; // is matched with a gen tau

    virtual int MyIsTau() const ; // tmp delete
    virtual int IsTau() const ;
    virtual int IsTauInvIso() const ;
    inline int IsObject() const override{ return IsTau(); }
    inline float Pt() const override {
        if (syst==0)return p4.Pt() ;
        else return (p4.Pt() * (1.0+syst*escale_));
    }

    // Return 15 = tau, 21 = Gluon, 1-4 = UDSC (maybe only 1) 
    // 0 no match
    int Rematch(Event *e,float dR=0.4);

    // this one uses the one written in the ntuples.
    // decommission ? 
    virtual bool IsMatch( ) { if (match >= 0) return true; else return false;}

    virtual void clearSyst(){
        Lepton::clearSyst(); 
        Object::clearSyst();
        syst=0;
        }

    // --- REGRESSION 
    struct regression{
        float nvtx; 
        float tauPt;
        float tauEta;
        float tauM;
        float tauQ;
        float tauIso;
        float tauIso2;
        float tauChargedIsoPtSum;
        float tauNeutralIsoPtSum;
        float jetPt;
        float jetEta;
    } regVars_;
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
