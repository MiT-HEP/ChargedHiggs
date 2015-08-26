#ifndef REGR_TRAINER_H
#define REGR_TRAINER_H

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TTree.h"

#include "interface/AnalysisBase.hpp"



class RegressionTrainer : virtual public AnalysisBase
{
    private:
        TMVA::Factory *factory_;
        TFile *fout_;
        vector<Double_t> variables_;

        DataStore varsValues_;
    public:
        RegressionTrainer();
        ~RegressionTrainer();
        virtual int analyze(Event*,string systname);
        virtual void Init();
        virtual void End();
        virtual const string name(){return "RegressionTrainer";}
        virtual void AddVariable( string, char, double=0.,double=0.);
        
        bool train;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
