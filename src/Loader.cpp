#include "interface/Loader.hpp"
#include "interface/Event.hpp"

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareEvent.hpp"
#include "NeroProducer/Core/interface/BareJets.hpp"
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"
#include "NeroProducer/Core/interface/BareMet.hpp"
#include "NeroProducer/Core/interface/BareFatJets.hpp"
#include "NeroProducer/Core/interface/BareLeptons.hpp"
#include "NeroProducer/Core/interface/BarePhotons.hpp"
#include "NeroProducer/Core/interface/BareTaus.hpp"
#include "NeroProducer/Core/interface/BareTrigger.hpp"
#include "NeroProducer/Core/interface/BareVertex.hpp"

#include "TNamed.h"

#include <assert.h>
#include <iostream>
#include <sstream>

//#define VERBOSE 2

// ---------------------------- FACTORY -----------------------
Loader* LoaderFactory::create(const std::string & name)
{
    const auto& i = table_.find(name); 
    if ( i == table_.end() ) { std::cout<<"[LoaderFactory]::[ERROR]::Unable to create: "<<name<<std::endl; return (Loader*)NULL ; }
    return i -> second -> create();
}
// ---------------------- END FACTORY ----------------------------

// ---------------------------- NERO -----------------------
int LoadNero::InitTree(){
    vector<string> classes {"BareEvent",
        "BareVertex",
        "BareMonteCarlo",
        "BareJets",
        "BareTaus",
        "BareLeptons",
        "BareMet",
        "BareTrigger",
        "BarePhotons",
    };
    for (const string & c : classes)
    {
        unsigned position = bare_.size();
        names_[ c ] = position;
        bare_ . push_back( BareFactory::get().create( c ) ) ;
    }

    // Set Extra: Extend, match ...
    bare_[ names_ ["BareTaus"] ] -> SetExtend();
    bare_[ names_ ["BarePhotons"] ] -> SetExtend();
    dynamic_cast<BareTaus*> (bare_[ names_ ["BareTaus"] ])  -> SetMatch();

    for (auto b : bare_ )
        b->setBranchAddresses(tree_);

    // branches are activate from configuration file
    tree_ -> SetBranchStatus("*",0);

    return 0;
}

int LoadNero::FillEvent(){

    //FillEventInfo(); // new file uses isRealData, but not the weights, called directly

    FillJets();
    FillLeptons();
    FillPhotons();
    FillTaus();
    FillMet();
    FillMC();
    FillTrigger(); //
}

void LoadNero::FillEventInfo(){
    BareEvent *e = dynamic_cast<BareEvent*> ( bare_ [names_["BareEvent"] ] ) ; assert(e!=NULL);
#ifdef VERBOSE
    if(VERBOSE>0)Log(__FUNCTION__,"DEBUG",string("Processing Event") + Form("%ld:%ld:%ld",e->runNum,e->lumiNum,e->eventNum));
#endif

    event_ -> isRealData_ = e->isRealData;
    event_ -> runNum_ = e->runNum;
    event_ -> lumiNum_ = e->lumiNum;
    event_ -> eventNum_ = e->eventNum;
    event_ -> rho_ = e->rho;

    event_ -> met_ . setFullRecommendation ( e->selBits & BareEvent::FullRecommendation );

    BareVertex *v = dynamic_cast<BareVertex*> ( bare_ [names_["BareVertex"] ] ) ; assert(v!=NULL);
    event_ -> npv_ = v->npv;
}

void LoadNero::FillJets(){
    //fill Jets
#ifdef VERBOSE
    if(VERBOSE>1) Log(__FUNCTION__,"DEBUG","Filling Jets");
#endif

    BareJets *bj = dynamic_cast<BareJets*> ( bare_ [ names_[ "BareJets" ] ] ); assert (bj !=NULL);

    if ( tree_ ->GetBranchStatus("jetP4") == 0 ){ 
        LogN(__FUNCTION__,"WARNING","Jets Not FILLED",10);
        return;
    }

    // EXTRA DEBUG INFO
#ifdef VERBOSE
    if(VERBOSE>1)
        cout <<"[LoadNero]::[FillJets]::[DEBUG] Jets length:"<<endl;
    cout <<"\t * selBits: "	<< tree_->GetBranchStatus("jetSelBits") << " : "<< bj->selBits->size()<<endl;
    cout <<"\t * unc: "		<< tree_->GetBranchStatus("jetUnc") << " : "<< bj->unc->size()<<endl;
    cout <<"\t * bdiscr: "	<< tree_->GetBranchStatus("jetBdiscr") << " : "<< bj->bDiscr->size()<<endl;
    cout <<"\t * qgl: "		<< tree_->GetBranchStatus("jetQG") << " : "<< bj->qgl->size()<<endl;
#endif

    for (int iJet=0;iJet< bj -> p4 ->GetEntries() ; ++iJet)
    {

#ifdef VERBOSE
        if (VERBOSE >1 )
        {
            cout <<"[LoadNero]::[FillJets]::[DEBUG2] considering jet: "<<iJet << " / "<< bj -> p4 ->GetEntries() <<endl;
            cout <<"\t\t * selBits size: "<< bj->selBits ->size()<<endl;
            cout <<"\t\t * unc size:" << bj -> unc ->size() <<endl;
            cout <<"\t\t * bdiscr size :"<< bj -> bDiscr ->size() <<endl;

            cout <<"\t\t * pdgId :" <<bj -> matchedPartonPdgId -> size()<<endl;
            cout <<"\t\t * mother :"<<bj->motherPdgId ->size()<<endl;
            cout <<"\t\t * gr mother:"<<  bj-> grMotherPdgId -> size()<<endl;
            cout <<"\t\t * puId :"<<  bj -> puId -> size() <<endl;

            cout <<"\t\t * Pt :"<<  ((TLorentzVector*) bj -> p4 -> At(iJet)) ->Pt() <<endl;
            cout <<"\t\t * Pt :"<<  ((TLorentzVector*) bj -> p4 -> At(iJet)) ->Eta() <<endl;
            cout <<"\t\t * Pt :"<<  ((TLorentzVector*) bj -> p4 -> At(iJet)) ->Phi() <<endl;
        }
#endif
//
        bool id = (bj->selBits -> at( iJet)  ) & BareJets::Selection::JetLoose;
        if (not id) continue;

        Jet *j =new Jet();
        j->SetP4( *(TLorentzVector*) ((*bj->p4)[iJet]) );
        // JES
        //Log(__FUNCTION__,"DEBUG",Form("Going to Fill Jes for jet: %d",iJet));

        j->SetValueUp  (Smearer::JES , (1. + bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        j->SetValueDown(Smearer::JES , (1. - bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt() ); //
        j->SetFilled(Smearer::JES);

        //Log(__FUNCTION__,"DEBUG",Form(" JesUp=%f", (1. + bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt()));
        //Log(__FUNCTION__,"DEBUG",Form(" JesDown=%f", (1. - bj -> unc -> at(iJet) ) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt()));

        // JER
        //Log(__FUNCTION__,"DEBUG","Going to Fill Jer");
        if (event_->IsRealData())
        {
            j->SetFilled(Smearer::JER,false);
        }
        else{
            j->SetValueUp  (Smearer::JER , bj->ptResUncUp->at(iJet)   );
            j->SetValueDown(Smearer::JER , bj->ptResUncDown->at(iJet) );
            j->SetFilled(Smearer::JER);
        }

        //Log(__FUNCTION__,"DEBUG","-> Done");
        // ---
        j->bdiscr = bj -> bDiscr -> at(iJet);

        if (tree_->GetBranchStatus("jetQGL") ) j->SetQGL( bj -> qgl -> at(iJet) );
        else j->SetQGL(  -10 ); // Add a warning ? 

        if (tree_->GetBranchStatus("jetQglMult") ) j->SetQGLVar( "mult", bj -> qglMult -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglNMult") ) j->SetQGLVar( "nmult", bj -> qglNMult -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglCMult") ) j->SetQGLVar( "cmult", bj -> qglCMult -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglPtD") ) j->SetQGLVar( "ptD", bj -> qglPtD -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglPtDrLog") ) j->SetQGLVar( "PtDrLog", bj -> qglPtDrLog -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglAxis2") ) j->SetQGLVar( "axis2", bj -> qglAxis2 -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglAxis1") ) j->SetQGLVar( "axis1", bj -> qglAxis1 -> at(iJet) );
        
        j->pdgId =  bj->matchedPartonPdgId -> at(iJet);
        j->motherPdgId = bj->motherPdgId -> at(iJet);
        j->grMotherPdgId =  bj-> grMotherPdgId -> at(iJet);
        j->SetPuId(bj -> puId -> at(iJet));

        // add it
        event_ -> jets_ . push_back(j);
    }
    return;

}// end fill jets


void LoadNero::FillLeptons(){
    // Fill Leptons
#ifdef VERBOSE
    if(VERBOSE>1)Log(__FUNCTION__,"DEBUG","Filling Leptons");
#endif
    BareLeptons *bl = dynamic_cast<BareLeptons*> ( bare_[ names_["BareLeptons"] ]); assert(bl != NULL ) ;
    BareTrigger *tr = dynamic_cast<BareTrigger*> ( bare_[names_["BareTrigger"]]);

    if ( tree_ ->GetBranchStatus("lepP4") ==0  ){ 
        LogN(__FUNCTION__,"WARNING","Leptons not FILLED",10);
        return;
    }

    for (int iL = 0;iL<bl->p4->GetEntries() ;++iL)
    {
        bool id = (bl->selBits->at(iL)) & BareLeptons::Selection::LepLoose;
        if (not id) continue;
        Lepton *l = new Lepton();
        l-> SetType( abs((*bl->pdgId)[iL]) );
        //l->SetP4( *(TLorentzVector*) ((*bl->p4)[iL]) );
        TLorentzVector lp4= *(TLorentzVector*) ((*bl->p4)[iL]);

        #warning Using Electrons MiniAOD P4 as is w/o corrections
        if (l->GetType() == 11) {
            lp4 *= bl->lepPfPt->at(iL) / lp4.Pt();
        }

        l-> SetIso ( (*bl->iso) [iL]) ;
        l-> SetMva ( (*bl->mva) [iL]);
        if(tree_->GetBranchStatus("lepMiniIso")) {
            l-> SetMiniIso  ( (*bl->miniIso) [iL]);
        } else {
            l-> SetMiniIso  ( -999 );
        }

        #warning ELE DELTA BETA
        if (l->GetType() == 11) {
            l->SetIso ((*bl->chIso) [iL]  +  TMath::Max( (*bl->nhIso) [iL] + (*bl->phoIso) [iL] - .5*(*bl->puIso) [iL], 0. ) );
        }
        l-> SetP4( lp4 );
        //l-> iso = ((*bl->iso) [iL])/(l->Pt());
        l-> SetCharge (((*bl->pdgId)[iL] >0) ?  -1: 1 ); 
        l-> SetTightId (( bl->selBits -> at(iL) & BareLeptons::Selection::LepTight)); 
        l-> SetMediumId ((bl->selBits ->at(iL) & BareLeptons::Selection::LepMedium));
        if (event_->IsRealData() and event_->runNum() <= 278801 and l->GetType() == 13) // B->F(HIP)
        {
            l-> SetMediumId ((bl->selBits ->at(iL) & BareLeptons::Selection::MuMediumB2F) );
        }
        //if (event_->IsRealData() and event_->runNum() >= 278802) // F->G (HIP_FIXED)
        //
        l-> SetPfPt  ( (*bl->lepPfPt) [iL]);
        l-> SetR9    ( (*bl->r9) [iL]);
        l-> SetEtaSC ( (*bl->etaSC) [iL]);

#ifdef VERBOSE
        if(VERBOSE>1) cout<<"[LoadNero]::[FillLeps]::[DEBUG] Filling Lep Trigger"<<endl;
#endif
        l->trigger =  0;
        if (tree_ -> GetBranchStatus("triggerLeps") !=0  && tr -> triggerLeps ->size() >iL) l->trigger = tr->triggerLeps->at(iL);

        event_ -> leps_ . push_back(l);
    }

} // end fill leptons


void LoadNero::FillPhotons(){
    BarePhotons*b = dynamic_cast<BarePhotons*>(bare_[names_["BarePhotons"] ] ) ; 
    assert(b!=NULL);

    if ( tree_ ->GetBranchStatus("photonP4") ==0  ){ 
        LogN(__FUNCTION__,"WARNING","Photons not FILLED",10);
        return;
    }

    for (int i = 0;i<b->p4->GetEntries() ;++i)
    {
        //bool id = (b->selBits->at(i)) & BarePhotons::Selection::PhoMedium;
        //if (not id) continue;
        bool eleVeto= b->selBits->at(i) & (1UL<<7); // v1.2.1
        if (not eleVeto) continue;

        Photon *p = new Photon();
        p->SetP4( *(TLorentzVector*) ((*b->p4)[i]) );
        p->iso = b->chIso->at(i);
        p->id = (b->selBits->at(i));
        event_ -> phos_ . push_back(p);
    }
    return;
} // end fill photons

void LoadNero::FillTaus(){
    //Fill Tau
#ifdef VERBOSE
    if(VERBOSE>1) Log(__FUNCTION__,"DEBUG","Filling Taus");
#endif
    BareTaus *bt = dynamic_cast<BareTaus*> ( bare_[ names_["BareTaus"] ]); assert (bt != NULL ) ;
    BareTrigger *tr = dynamic_cast<BareTrigger*> ( bare_[names_["BareTrigger"]]);

    if ( tree_ -> GetBranchStatus("tauP4") ==0 ){ 
        LogN(__FUNCTION__,"WARNING","Taus not FILLED",10);
        return;
    }

    for (int iL = 0; iL<bt -> p4 -> GetEntries() ;++iL)
    {
#ifdef VERBOSE
        if(VERBOSE>1)cout <<"[LoadNero]::[FillTaus]::[DEBUG] Filling Taus n."<<iL <<" of "<<bt -> p4 -> GetEntries() <<endl;
        if(VERBOSE>1)cout<<"\t iso= " << bt->iso->size()<<endl;
        if(VERBOSE>1)cout<<"\t charge= " << bt->Q->size()<<endl;
        if(VERBOSE>1)cout<<"\t iso2= " << bt->isoDeltaBetaCorr->size()<<endl;
        if(VERBOSE>1)cout<<"\t selBits= " << bt->selBits->size()<<endl;
        if(VERBOSE>1)cout<<"\t match= " << bt->match->size()<<endl;
#endif
        Tau *t = new Tau();
        t->SetP4( *(TLorentzVector*) ((*bt->p4)[iL]) );
        t-> SetIso ((*bt->iso) [iL] );
        t-> SetCharge( bt -> Q -> at(iL) );
        t-> SetType( 15 );
        t-> SetId  ((bt -> selBits -> at(iL) ) & BareTaus::Selection::TauDecayModeFindingNewDMs );
        t-> SetOldId(  (bt -> selBits -> at(iL) ) & BareTaus::Selection::TauDecayModeFinding ) ;
        t-> SetIso2 ( bt -> isoDeltaBetaCorr -> at(iL) );
        //t-> iso2 = bt -> isoMva -> at(iL);
        //t-> id_ele = (bt -> selBits -> at(iL) ) & BareTaus::Selection::AgainstEleMedium ; 
        t-> SetIdEle( (bt -> selBits -> at(iL) ) & BareTaus::Selection::AgainstEleTight ); 
        t-> SetIdMu (( bt -> selBits -> at(iL) ) & BareTaus::Selection::AgainstMuLoose ); 
        t-> SetMatch( bt -> match -> at(iL) );
        //t-> id_iso = ( bt -> selBits -> at(iL) ) & (BareTaus::byMediumCombinedIsolationDeltaBetaCorr3Hits); 
        t-> SetIdIso (( bt -> selBits -> at(iL) ) & (BareTaus::byLooseCombinedIsolationDeltaBetaCorr3Hits) ); 
        //t-> id_iso = ( bt -> selBits -> at(iL) ) & (BareTaus::byMediumIsolationMVArun2v1DBoldDMwLT); 
        if(bt->leadTrackPt->size() >iL ) t->SetTrackPt( bt->leadTrackPt -> at(iL) ) ;
        else LogN(__FUNCTION__,"WARNING","Lead Track PT not filled",30);

        if (bt -> selBits -> at(iL) & BareTaus::Selection::OneProng) t-> SetNProng( 1 );
        else if (bt -> selBits -> at(iL) & BareTaus::Selection::TwoProng) t-> SetNProng( 2 );
        else if (bt -> selBits -> at(iL) & BareTaus::Selection::ThreeProng) t-> SetNProng( 3 );
        else t->SetNProng(0);

        if (bt -> selBits -> at(iL) & BareTaus::Selection::OnePiZero) t-> SetNPiZero( 1 );
        else if (bt -> selBits -> at(iL) & BareTaus::Selection::TwoPiZero) t-> SetNPiZero( 2 );
        else if (bt -> selBits -> at(iL) & BareTaus::Selection::ThreePiZero) t-> SetNPiZero( 3 );
        else t->SetNPiZero(0);



        //---------------------------------------------
#ifdef VERBOSE
        if(VERBOSE>1) cout<<"[LoadNero]::[FillTaus]::[DEBUG] Filling Taus Trigger"<<endl;
#endif
        t->trigger =  0;
        t->triggerNone =  0;
        if (tree_ -> GetBranchStatus("triggerTaus") !=0  && tr -> triggerTaus ->size() >iL) t->trigger = tr->triggerTaus->at(iL);
        if (tree_ -> GetBranchStatus("triggerNoneTaus") !=0  && tr -> triggerNoneTaus ->size() >iL) t->triggerNone = tr->triggerNoneTaus->at(iL);

        //---------------------------------------------
#ifdef VERBOSE
        if(VERBOSE>1) cout<<"[LoadNero]::[FillTaus]::[DEBUG] Tau Regression"<<endl;
#endif
        BareVertex *v = dynamic_cast<BareVertex*> ( bare_ [names_["BareVertex"] ] ) ; assert(v!=NULL);
        BareJets *bj = dynamic_cast<BareJets*> ( bare_ [names_["BareJets"] ] ) ; assert(bj!=NULL);
        // --- duplicate regression variables
        t -> regVars_ . nvtx    = v -> npv;
        t -> regVars_ . tauPt   =  t->Pt() ; //  just copied, no corrections
        t -> regVars_ . tauEta  =  t->Eta() ;
        t -> regVars_ . tauIso  = (*bt->iso) [iL];
        t -> regVars_ . tauQ    = bt -> Q -> at(iL);
        t -> regVars_ . tauIso2 = bt -> isoDeltaBetaCorr -> at(iL);
        t -> regVars_ . tauM    = bt -> M -> at(iL);
        t -> regVars_ . tauChargedIsoPtSum  = bt -> chargedIsoPtSum -> at(iL);

        t -> regVars_ . jetPt =-10;
        t -> regVars_ . jetEta =-10;
        for(int ij=0;ij< bj->p4->GetEntries() ;ij++)
        {
            TLorentzVector* j = (TLorentzVector*)bj->p4->At(ij);
            if ( t -> GetP4() . DeltaR(*j) > 0.1) continue;
            t -> regVars_ . jetPt = j->Pt();
            t -> regVars_ . jetEta = j->Eta();
            break;
        }


        t -> regVars_ . tauNeutralIsoPtSum  = bt -> neutralIsoPtSum -> at(iL);

        //---------------------------------------------
        event_ -> taus_ . push_back(t);
    }
    //cout<<"[LoadNero]::[FillTaus]::[DEBUB] Taus Loaded:"<< event_->taus_.size() <<endl;


} // end fill taus

void LoadNero::FillMet(){
    // FillMEt
#ifdef VERBOSE
    if(VERBOSE>0)Log(__FUNCTION__,"DEBUG","Filling MET");
#endif
    BareMet * met = dynamic_cast<BareMet*> ( bare_[ names_["BareMet"]]);

    if ( tree_ -> GetBranchStatus("metP4") == 0 ){ 
        LogN(__FUNCTION__,"WARNING","MET not FILLED",10);
        return;
    }

    if ( met->p4 ->GetEntries() != 1)
        Log(__FUNCTION__,"ERROR",string("MET should have exactly 1 entry instead of") + Form("%u",met->p4->GetEntries()) );

    //event_ -> met_ . SetP4 ( * met -> pfMet_e3p0 ) ;
    // #ifdef VERBOSE
    //     if (VERBOSE>1) cout<<"[LoadNero]::[FillMet]::[DEBUG] Met PtUp ==1: "<<met-> ptJESUP -> size()<<endl;
    //     if (VERBOSE>1) cout<<"[LoadNero]::[FillMet]::[DEBUG] Met PtDown ==1: "<<met-> ptJESDOWN -> size()<<endl;;
    // #endif
    event_ -> met_ . SetP4 ( *(TLorentzVector*)(*met -> p4) [0]) ;
    //event_ -> met_ . SetP4 ( * met -> metPuppi ) ;
    //event_ -> met_ . SetP4 ( * met -> metNoHF ) ;
    //event_ -> met_ . ptUp = met-> ptJESUP -> at(0);
    //event_ -> met_ . ptDown = met-> ptJESDOWN -> at(0);

    // ---  JES ---
    // Log(__FUNCTION__,"DEBUG","Going to Fill Jes MET");
    // -- #warning Computing Met JES from jets
    // -- LogN(__FUNCTION__,"INFO","Met JES computed from jets",5);
    // -- BareJets *bj = dynamic_cast<BareJets*> ( bare_ [ names_[ "BareJets" ] ] ); assert (bj !=NULL);
    // -- TLorentzVector jesUp, jesDown;
    // -- jesUp  = *(TLorentzVector*)(*met -> p4) [0];
    // -- jesDown= *(TLorentzVector*)(*met -> p4) [0];
    // -- for (int iJet=0;iJet< bj -> p4 ->GetEntries() ; ++iJet)  
    // -- {
    // --     TLorentzVector delta;
    // --     delta.SetPtEtaPhiE(  bj -> unc -> at(iJet) * ((TLorentzVector*)(*bj->p4)[iJet])->Pt(), ((TLorentzVector*)(*bj->p4)[iJet])->Eta(), ((TLorentzVector*)(*bj->p4)[iJet])->Phi(), ((TLorentzVector*)(*bj->p4)[iJet])->E() * bj -> unc -> at(iJet));
    // --     jesUp += delta;
    // --     jesDown -= delta;
    // -- }
    // -- event_ -> met_ . SetValueUp  (Smearer::JES , jesUp.Pt() ); 
    // -- event_ -> met_ . SetValueDown(Smearer::JES , jesDown.Pt() );

    // JES from MiniAOD
    event_ -> met_ . SetValueUp  (Smearer::JES , ((TLorentzVector*)(*met->metSyst)[BareMet::JesUp]) -> Pt() );
    event_ -> met_ . SetValueDown(Smearer::JES , ((TLorentzVector*)(*met->metSyst)[BareMet::JesDown]) -> Pt() );
    event_-> met_ . SetFilled(Smearer::JES);

    // JER
    event_ -> met_ . SetValueUp  (Smearer::JER , ((TLorentzVector*)(*met->metSyst)[BareMet::JerUp]) -> Pt() );
    event_ -> met_ . SetValueDown(Smearer::JER , ((TLorentzVector*)(*met->metSyst)[BareMet::JerDown]) -> Pt() );
    event_-> met_ . SetFilled(Smearer::JER);

    // Unclustered
    // UnclusterUp, UnclusterDown
    event_ -> met_ . SetValueUp  (Smearer::UNCLUSTER , ((TLorentzVector*)(*met->metSyst)[BareMet::UnclusterUp]) -> Pt() );
    event_ -> met_ . SetValueDown(Smearer::UNCLUSTER , ((TLorentzVector*)(*met->metSyst)[BareMet::UnclusterDown]) -> Pt() );
    event_-> met_ . SetFilled(Smearer::UNCLUSTER);

    // ---- TAU SCALE
    // Taus are already filled
    /* tau scale propagated on the all miniaod tau collection
        BareTaus *bt = dynamic_cast<BareTaus*> ( bare_ [ names_[ "BareTaus" ] ] ); assert (bt !=NULL);
        TLorentzVector tauUp, tauDown;
        tauUp  = *(TLorentzVector*)(*met -> p4) [0];
        tauDown= *(TLorentzVector*)(*met -> p4) [0];
        for (int iTau=0;iTau< bt -> p4 ->GetEntries() ; ++iTau)  
        {
            TLorentzVector delta;
            delta.SetPtEtaPhiE(  1.03 * ((TLorentzVector*)(*bt->p4)[iTau])->Pt(), ((TLorentzVector*)(*bt->p4)[iTau])->Eta(), ((TLorentzVector*)(*bt->p4)[iTau])->Phi(), ((TLorentzVector*)(*bt->p4)[iTau])->E() * 1.03);
            tauUp += delta;
            tauDown -= delta;
        }
    */
    
    // tau scale propagated only on the leading tau
    TLorentzVector tauUp, tauDown;
    tauUp  = *(TLorentzVector*)(*met -> p4) [0];
    tauDown= *(TLorentzVector*)(*met -> p4) [0];

    for (auto& t : event_->taus_) { 
        t->SetIsoCut(2.5); 
        t->SetEtaCut(2.1); 
        t->SetPtCut(20); 
        t->SetMuRej(true); 
        t->SetEleRej(true);
        t->SetTrackPtCut(30.);
    }
    Tau*t = event_->GetTau(0);
    if (t!=NULL){
            TLorentzVector delta;
            delta.SetPtEtaPhiE(  .03 * t->Pt(), t->Eta(), t->Phi(), t->E() * .03);
            tauUp += delta;
            tauDown -= delta;
    }

    event_ -> met_ . SetValueDown(Smearer::TAUESCALE , tauUp.Pt() );
    event_ -> met_ . SetValueUp(Smearer::TAUESCALE , tauDown.Pt() );
    event_-> met_ . SetFilled(Smearer::TAUESCALE);

    //Log(__FUNCTION__,"DEBUG",Form("Met=%f MetJesUp=%f",((TLorentzVector*)(*met -> p4) [0])->Pt() , ((TLorentzVector*)(*met->metSyst)[BareMet::JesUp]) -> Pt()));

    //Log(__FUNCTION__,"DEBUG","Going to Fill Jer MET");
    // ---  JER ---
    if (event_->IsRealData())
    {
        event_-> met_ . SetFilled(Smearer::JER,false);
    }
    else{
        event_ -> met_ . SetValueUp  ( Smearer::JER ,( (TLorentzVector*)(*met->metSyst)[BareMet::JerUp]) -> Pt()   );
        event_ -> met_ . SetValueDown( Smearer::JER ,( (TLorentzVector*)(*met->metSyst)[BareMet::JerDown]) -> Pt() );
        event_-> met_ . SetFilled(Smearer::JER);
    }
    //Log(__FUNCTION__,"DEBUG","Done");


#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[LoadNero]::[FillMet]::[DEBUG] Met XXX is ="<< event_->met_.Pt() << "=="<< event_->met_.PtUncorr()<<endl;
    if(VERBOSE>1)cout <<"[LoadNero]::[FillMet]::[DEBUG] Met is ="<< event_->GetMet().Pt() << "=="<< event_->GetMet().PtUncorr()<<endl;
    if (VERBOSE>1) cout<<"[LoadNero]::[FillMet]::[DEBUG] GEN Info "<<endl;
#endif

    // FILL GEN MET
    if ( event_->IsRealData() ) event_ -> met_ . gen = 0;
    else event_ -> met_ . gen =( (TLorentzVector*)(*met->genP4)[0] )->Pt();

#ifdef VERBOSE
    if (VERBOSE>1) cout<<"[LoadNero]::[FillMet]::[DEBUG] Grace Exit "<<endl;
#endif

} // end fill Met

void LoadNero::FillMC(){
    // FillMonteCarlo
#ifdef VERBOSE
    if(VERBOSE>1)
        Log(__FUNCTION__,"DEBUG","Filling Monte Carlo Information");
#endif
    BareMonteCarlo * mc = dynamic_cast<BareMonteCarlo*> ( bare_[ names_["BareMonteCarlo"]]);

    event_ -> GetWeight() -> SetMcWeight(  mc->mcWeight );

    if(tree_->GetBranchStatus("genTtbarId")) {
        event_ -> SetGenTtbarId( mc->genTtbarId );
    } else {
        event_ -> SetGenTtbarId( -999 );
    }

    if (tree_->GetBranchStatus("r1f2") )  // after setmcw, because it will reset the status of the scales
    {
        event_ -> GetWeight() -> SetScaleWeight( mc->r1f2 , MC::r1f2 ) ;
        event_ -> GetWeight() -> SetScaleWeight( mc->r1f5 , MC::r1f5 ) ;
        event_ -> GetWeight() -> SetScaleWeight( mc->r2f1 , MC::r2f1 ) ;
        event_ -> GetWeight() -> SetScaleWeight( mc->r2f2 , MC::r2f2 ) ;
        event_ -> GetWeight() -> SetScaleWeight( mc->r5f1 , MC::r5f1 ) ;
        event_ -> GetWeight() -> SetScaleWeight( mc->r5f5 , MC::r5f5 ) ;
    }
    else{
        LogN(__FUNCTION__,"WARNING","Running w/o scale uncertainties. Correct if no-syst.",10);
    }
    if (tree_->GetBranchStatus("pdfRwgt") and mc->pdfRwgt->size() >= MC_MAX_PDFS)
    {
        for (unsigned i=0 ; i< MC_MAX_PDFS;++i) event_->GetWeight()->SetPdfWeight( mc->pdfRwgt->at(i), i);
    }
    else{
        LogN(__FUNCTION__,"WARNING","Running w/o pdf uncertainties. Correct if no-syst.",10);
    }

    event_ -> GetWeight() -> SetPU( mc -> puTrueInt ,  event_ -> runNum_);

    if ( tree_->GetBranchStatus("genP4") == 0  ){ 
        LogN(__FUNCTION__,"WARNING","Gen Particles not FILLED",10);
        return;
    }

#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[LoadNero]::[FillMC]::[DEBUG] Reading GP:" 
        << mc->p4->GetEntries()<<":"
            << mc->pdgId->size()<<":"
            << mc->flags->size()<<":"
            << mc->parent->size()<<":"
            <<endl;
#endif

    for (int iGP=0;iGP< mc -> p4 ->GetEntries() ; ++iGP)
    {
        int apdg = abs(mc -> pdgId -> at(iGP) );
        bool keep=false;
        if (  (apdg == 11 or apdg ==13) and (mc -> flags ->at(iGP) & BareMonteCarlo::PromptFinalState) ) keep=true; // keep status 1 electrons and muons
        // keep all regardless to the flag
        if (  (apdg == 11 or apdg ==13) ) keep=true; // keep status 1 electrons and muons
        // keep Q/G/Tau
        if ( (apdg == 15 or apdg==21 or apdg <6 ) and (mc->flags->at(iGP) & ( BareMonteCarlo::HardProcess | BareMonteCarlo::HardProcessBeforeFSR | BareMonteCarlo::HardProcessDecayed) )) keep=true;
        if (  (apdg == 24 or apdg ==23 or apdg ==37) )  keep=true; // keep W/Z/chHiggs
        if (  (apdg == 5 or apdg ==6 ) ) keep=true; // keep top bottom

        if (not keep) continue;

        int motherPdgId = -1;
        int motherPdgIdx = -1;
        int grandMotherPdgId = -1;

        // 76X has no gen parent inforamtion, 
        if (mc->parent->size()>0){
            if( mc -> parent -> at(iGP) != -1) { motherPdgIdx = mc ->parent -> at(iGP); motherPdgId = mc -> pdgId -> at(motherPdgIdx); }
            if(motherPdgIdx != -1 and mc -> parent -> at(motherPdgIdx) != -1) { grandMotherPdgId = mc -> pdgId -> at( mc ->parent -> at(motherPdgIdx)); }
        }

        GenParticle *g =new GenParticle();
        g->SetP4( *(TLorentzVector*) ((*mc->p4)[iGP]) );
        g->SetPdgId( mc -> pdgId -> at(iGP));
        if (mc->parent->size()>0) g->SetParentIdx( mc ->parent -> at(iGP));
        g->SetParentPdgId(motherPdgId);
        g->SetGrandParentPdgId(grandMotherPdgId);
        g->SetFlags( mc ->flags ->at(iGP) );
        event_ -> genparticles_ . push_back(g);
    }
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[LoadNero]::[FillMC]::[DEBUG] Reading END" <<endl;
#endif
    return ;

} // end fill MC

void LoadNero::FillTrigger(){
    // This load the event trigger, not the trigger-object association
    BareTrigger *tr = dynamic_cast<BareTrigger*> ( bare_[names_["BareTrigger"]]);
    if (tree_->GetBranchStatus( "triggerFired") == 0 )
    {
        LogN(__FUNCTION__,"WARNING","Trigger (Event) Not FILLED",10);
        return;
    }
    event_ -> triggerFired_ . clear();
    for(size_t i=0;i< tr ->triggerFired ->size() ;++i)
        event_ -> triggerFired_ . push_back ( bool( (*tr->triggerFired)[i]  ) );

} // end fill trigger
void LoadNero::NewFile(){
    // LOAD TRIGGER NAMES
    TNamed * tn=  (TNamed*)tree_->GetFile()->Get("nero/triggerNames");

    if (tn == NULL ) 
    {
        string fname = tree_->GetFile()->GetName();
        Log(__FUNCTION__,"ERROR","No Trigger Information in file" + fname );
    }

    // split by comma and save in the  triggerNames_
    {
        event_ -> triggerNames_ . clear();
        stringstream ss(tn->GetTitle());
        string token;
        while(std::getline(ss, token, ',')) {
            //std::cout << token << '\n';
#ifdef VERBOSE
            if(VERBOSE>0)cout<<"[LoadNero]::[NewFile]::[DEBUG] TriggerNames"<< event_->triggerNames_ .size() <<" = "<<token<<endl;
#endif
            event_ -> triggerNames_ .push_back(token);
        }
        event_ -> IsTriggered(""); // reset trigger caching
    }
}; // should take care of loading the trigger names
// ---------------------------END NERO ---------------------

// Register
REGISTER(LoadNero);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
