#ifndef VERTEX_KINFIT_H
#define VERTEX_KINFIT_H

#include "interface/Corrector.hpp"
#include <memory>
#include <string>

class VertexKinfit : public CorrectorBase{
	public:
		int correct(Event *e) override ;
		const string name() const override { return "VertexKinfit";}
		void Init() override;
};


#include "interface/EnergyScaleCorrection.hpp"

class FSRRecovery : public CorrectorBase{
	public:
        float maxpt{-1};
        float maxrelpt{-1};
        bool onlyiso{true};
		int correct(Event *e) override ;
		const string name() const override { return "FSRRecovery";}
		void Init() override;

        bool egmcorrections{false};
        int year{2017};
    protected:
		std::unique_ptr<EnergyScaleCorrection> corrector_;
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
