#ifndef CHARGEDHIGGSHW_H
#define CHARGEDHIGGSHW_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsHW:  virtual public AnalysisBase
{
public:
	virtual void Init();
	virtual int analyze(Event*,string systname);
	virtual const string name(){return "ChargedHiggsHW";}
};

#endif
