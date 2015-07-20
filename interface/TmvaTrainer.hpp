#ifndef TMVA_TRAINER_H
#define TMVA_TRAINER_H

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TTree.h"

#include "interface/AnalysisBase.hpp"


class DataStore{
        map<string,double> valuesD_;
        map<string,float> valuesF_;
        map<string,int >  valuesI_;

    public:

        void Add(string name, char type);
        bool Exists(string name);
        void* GetPointer(string name);
        
        template<class T>
        void Set(string name, T value);

};

class TmvaTrainer : virtual public AnalysisBase
{
    private:
        TMVA::Factory *factory_;
        TFile *fout_;
        TTree *tree_;
        vector<Double_t> variables_;

        DataStore varsValues_;
    public:
        TmvaTrainer();
        ~TmvaTrainer();
        virtual int analyze(Event*,string systname);
        virtual void Init();
        virtual void End();
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
