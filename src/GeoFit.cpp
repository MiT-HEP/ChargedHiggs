#include <math.h>
#include "interface/GeoFit.hpp"
using namespace std;

namespace PtGeoCor{

  float PtGeoFit_mod(float d0, float pt, float eta, int year) { 
    float pt_cor = 0.0F;
    if (year == 2016) { 
      if      (abs(eta) < 0.9F) pt_cor = ( 1.183F * tanh(606.21F*d0) + 163.53F * d0 ) * pt * pt / 10000.0F;
      else if (abs(eta) < 1.7F) pt_cor = ( 2.226F * tanh(380.74F*d0) + 244.43F * d0 ) * pt * pt / 10000.0F;
      else                      pt_cor = ( 3.688F * tanh(233.96F*d0) + 398.26F * d0 ) * pt * pt / 10000.0F;
    }
    else if (year == 2017) {
      if      (abs(eta) < 0.9F) pt_cor = ( 0.705F * tanh(948.94F*d0) + 400.52F * d0 ) * pt * pt / 10000.0F;
      else if (abs(eta) < 1.7F) pt_cor = ( 0.853F * tanh(733.68F*d0) + 685.24F * d0 ) * pt * pt / 10000.0F;
      else                      pt_cor = ( 5.518F * tanh(260.07F*d0) + 282.20F * d0 ) * pt * pt / 10000.0F;
    }
    else if (year == 2018) {
      if      (abs(eta) < 0.9F) pt_cor = ( 0.969F * tanh(690.73F*d0) + 294.48F * d0 ) * pt * pt / 10000.0F;
      else if (abs(eta) < 1.7F) pt_cor = ( 1.466F * tanh(527.43F*d0) + 472.16F * d0 ) * pt * pt / 10000.0F;
      else                      pt_cor = ( 1.024F * tanh(618.79F*d0) + 1025.5F * d0 ) * pt * pt / 10000.0F;
    }
    return (pt - pt_cor);
  } // end of float PtGeoFit_mod(float d0, float pt, float eta, int year)

  float PtGeo_BS_Roch(float d0, float pt, float eta, int year) {
    float pt_cor = 0.0;
    if (year == 2016) {
      if      (abs(eta) < 0.9) pt_cor = 411.34 * d0 * pt * pt / 10000.0;
      else if (abs(eta) < 1.7) pt_cor = 673.40 * d0 * pt * pt / 10000.0;
      else                     pt_cor = 1099.0 * d0 * pt * pt / 10000.0;
    }
    else if (year == 2017) {
      if      (abs(eta) < 0.9) pt_cor = 582.32 * d0 * pt * pt / 10000.0;
      else if (abs(eta) < 1.7) pt_cor = 974.05 * d0 * pt * pt / 10000.0;
      else                     pt_cor = 1263.4 * d0 * pt * pt / 10000.0;
    }
    else if (year == 2018) {
      if      (abs(eta) < 0.9) pt_cor = 650.84 * d0 * pt * pt / 10000.0;
      else if (abs(eta) < 1.7) pt_cor = 988.37 * d0 * pt * pt / 10000.0;
      else                     pt_cor = 1484.6 * d0 * pt * pt / 10000.0;
    }
    return (pt - pt_cor);
  } // end of float PtGeo_BS_Roch(float d0, float pt, float eta, int year) 

} // end namespace PtGeoCor


/// --- Corrector
//
void GeoFit::Init(){
    Log(__FUNCTION__,"INFO","Init GeoFit");
}

int GeoFit::correct(Event *e){
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);

        float pt=lep->Pt();
        float pt_cor=pt;
        if (useBS)
        {
            pt_cor=PtGeoCor::PtGeo_BS_Roch(lep->GetDxyBS()*lep->Charge(), lep->Pt(), lep->Eta(), year) ;
        }
        else
        {
            pt_cor=PtGeoCor::PtGeoFit_mod(lep->GetDxy()*lep->Charge(), lep->Pt(), lep->Eta(), year) ;
        }

        //pt_new=pt -pt_cor
        //sf=pt_new/pt = 1. -pt_cor/pt

        //Log(__FUNCTION__,"DEBUG",Form("GeoFit changing muon pt from %f to %f",pt,pt_cor));
        Scale(*lep,pt_cor/pt);
        //Log(__FUNCTION__,"DEBUG",Form("         (Confirm pt)     ------>  %f",lep->Pt()));


    }//end lepton

    return 0;
    // end correct
}


