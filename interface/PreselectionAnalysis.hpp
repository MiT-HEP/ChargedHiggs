#ifndef PRESELECTION_H
#define PRESELECTION_H
#include "interface/AnalysisBase.hpp"

class Preselection:  virtual public AnalysisBase
{
public:
	virtual void Init();
	virtual int analyze(Event*,string systname);
	virtual const string name(){return "Preselection";}
};

#endif
