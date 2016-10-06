#include "interface/Tau.hpp"

#include <iostream>
using namespace std;

Tau::Tau() : Lepton(){
    etacut_= 0;
    ptcut_ = 0;
    isocut_= 0;  // inv iso may be different
    trackptcut_=-1;

    match = -999; ;  // matching with generator
    iso2 = -999;  // Iso with Delta beta correction
}

int Tau::IsTau() const {
    if ( not id ) return 0;
    if ( doEleRej_   and not id_ele) return 0;
    if ( doMuRej_    and not id_mu) return 0;
    if ( isocut_ >=0 and iso2 >= isocut_ ) return 0;
    if ( Pt() < ptcut_ ) return 0;
    if ( fabs(Eta() ) > etacut_) return 0;
    if ( trackptcut_ >0 and trackptcut_ > trackpt_) return 0;
    if ( GetNProng() != 1) return 0;
    if ( not id_iso ) return 0; // this include additional cuts on top of the iso cut
    return 1;
}

int Tau::IsTauInvIso() const {
    if ( not id ) return 0;
    if ( doEleRej_ and not id_ele) return 0;
    if ( doMuRej_ and not id_mu) return 0;
    if (iso2 < 3.0 ) return 0;
    //#warning loose inv Iso tau
    //if (iso2 < 2.5 ) return 0;
    if (iso2 > 20.0 ) return 0;
    if ( Pt() < ptcut_ ) return 0;
    if ( GetNProng() != 1) return 0;
    if ( trackptcut_ >0 and trackptcut_ > trackpt_) return 0;
    return 1;
}

#include "interface/Event.hpp"

int Tau::Rematch(Event *e,float dR){
    if (rematch_ >=0 ) return rematch_; // cache

    GenParticle * gp =NULL;
    int ig=0;
    bool isTau=false;
    bool isQuark=false;
    bool isGluon=false;
    bool isEle = false;
    bool isMuon= false;
    float hardestPt=-1.;
    bool isHardestQ = false;

    for (gp = e->GetGenParticle(ig) ; gp != NULL ; gp=e->GetGenParticle(++ig))
    {
        if  (gp->DeltaR(this) >dR) continue;
        //if  (gp->IsPromptFinalState() ) continue; // useless
        if  ( abs(gp->GetPdgId()) == 15 ) isTau = true; // I don't care the status
        if ( abs(gp->GetPdgId()) == 11) 
        {
            isEle = true;
        }
        if ( abs(gp->GetPdgId()) == 13) 
        {
            isMuon = true;
        }
        if ( abs(gp->GetPdgId() ) == 21 )
            {
            isGluon=true;
            if (hardestPt < gp->Pt() ) { hardestPt=gp->Pt(); isHardestQ=false;}
            }
        if ( abs(gp->GetPdgId() ) <= 4 )
            {
            isQuark=true;
            if (hardestPt < gp->Pt() ) { hardestPt=gp->Pt(); isHardestQ=true;}
            }
    }
    if (isTau) rematch_=15;
    //the additional check isQuark, prevent the default value on isHardest
    if (rematch_<0 and isMuon) rematch_=13;
    if (rematch_<0 and isEle) rematch_=11;
    if (rematch_ <0 and isQuark and isHardestQ) rematch_=1;
    if (rematch_ <0 and isGluon and not isHardestQ) rematch_=21;
    // no match
    if (rematch_ <0 ) rematch_=0;
    return rematch_;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
