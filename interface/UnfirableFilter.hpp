#ifndef UNFIRABLE_FILTERS_H
#define UNFIRABLE_FILTERS_H

#include "interface/AnalysisBase.hpp"
#include <map>
#include <set>
#include <tuple>
#include <string>

using namespace std;

class UnprefirableFiltersAnalysis : virtual public AnalysisBase
{
    
    Long64_t run, lumi, event;
    set< tuple< Long64_t, Long64_t, Long64_t > > store_;
    string fname_{"aux/SingleMuon_Run2017F_unprefirableEvents.root"};
    string tname_{"tree"};
    public:

        int analyze(Event*,string systname) override;
        const string name() const override {return "UnprefirableFilterAnalysis";}
        void Init() override ;


        void SetFileName(const string& fname){fname_=fname;}
        void SetTreeName(const string& tname){tname_=tname;}

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
