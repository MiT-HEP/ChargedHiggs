#include "interface/Event.hpp"
#include "interface/GeneralFunctions.hpp"
#include "TMath.h"
#include <iostream>

void Event::ClearEvent(){

    for (auto o :  jets_ ) delete o;
    for (auto o :  leps_ ) delete o;
    for (auto o :  taus_ ) delete o;
    for (auto o :  genparticles_ ) delete o;

    jets_ . clear();
    leps_ . clear();
    taus_ . clear();
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


float Event::Mt() { 
    if ( Ntaus() <=0 ) return -1; 
    float pt_t  =  LeadTau() -> Pt();
    float phi_t =  LeadTau() -> Phi();
    float pt_m = met_ . Pt(); 
    float phi_m= met_. Phi(); 
    return TMath::Sqrt( 2* pt_t * pt_m * ( 1.-TMath::Cos(ChargedHiggs::deltaPhi(phi_t,phi_m)) ) );
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


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
