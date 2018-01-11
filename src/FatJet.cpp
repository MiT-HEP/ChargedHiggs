#include "interface/FatJet.hpp"

FatJet::FatJet() :Object(){
  syst=0;
  isValid=1;
  ptcut_=100;
  etacut_=3.;
  tau1cut_=0;
  tau2cut_=0;
  tau3cut_=0;
  tau21cut_=0;
  hadFlavor_=-1;
  corrprunedMasscut_=0;
  prunedMasscut_=0;
  softdropMasscut_=0;
  nSubjetscut_=0;
  hbbcut_=0;
}
