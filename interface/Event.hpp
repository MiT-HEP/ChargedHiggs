#ifndef EVENT_H
#define EVENT_H

#include "TLorentzVector.h"
#include <vector>
#include <map>
using namespace std;

// -----
#include "interface/Object.hpp"
#include "interface/Jet.hpp"
#include "interface/FatJet.hpp"
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

/**
 * @brief Event class containing a complete set of event
 */
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
    vector<FatJet*>    fat_;
    vector<Tau*>    taus_;
    vector<Photon*> phos_;
    vector<GenParticle*>    genparticles_; // gen particles
    vector<GenJet*> genjets_;

    Met met_;
    vector<bool>    triggerFired_;
    int isRealData_;
    int runNum_;
    int lumiNum_;
    unsigned eventNum_;
    float rho_;
    int npv_;

    int genTtbarId_;

    vector<string> triggerNames_;

    string fName_;
    string tag_{""};
    Weight *weight_;

    public:

    //--- Constructor
    Event();
    ~Event();

    Weight *GetWeight(){ return weight_;} 

    ///@brief run number
    inline int runNum(){return runNum_; }
    ///@brief luminosity block number
    inline int lumiNum(){return lumiNum_; }
    ///@brief event number
    inline unsigned eventNum(){return eventNum_; }
    ///@brief  file name being processed
    inline string GetName()const{ return fName_;}

    // Bare version  -- don't use these functions, unless for debug or sync
    ///@brief get ntuple jet, without selection (Bare), without resorting.
    inline Jet * GetBareJet(unsigned iJet) const { if (jets_.size() <= iJet) return (Jet*) NULL; return jets_[iJet]; }
    ///@brief get ntuple fatjet, without selection (Bare), without resorting.
    inline FatJet * GetBareFatJet(unsigned iJet) const { if (fat_.size() <= iJet) return (FatJet*) NULL; return fat_[iJet]; }
    ///@brief get ntuple tau, without selection (Bare), without resorting.
    inline Tau * GetBareTau(unsigned iTau) const { if (taus_.size() <= iTau) return (Tau*) NULL; return taus_[iTau]; }
    ///@brief get ntuple lepton, without selection (Bare), without resorting.
    inline Lepton * GetBareLepton(unsigned iLepton) const { if (leps_.size() <= iLepton) return (Lepton*) NULL; return leps_[iLepton]; }
    ///@brief get ntuple photon, without selection (Bare), without resorting.
    inline Photon * GetBarePhoton(unsigned iPhoton) const { if (phos_.size() <= iPhoton) return (Photon*) NULL; return phos_[iPhoton]; }

    //

    ///@brief get gen id tag for ttbar
    inline int GetGenTtbarId() const { return genTtbarId_; }
    ///@brief set gen id tag for ttbar
    inline void SetGenTtbarId (const int x){ genTtbarId_=x; }

    // ---
    
    // This functions should check if the objects are valid
    // Get NULL in case of failure
    ///@brief Get cleaned jet (iJet) in the event. Pt Sorted.
    Jet * GetJet( int iJet );
    ///@brief Get cleaned jet (iJet) in the event. Tau cleaning is done with the inverted isolation requirement. Pt Sorted.
    Jet * GetJetInvIso( int iJet );
    ///@brief Get cleaned jet (iJet) in the event and central eta cut. Pt Sorted.
    Jet * GetCentralJet( int iJet );
    ///@brief Get cleaned bjet in the event. Pt sorted.
    Jet * GetBjet( int iJet );
    ///@brief Get cleaned bjet in the event. Tau cleaning is done with the inverted isolation requirement. Pt sorted.
    Jet * GetBjetInvIso( int iJet );
    ///@brief Get non-b tagged jets in the event. Pt sorted.
    Jet * GetLjet( int iJet );
    ///@brief Get Fat jet. Pt sorted.
    FatJet * GetFatJet( int iJet );
    FatJet * GetWJet( int iJet );
    FatJet * GetTopJet( int iJet );
    ///@brief Get Tau. Pt sorted.
    Tau * GetTau( int iTau );
    ///@brief Get Tau with inverted isolation requirements. Pt sorted.
    Tau * GetTauInvIso( int iTau );
    ///@brief Get Leptons. Pt sorted.
    Lepton * GetLepton( int iLep );
    ///@brief Get Electron. Pt sorted.
    Lepton * GetElectron( int iEle );
    ///@brief Get Muon. Pt sorted.
    Lepton * GetMuon( int iMu );
    ///@brief Get Photon. Pt Sorted.
    Photon * GetPhoton( int iPho );

    /*****************************************
     **                GEN                  **
     *****************************************/
    ///@brief Get Gen Jet. (No sorting)
    GenJet * GetGenJet(int iGenJet){ if (iGenJet>=0 and iGenJet<genjets_.size() ) return genjets_[iGenJet]; return NULL;}
    ///@brief Get Gen Particle. (No sorting)
    GenParticle * GetGenParticle( int iGenPar ); // ?!?
    ///@brief Return true if gen particle iGenPar decayd from a particle with |pdg| =apdgid
    ///@param iGenPar gen particle index
    ///@param apdgid |pdgid| of the mother
    inline bool GenParticleDecayedFrom( int iGenPar, int apdgid ){int idx; return GenParticleDecayedFrom(iGenPar,apdgid ,idx);}
    ///@brief Return true if gen particle iGenPar decayd from a particle with |pdg| =apdgid and idx position of the apdgid particle
    ///@param iGenPar gen particle index
    ///@param apdgid |pdgid| of the mother
    ///@param idx index position of the mother particle
    bool GenParticleDecayedFrom( int iGenPar, int apdgid ,int& idx);
    ///@brief Get Stable gen particle with max |eta| requirement. Pt Sorted.
    GenParticle * GetGenStable( int iGenPar ,int pdgid, float aeta=2.5);
    ///@brief Get Stable gen electron with max |eta| requirement. Pt Sorted.
    inline GenParticle * GetGenElectron( int iGenPar, float aeta = 2.5 ){return GetGenStable(iGenPar,11,aeta);}
    ///@brief Get Stable gen muon with max |eta| requirement. Pt Sorted.
    inline GenParticle * GetGenMuon( int iGenPar, float aeta=2.5 ){return GetGenStable(iGenPar,13,aeta);}

    // 
    /*****************************************
     **                MET                  **
     *****************************************/
    ///@brief get met
    inline Met GetMet( ) const { return met_;} // should be const,  -- FIXED? but noCorrPt is not set correctly without &

    ///@brief get rho
    inline float Rho() const { return rho_; }
    ///@brief get n. primary vertexes
    inline int Npv() const { return npv_ ;} 
    ///@brief get ht from selected jets
    inline float Ht() const   { float ht=0 ; for(auto j : jets_ ) if( j->IsJet()  ) ht+= j->Pt() ; return ht;}
    ///@brief get ht from selected jets with inverse isolation requirement on the tau
    inline float HtInvIso() const   { float ht=0 ; for(auto j : jets_ ) if( j->IsJetInvIso()  ) ht+= j->Pt() ; return ht;}

    ///@brief get n. of selected jets
    inline int Njets()const {int n=0; for(auto j : jets_) if(j->IsJet()) n++; return n;}
    inline int NjetsInvIso()const {int n=0; for(auto j : jets_) if(j->IsJetInvIso()) n++; return n;}
    inline int NcentralJets() const {int n=0; for(auto j : jets_) if(j->IsCentralJet()) n++; return n;}
    inline int NcentralJetsInvIso() const {int n=0; for(auto j : jets_) if(j->IsCentralJetInvIso()) n++; return n;}
    inline int NforwardJets()const {int n=0; for(auto j : jets_) if(j->IsForwardJet()) n++; return n;}
    inline int Bjets()const {int n=0; for(auto j : jets_) if(j->IsBJet()) n++; return n;}
    inline int Wjets()const {int n=0; for(auto j : fat_) if(j->IsWJet()) n++; return n;}
    inline int Topjets()const {int n=0; for(auto j : fat_) if(j->IsTopJet()) n++; return n;}
    inline int WjetsMirror()const {int n=0; for(auto j : fat_) if(j->IsWJetMirror()) n++; return n;}
    inline int TopjetsMirror()const {int n=0; for(auto j : fat_) if(j->IsTopJetMirror()) n++; return n;}
    inline int Ljets()const {int n=0; for(auto j : jets_) if(not j->IsBJet()) n++; return n;}
    inline int BjetsInvIso()const {int n=0; for(auto j : jets_) if(j->IsBJetInvIso()) n++; return n;}
    inline int NFatJets()const {int n=0; for(auto j : fat_) if(j->IsFatJet()) n++; return n;}
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
    void ApplyBTagSF(int wp=0);
    void ApplyTauSF(Tau*t,bool prongs=true,const string& extra="");
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
