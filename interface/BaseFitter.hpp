#ifndef BASE_FITTER_H
#define BASE_FITTER_H
#include "interface/Named.hpp"

class BaseFitter: virtual public Named
{
    /*
     * Original Author: Andrea Carlo Marini
     * Date: 20/05/2016
     * Comment: this class will be used by fitter.py to interface and load the fitter macro
     */
    public:
        virtual void init()=0;
        virtual void fit()=0;
        const string name() const override { return "BaseFitter";}
        double lumi{2318};

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
