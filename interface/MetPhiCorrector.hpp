#ifndef METPHI_CORRECTOR_H
#define METPHI_CORRECTOR_H

#include "interface/Corrector.hpp"
#include "TFile.h"
#include "TF1.h"

class MetPhiCorrector : virtual public CorrectorBase
{
    public:
        MetPhiCorrector () : CorrectorBase() {}
        void Init() override final;
        const string name() const override final { return "MetPhiCorrector";}
        int correct (Event *e) override final;

        //string fileName = "aux/MetPhi.root";
        int year{2017};
    protected:
        double MetPhiCorr(int Era, int npv, bool  runOnData, int metCoord);

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
