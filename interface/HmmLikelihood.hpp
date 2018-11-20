#ifndef HMM_LIKELIHOOD_H
#define HMM_LIKELIHOOD_H
#include "TMath.h"
#include "TF1.h"
#include <map>
#include <cmath>
#include <string>

class HmmLikelihood
{
    private:
        std::map<std::string,TF1*> store_;
    public:
        void init();
        HmmLikelihood(){init();}
        ~HmmLikelihood();

        
        float Eval(const std::string& name, float value)  { return store_[name]->Eval(value);}

        float likelihood(
                float Hpt, 
                float Heta,
                float mjj_1, 
                float mjj_2, 
                float detajj_1,
                float detajj_2, 
                float ncentjets, 
                //float deta,
                //float dphi,
                float firstQGL,
                float maxDeepB,
                float softHt1
                );


};
#endif
