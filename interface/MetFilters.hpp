#ifndef MET_FILTERS_H
#define MET_FILTERS_H

#include "interface/AnalysisBase.hpp"
#include <map>
using namespace std;

class MetFiltersAnalysis : virtual public AnalysisBase
{
    /*Select Data pass a MET_FILTERS File
    */

    public:

        int analyze(Event*,string systname) override;
        const string name() const override {return "MetFilterAnalysis";}
        void Init() override {};

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
