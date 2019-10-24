#ifndef HEM_H
#define HEM_H

#include "interface/Corrector.hpp"

class HEMCorrector: public CorrectorBase
{
    public:
		int correct(Event *e) override;
		const string name() const override { return "HEMCorrector";}
		void Init() override ;
        int year{2018};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
