#include "interface/Jet.hpp"
#include "interface/Logger.hpp"

Jet::Jet() : Object(){
    syst = 0;
    bdiscr = 0; 
    isValid=1;
    pdgId=0;
    motherPdgId=0;
    grMotherPdgId=0;
    puId=0;
    hadFlavor_=-1;
    // -- this cut should remain constant
    bcut_=0.814; // define bjets
    ptcut_=20.;
    etacut_=4.7;
    betacut_=2.4;
    puidcut_=0;
    etacutcentral_=2.4;

}

int Jet::IsBJet() const {
    if (bcut_ > -100 and deepbcut_>-100) {
        Logger::getInstance().Log("Jet",__FUNCTION__,"ERROR",Form("Cannot set both standard and deep B requirements: bcut=%f deepbcut=%f. Unset one (-100)",bcut_,deepbcut_));
        throw abortException();
    }
    if ( fabs(Eta()) > betacut_ ) return 0;
    if (bcut_ >-99. and bdiscr <= bcut_ ) return 0;
    if (deepbcut_ > -99. and GetDeepB() <= deepbcut_ ) return 0;
    if (not IsJet() ) return 0;
    return 1;

}

int Jet::IsBJetInvIso()const{
    //if( bdiscr > bcut_  and IsJetInvIso() and fabs(Eta()) <= betacut_ )   return 1; return 0;
    if ( fabs(Eta()) > betacut_ ) return 0;
    if (bcut_ >-99. and bdiscr <= bcut_ ) return 0;
    if (deepbcut_ > -99. and deepB <= deepbcut_ ) return 0;
    if (not IsJetInvIso() ) return 0;
    return 1;

}

int Jet::PassPuId() const {
    if( puidcut_ > -100 and puidcut_ < 100 and puId < puidcut_ ) return 0; 
    if(puidcut_ == 100 ) { // Loose
        float aeta= abs(Eta());
        float pt = p4.Pt(); // no syst
        if ( aeta< 2.5){
            if (pt >=30 and pt< 50 and puId <-0.89)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.97)  return 0;
        }
        else if (aeta < 2.75){
            if (pt >=30 and pt< 50 and puId <-0.52)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.68)  return 0;
        }
        else if (aeta < 3.00){
            if (pt >=30 and pt< 50 and puId <-0.38)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.53)  return 0;
        }
        else if (aeta < 5.00){
            if (pt >=30 and pt< 50 and puId <-0.30)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.47)  return 0;
        }
    }
    if(puidcut_ == 200 ) { // Medium
        float aeta= abs(Eta());
        float pt = p4.Pt(); // no syst
        if ( aeta< 2.5){
            if (pt >=30 and pt< 50 and puId <+0.61)  return 0;
            if (pt >=10 and pt< 30 and puId <+0.18)  return 0;
        }
        else if (aeta < 2.75){
            if (pt >=30 and pt< 50 and puId <-0.35)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.55)  return 0;
        }
        else if (aeta < 3.00){
            if (pt >=30 and pt< 50 and puId <-0.23)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.42)  return 0;
        }
        else if (aeta < 5.00){
            if (pt >=30 and pt< 50 and puId <-0.17)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.36)  return 0;
        }
    }
    if(puidcut_ == 300 ) { // Tight
        float aeta= abs(Eta());
        float pt = p4.Pt(); // no syst
        if ( aeta< 2.5){
            if (pt >=30 and pt< 50 and puId <+0.86)  return 0;
            if (pt >=10 and pt< 30 and puId <+0.69)  return 0;
        }
        else if (aeta < 2.75){
            if (pt >=30 and pt< 50 and puId <-0.10)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.35)  return 0;
        }
        else if (aeta < 3.00){
            if (pt >=30 and pt< 50 and puId <-0.05)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.26)  return 0;
        }
        else if (aeta < 5.00){
            if (pt >=30 and pt< 50 and puId <-0.01)  return 0;
            if (pt >=10 and pt< 30 and puId <-0.21)  return 0;
        }
    }
    return 1;
}

int Jet::PassEENoise()const {
    // -- if (eenoise_) {
    // --     float aeta= abs(Eta());
    // --     float pt = p4.Pt(); // no syst
    // --     // MET Recipe v2 for EE noise

    // --     if (rawPt < 50 and aeta <3.139 and aeta > 2.65)
    // --         return 0;
    // --     
    // --     // SMP recommendation
    // --     //if (rawPt < 50 and 2.7 < aeta and aeta < 3.0 and nemf_>0.55 )
    // --     //    return 0;
    // -- }
    // in house Hmm: 2.6< aeta< 3.0 tight pileup id in 2017.
    //
    if (eenoise_){
       float aeta= abs(Eta());
       float pt = p4.Pt(); // no syst
       if (2.6 <aeta and aeta <3.0){
            if (pt >=30 and puId <-0.10)  return 0; // also above 50 GeV
            if (pt >=10 and pt< 30 and puId <-0.35)  return 0;
       }
    }

    return 1;
}

// HEM -3.0 < eta < -1.3, -1.57 < phi < -0.87) for Run2018C and D
// 20 % for jets with -1.57 <phi< -0.87 and -2.5<eta<-1.3
// 35 % for jets with -1.57 <phi< -0.87 and -3.0<eta<-2.5
// (all jets with pt>15 GeV passing the tight ID -in order to reject 
// muons/electrons-, and propagate this to the MET as well)

int Jet::IsJetExceptValidity() const { 
    if( std::isnan(Pt()) ) return 0; 
    if( Pt() < ptcut_ ) return 0; 
    if( fabs(Eta()) >= etacut_) return 0;
    if (PassPuId() == 0) return 0;
    if (PassEENoise() ==0 ) return 0;
    return 1;
}



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
