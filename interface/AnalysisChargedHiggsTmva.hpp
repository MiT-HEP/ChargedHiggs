#ifndef ANALYSIS_TMVA_H
#define ANALYSIS_TMVA_H

#include "interface/AnalysisBase.hpp"
#include "interface/Output.hpp" // DataStore
#include "interface/CatSelector.hpp"

#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

class TmvaAnalysis : virtual public AnalysisBase
{
    public:
        TmvaAnalysis(){};
        ~TmvaAnalysis(){};
        int analyze(Event*,string systname) override;
        void Init() override;
        void End() override;
        const string name() override {return "TmvaAnalysis";}

        // Variables
        template<class T>
        void SetVariable( string name, T value){ varValues_.Set(name, value); }

        void AddVariable( string name, char type);

        vector<string> weights;

        // multi category analysis
        CatSelector categories;

    private:
        // this allows to handle different data type (namely float int) in a unique structure. -- Like Trees
        DataStore varValues_;

        // 
        //TMVA::Reader *reader_ ;
        vector<TMVA::Reader*> readers_;
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
