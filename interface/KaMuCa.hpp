#ifndef KAMUCA_H
#define KAMUCA_H

#include "interface/Corrector.hpp"
#include "interface/Smearable.hpp"
#include "interface/KalmanMuonCalibrator.hpp"

#include <memory>

class KaMuCa : public CorrectorBase,
    public SmearableBase
{
	public:
		int correct(Event *e) override ;
		const string name() const override { return "KaMuCa";}
		void Init() override;
    private:
        //
		std::unique_ptr<KalmanMuonCalibrator> correctorMC_;
		std::unique_ptr<KalmanMuonCalibrator> correctorDATA_;
};

#include "interface/Smearer.hpp"

class SmearKaMuCa : public SmearBase
{
    int num_; // from 0 to c.getN()
    public:
        SmearKaMuCa(): SmearBase() { name_ = "KAMUCA"; num_=0;}
        SmearKaMuCa(int num): SmearBase() { name_ = "KAMUCA"; num_=num;}
        const inline string name() const override { return  name_ + Form("%d",num_);}
        int smear(CorrectorBase*c) override; 	
    
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
