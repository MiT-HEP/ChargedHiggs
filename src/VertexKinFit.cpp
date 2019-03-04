#include "interface/VertexKinFit.hpp"

void VertexKinfit::Init(){
    Log(__FUNCTION__,"INFO","Init Vertex Kinfit");
}

int VertexKinfit::correct(Event *e){
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);

        if (lep->GetKFP4().Pt() >5) SetP4(*lep,lep->GetKFP4());

    }//end lepton
    // end correct
}




// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
