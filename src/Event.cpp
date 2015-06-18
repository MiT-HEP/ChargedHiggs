#include "interface/Event.hpp"
#include "interface/GeneralFunctions.hpp"
#include "TMath.h"
#include <iostream>

void Event::ClearEvent(){

    for (auto o :  jets_ ) delete o;
    for (auto o :  leps_ ) delete o;
    for (auto o :  taus_ ) delete o;

    jets_ . clear();
    leps_ . clear();
    taus_ . clear();

    weight_ . clearSF( );

}

void Event::clearSyst(){
    for ( auto o: jets_) o->clearSyst();
    for ( auto o: taus_) o->clearSyst();
    for ( auto o: leps_) o->clearSyst();
    met_ . clearSyst();
    // clear SF syst
    weight_ . clearSF();
    weight_ . resetSystSF();
    weight_ . clearSystPU();
}


float Event::Mt() { 
    if ( Ntaus() <=0 ) return -1; 
    float pt_t  = taus_[ LeadTau() ] -> Pt();
    float phi_t = taus_[ LeadTau() ] -> Phi();
    float pt_m = met_ . Pt(); 
    float phi_m= met_. Phi(); 
    return TMath::Sqrt( 2* pt_t * pt_m * TMath::Cos(ChargedHiggs::deltaPhi(phi_t,phi_m) ) );
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

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
