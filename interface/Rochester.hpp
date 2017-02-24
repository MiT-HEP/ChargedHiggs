#ifndef ROCHESTER_H
#define ROCHESTER_H

#include "interface/Corrector.hpp"
#include "interface/RoccoR.hpp"
#include <memory>

class RochesterCorrections : public CorrectorBase{
	public:
		int correct(Event *e) override ;
		const string name() const override { return "RochesterCorrections";}
		void Init() override;
		std::unique_ptr<RoccoR> corrector_;
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
