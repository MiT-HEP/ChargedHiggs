#ifndef TAU_H
#define TAU_H

#include "interface/Lepton.hpp"

class Tau: virtual public Object,
	virtual public Lepton
{
	float idcut_;
public:
	Tau() : Lepton() { idcut_ = 0.5;}
	float id;
	virtual int IsTau() ;
	virtual inline int IsObject(){ return IsTau(); }
};

#endif
