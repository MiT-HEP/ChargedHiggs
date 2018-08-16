
#ifndef MET_NO_JEC_H
#define MET_NO_JEC_H

#include "interface/Corrector.hpp"
#include "interface/Event.hpp"

class MetNoJEC: public CorrectorBase{

    public:
        const string name () const override { return "MetNoJEC";}
        void Init()override{}
        int correct(Event*e) override;

        double eta0{2.5};
        double eta1{3.0};
};
#endif
