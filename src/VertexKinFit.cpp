#include "interface/VertexKinFit.hpp"
#include "TRandom.h"

void VertexKinfit::Init(){
    Log(__FUNCTION__,"INFO","Init Vertex Kinfit");
}

int VertexKinfit::correct(Event *e){
    int nMuons=0;
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);

        if (lep->GetKFP4().Pt() >5){
            SetP4(*lep,lep->GetKFP4());
            ++nMuons;
        }

        if (nMuons>=2) break; // only the first two muons.

    }//end lepton
    return 0;
    // end correct
}

void FSRRecovery::Init(){
    Log(__FUNCTION__,"INFO","Init FSR Recovery");
   
    /* 
    if (egmcorrections){
        Log(__FUNCTION__,"INFO","Init ScaleAndSmear Corrections.");
        if (year == 2016)
        corrector_ . reset ( new EnergyScaleCorrection("Legacy2016_07Aug2017_FineEtaR9_v3_ele_unc") ) ;
        if (year == 2017)
        corrector_ . reset ( new EnergyScaleCorrection("Run2017_17Nov2017_v1_ele_unc") ) ;
        if (year == 2018)
        corrector_ . reset ( new EnergyScaleCorrection("Run2018_Step2Closure_CoarseEtaR9Gain_v2") ) ;

        //corrector_ -> doScale = true;
        //corrector_ -> doSmearings = true;
    }
    */
}

int FSRRecovery::correct(Event *e){
    int nMuons=0;
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);

        TLorentzVector p4=lep->GetP4();
        TLorentzVector fsrP4=lep->GetFsrP4();

        if (e->eventNum() == 3086640) Log(__FUNCTION__,"DEBUG-SYNC",Form("P4=%f fsr (uncorr)=%f",p4.Pt(),fsrP4.Pt()));

        // - if (egmcorrections){
        // -     float pt= fsrP4.Pt();
        // -     float eta= fabs(fsrP4.Eta());
        // -     bool isBarrel = abs(eta) <1.4442;
        // -     bool isEndcap = abs(eta) > 1.566;
        // -     float r9 = 0.99; //assume good photons
        // -     unsigned int gainSeed=12; // the switch should be for very energetic photons
        // -     if ( e->IsRealData()){
        // -         double dataSF = corrector_->scaleCorr(e->runNum(),pt,eta,r9,gainSeed);
        // -         fsrP4*=dataSF;
        // -     } //DATA
        // -     else {
        // -         double random = gRandom->Gaus(1,1);
        // -         double mcSF = corrector_->smearingSigma(e->runNum(), pt,eta, r9,gainSeed,1.,0.); // rho and phi
        // -         fsrP4*=mcSF*random;
        // -         if (e->eventNum() == 3086640) Log(__FUNCTION__,"DEBUG-SYNC",Form("fsr (corr)=%f SF=%f",fsrP4.Pt(),mcSF));
        // -     } //MC

        // - }
    

        // correct only muons with pt> 20
        if (fsrP4.Pt() >20 and (maxpt <0 or fsrP4.Pt() <maxpt) and (maxrelpt<0 or fsrP4.Pt()/p4.Pt()<maxrelpt )){
            if (not onlyiso)
            {
                p4+= fsrP4;
                SetP4(*lep,p4);
            }
            lep->SetIso(  lep->Isolation() - fsrP4.Pt() );
            lep->SetMiniIso ( lep->MiniIsolation() - fsrP4.Pt());
        }

    }//end lepton
    return 0;
    // end correct
}




// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
