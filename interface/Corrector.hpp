#ifndef CORRECTOR_H
#define CORRECTOR_H

#include "interface/Event.hpp"

class CorrectorBase{
	public:
		virtual int correct(Event *e) {return 0;};
		virtual const string name(){ return "CorrectorBase";}
		virtual void Init() {}
	
		// implement read/write access to event	
		Met & GetMet(Event *e) { return e->met_ ;}

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
