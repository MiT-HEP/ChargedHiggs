#include "interface/Rochester.hpp"
#include "TRandom.h"

void RochesterCorrections::Init(){
    Log(__FUNCTION__,"INFO","Init Rochester Corrections with aux/rochester/RoccoR2017v0.txt");
    corrector_.reset(new RoccoR("aux/rochester/RoccoR2017v0.txt") ) ;
}

int RochesterCorrections::correct(Event *e){
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);
        //
        int s=0; int m=0;
        if ( e->IsRealData()){
            double dataSF = corrector_->kScaleDT(lep->Charge(),lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta(),lep->GetP4Dirty().Phi(),s,m);
            if (dataSF>1.5 or dataSF<0.5) {
                //Log(__FUNCTION__,"WARNING",Form("Rochester corrections SF on data is too high (or too low) %lf. Setting it to 1!",dataSF));
                dataSF=1;
            }
            Scale( *lep, dataSF);
        }
        else {
            //for MC, if matched gen-level muon (genPt) is available, use this function
            int mcIdx=-1;
            vector<GenParticle*> gen=GetGenVector(e);
            for (int gp=0; gp< gen.size();++gp)
            {
                if (gen[gp]->DeltaR(*lep) >.1) continue;
                if ( abs(gen[gp]->GetPdgId() ) != 13 ) continue; // muon
                if ( not gen[gp]->IsPromptFinalState() ) continue; 
                mcIdx=gp ;
                break;
            }
            double mcSF = 1;
            double u1=gRandom->Rndm(),u2=gRandom->Rndm();

            int nl=15; 
            if (lep->GetNLayers() >0 ) nl=lep->GetNLayers();

            if (mcIdx>=0)
            {
                //Log(__FUNCTION__,"DEBUG",Form("-> Correcting MC with: ch=%d pt=%f eta=%f nl=%d u1=%f s=%f m=%f idx=%d",lep->Charge(), lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta(),nl,u1,s,m,mcIdx));
                //Log(__FUNCTION__,"DEBUG",Form("->  -----      and gen: idx=%d pt=%f eta=%f",mcIdx,gen[mcIdx]->Pt(),gen[mcIdx]->Eta()));
                mcSF=corrector_->kScaleFromGenMC(lep->Charge(), lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta(),lep->GetP4Dirty().Phi(), nl, gen[mcIdx]->Pt(), u1, s, m);
            }
            else
            {
                mcSF = corrector_->kScaleAndSmearMC(lep->Charge(),lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta(),lep->GetP4Dirty().Phi(), nl, u1, u2, s, m);
            }
            if (mcSF>1.5 or mcSF<0.5) {
                Log(__FUNCTION__,"WARNING",Form("Rochester corrections SF on MC is too high (or too low) %lf. Setting it to 1!",mcSF));
                mcSF=1;
            }
            Scale( *lep, mcSF);
        }

    }
}

/*
		// implement read/write access to event	
		Met & GetMet(Event *e) { return e->met_ ;}
		vector<Tau *> & GetTauVector(Event *e) { return e->taus_;}
        vector<Lepton *>& GetLepVector(Event *e) { return e->leps_;}
        
        // R/W Access to Objects, private/protected members
        inline void Scale( Object&o, float value) { o.Scale(value);}
        inline void Add(Object &o, TLorentzVector&v, float c){o.Add(v,c);}
        inline void SetP4(Object&o, TLorentzVector &v){o.p4 = v; o.Scale(1.) ; } // the last set is Correct
*/



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
