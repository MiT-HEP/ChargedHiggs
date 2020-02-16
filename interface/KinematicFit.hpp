#ifndef KINEMATIC_FIT_H
#define KINEMATIC_FIT_H

#include "TLorentzVector.h"
#include <vector>

class KinematicFit
{
    // this class is an helper to perform kinematic fit and minimizations
    public:
        KinematicFit(){}
        ~KinematicFit(){}

        void clear(){p4.clear();sigma.clear();ghostAdded=false;}
        void run();
        void runGeneric(); 

    // parameters to set
        std::vector<TLorentzVector> p4;
        std::vector<float> sigma;
        TLorentzVector H;
    
    // parameters for Generic minimization
        int genericType{0}; // 0 = no met;  1= min met
        float lambda{1.}; //mixing value for met minimization
        bool doNorm{true}; // mixing is normalized first
        int maxIteration=1000;

    // corrections -- output
        std::vector<float> alpha;
        float value{0.}; // value at minimum

    // Ghost -- config
        bool doGhost{false};
        int nGhost{3}; float sigmaGhost{0.3333}; float ptGhost{30.};
        void print();

    protected:
        void addGhost();
        bool ghostAdded{false};
        float inline var(int i) const { return sigma[i]*sigma[i];}


};

#define KF2
#ifdef KF2
class KinematicFit2
{
    public:
        KinematicFit2(){}
        ~KinematicFit2(){}

        void clear(){p4.clear();sigma.clear();notV.clear();index_out.clear();V=0;value_out=-1;alpha_out.clear();}
        void run();
        void print();

        int maxIteration=100000;
    //configuration
        double lambda_MET{0.};
        double lambda_V{0.};
        double lambda_mqq{0.};
        int doV{3};//1=Z, 2=W, 3 both
        double cutWidth=30.;  // do not try combination if M base is outside this window

    // parameters to set
        std::vector<TLorentzVector> p4;
        std::vector<double> sigma;
        std::vector<int> notV;// is not a V cand

    // corrections -- output
        std::vector<int> index_out; // v1, v2, q1, q2
        int V{0}; // 1->Z, 2->W,

        double value_out;
        std::vector<double> alpha_out;

        int debug{0}; // more verbose on progress

    private:
        // compute this information and store it
        // this is for each combinatorics run
        void run_single();
        std::vector<double> alpha;
        double value{0.}; // value at minimum
        int v1, v2; // running

};
#endif

#endif
