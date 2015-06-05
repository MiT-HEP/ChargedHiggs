#ifndef LEPTON_H
#define LEPTON_H

#include "interface/Object.hpp"

class Lepton : virtual public Object
{
protected:
	float isocut_;
public:
	Lepton() ;

	float iso; // isolation 
	int charge; // charge +1 -1
	int type;// abspdgid 11 o 13 
	
	virtual inline int IsLep(){ 
		if (iso> isocut_) return 0;
	       	return 1;
		}
	virtual inline int   IsObject(){return IsLep();}
};

#endif
