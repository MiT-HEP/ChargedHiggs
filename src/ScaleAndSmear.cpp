#include "interface/ScaleAndSmear.hpp"

void ScaleAndSmear::Init(){
    Log(__FUNCTION__,"INFO","Init ScaleAndSmear Corrections with MC_80X_13TeV and DATA_80X_13TeV");
    corrector_ . reset ( new EnergyScaleCorrection_class("Moriond17_23Jan_ele") ) ;
    corrector_ -> doScale = true;
    corrector_ -> doSmearings = true;
}

int ScaleAndSmear::correct(Event *e){
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsElectronDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);
        float pt= lep->GetPfPt();
        float eta= lep->GetEtaSC();
        bool isBarrel = abs(eta) <1.4442;
        bool isEndcap = abs(eta) > 1.566;
        bool tooBad = lep->GetR9() <0;

        if ( (not isBarrel and not isEndcap) or tooBad)
        {   // don't do anything
            double SF = pt/lep->GetP4Dirty().Pt(); // uncorrect corrections already applied
            Scale( *lep, SF);
            continue;
        }

        // do scale and smear corrections
        if ( e->IsRealData()){
            double dataSF = corrector_->ScaleCorrection(e->runNum(), isBarrel, lep->GetR9(), abs(eta), pt);
            //tagError = eleCorr.ScaleCorrectionUncertainty(info->runNum, tagisBarrel, tag->r9, tagAbsEta, tagEt);
            dataSF *= pt/lep->GetP4Dirty().Pt(); // uncorrect corrections already applied
            Scale( *lep, dataSF);
        }
        else {
            // correcting scale in MC, they should align everything to the best knowledge
            double random = gRandom->Gaus(0,1);
            //cout<<"Smearing Sigma for:"<<e->runNum()<<" isB"<< isBarrel<<" r9="<< lep->GetR9()<<" etaSC="<< abs(eta)<<" pt="<< pt<<endl;
            double mcSF = corrector_->getSmearingSigma(e->runNum(), isBarrel, lep->GetR9(), abs(eta), pt, 0., 0.);
            mcSF *= pt/lep->GetP4Dirty().Pt(); // uncorrect corrections already applied
            //float tagSmearEP = eleCorr.getSmearingSigma(info->runNum, tagisBarrel, tagR9Prime, tagAbsEta, tagEt, 1., 0.);
            //float tagSmearEM = eleCorr.getSmearingSigma(info->runNum, tagisBarrel, tagR9Prime, tagAbsEta, tagEt, -1., 0.);
            Scale( *lep, mcSF * random);
        }

    }

}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
