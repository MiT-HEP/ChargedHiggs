#ifndef JET_FILTER_2016_H
#define JET_FILTER_2016_H

#include "interface/Corrector.hpp"

#include "TH2D.h"
#include <memory>

class JetFilter2016: public CorrectorBase
{
    // kill jets accordingly to JetMet Map
    std::unique_ptr<TH2D> h; 

    public:
        void Init() override;
	    const string name() const override { return "JetFilter2016";}
	    int correct(Event *e) override ;
	    //vector<Jet *> & GetJetVector(Event *e) { return e->jets_;}
        //inline void Scale( Object&o, float value) { o.Scale(value);}
};

#endif
