#ifndef SmearJesSources_H
#define SmearJesSources_H

#include "interface/Smearer.hpp"
#include "interface/JetCorrectorParameters.hpp"
#include "interface/JetCorrectionUncertainty.hpp"

#include <memory>

class SmearJesSource : virtual public SmearBase
{
    //syst_ =  -1 , 1 / UP/DOWN
    protected:
        std::unique_ptr<JetCorrectorParameters> params_;
	    std::unique_ptr<JetCorrectionUncertainty> jecUnc_;
    public:
        SmearJesSource() : SmearBase(){ name_ = "JES_SOURCE";}
        SmearJesSource(const string &n) ;
        SmearJesSource(const string &fname, const string &n) ;
        int smear(Event*e) override;	
        string fname_{"aux/JEC/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_UncertaintySources_AK8PFchs.txt"};
};

class SmearJesSourceAK8 : virtual public SmearJesSource
{
    public:
        SmearJesSourceAK8() : SmearJesSource(){ name_ = "JESAK8_SOURCE";}
        SmearJesSourceAK8(const string &n) ;
        SmearJesSourceAK8(const string &fname, const string &n) ;
        int smear(Event*e) override;	
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
