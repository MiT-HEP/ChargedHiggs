#ifndef CHARGEDHIGGSMET_H
#define CHARGEDHIGGSMET_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsMET:  virtual public AnalysisBase
{
public:
	virtual void Init();
	virtual int analyze(Event*,string systname);
	virtual const string name(){return "ChargedHiggsMET";}
};

#endif
