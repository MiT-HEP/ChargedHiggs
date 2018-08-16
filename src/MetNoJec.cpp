#include "interface/MetNoJec.hpp"
#include "TLorentzVector.h"

/* This remove the JEC from the selected region, but do not perform well
int MetNoJEC::correct(Event *e)
{
    // TODO: check what happens with systematics 
    //
    TLorentzVector res;
    for (auto j : GetJetVector(e))
    {
        if (fabs(j->Eta()) < eta0) continue;
        if (fabs(j->Eta()) > eta1) continue;
        
        TLorentzVector nojec=j->GetP4();
        nojec *= j->rawPt/j->GetP4().Pt();
        res += nojec - j->GetP4();
    }

    Add(GetMet(e),res,1);
}
*/

// this removes the jet with some energy fraction
int MetNoJEC::correct(Event *e)
{
    // TODO: check what happens with systematics 
    //
    TLorentzVector res;
    for (auto j : GetJetVector(e))
    {
        if (fabs(j->Eta()) < eta0) continue;
        if (fabs(j->Eta()) > eta1) continue;
       
        if (j->GetCEMF() + j->GetNEMF() < 0.75) continue;
        res += j->GetP4();
    }

    Add(GetMet(e),res,-1);
}
