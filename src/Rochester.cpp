#include "interface/Rochester.hpp"

void RochesterCorrections::Init(){
    Log(__FUNCTION__,"INFO","Init Rochester Corrections with aux/rcdata.2016.v3");
    corrector_.reset(new RoccoR("aux/rcdata.2016.v3") ) ;
}

int RochesterCorrections::correct(Event *e){
    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        /*
           Here: 
           Q is charge
           nl is trackerLayersWithMeasurement \\
           u1 and u2 are random numbers distributed uniformly between 0 and 1 (gRandom->Rndm());
           s is error set (default is 0)
           m is error member (default is 0)

    Following variations are provided currently
    -------------------------------------------------------------------------------------
    	 set        members	comment
    Default  0	    1		default, reference based on madgraph sample, with pt distribution reweighted to data in |Y| bins. 
    Stat     1          100         pre-generated stat. variations; can include more (e.g. 400). RMS over these would give stat. uncertainty
    Zpt      2          1           derived without reweighting reference to data. Difference wrt central can be taken as systematics
    Ewk      3          1           Ad-hoc weighting applied to reference to change sw2 and Zw. Use for x-check for now. Need to compare with Powheg (may become default). 
    CorDm    4          5           varied profile mass window; one can take maximum deviation as systematics
    FitDm    5          5           varied fitting mass window; one can take maximum deviation as systematics
    LHEw     6          0           (please ignore for now)
    Run      7          7           derived from B,C,D,E,F,G,H; for cross-check study. No dramatic run denendence observed. MC's should be split by lumi if applied this way. 
    AMCNLO   8          1           derived from limited stat. amcnlo sample; for cross-check study
         */
        // reset uncorrected value
        ResetUncorr(*lep);
        //
        int s=0; int m=0;
        if ( e->IsRealData()){
            double dataSF = corrector_->kScaleDT(lep->Charge(),lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta(),lep->GetP4Dirty().Phi(),s,m);
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
