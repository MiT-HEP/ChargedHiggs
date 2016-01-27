#include "interface/Event.hpp"
#include "interface/GeneralFunctions.hpp"
#include "TMath.h"
#include <iostream>

//#define VERBOSE 2

void Event::ClearEvent(){

    for (auto o :  jets_ ) ChargedHiggs::Delete(o);
    for (auto o :  leps_ ) ChargedHiggs::Delete(o);
    for (auto o :  taus_ ) ChargedHiggs::Delete(o);
    for (auto o :  genparticles_ ) ChargedHiggs::Delete(o);

    jets_ . clear();
    leps_ . clear();
    taus_ . clear();
    phos_ . clear();
    genparticles_ . clear();

    weight_ . clearSF( );

}

void Event::clearSyst(){
    for ( auto o: jets_) o->clearSyst();
    for ( auto o: taus_) o->clearSyst();
    for ( auto o: leps_) o->clearSyst();
    for ( auto o: genparticles_) o->clearSyst();
    met_ . clearSyst();
    // clear SF syst
    weight_ . clearSF();
    weight_ . resetSystSF();
    weight_ . clearSystPU();
}

float Event::Mt(MtType type)  {  // 0 tau, 1 muon, 2 electron, 3 lepton
    float pt_m = met_ . Pt(); 
    float phi_m= met_. Phi(); 
    switch(type){
    case MtTau:
        {
        if ( Ntaus() <=0 ) return -1; 
        float pt_t  =  LeadTau() -> Pt();
        float phi_t =  LeadTau() -> Phi();
        return ChargedHiggs::mt(pt_t,pt_m,phi_t,phi_m); 
        }
    case MtMuon:
        {
        if (GetMuon(0) == NULL ) return -1;
        float pt_mu = GetMuon(0)-> Pt();
        float phi_mu = GetMuon(0)-> Phi();
        return ChargedHiggs::mt(pt_mu,pt_m,phi_mu,phi_m); 
        }
    case MtTauInv:
        {
            Tau *tInv = GetTauInvIso(0); 
            if (tInv == NULL) return -1;
            float pt_t  =  tInv -> Pt();
            float phi_t =  tInv -> Phi();
            return ChargedHiggs::mt(pt_t,pt_m,phi_t,phi_m); 
        }
    } 
    return -3;
} 

float Event::RbbMin(int iMax,Tau *t) {
    // notice the Pi-...
    if (t == NULL) return -1;
    float dphietmisstau = TMath::Pi() - fabs(GetMet().DeltaPhi( t ) );

    float rbbmin = -1;
    for(int i=0 ; i< iMax; ++i)
    {
        if( GetJet(i) == NULL ) break;
        float dphietmissjet= fabs( GetMet().DeltaPhi( GetJet(i) ) ) ;
        float myrbb = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

        if (rbbmin<0 or myrbb<rbbmin) rbbmin = myrbb;
    }

    return rbbmin;
}
float Event::RCollMin(int iMax,Tau *t) {
    // notice the Pi-...
    if (t == NULL) return -1;
    float dphietmisstau = fabs(GetMet().DeltaPhi( t ) );
    float rcollmin = -1;
    for(int i=0 ; i< iMax; ++i)
    {
        if( GetJet(i) == NULL ) break;
        float dphietmissjet= TMath::Pi() - fabs( GetMet().DeltaPhi( GetJet(i) ) ) ;
        float myrcoll = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

        if (rcollmin<0 or myrcoll<rcollmin) rcollmin = myrcoll;
    }

    return rcollmin;
}

float Event::RsrMax(int iMax, Tau *t) {
    if (t == NULL) return -1;
    float dphietmisstau = TMath::Pi() - fabs(GetMet().DeltaPhi( t ) );
    float rsrmax = -1;
    for(int i=0 ; i< iMax; ++i)
    {
        if( GetJet(i) == NULL ) break;
        float dphietmissjet= TMath::Pi() - fabs( GetMet().DeltaPhi( GetJet(i) ) ) ;
        float myrsr = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

        // CHECKME, is this correct ? min ? 
        if (rsrmax<0 or myrsr<rsrmax) rsrmax = myrsr;
    }

    return rsrmax;
}

double Event::weight(){
    if (isRealData_ ) return 1;
    return weight_ . weight();
}

void Event::validate(){
    //reject all the jets that are identified as : lepton or tau
    for ( auto j : jets_ )
    {
        j-> resetValidity();
        for(auto l : leps_)
            if(l->IsLep() )j-> computeValidity(l);
        for(auto t: taus_)
            if(t->IsTau() )j-> computeValidity(t);
    }
    return ;
}

// Get Object functions
Jet * Event::GetJet( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsJet()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

// Get Object functions
Jet * Event::GetCentralJet( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsCentralJet()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

Jet * Event::GetBjet( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsBJet()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

Tau * Event::GetTau( int iTau ) 
{ // { return taus_.at(iTau);} // old
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<taus_.size() ;++i)
    {
        if ( taus_[i]->IsTau()) valid.push_back(pair<float,int>(taus_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iTau  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return taus_[ valid[iTau].second];
}

Lepton * Event::GetLepton( int iLepton ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<leps_.size() ;++i)
    {
        if ( leps_[i]->IsLep()) valid.push_back(pair<float,int>(leps_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iLepton  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return leps_[ valid[iLepton].second];
}

Lepton * Event::GetElectron( int iEle ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<leps_.size() ;++i)
    {
        if ( leps_[i]->IsLep() and leps_[i]->IsElectron() ) 
            valid.push_back(pair<float,int>(leps_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iEle  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return leps_[ valid[iEle].second];
}

Lepton * Event::GetMuon( int iMu ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<leps_.size() ;++i)
    {
        if ( leps_[i]->IsLep() and leps_[i]->IsMuon() ) 
            valid.push_back(pair<float,int>(leps_[i]->Pt(),i)); 

    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iMu  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return leps_[ valid[iMu].second];
}

Tau * Event::GetTauInvIso( int iTau ) 
{ // { return taus_.at(iTau);} // old
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<taus_.size() ;++i)
    {
        if ( taus_[i]->IsTauInvIso()) valid.push_back(pair<float,int>(taus_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iTau  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return taus_[ valid[iTau].second];
}

bool Event::IsTriggered( string name ,Trigger *trigger, bool isNone)
{
    // TODO: make event inheriths from trigger, and remove this switch
    #ifdef VERBOSE
    if (VERBOSE >1) cout <<"[Event]::[IsTriggered]::[DEBUG] name="<<name<<" trigger="<<trigger<<endl;
    #endif
    
    static string lastName = "";
    static int lastPos = -1;

    //cout <<"name = "<<name<<" last="<<lastName<<" lastPos="<<lastPos<<endl;

    if (name == lastName and lastPos >=0 )
    {
        if (trigger == NULL)
            return triggerFired_[ lastPos ] ;
        else 
        {
            if (isNone)  return trigger -> IsTriggeredNone ( lastPos ) ;
            else return trigger -> IsTriggered( lastPos ) ;
        }
    }
    
    lastPos = -1;
    for (size_t i=0; i <  triggerNames_.size() ;++i)
    {
        if (name == triggerNames_[i] ) { lastPos=i; break;} 
    }
    lastName = name;
    //cout <<"[Event]::[IsTriggered]::[DEBUG] Found trigger menu with name '"<<name<<"' at pos "<<lastPos<<endl;
    if (lastPos >=0 ) {
        #ifdef VERBOSE
        if (VERBOSE >1) cout <<"[Event]::[IsTriggered]::[DEBUG] grace exit"<<endl;
        #endif
        if (trigger == NULL)
            return triggerFired_[ lastPos ] ; 
        else 
        {
            if (isNone) return trigger->IsTriggeredNone (lastPos ) ;
            else return trigger -> IsTriggered( lastPos) ;
        }
    }
   
    // Log only if it's not empty  -- can be used to reset stuff
    if (name != "") cout<<"[Event]::[IsTriggered]::[WARNING] Trigger menu not found: '"<<name<<"'"<<endl;
    return false;
}

Photon * Event::GetPhoton( int iPho ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<phos_.size() ;++i)
    {
        if ( phos_[i]->IsPho()) 
            valid.push_back(pair<float,int>(phos_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iPho  ) return NULL;

    sort(valid.begin(),valid.end(),[](pair<float,int> &a,pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return phos_[ valid[iPho].second];
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
