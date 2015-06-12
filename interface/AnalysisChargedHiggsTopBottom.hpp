#ifndef CHARGEDHIGGSTOPBOTTOM_H
#define CHARGEDHIGGSTOPBOTTOM_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsTopBottom:  virtual public AnalysisBase
{
public:
	virtual void Init();
	virtual int analyze(Event*,string systname);
	virtual const string name(){return "ChargedHiggsTopBottom";}
};

#endif
