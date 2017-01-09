#ifndef TMVA_TRAINER_H
#define TMVA_TRAINER_H

#include "TTree.h"

#include "interface/AnalysisBase.hpp"



class TmvaTrainer : virtual public AnalysisBase
{
    private:
        vector<Double_t> variables_;

        DataStore varsValues_;
    public:
        TmvaTrainer():AnalysisBase(){}
        ~TmvaTrainer(){}
        virtual int analyze(Event*,string systname);
        int analyzeInvIso(Event*e,string systname);
        virtual void Init();
        virtual const string name(){return "TmvaTrainer";}
        virtual void AddVariable( string, char, double=0.,double=0.);
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
