#include "interface/MetFilters.hpp"

#define MET_EVENT_FAIL 1
#define MET_EVENT_PASS 0

#define VERBOSE 0

int MetFiltersAnalysis::analyze(Event *e,string systname) 
{
    if (not e->IsRealData() ) return MET_EVENT_PASS;
    if (e->GetMet() . passFullRecommendation()) { 
        if(VERBOSE>0)Log(__FUNCTION__,"INFO",Form("Event (%d,%d,%u) PASS met filters",e->runNum(),e->lumiNum(),e->eventNum()));
        return MET_EVENT_PASS;
    }
    if(VERBOSE>0)Log(__FUNCTION__,"INFO",Form("Event (%d,%d,%u) have been rejected by met filters",e->runNum(),e->lumiNum(),e->eventNum()));
    return MET_EVENT_FAIL;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
