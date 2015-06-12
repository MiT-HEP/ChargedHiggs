#ifndef CHARGEDHIGGSTAUNU_H
#define CHARGEDHIGGSTAUNU_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsTauNu:  virtual public AnalysisBase
{
public:
	virtual void Init();
	virtual int analyze(Event*,string systname);
	virtual const string name(){return "ChargedHiggsTauNu";}
};

#endif
