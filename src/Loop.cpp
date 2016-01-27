#include "interface/Loop.hpp"

#include <assert.h>
#include <iostream>

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

#include "interface/Handlers.hpp"
#include "interface/Logger.hpp"

#include <sstream>

//#define VERBOSE 2

Looper::Looper(){
    output_=new Output(); 
    tree_=new TChain("nero/events");
    event_= new Event(); 
    fNumber = -1;
    define_handlers();
}
Looper::Looper(string chain){
    output_=new Output(); 
    tree_=new TChain(chain.c_str());
    event_= new Event(); 
    fNumber = -1;
    define_handlers();
}

int Looper::InitSmear(){
    int R=0;
    R+=AddSmear("NONE");
    R+=AddSmear("JES");
    R+=AddSmear("JER");
    R+=AddSmear("PU");
    return R;
}

int Looper::InitCorrector(){
	for(auto& c : correctors_ ) { c-> Init() ; }
}

int Looper::AddSmear(string name){
#ifdef VERBOSE
    if(VERBOSE>0)cout <<"[Looper]::[AddSmear] Adding Smear '"<<name<<"'."<<endl;
#endif
    if (name == "NONE" or name == ""){ SmearBase *s = new SmearBase(); systs_ . push_back(s); return 0;}
    if (name == "JES"){ SmearJes *s = new SmearJes(); systs_ . push_back(s); return 0;}
    if (name == "JER"){ SmearJer *s = new SmearJer(); systs_ . push_back(s); return 0;}
    if (name == "PU"){ SmearPu *s = new SmearPu(); systs_ . push_back(s); return 0;}

    cout <<"[Looper]::[AddSmear]::[WARNING] Smear "<<name<<" does NOT exist!!!"<<endl;
    return 1; // maybe throw exception
}

int Looper::InitTree()
{
#ifdef VERBOSE
    if(VERBOSE>0)cout <<"[Looper]::[InitTree] InitTree "<<endl;
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Event "<<endl;
#endif
    // --- declare branches, and structures
    BareEvent *e = new BareEvent(); 
    names_[ "Event" ] = bare_.size();
    bare_.push_back(e);
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Vertex "<<endl;
#endif
    BareVertex *v = new BareVertex(); 
    names_[ "Vertex" ] = bare_.size();
    bare_.push_back(v);
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init MonteCarlo "<<endl;
#endif
    // ---
    BareMonteCarlo *mc = new BareMonteCarlo(); 
    names_[ "MonteCarlo" ] = bare_.size();
    bare_.push_back(mc);
    // ---
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Jets "<<endl;
#endif
    BareJets *j = new BareJets(); 
    names_[ "Jets" ] = bare_.size();
    bare_.push_back(j);
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Taus "<<endl;
#endif
    // ---
    BareTaus *t = new BareTaus(); 
    t->SetExtend();
    t->SetMatch(); // matching informations
    names_[ "Taus" ] = bare_.size();
    bare_.push_back(t);
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Leptons "<<endl;
#endif
    // ---
    BareLeptons *l = new BareLeptons(); 
    names_[ "Leptons" ] = bare_.size();
    bare_.push_back(l);

#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Photons "<<endl;
#endif
    // ---
    BarePhotons *p = new BarePhotons(); 
    names_[ "Photons" ] = bare_.size();
    bare_.push_back(p);

#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Met "<<endl;
#endif
    BareMet *met = new BareMet(); 
    met->SetExtend(); // pfmet3p0
    names_[ "Met" ] = bare_.size();
    bare_.push_back(met);

#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Init Trigger "<<endl;
#endif
    BareTrigger *tr = new BareTrigger(); 
    names_[ "Trigger" ] = bare_.size();
    bare_.push_back(tr);

#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] SetAddresses "<<endl;
#endif
    for (auto c : bare_ )
        c->setBranchAddresses(tree_);

	/// FIXME, id for taus v1.1
    // static int guard=0;
    // if(++guard<10)cout<<" TAUS FIX FOR v1.1"<<endl;
    // BareTaus *bt = dynamic_cast<BareTaus*> ( bare_[ names_["Taus"] ]); assert (bt != NULL ) ;
    // tree_ ->SetBranchAddress("tauId", &bt -> selBits);
    ///

    tree_ -> SetBranchStatus("*",0);
    // branches are activate from configuration file
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[InitTree] Return "<<endl;
#endif
    
    return 0;
}

void Looper::Loop()
{
    unsigned long nEntries = tree_->GetEntries();

    //ostringstream os; os<<"Running on "<<nEntries<<" entries" ;
    Log(__FUNCTION__,"INFO", Form("Running on %d entries",nEntries) );

    sw_. Reset();

    try{	
        for(unsigned long iEntry = 0 ;iEntry< nEntries ;++iEntry)
        {
            if(iEntry %10000 == 0 ) {
                sw_.Stop();
		Log(__FUNCTION__,"INFO",Form("Getting Entry %lu / %lu in (Cpu) %.4f (Real) %.4f",iEntry,nEntries, sw_.CpuTime(),sw_.RealTime()) );
                sw_ .Reset();
                sw_ .Start();
            }

#ifdef VERBOSE
            if (VERBOSE > 1) cout <<"[Looper]::[Loop] Getting Entry "<<iEntry << " of "<<nEntries<<endl;
#endif
            ClearEvent();
            // load tree
            tree_ -> GetEntry(iEntry);
            //move content into the event
            FillEvent();	
            // for each smearing
            for(auto s : systs_)
            {
#ifdef VERBOSE
                if (VERBOSE > 1) cout <<"[Looper]::[Loop] Doing syst "<< s -> name() <<endl;
#endif
                for(int i=-1; i<=1 ;++i)
                {
                    if ( s->name().find("NONE") != string::npos and i!=0) continue;
                    if ( s->name().find("NONE") == string::npos and i==0) continue; // for the 
                    //reset 	
                    event_->clearSyst();
                    // 
                    s->SetSyst(i);
                    //smear
                    s->smear(event_);
		    //do the corrections on top
		    for(auto& c : correctors_)
			    c->correct(event_);

                    //do the analysis
                    for(auto a : analysis_)
                    {
#ifdef VERBOSE
                        if (VERBOSE > 1) Log(__FUNCTION__,"DEBUG", string("Doing Analysis") + a->name());
#endif
                    	event_->validate(); // validate the objects
                        // each analysis step will apply the SF accordingly to the object it is using
                        event_ -> weight_ . clearSF() ;
                        if ( a->doAnalyze(event_,s->name()) > 0 ) break; // go on analyzing event, if no analysis returns >0
                    }
                }
                s->SetSyst(0); // not necessary, but cleaner in this way
            }
        }
    }
    catch( sigint_exception &e)
    {
	Log(__FUNCTION__,"SIGNAL"," Caught SIGINT/SIGTERM: exiting! ");
        Write();
        Close();
        throw e; 
    }
    //call end procedures for the analyis
    for(auto a : analysis_)
        a->doEnd();
    // save output

    Write();
    Close();
    return;	
}

void Looper::ClearEvent(){

    event_ -> ClearEvent();

}

void Looper::NewFile()
{
    fNumber = tree_->GetTreeNumber();
    // check name and weight TODO
    string fname = tree_->GetFile()->GetName();
    event_ -> fName_ = fname;

    Log(__FUNCTION__,"INFO","Openining new file: '"+ fname +"'");
    //"root://eoscms//store/../label/abc.root"
    size_t last = fname.rfind('/');
    //size_t prevLast = fname.rfind('/',last-1);
    size_t eos = fname.find("/store/");
    //string label=fname.substr(prevLast+1,last - 1 - prevLast ); //pos,len
    
    string label="";
    string dir =fname.substr(0,last); // remove the filename
    if (eos != string::npos) // strip out everything before /store/
        dir = dir.substr(eos, string::npos);

    // split by dirs
    vector<string> dirs;

    {
    	istringstream ss (fname);
    	string token;
    	while (std::getline(ss, token, '/')){
    	    if (token.find("root") != string::npos) continue;
    	    if (token.find("eos") != string::npos) continue;
    	    if (token.find("cms") != string::npos) continue;
    	    if (token == "" ) continue;
    	    dirs.push_back(token); 
    	} 
    }// scope loop

    if ( event_->IsRealData() ) {  
        cout<<"[Looper]::[NewFile]::[INFO] Data file found"<<endl;;
        event_ -> weight_ . LoadMC("data");
    }
    // -- Load current MC --
    else {
	// try as labels all the directories in the given order
	//
        string savedDir= "" ;
	int iDir= dirs.size()-1;
	while ( savedDir == "" and iDir>=0 )
		{
		label = dirs[iDir];
		savedDir=event_ -> weight_ . LoadMC( label );
		--iDir;
		}

	// last change
        if (savedDir =="")
        {
            cout<<"[Looper]::[NewFile]::[WARNING] failed to search MC by LABEL '"<<label<<"' search by dir '"<<dir<<"'"<<endl;
            // search for dir
            label = event_ -> weight_ . LoadMCbyDir(dir);
            savedDir = dir;
            cout<<"[Looper]::[NewFile]::[WARNING] label found '"<<label<<"'"<<endl;
        }

        if ( dir != savedDir or label == "")
            cout<<"[Looper]::[NewFile]::[WARNING] saved dir '"<<savedDir<<"' and current dir '"<< dir <<"' label '"<<label<<"'"<<endl;

    } // end MC

    // LOAD TRIGGER NAMES
    TNamed * tn=  (TNamed*)tree_->GetFile()->Get("nero/triggerNames");
    if (tn == NULL ) 
        cout<<"[Looper]::[NewFile]::[ERROR] no trigger informations in file:"<<fname<<endl; 
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
    return;
}

void Looper::FillEventInfo(){

    BareEvent *e = dynamic_cast<BareEvent*> ( bare_ [names_["Event"] ] ) ; assert(e!=NULL);
#ifdef VERBOSE
    if(VERBOSE>0)cout <<"[Looper]::[FillEventInfo]::[INFO] Processing "<<e->runNum<<":"<<e->lumiNum<<":"<<e->eventNum<<endl;
#endif
    event_ -> isRealData_ = e->isRealData;
    event_ -> runNum_ = e->runNum;
    event_ -> lumiNum_ = e->lumiNum;
    event_ -> eventNum_ = e->eventNum;
    event_ -> rho_ = e->rho;

    BareVertex *v = dynamic_cast<BareVertex*> ( bare_ [names_["Vertex"] ] ) ; assert(v!=NULL);
    event_ -> npv_ = v->npv;


}


void Looper::FillJets(){
    //fill Jets
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillJets]::[DEBUG] Filling Jets. FIXME JES" <<endl;
#endif
    BareJets *bj = dynamic_cast<BareJets*> ( bare_ [ names_[ "Jets" ] ] ); assert (bj !=NULL);

    if ( tree_ ->GetBranchStatus("jetP4") == 0 ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillJets]::[WARNING] Jets Not Filled "<<endl; }
        return;
    }

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
	bool id = (bj->selBits -> at( iJet)  ) & BareJets::Selection::JetLoose;
	if (not id) continue;
	// when create an Object, put it into the jets_ stuff, otherwise won't be deleted
        Jet *j =new Jet();
        j->SetP4( *(TLorentzVector*) ((*bj->p4)[iJet]) );
        j->unc = bj -> unc -> at(iJet); //
        j->bdiscr = bj -> bDiscr -> at(iJet);

	if (tree_->GetBranchStatus("jetQGL") ) j->SetQGL( bj -> qgl -> at(iJet) );
	else j->SetQGL(  -10 ); // Add a warning ? 
	// TODO add PuId, and syst
        event_ -> jets_ . push_back(j);
    }
    return;
}

void Looper::FillPhotons(){
	BarePhotons*b = dynamic_cast<BarePhotons*>(bare_[names_["Photons"] ] ) ; 
	if (b == NULL)
	{
		static int count=0;
		if (count <10 )  Log(__FUNCTION__,"WARNING", "No photons available");
		count ++;
		return ;
	}
    	if ( tree_ ->GetBranchStatus("photonP4") ==0  ){ 
		static int count2 = 0;
		count2++;
		if (count2 <10 )  Log(__FUNCTION__,"WARNING", "Photons not filled");
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

}

void Looper::FillLeptons(){
    // Fill Leptons
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillLeptons]::[DEBUG] Filling Leptons" <<endl;
#endif
    BareLeptons *bl = dynamic_cast<BareLeptons*> ( bare_[ names_["Leptons"] ]); assert(bl != NULL ) ;
    BareTrigger *tr = dynamic_cast<BareTrigger*> ( bare_[names_["Trigger"]]);

    if ( tree_ ->GetBranchStatus("lepP4") ==0  ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillLeptons]::[WARNING] Leptons Not Filled "<<endl; }
        return;
    }

    for (int iL = 0;iL<bl->p4->GetEntries() ;++iL)
    {
	bool id = (bl->selBits->at(iL)) & BareLeptons::Selection::LepLoose;
	//if (not id) continue;
        Lepton *l = new Lepton();
        l->SetP4( *(TLorentzVector*) ((*bl->p4)[iL]) );
        l-> iso = (*bl->iso) [iL];
        l-> charge = ((*bl->pdgId)[iL] >0) ?  -1: 1; 
        l-> type = abs((*bl->pdgId)[iL]);
	#ifdef VERBOSE
		if(VERBOSE>1) cout<<"[Looper]::[FillLeps]::[DEBUG] Filling Lep Trigger"<<endl;
	#endif
	l->trigger =  0;
	if (tree_ -> GetBranchStatus("triggerLeps") !=0  && tr -> triggerLeps ->size() >iL) l->trigger = tr->triggerLeps->at(iL);

        event_ -> leps_ . push_back(l);
    }

}

void Looper::FillTaus(){
    //Fill Tau
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillTaus]::[DEBUG] Filling Taus" <<endl;
#endif
    BareTaus *bt = dynamic_cast<BareTaus*> ( bare_[ names_["Taus"] ]); assert (bt != NULL ) ;
    BareTrigger *tr = dynamic_cast<BareTrigger*> ( bare_[names_["Trigger"]]);

    if ( tree_ -> GetBranchStatus("tauP4") ==0 ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillTaus]::[WARNING] Tau Not Filled "<<endl; }
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
        BareVertex *v = dynamic_cast<BareVertex*> ( bare_ [names_["Vertex"] ] ) ; assert(v!=NULL);
        BareJets *bj = dynamic_cast<BareJets*> ( bare_ [names_["Jets"] ] ) ; assert(bj!=NULL);
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

}


void Looper::FillMC(){
    // FillMonteCarlo
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillMC]::[DEBUG] Filling MonteCarlo" <<endl;
#endif
    BareMonteCarlo * mc = dynamic_cast<BareMonteCarlo*> ( bare_[ names_["MonteCarlo"]]);
    event_ -> weight_ . mcWeight_ = mc->mcWeight;

    event_ -> weight_ . SetPU( mc -> puTrueInt ,  event_ -> runNum_);

    if ( tree_->GetBranchStatus("genP4") == 0  ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillMC]::[WARNING] Gen Particles Not Filled "<<endl; }
        return;
    }

    for (int iGP=0;iGP< mc -> p4 ->GetEntries() ; ++iGP)
    {
        GenParticle *g =new GenParticle();
        g->SetP4( *(TLorentzVector*) ((*mc->p4)[iGP]) );
        g->pdgid_ = mc -> pdgId -> at(iGP);
        event_ -> genparticles_ . push_back(g);
    }
    return ;
}

void Looper::FillMet(){
    // FillMEt
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillMet]::[DEBUG] Filling MET" <<endl;
#endif
    BareMet * met = dynamic_cast<BareMet*> ( bare_[ names_["Met"]]);

    if ( tree_ -> GetBranchStatus("metP4") == 0 ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillMet]::[WARNING] MET Not Filled "<<endl; }
        return;
    }

    if ( met->p4 ->GetEntries() != 1)
        cout<<"[Looper]::[FillMet]::[ERROR] MET should have exactly 1 entry instead of "<<met->p4 ->GetEntries() <<endl;

    //event_ -> met_ . SetP4 ( * met -> pfMet_e3p0 ) ;
#ifdef VERBOSE
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] Met PtUp ==1: "<<met-> ptJESUP -> size()<<endl;
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] Met PtDown ==1: "<<met-> ptJESDOWN -> size()<<endl;;
#endif
    //event_ -> met_ . SetP4 ( *(TLorentzVector*)(*met -> p4) [0]) ;
    event_ -> met_ . SetP4 ( * met -> metPuppi ) ;
    //event_ -> met_ . SetP4 ( * met -> metNoHF ) ;
    event_ -> met_ . ptUp = met-> ptJESUP -> at(0);
    event_ -> met_ . ptDown = met-> ptJESDOWN -> at(0);


#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillMet]::[DEBUG] Met XXX is ="<< event_->met_.Pt() << "=="<< event_->met_.PtUncorr()<<endl;
    if(VERBOSE>1)cout <<"[Looper]::[FillMet]::[DEBUG] Met is ="<< event_->GetMet().Pt() << "=="<< event_->GetMet().PtUncorr()<<endl;
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] GEN Info "<<endl;
#endif
    if ( event_->IsRealData() )
	event_ -> met_ . gen = 0;
    else
    	event_ -> met_ . gen =( (TLorentzVector*)(*met->genP4)[0] )->Pt();

#ifdef VERBOSE
    if (VERBOSE>1) cout<<"[Looper]::[FillMet]::[DEBUG] Grace Exit "<<endl;
#endif
}

void Looper::FillTrigger(){
    BareTrigger *tr = dynamic_cast<BareTrigger*> ( bare_[names_["Trigger"]]);
    if (tree_->GetBranchStatus( "triggerFired") == 0 )
    {
        static int counter = 0 ;
        if (counter<10){counter++; cout<<"[Loope]::[FillTrigger]::[WARNING] No trigger"<<endl;}
        return;
    }
    event_ -> triggerFired_ . clear();
    for(size_t i=0;i< tr ->triggerFired ->size() ;++i)
        event_ -> triggerFired_ . push_back ( bool( (*tr->triggerFired)[i]  ) );
}

void Looper::FillEvent(){

    FillEventInfo(); // new file uses isRealData, but not the weights

    if ( tree_ -> GetTreeNumber() != fNumber)
    {
        NewFile();
    }
    //usleep(100); // DEBUG XROOTD
    FillJets();
    FillLeptons();
    FillPhotons();
    FillTaus();
    FillMet();
    FillMC();
    FillTrigger();


#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillEvent]::[DEBUG] Clearing collections" <<endl;
#endif
    for (auto c : bare_)
        c->clear();
#ifdef VERBOSE
    if(VERBOSE>0)cout <<"[Looper]::[FillEvent]::[DONE]"<<endl;
#endif 
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:

