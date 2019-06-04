#include "interface/VertexKinFit.hpp"

void VertexKinfit::Init(){
    Log(__FUNCTION__,"INFO","Init Vertex Kinfit");
}

int VertexKinfit::correct(Event *e){
    int nMuons=0;
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);

        if (lep->GetKFP4().Pt() >5){
            SetP4(*lep,lep->GetKFP4());
            ++nMuons;
        }

        if (nMuons>=2) break; // only the first two muons.

    }//end lepton
    return 0;
    // end correct
}

void FSRRecovery::Init(){
    Log(__FUNCTION__,"INFO","Init Vertex Kinfit");
}

int FSRRecovery::correct(Event *e){
    int nMuons=0;
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);

        TLorentzVector p4=lep->GetP4();

        if (lep->GetFsrP4().Pt() >0 and (maxpt <0 or lep->GetFsrP4().Pt() <maxpt) and (maxrelpt<0 or lep->GetFsrP4().Pt()/p4.Pt()<maxrelpt )){
            if (not onlyiso)
            {
                p4+= lep->GetFsrP4();
                SetP4(*lep,p4);
            }
            lep->SetIso(  lep->Isolation() - lep->GetFsrP4().Pt() );
            lep->SetMiniIso ( lep->MiniIsolation() - lep->GetFsrP4().Pt());
        }

    }//end lepton
    return 0;
    // end correct
}




// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
