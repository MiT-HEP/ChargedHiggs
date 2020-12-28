#ifndef ANALYSIS_One_H
#define ANALYSIS_One_H

#include "interface/AnalysisBase.hpp"

class OneAnalysis: virtual public AnalysisBase
{
    public:
        OneAnalysis() : AnalysisBase () {}
        virtual ~OneAnalysis (){}

        void Init() override;
        int analyze(Event*,string systname) override;
        //void EndEvent() override;

        const string name() const override {return "OneAnalysis";}

        bool doSyst{true};

    private:
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
