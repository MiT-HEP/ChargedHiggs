#include "interface/KaMuCa.hpp"

void KaMuCa::Init(){
    Log(__FUNCTION__,"INFO","Init KaMuCa Corrections with MC_80X_13TeV and DATA_80X_13TeV");
    correctorMC_.reset(new KalmanMuonCalibrator("MC_80X_13TeV") ) ;
    correctorDATA_.reset(new KalmanMuonCalibrator("DATA_80X_13TeV") ) ;
}

int KaMuCa::correct(Event *e){
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);
        //
        if ( e->IsRealData()){
            correctorDATA_ -> reset();
            double dataSF = correctorDATA_ ->getCorrectedPt(
                    lep->GetP4Dirty().Pt(), // bypass syst
                    lep->GetP4Dirty().Eta(),
                    lep->GetP4Dirty().Phi(),
                    lep->Charge()) / lep->GetP4Dirty().Pt();
            Scale( *lep, dataSF);
        }
        else {
            correctorMC_ -> reset();
            // correcting scale in MC, they should align everything to the best knowledge
            double mcSF = correctorMC_ ->getCorrectedPt(
                    lep->GetP4Dirty().Pt(), // bypass syst
                    lep->GetP4Dirty().Eta(),
                    lep->GetP4Dirty().Phi(),
                    lep->Charge()) / lep->GetP4Dirty().Pt();
            mcSF *= correctorMC_ -> smear ( lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta()) /lep->GetP4Dirty().Pt(); 
            Scale( *lep, mcSF);
        }

        //Uncertainties https://github.com/bachtis/Analysis/blob/KaMuCa_V3/Calibration/test/test.py
        //print 'propagate the statistical error of the calibratio to a + 40 GeV muon at eta=phi=0'
        //print 'first get number of parameters'
        //N=c.getN()
        //print N,'parameters'
        //for i in range(0,N):
        //    c.vary(i,+1)
        //      print 'variation',i,'ptUp', c.getCorrectedPt(40,0.0,0.0,1)
        //      c.vary(i,-1)
        //      print 'variation',i,'ptDwn', c.getCorrectedPt(40,0.0,0.0,1)
        //
        //
        //c.reset()
        //
        //print 'propagate the closure error to a + 40 GeV muon at eta=phi=0'
        //
        //c.varyClosure(+1)
        //print 'After closure shift pt=', c.getCorrectedPt(40,0.0,0.0,1)

    }

}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
