#ifndef TAU_H
#define TAU_H

#include "interface/Lepton.hpp"
#include "interface/GenParticle.hpp"
#include "interface/Trigger.hpp"

class Tau: virtual public Object,
    virtual public Lepton,
    virtual public Trigger
{
    float etacut_; 
    bool doEleRej_;
    bool doMuRej_;


    public:
    void SetEtaCut(float x){etacut_=x;}
    void SetMuRej(bool x ) { doMuRej_ = x;}
    void SetEleRej(bool x ) { doEleRej_ = x;}

    Tau() ;
    bool id;
    float iso2;
    bool id_ele =0;
    bool id_mu =0 ;
    int match ; // is matched with a gen tau

    virtual int IsTau() const ;
    virtual int IsTauInvIso() const ;
    inline int IsObject() const override{ return IsTau(); }

    virtual bool IsMatch( ) { if (match >= 0) return true; else return false;}

    virtual void clearSyst(){
        Lepton::clearSyst(); 
        Object::clearSyst();
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
