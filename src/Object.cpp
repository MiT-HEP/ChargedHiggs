#include "interface/Object.hpp"
#include "TMath.h"
#include <iostream>


float Object::InvMass(Object &o){ 
  TLorentzVector hp4(p4);
  return (hp4.operator+=(o.GetP4())).M(); 
}
