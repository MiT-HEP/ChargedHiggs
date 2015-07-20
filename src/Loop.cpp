#include "interface/Loop.hpp"

#include <assert.h>
#include <iostream>

#include "NeroProducer/Core/interface/BareEvent.hpp"
#include "NeroProducer/Core/interface/BareJets.hpp"
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"
#include "NeroProducer/Core/interface/BareMet.hpp"
#include "NeroProducer/Core/interface/BareFatJets.hpp"
#include "NeroProducer/Core/interface/BareLeptons.hpp"
#include "NeroProducer/Core/interface/BareTaus.hpp"

#include "interface/Handlers.hpp"

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
    // --- declare branches, and structures
    BareEvent *e = new BareEvent(); 
    names_[ "Event" ] = bare_.size();
    bare_.push_back(e);
    // ---
    BareMonteCarlo *mc = new BareMonteCarlo(); 
    names_[ "MonteCarlo" ] = bare_.size();
    bare_.push_back(mc);
    // ---
    BareJets *j = new BareJets(); 
    names_[ "Jets" ] = bare_.size();
    bare_.push_back(j);
    // ---
    BareTaus *t = new BareTaus(); 
    t->SetExtend();
    t->SetMatch(); // matching informations
    names_[ "Taus" ] = bare_.size();
    bare_.push_back(t);
    // ---
    BareLeptons *l = new BareLeptons(); 
    names_[ "Leptons" ] = bare_.size();
    bare_.push_back(l);

    BareMet *met = new BareMet(); 
    names_[ "Met" ] = bare_.size();
    bare_.push_back(met);

    for (auto c : bare_ )
        c->setBranchAddresses(tree_);

    tree_ -> SetBranchStatus("*",0);
    // branches are activate from configuration file
    
    return 0;
}

void Looper::Loop()
{
    unsigned long nEntries = tree_->GetEntries();

    cout<<"[Looper]::[Loop]::[INFO] Running on "<<nEntries<<" entries" <<endl;

    sw_. Reset();

    try{	
        for(unsigned long iEntry = 0 ;iEntry< nEntries ;++iEntry)
        {
            if(iEntry %10000 == 0 ) {
                sw_.Stop();
                cout<<"[Looper]::[Loop]::[INFO] Getting Entry "<<iEntry<<" / "<<nEntries << " in (Cpu)"<< sw_ .CpuTime() <<" (Real) "<< sw_.RealTime()<<endl;
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
                    //do the analysis
                    event_->validate();
                    for(auto a : analysis_)
                    {
#ifdef VERBOSE
                        if (VERBOSE > 1) cout <<"[Looper]::[Loop] Doing Analysis "<<a->name()<<endl;;
#endif
                        // each analysis step will apply the SF accordingly to the object it is using
                        event_ -> weight_ . clearSF() ;
                        if ( a->analyze(event_,s->name()) > 0 ) break; // go on analyzing event, if no analysis returns >0
                    }
                }
                s->SetSyst(0); // not necessary, but cleaner in this way
            }
        }
    }
    catch( sigint_exception &e)
    {
        cout<<" Caught SIGINT/SIGTERM: exiting! "<<endl;
        Write();
        Close();
        throw e; 
    }
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
    cout<<"[Looper]::[NewFile]::[INFO] Opening new file: '"<<fname<<"'"<<endl;
    //"root://eoscms//store/../label/abc.root"
    size_t last = fname.rfind('/');
    size_t prevLast = fname.rfind('/',last-1);
    size_t eos = fname.find("/store/");
    string label=fname.substr(prevLast+1,last - 1 - prevLast ); //pos,len
    string dir =fname.substr(0,last); // remove the filename
    if (eos != string::npos) // strip out everything before /store/
        dir = dir.substr(eos, string::npos);

    if ( event_->IsRealData() ) { 
        cout<<"[Looper]::[NewFile]::[INFO] Data file found"<<label;
        event_ -> weight_ . LoadMC("data");
        return ;
    }
    // -- Load current MC --
    string savedDir=event_ -> weight_ . LoadMC( label );
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

}


void Looper::FillJets(){
    //fill Jets
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillJets]::[DEBUG] Filling Jets: FIXME JES" <<endl;
#endif
    BareJets *bj = dynamic_cast<BareJets*> ( bare_ [ names_[ "Jets" ] ] ); assert (bj !=NULL);

    if ( tree_ ->GetBranchStatus("jetP4") == 0 ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillJets]::[WARNING] Jets Not Filled "<<endl; }
        return;
    }

    for (int iJet=0;iJet< bj -> p4 ->GetEntries() ; ++iJet)
    {
        Jet *j =new Jet();
        j->SetP4( *(TLorentzVector*) ((*bj->p4)[iJet]) );
        j->unc = 0.03; //bj -> unc -> at(iJet); FIXME 3% flat
        j->bdiscr = bj -> bDiscr -> at(iJet);
        event_ -> jets_ . push_back(j);
    }
    return;
}

void Looper::FillLeptons(){
    // Fill Leptons
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillLeptons]::[DEBUG] Filling Leptons" <<endl;
#endif
    BareLeptons *bl = dynamic_cast<BareLeptons*> ( bare_[ names_["Leptons"] ]); assert(bl != NULL ) ;

    if ( tree_ ->GetBranchStatus("lepP4") ==0  ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillLeptons]::[WARNING] Leptons Not Filled "<<endl; }
        return;
    }

    for (int iL = 0;iL<bl->p4->GetEntries() ;++iL)
    {
        Lepton *l = new Lepton();
        l->SetP4( *(TLorentzVector*) ((*bl->p4)[iL]) );
        l-> iso = (*bl->iso) [iL];
        l-> charge = ((*bl->pdgId)[iL] >0) ?  -1: 1; 
        l-> type = abs((*bl->pdgId)[iL]);
        event_ -> leps_ . push_back(l);
    }

}

void Looper::FillTaus(){
    //Fill Tau
#ifdef VERBOSE
    if(VERBOSE>1)cout <<"[Looper]::[FillTaus]::[DEBUG] Filling Taus" <<endl;
#endif
    BareTaus *bt = dynamic_cast<BareTaus*> ( bare_[ names_["Taus"] ]); assert (bt != NULL ) ;

    if ( tree_ -> GetBranchStatus("tauP4") ==0 ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillTaus]::[WARNING] Tau Not Filled "<<endl; }
        return;
    }

    for (int iL = 0; iL<bt -> p4 -> GetEntries() ;++iL)
    {
        Tau *t = new Tau();
        t->SetP4( *(TLorentzVector*) ((*bt->p4)[iL]) );
        t-> iso = (*bt->iso) [iL];
        t-> charge = bt -> Q -> at(iL);
        t-> type = 15;
        t-> id = bt-> id -> at(iL);
        t-> iso2 = bt -> isoDeltaBetaCorr -> at(iL);
        t-> id_ele = bt -> againstEleLoose-> at(iL); 
        t-> id_mu = bt -> againstMuLoose -> at(iL); 
        t-> match = bt -> match -> at(iL);
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
    if(VERBOSE>1)cout <<"[Looper]::[FillEvent]::[DEBUG] Filling MET" <<endl;
#endif
    BareMet * met = dynamic_cast<BareMet*> ( bare_[ names_["Met"]]);

    if ( tree_ -> GetBranchStatus("metP4") == 0 ){ 
        static int counter = 0;
        if(counter< 10 ) { counter ++ ; cout<<"[Looper]::[FillMet]::[WARNING] MET Not Filled "<<endl; }
        return;
    }

    if ( met->p4 ->GetEntries() != 1)
        cout<<"[Looper]::[FillEvent]::[ERROR] MET should have exactly 1 entry instead of "<<met->p4 ->GetEntries() <<endl;

    event_ -> met_ . SetP4 ( *(TLorentzVector*)(*met -> p4) [0]) ;
    event_ -> met_ . ptUp = met-> ptJESUP -> at(0);
    event_ -> met_ . ptDown = met-> ptJESDOWN -> at(0);

}

void Looper::FillEvent(){

    if ( tree_ -> GetTreeNumber() != fNumber)
    {
        NewFile();
    }
    //usleep(100); // DEBUG XROOTD
    FillEventInfo();
    FillJets();
    FillLeptons();
    FillTaus();
    FillMet();
    FillMC();


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
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
