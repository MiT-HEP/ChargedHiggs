#include "interface/Tau.hpp"


int Tau::IsTau(){
	if (id<idcut_ ) return 0;
	if (iso<isocut_ ) return 0;
	return 1;
}
