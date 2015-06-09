#ifndef EVENT_VARIABLES_ANALYSIS_H
#define EVENT_VARIABLES_ANALYSIS_H

#include "interface/AnalysisBase.hpp"

class EventVariables:  virtual public AnalysisBase
{
public:
	virtual void Init();
	virtual int analyze(Event*,string systname);
	virtual const string name(){return "EventVariables";}

};

#endif
