#include "interface/Object.hpp"
#include "TMath.h"
#include <iostream>

using namespace std;

double Object::InvMass(Object &o) const { 
  TLorentzVector hp4(p4);
  return (hp4.operator+=(o.GetP4())).M(); 
}

void Object::SetP4(TLorentzVector &x){  
    p4NoCorr = x;
    p4 =x ;
    //p4NoCorr.SetXYZT(x.X(),x.Y(),x.Z(),x.T() ); 
    //p4.SetXYZT(x.X(),x.Y(),x.Z(),x.T() ) ; 
    isCorrect=false; 
}
