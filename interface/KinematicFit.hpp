#ifndef KINEMATIC_FIT_H
#define KINEMATIC_FIT_H

#include "TLorentzVector.h"
#include <vector>

class KinematicFit
{
    // this class is an helper to perform kinematic fit and minimizations
    public:
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
    // corrections -- output
        std::vector<float> alpha;

    // Ghost -- config
        bool doGhost{false};
        int nGhost{3}; float sigmaGhost{0.3333}; float ptGhost{30.};

    protected:
        void addGhost();
        bool ghostAdded{false};
        float inline var(int i) const { return sigma[i]*sigma[i];}


};

#endif
