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
#include "interface/GenParticle.hpp"

// ----
#include "interface/Weight.hpp"

class Looper;
class AnalysisBase;

class Event{
    friend class Looper;
    friend class SmearBase;
    friend class AnalysisBase;
    protected:
    vector<Lepton*> leps_;
    vector<Jet*>    jets_;
    vector<Tau*>    taus_;
    vector<GenParticle*>    genparticles_; // gen particles
    Met met_;
    int isRealData_;
    int runNum_;
    float rho_;

    public:
    Weight weight_;

    inline int runNum(){return runNum_; }
    
    // This functions should check if the objects are valid
    // Get NULL in case of failure
    Jet * GetJet( int iJet );
    Jet * GetCentralJet( int iJet );
    Jet * GetBjet( int iJet );
    Tau * GetTau( int iTau );
    Lepton * GetLepton( int iLep );
    Lepton * GetElectron( int iEle );
    Lepton * GetMuon( int iMu );

    //
    inline Met GetMet( ) { return met_;}

    inline float Rho() { return rho_; }
    inline float Ht()   { float ht=0 ; for(auto j : jets_ ) if( j->IsJet()  ) ht+= j->Pt() ; return ht;}
    inline int   Njets(){ int   n=0  ; for(auto j : jets_ ) if( j->IsJet()  ) n+=1; return n; }
    inline int   NcentralJets(){ int   n=0  ; for(auto j : jets_ ) if( j->IsCentralJet()  ) n+=1; return n; }
    inline int   Bjets(){ int   n=0  ; for(auto j : jets_ ) if( j->IsBJet() ) n+=1; return n; }
    inline int   Ntaus(){ int   n=0  ; for(auto t : taus_ ) if( t->IsTau()  )  n+=1; return n; }
    inline int   Nleps(){ int   n=0  ; for(auto t : leps_ ) if( t->IsLep()  )  n+=1; return n; }

    inline Tau*  LeadTau(){ return GetTau(0);} 
    inline Jet*  LeadJet(){ return GetJet(0);}
    inline Jet*  LeadBjet(){ return GetBjet(0);}

    inline float Mvis() {  TLorentzVector e(0,0,0,0); 
        for(auto l : leps_ )  if ( l->IsLep() ) e += l->GetP4(); 
        for(auto j : jets_ )  if ( j->IsJet() ) e += j->GetP4();
        for(auto t : taus_ )  if ( t->IsTau() ) e += t->GetP4();
        return e.M();
    }
    float Mt() ;
    virtual void ClearEvent();
    virtual inline int IsRealData(){ return isRealData_ ;} 
    virtual void clearSyst(); // call clearSyst in all object it posses
    double weight();
    // update objects that can be invalid (jets)
    virtual void validate();
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
