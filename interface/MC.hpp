#ifndef MC_H
#define MC_H

#define MC_MAX_SCALES 6
#define MC_MAX_PDFS 100

#include "interface/Named.hpp"

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
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
