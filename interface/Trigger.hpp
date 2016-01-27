#ifndef TRIGGER_H
#define TRIGGER_H

// all objects that has trigger bits information should inherit from here
//
class Trigger{

public:
	int trigger{0};
    int triggerNone{0};
    // ---
	bool IsTriggered(int triggerPos){ return (trigger & (1UL << triggerPos)); }
	bool IsTriggeredNone(int triggerPos){ return (triggerNone & (1UL << triggerPos)); }
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
