#ifndef JSON_H
#define JSON_H

#include "interface/AnalysisBase.hpp"
#include <map>
using namespace std;

class JsonAnalysis : virtual public AnalysisBase
{
    /*Select Data pass a JSON File
    */

    bool CheckLumi(long run, int lumi);
    std::map<long, std::vector<std::pair<int,int> > > goodLumis_;


    public:

    virtual int analyze(Event*,string systname);
    virtual const string name(){return "JsonAnalysis";}

    void addGoodLumi(long run, int lumi1, int lumi2);

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
