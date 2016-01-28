#ifndef MC_H
#define MC_H

#include "interface/Named.hpp"

class MC : public Named {
    // This class will contain the basic MC informations
    // XSEC, SumOf Event weights, directory ...
    public:
        const string name() const { return "MC";}
        string dir;
        double xsec;
        double nevents;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
