#ifndef ANALYSIS_SCIKIT_H
#define ANALYSIS_SCIKIT_H

#include "interface/AnalysisBase.hpp"
#include "interface/Output.hpp" // DataStore
#include "TPython.h"
#include <memory>

class SciKitAnalysis : virtual public AnalysisBase
{
    public:
        SciKitAnalysis(){};
        ~SciKitAnalysis(){};

        int analyze(Event*,string systname) override;
        void Init() override;
        void End() override;
        const string name() const override {return "SciKitAnalysis";}

    protected:
        std::unique_ptr<TPython> py;
        vector<float> x;



};


#endif
