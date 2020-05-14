#include <math.h>
#include "interface/SDMasscorr.hpp"
using namespace std;

/// --- Corrector
//
void SDMasscorr::Init(){
    Log(__FUNCTION__,"INFO","Init SDMasscorr");

  // https://twiki.cern.ch/twiki/bin/view/CMS/JetWtagging#Recipes_to_obtain_the_PUPPI_soft
    _file.reset(TFile::Open("aux/puppiCorr.root"));
    puppisd_corrGEN. reset ( (TF1*) _file->Get("puppiJECcorr_gen"));
    puppisd_corrRECO_cen. reset ( (TF1*) _file->Get("puppiJECcorr_reco_0eta1v3"));
    puppisd_corrRECO_for. reset ( (TF1*) _file->Get("puppiJECcorr_reco_1v3eta2v5"));

}

float SDMasscorr::getPUPPIweight(float puppipt, float puppieta ){

  float genCorr  = 1.;
  float recoCorr = 1.;
  float totalWeight = 1.;

  genCorr =  puppisd_corrGEN->Eval( puppipt );
  if( fabs(puppieta)  <= 1.3 ){
    recoCorr = puppisd_corrRECO_cen->Eval( puppipt );
  }
  else{
    recoCorr = puppisd_corrRECO_for->Eval( puppipt );
  }

  totalWeight = genCorr * recoCorr;

  return totalWeight;
}


int SDMasscorr::correct(Event *e){
    for (auto & jet : GetFatJetVector(e))
    {
      
      float puppiCorr = getPUPPIweight( jet->Pt() , jet->Eta() );
      //      Log(__FUNCTION__,"DEBUG",Form("puppi corr before %f",jet->SDMass()));
      ScaleSDMass(*jet,puppiCorr);
      //      Log(__FUNCTION__,"DEBUG",Form("puppi corr after %f",jet->SDMass()));

    }//end jet

    return 0;
    // end correct
}
