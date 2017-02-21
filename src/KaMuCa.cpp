#include "interface/KaMuCa.hpp"
#include "interface/Handlers.hpp"

void KaMuCa::Init(){
    Log(__FUNCTION__,"INFO","Init KaMuCa Corrections with MC_80X_13TeV and DATA_80X_13TeV");
    correctorMC_.reset(new KalmanMuonCalibrator("MC_80X_13TeV") ) ;
    correctorDATA_.reset(new KalmanMuonCalibrator("DATA_80X_13TeV") ) ;
}

int KaMuCa::correct(Event *e){

    correctorDATA_ -> reset();
    correctorMC_ -> reset();

    if (syst!=0)
    {
        // check
        if (syst >= correctorDATA_->getN()+1 or syst < -correctorDATA_->getN())
        {
            Log(__FUNCTION__,"ERROR",Form("CorrectorData has %d variations (2n+1), and position %d has been requested ",correctorDATA_->getN(),syst));
            throw abortException();
        }
    
        // check
        if (syst >= correctorMC_->getN()+1 or syst < -correctorMC_->getN())
        {
            Log(__FUNCTION__,"ERROR",Form("CorrectorData has %d variations (2n+1), and position %d has been requested ",correctorMC_->getN(),syst));
            throw abortException();
        }

        // set syst N+1 = closure
        if (e->IsRealData() and syst ==  correctorDATA_->getN()+1)
        {
            correctorDATA_->varyClosure(+1) ;
        }
        else if (not e->IsRealData() and syst ==  correctorMC_->getN()+1)
        {
            correctorMC_->varyClosure(+1) ;
        }
        else if ( syst >0 )
        {
            if ( e->IsRealData() ) correctorDATA_->vary(syst,+1); 
            else correctorMC_ -> vary ( syst , + 1);
        }
        else //(syst <0)
        {
            if ( e->IsRealData() ) correctorDATA_->vary(syst,-1); 
            else correctorMC_ -> vary ( syst , -1);
        }

    }


    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);
        //
        if ( e->IsRealData()){
            double dataSF = correctorDATA_ ->getCorrectedPt(
                    lep->GetP4Dirty().Pt(), // bypass syst
                    lep->GetP4Dirty().Eta(),
                    lep->GetP4Dirty().Phi(),
                    lep->Charge()) / lep->GetP4Dirty().Pt();
            Scale( *lep, dataSF);
        }
        else {
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

int SmearKaMuCa::smear(CorrectorBase*c)
{
    //syst_
    //check that the corrector is right, first should be faster
    if (dynamic_cast<KaMuCa*>(c) == NULL ) return 0;
    if (c->name() != "KaMuCa") return 0; // nothing to correct
    dynamic_cast<KaMuCa*>(c)->syst = syst_;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
