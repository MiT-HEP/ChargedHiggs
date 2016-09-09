#ifndef EVENT_H
#define EVENT_H

#include "TLorentzVector.h"
#include <vector>
#include <map>
using namespace std;

// -----
#include "interface/Object.hpp"
#include "interface/Jet.hpp"
#include "interface/Lepton.hpp"
#include "interface/Tau.hpp"
#include "interface/Met.hpp"
#include "interface/Photon.hpp"
#include "interface/GenParticle.hpp"

// ----
#include "interface/Weight.hpp"
#include "interface/GeneralFunctions.hpp"

class Looper;
class AnalysisBase;
class ChargedHiggsEWKEmbedding;
class Loader;
class LoadNero;

class Event{
    friend class Looper;
    friend class Loader;
    friend class LoadNero;
    friend class SmearBase;
    friend class AnalysisBase;
    friend class CorrectorBase;
    friend class ChargedHiggsEWKEmbedding;  // embedding will replace the taus --- should be made a corrector

    protected:
    vector<Lepton*> leps_;
    vector<Jet*>    jets_;
    vector<Tau*>    taus_;
    vector<Photon*> phos_;
    vector<GenParticle*>    genparticles_; // gen particles
    Met met_;
    vector<bool>    triggerFired_;
    int isRealData_;
    int runNum_;
    int lumiNum_;
    unsigned eventNum_;
    float rho_;
    int npv_;

    vector<string> triggerNames_;

    string fName_;
    Weight *weight_;

    public:

    //--- Constructor
    Event();
    ~Event();

    Weight *GetWeight(){ return weight_;} 

    inline int runNum(){return runNum_; }
    inline int lumiNum(){return lumiNum_; }
    inline unsigned eventNum(){return eventNum_; }
    inline string GetName()const{ return fName_;}

    // Bare version  -- don't use these functions, unless for debug or sync
    inline Jet * GetBareJet(unsigned iJet) const { if (jets_.size() <= iJet) return (Jet*) NULL; return jets_[iJet]; }
    inline Tau * GetBareTau(unsigned iTau) const { if (taus_.size() <= iTau) return (Tau*) NULL; return taus_[iTau]; }
    inline Lepton * GetBareLepton(unsigned iLepton) const { if (leps_.size() <= iLepton) return (Lepton*) NULL; return leps_[iLepton]; }
    inline Photon * GetBarePhoton(unsigned iPhoton) const { if (phos_.size() <= iPhoton) return (Photon*) NULL; return phos_[iPhoton]; }

    // ---
    
    // This functions should check if the objects are valid
    // Get NULL in case of failure
    Jet * GetJet( int iJet );
    Jet * GetJetInvIso( int iJet );
    Jet * GetCentralJet( int iJet );
    Jet * GetBjet( int iJet );
    Jet * GetBjetInvIso( int iJet );
    Jet * GetLjet( int iJet );
    Tau * GetTau( int iTau );
    Tau * GetTauInvIso( int iTau );
    Lepton * GetLepton( int iLep );
    Lepton * GetElectron( int iEle );
    Lepton * GetMuon( int iMu );
    GenParticle * GetGenParticle( int iGenPar ); // ?!?
    GenParticle * GetGenStable( int iGenPar ,int pdgid, float aeta=2.5);
    //
    inline GenParticle * GetGenElectron( int iGenPar, float aeta = 2.5 ){return GetGenStable(iGenPar,11,aeta);}
    inline GenParticle * GetGenMuon( int iGenPar, float aeta=2.5 ){return GetGenStable(iGenPar,13,aeta);}

    Photon * GetPhoton( int iPho );

    //
    inline Met GetMet( ) const { return met_;} // should be const,  -- FIXED? but noCorrPt is not set correctly without &

    inline float Rho() { return rho_; }
    inline int Npv() { return npv_ ;} 
    inline float Ht()   { float ht=0 ; for(auto j : jets_ ) if( j->IsJet()  ) ht+= j->Pt() ; return ht;}

    inline int Njets(){int n=0; for(auto j : jets_) if(j->IsJet()) n++; return n;}
    inline int NjetsInvIso(){int n=0; for(auto j : jets_) if(j->IsJetInvIso()) n++; return n;}
    inline int NcentralJets(){int n=0; for(auto j : jets_) if(j->IsCentralJet()) n++; return n;}
    inline int NforwardJets(){int n=0; for(auto j : jets_) if(j->IsForwardJet()) n++; return n;}
    inline int Bjets(){int n=0; for(auto j : jets_) if(j->IsBJet()) n++; return n;}
    inline int Ljets(){int n=0; for(auto j : jets_) if(not j->IsBJet()) n++; return n;}
    inline int BjetsInvIso(){int n=0; for(auto j : jets_) if(j->IsBJetInvIso()) n++; return n;}
    inline int Ntaus(){int n=0; for(auto t : taus_) if(t->IsTau()) n++; return n;}
    inline int Nleps(){int n=0; for(auto t : leps_) if(t->IsLep()) n++; return n;}
    inline int NGenPar(){return genparticles_.size();}

    inline Tau*  LeadTau(){ return GetTau(0);} 
    inline Jet*  LeadJet(){ return GetJet(0);}
    inline Jet*  LeadBjet(){ return GetBjet(0);}

    inline float Mvis() {  TLorentzVector e(0,0,0,0); 
        for(auto l : leps_ )  if ( l->IsLep() ) e += l->GetP4(); 
        for(auto j : jets_ )  if ( j->IsJet() ) e += j->GetP4();
        for(auto t : taus_ )  if ( t->IsTau() ) e += t->GetP4();
        return e.M();
    }
    enum MtType { MtTau =0 , MtMuon, MtLepton, MtTauInv };
    float Mt(MtType type=MtTau ) ; // 0 tau, 1 muon ,...

    // Q and 
    inline float MtDecoQ(Object *t,float mt0 = ChargedHiggs::Mw) const {if (t==NULL) return -999; return 1- std::pow(mt0,2)/(2*t->Pt()*GetMet().Pt());} 
    inline float MtDecoCosPhi(Object *t) const { if (t==NULL) return -10; return std::cos(GetMet().DeltaPhi(*t)) ;} 
    float MtDecoQ(MtType type=MtTau, float mt0=ChargedHiggs::Mw);
    float MtDecoCosPhi(MtType type=MtTau);



    // --------- Angular variables
    inline float RbbMin(int iMax=3) { return RbbMin(iMax,GetTau(0) ) ; }
    inline float RCollMin(int iMax=3) { return RCollMin(iMax, GetTau(0)); }
    inline float RsrMax(int iMax=3) { return RsrMax(iMax, GetTau(0)); } 

    float RbbMin(int iMax,Tau *t) ;
    float RCollMin(int iMax,Tau *t) ;
    float RsrMax(int iMax,Tau *t) ;

    inline float DPhiEtMissJet(int iJet=0){ Jet*j= GetJet(iJet) ; if( j == NULL) return -1; return fabs(GetMet().DeltaPhi(j) );}
    inline float DPhiTauJet(Tau*t,int iJet=0){Jet*j=GetJet(iJet) ; if (j==NULL) return -1 ;if(t==NULL) return -1; return fabs(j->DeltaPhi(*t));}

    //-----------------------------
    virtual void ClearEvent();
    virtual inline int IsRealData(){ return isRealData_ ;} 
    virtual void clearSyst(); // call clearSyst in all object it posses
    double weight(bool safe=true); // safe will return 1 for data, ALWAYS
    // update objects that can be invalid (jets)
    virtual void validate();
    bool IsTriggered(string name, Trigger *trigger = NULL, bool isNone=false); // the None do not check if it is the L3 or LF -- only for taus and matching

    // SF utils
    inline void SetPtEtaSF(string label, float pt, float eta){  weight_ -> SetPtEtaSF(label,pt,eta);}
    inline void SetWPSF(string label, int wp){  weight_ -> SetWPSF(label,wp);}
    inline void SetJetFlavorSF(string label, int flavor){  weight_ -> SetJetFlavorSF(label,flavor);}
    void ApplySF(string label){ weight_ -> ApplySF(label) ; } 
    bool ExistSF(string label){ return weight_ -> ExistSF(label); }

    // 
    void ApplyTopReweight();
    void ApplyWReweight();
    void ApplyBTagSF(int wp=0);
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
