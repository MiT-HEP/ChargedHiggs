#include "interface/GeneralFunctions.hpp"
#include "TMath.h"

float ChargedHiggs::deltaPhi(float phi1,float phi2)
{
	float dphi = phi1 - phi2;
	while (dphi > TMath::Pi() ) dphi -= TMath::TwoPi(); 
	while (dphi < -TMath::Pi() ) dphi += TMath::TwoPi();
	return dphi;
}
