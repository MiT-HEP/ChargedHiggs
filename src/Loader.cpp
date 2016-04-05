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
        cout <<"[Looper]::[FillJets]::[DEBUG] Jets length:"<<endl;
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
            cout <<"[Looper]::[FillJets]::[DEBUG2] considering jet: "<<iJet << " / "<< bj -> p4 ->GetEntries() <<endl;
            cout <<"\t\t * selBits size: "<< bj->selBits ->size()<<endl;
            cout <<"\t\t * unc size:" << bj -> unc ->size() <<endl;
            cout <<"\t\t * bdiscr size :"<< bj -> bDiscr ->size() <<endl;

            cout <<"\t\t * pdgId :" <<bj -> matchedPartonPdgId -> size()<<endl;
            cout <<"\t\t * mother :"<<bj->motherPdgId ->size()<<endl;
            cout <<"\t\t * gr mother:"<<  bj-> grMotherPdgId -> size()<<endl;
            cout <<"\t\t * puId :"<<  bj -> puId -> size() <<endl;
        }
#endif

        bool id = (bj->selBits -> at( iJet)  ) & BareJets::Selection::JetLoose;
        if (not id) continue;

        Jet *j =new Jet();
        j->SetP4( *(TLorentzVector*) ((*bj->p4)[iJet]) );
        j->unc = bj -> unc -> at(iJet); //
        j->bdiscr = bj -> bDiscr -> at(iJet);

        if (tree_->GetBranchStatus("jetQGL") ) j->SetQGL( bj -> qgl -> at(iJet) );
        else j->SetQGL(  -10 ); // Add a warning ? 

        if (tree_->GetBranchStatus("jetQglMult") ) j->SetQGLVar( "mult", bj -> qglMult -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglPtD") ) j->SetQGLVar( "ptD", bj -> qglPtD -> at(iJet) );
        if (tree_->GetBranchStatus("jetQglAxis2") ) j->SetQGLVar( "axis2", bj -> qglAxis2 -> at(iJet) );

        j->pdgId =  bj->matchedPartonPdgId -> at(iJet);
        j->motherPdgId = bj->motherPdgId -> at(iJet);
        j->grMotherPdgId =  bj-> grMotherPdgId -> at(iJet);
        j->puId = bj -> puId -> at(iJet);

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
        //if (not id) continue;
        Lepton *l = new Lepton();
        l->SetP4( *(TLorentzVector*) ((*bl->p4)[iL]) );
        l-> iso = ((*bl->iso) [iL])/(l->Pt());
        l-> charge = ((*bl->pdgId)[iL] >0) ?  -1: 1; 
        l-> type = abs((*bl->pdgId)[iL]);
        l-> tightId = ( bl->selBits -> at(iL) & BareLeptons::Selection::LepTight); 

#ifdef VERBOSE
        if(VERBOSE>1) cout<<"[Looper]::[FillLeps]::[DEBUG] Filling Lep Trigger"<<endl;
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
        if(VERBOSE>1)cout <<"[Looper]::[FillTaus]::[DEBUG] Filling Taus n."<<iL <<" of "<<bt -> p4 -> GetEntries() <<endl;
        if(VERBOSE>1)cout<<"\t iso= " << bt->iso->size()<<endl;
        if(VERBOSE>1)cout<<"\t charge= " << bt->Q->size()<<endl;
        if(VERBOSE>1)cout<<"\t iso2= " << bt->isoDeltaBetaCorr->size()<<endl;
        if(VERBOSE>1)cout<<"\t selBits= " << bt->selBits->size()<<endl;
        if(VERBOSE>1)cout<<"\t match= " << bt->match->size()<<endl;
#endif
        Tau *t = new Tau();
        t->SetP4( *(TLorentzVector*) ((*bt->p4)[iL]) );
        t-> iso = (*bt->iso) [iL];
        t-> charge = bt -> Q -> at(iL);
        t-> type = 15;
        //t-> id =  (bt -> selBits -> at(iL) ) & BareTaus::Selection::TauDecayModeFinding;
        t-> id =  (bt -> selBits -> at(iL) ) & BareTaus::Selection::TauDecayModeFindingNewDMs;
        t-> iso2 = bt -> isoDeltaBetaCorr -> at(iL);
        t-> id_ele = (bt -> selBits -> at(iL) ) & BareTaus::Selection::AgainstEleLoose ; 
        t-> id_mu = ( bt -> selBits -> at(iL) ) & BareTaus::Selection::AgainstMuLoose; 
        t-> match = bt -> match -> at(iL);

        //---------------------------------------------
#ifdef VERBOSE
        if(VERBOSE>1) cout<<"[Looper]::[FillTaus]::[DEBUG] Filling Taus Trigger"<<endl;
#endif
        t->trigger =  0;
        t->triggerNone =  0;
        if (tree_ -> GetBranchStatus("triggerTaus") !=0  && tr -> triggerTaus ->size() >iL) t->trigger = tr->triggerTaus->at(iL);
        if (tree_ -> GetBranchStatus("triggerNoneTaus") !=0  && tr -> triggerNoneTaus ->size() >iL) t->triggerNone = tr->triggerNoneTaus->at(iL);

        //---------------------------------------------
#ifdef VERBOSE
        if(VERBOSE>1) cout<<"[Looper]::[FillTaus]::[DEBUG] Tau Regression"<<endl;
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
    //cout<<"[Looper]::[FillTaus]::[DEBUB] Taus Loaded:"<< event_->taus_.size() <<endl;


} // end fill taus
void LoadNero::FillMet(){
    // FillMEt
#ifdef VERBOSE
    if(VERBOSE>1)Log(__FUNCTION__,"DEBUG","Filling MET");
#endif
    BareMet * met = dynamic_cast<BareMet*> ( bare_[ names_["BareMet"]]);

    if ( tree_ -> GetBranchStatus("metP4") == 0 ){ 
        LogN(__FUNCTION__,"WARNING","MET not FILLED",10);
        return;
    }

    if ( met->p4 ->GetEntries() != 1)
        Log(__FUNCTION__,"ERROR",string("MET should have exactly 1 entry instead of") + Form("%u",met->p4->GetEntries()) );

    //event_ -> met_ . SetP4 ( * met -> pfMet_e3p0 ) ;
#ifdef VERBOSE
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] Met PtUp ==1: "<<met-> ptJESUP -> size()<<endl;
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] Met PtDown ==1: "<<met-> ptJESDOWN -> size()<<endl;;
#endif
    event_ -> met_ . SetP4 ( *(TLorentzVector*)(*met -> p4) [0]) ;
    //event_ -> met_ . SetP4 ( * met -> metPuppi ) ;
    //event_ -> met_ . SetP4 ( * met -> metNoHF ) ;
    event_ -> met_ . ptUp = met-> ptJESUP -> at(0);
    event_ -> met_ . ptDown = met-> ptJESDOWN -> at(0);


#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillMet]::[DEBUG] Met XXX is ="<< event_->met_.Pt() << "=="<< event_->met_.PtUncorr()<<endl;
    if(VERBOSE>1)cout <<"[Looper]::[FillMet]::[DEBUG] Met is ="<< event_->GetMet().Pt() << "=="<< event_->GetMet().PtUncorr()<<endl;
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] GEN Info "<<endl;
#endif

    // FILL GEN MET
    if ( event_->IsRealData() ) event_ -> met_ . gen = 0;
    else event_ -> met_ . gen =( (TLorentzVector*)(*met->genP4)[0] )->Pt();

#ifdef VERBOSE
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] Grace Exit "<<endl;
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

    event_ -> GetWeight() -> SetPU( mc -> puTrueInt ,  event_ -> runNum_);

    if ( tree_->GetBranchStatus("genP4") == 0  ){ 
        LogN(__FUNCTION__,"WARNING","Gen Particles not FILLED",10);
        return;
    }

#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillMC]::[DEBUG] Reading GP:" 
        << mc->p4->GetEntries()<<":"
            << mc->pdgId->size()<<":"
            << mc->flags->size()<<":"
            <<endl;
#endif

    for (int iGP=0;iGP< mc -> p4 ->GetEntries() ; ++iGP)
    {
        int apdg = abs(mc -> pdgId -> at(iGP) );
        bool keep=false;
        if (  (apdg == 11 or apdg ==13) and (mc -> flags ->at(iGP) & BareMonteCarlo::PromptFinalState) ) keep=true; // keep status 1 electrons and muons
        // keep Q/G/Tau
        if ( (apdg == 15 or apdg==21 or apdg <6 ) and (mc->flags->at(iGP) & ( BareMonteCarlo::HardProcess | BareMonteCarlo::HardProcessBeforeFSR | BareMonteCarlo::HardProcessDecayed) )) keep=true;

        if (not keep) continue;

        GenParticle *g =new GenParticle();
        g->SetP4( *(TLorentzVector*) ((*mc->p4)[iGP]) );
        g->SetPdgId( mc -> pdgId -> at(iGP));
        g->SetFlags( mc ->flags ->at(iGP) );
        event_ -> genparticles_ . push_back(g);
    }
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillMC]::[DEBUG] Reading END" <<endl;
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
            if(VERBOSE>0)cout<<"[Looper]::[NewFile]::[DEBUG] TriggerNames"<< event_->triggerNames_ .size() <<" = "<<token<<endl;
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
