#ifndef MC_H
#define MC_H

#define MC_MAX_SCALES 6
//#define MC_MAX_PDFS 100
#define MC_MAX_PDFS 120

#include "interface/Named.hpp"
#include <map>
#include <vector>

// @brief This class will contain the basic MC informations
//          XSEC, SumOf Event weights, directory ...
class MC : public Named {
    public:
        enum SCALES{none=-1,r1f2=0,r1f5,r2f1,r2f2,r5f1,r5f5};

        const string name() const { return "MC";}
        string dir;
        double xsec;
        double nevents;

        // if set, set the SumW base / SumW^R
        double scalesNeventsReweight[MC_MAX_SCALES];
        double pdfsNeventsReweight[MC_MAX_PDFS];

        // for AQGC. using dynamic
        std::map<std::string,double> aQGCNeventsReweight;
        
        const static std::vector<std::string> aqgc_names;//={ "fs0_0p00", "fs0_5p00", "fs0_10p00", "fs0_15p00", "fs0_20p00", "fs0_25p00", "fs0_30p00", "fs0_35p00", "fs0_40p00", "fs0_45p00", "fs0_50p00", "fs0_55p00", "fs0_60p00", "fs0_65p00", "fs0_70p00"};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
