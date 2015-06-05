#include "interface/Jet.hpp"

Jet::Jet() : Object(){
	syst = 0;
	unc =0; 
	bdiscr = 0; 
	bsyst = 0; 
	bunc=0; 
	isValid=1;
	// -- this cut should remain constant
	bcut_=0.5; // define bjets
	ptcut_=30.;
	etacut_=4.7;
}

