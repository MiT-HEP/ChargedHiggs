#ifndef SplitMC_H
#define SplitMC_H

#include "interface/AnalysisBase.hpp"
#include <map>
#include <set>
#include <string>
#include <tuple>

using namespace std;

class SplitMCAnalysis : virtual public AnalysisBase
{
    /*Select Data pass a JSON File
    */

    //mc run lumi evt
    std::set< std::tuple <unsigned,unsigned,unsigned,unsigned,uint64_t > > bad_;
    // save mc here to be able to book histograms
    std::set<unsigned> mc_;

    //mc run lumi evt
    std::set< std::tuple <unsigned,unsigned,unsigned,unsigned,uint64_t > > badCR_;
    // save mc here to be able to book histograms
    std::set<unsigned> mcCR_;


    public:

    string fileName="aux/example.txt";

    int analyze(Event*,string systname) override;
    void Init() override ;
    const string name() const override {return "SplitMCAnalysis";}


    void inline addBadEvent(unsigned mc, unsigned lep_category, unsigned run, unsigned lumi, uint64_t event){ bad_.emplace( make_tuple(mc,lep_category,run,lumi,event) ); mc_.emplace(mc);}
    bool inline isBadEvent(unsigned mc, unsigned lep_category, unsigned run, unsigned lumi, uint64_t event) const { if (bad_.find( make_tuple(mc,lep_category,run,lumi,event)) == bad_.end() ) return false; else return true;}

    void inline addBadEventCR(unsigned mc, unsigned lep_category, unsigned run, unsigned lumi, uint64_t event){ badCR_.emplace( make_tuple(mc,lep_category,run,lumi,event) ); mcCR_.emplace(mc);}
    bool inline isBadEventCR(unsigned mc, unsigned lep_category, unsigned run, unsigned lumi, uint64_t event) const { if (badCR_.find( make_tuple(mc,lep_category,run,lumi,event)) == badCR_.end() ) return false; else return true;}


    unsigned findMC(Event *e);

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
