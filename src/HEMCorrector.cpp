#include "interface/HEMCorrector.hpp"

void HEMCorrector::Init()
{
}

int HEMCorrector::correct(Event *e) {

    if (year!=2018) return 0;
    if (e->IsRealData()) return 0;

    TLorentzVector sum;

    for (auto & jet : GetJetVector(e))
    {
        ResetUncorr(*jet);
        float eta=jet->Eta();
        float phi=jet->Phi();
        // HEM -3.0 < eta < -1.3, -1.57 < phi < -0.87) for Run2018C and D
        // 20 % for jets with -1.57 <phi< -0.87 and -2.5<eta<-1.3
        // 35 % for jets with -1.57 <phi< -0.87 and -3.0<eta<-2.5
        // (all jets with pt>15 GeV passing the tight ID -in order to reject 
        // muons/electrons-, and propagate this to the MET as well)
        float sf=1.0;

        if ( -1.57F <phi and phi< -0.87F and -2.5F<eta and eta <-1.3F ) sf = 0.8;
        if ( -1.57F <phi and phi< -0.87F and -3.0F<eta and eta <-2.5F) sf=0.75; 
        //if (jet->PassPuId()) tightID is applied to all the jets
        if (sf !=1.0)
        {
            // diff 
            TLorentzVector v(jet->GetP4());
            v*=(1.-sf);
            sum+=v;
        }
        Scale(*jet,sf);
    } // end for loop
    ResetUncorr(GetMet(e));
    Add(GetMet(e),sum,-1.0F); // propagate to MET
}
