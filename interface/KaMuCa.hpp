#ifndef KAMUCA_H
#define KAMUCA_H

#include "interface/Corrector.hpp"
#include "interface/KalmanMuonCalibrator.hpp"

#include <memory>

class KaMuCa : public CorrectorBase{
	public:
		int correct(Event *e) override ;
		const string name() const override { return "KaMuCa";}
		void Init() override;
    private:
        //
		std::unique_ptr<KalmanMuonCalibrator> correctorMC_;
		std::unique_ptr<KalmanMuonCalibrator> correctorDATA_;
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
