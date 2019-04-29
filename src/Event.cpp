#include "interface/Event.hpp"
#include "interface/GeneralFunctions.hpp"
#include "TMath.h"
#include "interface/Logger.hpp"
#include <iostream>

//#define VERBOSE 2
//
Event::Event()
{
    weight_ = new Weight();
}

Event::~Event(){
    ClearEvent();
    delete weight_;
}

void Event::ClearEvent(){

    for (auto o :  jets_ ) ChargedHiggs::Delete(o);
    for (auto o :  tracks_ ) ChargedHiggs::Delete(o);
    for (auto o :  fat_ ) ChargedHiggs::Delete(o);
    for (auto o :  leps_ ) ChargedHiggs::Delete(o);
    for (auto o :  taus_ ) ChargedHiggs::Delete(o);
    for (auto o :  genparticles_ ) ChargedHiggs::Delete(o);
    for (auto o :  genjets_ ) ChargedHiggs::Delete(o);

    jets_ . clear();
    fat_ . clear();
    leps_ . clear();
    taus_ . clear();
    phos_ . clear();
    genparticles_ . clear();
    genjets_ . clear();
    tracks_ . clear();

    weight_ -> clearSF( );

    //Log(__FUNCTION__,"DEBUG","End ClearEvent");
    //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG","End Clear EVENT ----------" );

}

void Event::clearSyst(){
    for ( auto o: jets_) o->clearSyst();
    for ( auto o: tracks_) o->clearSyst();
    for ( auto o: fat_) o->clearSyst();
    for ( auto o: taus_) o->clearSyst();
    for ( auto o: leps_) o->clearSyst();
    for ( auto o: genparticles_) o->clearSyst();
    for ( auto o: genjets_) o->clearSyst();
    met_ . clearSyst();
    // clear SF syst
    weight_ -> clearSF();
    weight_ -> resetSystSF();
    weight_ -> clearSystPU();
    weight_ -> SetSyst( MC::none ) ;
}

float Event::Mt(MtType type)  {  // 0 tau, 1 muon, 2 electron, 3 lepton
    float pt_m = met_ . Pt(); 
    float phi_m= met_. Phi(); 
    switch(type){
    case MtTau:
        {
        if ( Ntaus() <=0 ) return -1; 
        float pt_t  =  LeadTau() -> Pt();
        float phi_t =  LeadTau() -> Phi();
        return ChargedHiggs::mt(pt_t,pt_m,phi_t,phi_m); 
        }
    case MtMuon:
        {
        if (GetMuon(0) == NULL ) return -1;
        float pt_mu = GetMuon(0)-> Pt();
        float phi_mu = GetMuon(0)-> Phi();
        return ChargedHiggs::mt(pt_mu,pt_m,phi_mu,phi_m); 
        }
    case MtLepton:
        {
        if (GetLepton(0) == NULL ) return -1;
        float pt_lep = GetLepton(0)-> Pt();
        float phi_lep = GetLepton(0)-> Phi();
        return ChargedHiggs::mt(pt_lep,pt_m,phi_lep,phi_m);
        }
    case MtTauInv:
        {
            Tau *tInv = GetTauInvIso(0); 
            if (tInv == NULL) return -1;
            float pt_t  =  tInv -> Pt();
            float phi_t =  tInv -> Phi();
            return ChargedHiggs::mt(pt_t,pt_m,phi_t,phi_m); 
        }
    } 
    return -3;
} 

float Event::MtDecoQ(MtType type, float mt0){
    switch(type){
        case MtTau:
            return MtDecoQ(GetTau(0), mt0);
        case MtMuon:
            return MtDecoQ(GetMuon(0), mt0);
        case MtTauInv:
            return MtDecoQ(GetTauInvIso(0), mt0);
    } // end switch
}

float Event::MtDecoCosPhi(MtType type){
    switch(type){
        case MtTau:
            return MtDecoCosPhi(GetTau(0));
        case MtMuon:
            return MtDecoCosPhi(GetMuon(0));
        case MtTauInv:
            return MtDecoCosPhi(GetTauInvIso(0));
    } // end switch
}

float Event::RbbMin(int iMax,Tau *t) {

    // notice the Pi-...
    if (t == NULL) return -1;
    bool direct= t->IsTau(); // otherwise I suppose inviso
    float dphietmisstau = TMath::Pi() - fabs(GetMet().DeltaPhi( t ) );

    float rbbmin = -1;
    if (direct)
    {
        for(int i=0 ; i< iMax; ++i)
        {
            if( GetJet(i) == NULL ) break;
            float dphietmissjet= fabs( GetMet().DeltaPhi( GetJet(i) ) ) ;
            float myrbb = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

            if (rbbmin<0 or myrbb<rbbmin) rbbmin = myrbb;
        }
    }else{
        for(int i=0 ; i< iMax; ++i)
        {
            if( GetJetInvIso(i) == NULL ) break;
            float dphietmissjet= fabs( GetMet().DeltaPhi( GetJetInvIso(i) ) ) ;
            float myrbb = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

            if (rbbmin<0 or myrbb<rbbmin) rbbmin = myrbb;
        }
    }

    return rbbmin;
}
float Event::RCollMin(int iMax,Tau *t) {
    // notice the Pi-...
    if (t == NULL) return -1;
    bool direct= t->IsTau(); // otherwise I suppose inviso
    float dphietmisstau = fabs(GetMet().DeltaPhi( t ) );
    float rcollmin = -1;
    if (direct)
    {
        for(int i=0 ; i< iMax; ++i)
        {
            if( GetJet(i) == NULL ) break;
            float dphietmissjet= TMath::Pi() - fabs( GetMet().DeltaPhi( GetJet(i) ) ) ;
            float myrcoll = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

            if (rcollmin<0 or myrcoll<rcollmin) rcollmin = myrcoll;
        }
    }else{
        for(int i=0 ; i< iMax; ++i)
        {
            if( GetJetInvIso(i) == NULL ) break;
            float dphietmissjet= TMath::Pi() - fabs( GetMet().DeltaPhi( GetJetInvIso(i) ) ) ;
            float myrcoll = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

            if (rcollmin<0 or myrcoll<rcollmin) rcollmin = myrcoll;
        }
    }

    return rcollmin;
}

float Event::RsrMax(int iMax, Tau *t) {
    if (t == NULL) return -1;
    bool direct= t->IsTau(); // otherwise I suppose inviso
    float dphietmisstau = TMath::Pi() - fabs(GetMet().DeltaPhi( t ) );
    float rsrmax = -1;
    if (direct){
        for(int i=0 ; i< iMax; ++i)
        {
            if( GetJet(i) == NULL ) break;
            float dphietmissjet= TMath::Pi() - fabs( GetMet().DeltaPhi( GetJet(i) ) ) ;
            float myrsr = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

            // CHECKME, is this correct ? min ? 
            if (rsrmax<0 or myrsr<rsrmax) rsrmax = myrsr;
        }
    }else{
        for(int i=0 ; i< iMax; ++i)
        {
            if( GetJetInvIso(i) == NULL ) break;
            float dphietmissjet= TMath::Pi() - fabs( GetMet().DeltaPhi( GetJetInvIso(i) ) ) ;
            float myrsr = sqrt(dphietmisstau*dphietmisstau + dphietmissjet*dphietmissjet) ;

            // CHECKME, is this correct ? min ? 
            if (rsrmax<0 or myrsr<rsrmax) rsrmax = myrsr;
        }
    }

    return rsrmax;
}

double Event::weight(bool safe){
    if (isRealData_ and safe) return 1;
    return weight_ -> weight();
}

void Event::validate(){
    //reject all the jets that are identified as : lepton or tau
    //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG","---------- VALIDATE EVENT ----------" );
    for ( auto j : jets_ )
    {
        //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("Compute Validity of jet pt=%f, eta=%f vs Tau",j->Pt(),j->Eta()) );

        j-> resetValidity();
        for(auto l : leps_)
            if(l->IsLep() )j-> computeValidity(l);

        for(auto t: taus_)
        {
            if(t->IsTau() ) { 
                //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("--- aganinst Tau of pt=%f, eta=%f DR=",t->Pt(),t->Eta(), t->DeltaR(*j)) );
                j-> computeValidity(t);
                //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("--- Jet for the moment is=%d ",j->isValid) );
            }
            if(t->IsTauInvIso() )j-> computeValidity(t,0.4,true);
        }

        for(auto p: phos_)
        {
            if(p->IsPho() )j-> computeValidity(p);
        }

    }
    /// only against leptons, 
    // against jet we will need to see in the variables
    for ( auto j : tracks_ )
    {
        j-> resetValidity();
        for(auto l : leps_)
            if(l->IsLep() )j-> computeValidity(l);
    }
    return ;
}

// Get Object functions
Jet * Event::GetJet( int iJet )
{
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsJet()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i));
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

Jet * Event::GetJetInvIso( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsJetInvIso()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

// Get Object functions
Jet * Event::GetCentralJet( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsCentralJet()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

Jet * Event::GetBjetInvIso( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsBJetInvIso()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

Jet * Event::GetBjet( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( jets_[i]->IsBJet()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

Jet * Event::GetLjet( int iJet ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<jets_.size() ;++i)
    {
        if ( not jets_[i]->IsBJet()) valid.push_back(pair<float,int>(jets_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return jets_[ valid[iJet].second];
}

FatJet * Event::GetFatJet( int iJet )
{
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<fat_.size() ;++i)
    {
        if ( fat_[i]->IsFatJet()) valid.push_back(pair<float,int>(fat_[i]->Pt(),i));
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return fat_[ valid[iJet].second];
}


FatJet * Event::GetWJet( int iJet )
{
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<fat_.size() ;++i)
    {
        if ( fat_[i]->IsWJet()) valid.push_back(pair<float,int>(fat_[i]->Pt(),i));
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iJet  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return fat_[ valid[iJet].second];
}


Tau * Event::GetTau( int iTau ) 
{ // { return taus_.at(iTau);} // old
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<taus_.size() ;++i)
    {
        if ( taus_[i]->IsTau()) valid.push_back(pair<float,int>(taus_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iTau  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return taus_[ valid[iTau].second];
}

Lepton * Event::GetLepton( int iLepton ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<leps_.size() ;++i)
    {
        if ( leps_[i]->IsLep()) valid.push_back(pair<float,int>(leps_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iLepton  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return leps_[ valid[iLepton].second];
}

Lepton * Event::GetElectron( int iEle ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<leps_.size() ;++i)
    {
        if ( leps_[i]->IsLep() and leps_[i]->IsElectron() ) 
            valid.push_back(pair<float,int>(leps_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iEle  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return leps_[ valid[iEle].second];
}

Lepton * Event::GetMuon( int iMu ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<leps_.size() ;++i)
    {
        if ( leps_[i]->IsLep() and leps_[i]->IsMuon() ) 
            valid.push_back(pair<float,int>(leps_[i]->Pt(),i)); 

    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iMu  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return leps_[ valid[iMu].second];
}

Tau * Event::GetTauInvIso( int iTau ) 
{ 
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<taus_.size() ;++i)
    {
        if ( taus_[i]->IsTauInvIso()) valid.push_back(pair<float,int>(taus_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iTau  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return taus_[ valid[iTau].second];
}

//#define VERBOSE 1
bool Event::IsTriggered( string name ,Trigger *trigger, bool isNone)
{
    // TODO: make event inheriths from trigger, and remove this switch
    #ifdef VERBOSE
        if(VERBOSE>0)Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("(%d,%d,%u) Trigger name=%s",runNum(),lumiNum(),eventNum(),name.c_str()) );
    #endif
    
    static string lastName = "";
    static int lastPos = -1;

    if (name == lastName and lastPos >=0 )
    {
        if (trigger == NULL)
        {
#ifdef VERBOSE
            if(VERBOSE>0)Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("Returning trigger in pos %d and name=%s",lastPos,lastName.c_str()));
#endif
            return triggerFired_[ lastPos ] ;
        }
        else 
        {
            if (isNone)  return trigger -> IsTriggeredNone ( lastPos ) ;
            else return trigger -> IsTriggered( lastPos ) ;
        }
    }
    
    lastPos = -1;
    for (size_t i=0; i <  triggerNames_.size() ;++i)
    {
        if (name == triggerNames_[i] ) { lastPos=i; break;} 
    }
    lastName = name;
    if (lastPos >=0 ) {
        #ifdef VERBOSE
        if (VERBOSE >1) cout <<"[Event]::[IsTriggered]::[DEBUG] grace exit"<<endl;
        #endif
        if (trigger == NULL)
        {
#ifdef VERBOSE
            if(VERBOSE>0)Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("Returning trigger in pos %d and name=%s",lastPos,lastName.c_str()));
#endif
            return triggerFired_[ lastPos ] ; 
        }
        else 
        {
            if (isNone) return trigger->IsTriggeredNone (lastPos ) ;
            else return trigger -> IsTriggered( lastPos) ;
        }
    }
   
    // Log only if it's not empty  -- can be used to reset stuff
    if (name != "") cout<<"[Event]::[IsTriggered]::[WARNING] Trigger menu not found: '"<<name<<"'"<<endl;
    return false;
}
//#define VERBOSE 0
GenParticle * Event::GetGenParticle( int iGenPar ) 
{  
    //FIXME: what is the purpose of this function ? 
    return (iGenPar >= 0 && iGenPar < genparticles_.size() ? genparticles_.at(iGenPar) : NULL);
}

GenParticle * Event::GetGenStable( int iGenPar ,int pdgid,float aeta) 
{  
    // status 1, electrons
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<genparticles_.size() ;++i)
    {
        GenParticle *gp = genparticles_[i];
        if (not gp->IsPromptFinalState()  ) continue;
        if ( abs(gp->GetPdgId()) != pdgid)  continue;
        if ( fabs(gp->Eta() ) > aeta) continue;
        valid.push_back(pair<float,int>(gp->Pt(),i)); 
    }
    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iGenPar  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return genparticles_[ valid[iGenPar].second];
}

Photon * Event::GetPhoton( int iPho ) 
{     
    vector<pair<float,int> > valid; // pt, idx
    for(int i = 0 ; i<phos_.size() ;++i)
    {
        if ( phos_[i]->IsPho()) 
            valid.push_back(pair<float,int>(phos_[i]->Pt(),i)); 
    }

    if (valid.size() == 0 ) return NULL;
    if (valid.size() <= iPho  ) return NULL;

    sort(valid.begin(),valid.end(),[](const pair<float,int> &a,const pair<float,int> &b) { if (a.first> b.first) return true; if (a.first<b.first) return false; return a.second<b.second;} ) ;

    return phos_[ valid[iPho].second];
}

void Event::ApplyTopReweight(){
    if( GetWeight() -> GetMC() . find("TT") == string::npos)
    { // not ttbar sample
        return;
    }

    if (not ExistSF("topreweight") )
    {
        Logger::getInstance().LogN("Event",__FUNCTION__,"WARNING","TOP SF DOES NOT EXIST",5);
        return;
    }

    double pt1=-1,pt2=-1;
    // look for gen particles
    for(auto const &g : genparticles_)
    {
        //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("considering particle with pt=%f, and pdg=%i",g->Pt(),g->GetPdgId()));
        if (g->GetPdgId() == 6 ) pt1 = g->Pt();
        if (g->GetPdgId() == -6) pt2 = g->Pt();

        if ( pt1 > 400 ) pt1 = 400;
        if ( pt2 > 400 ) pt2 = 400;

        if(pt1 >0 and pt2>0) break;
    }

    if (pt1<0 or pt2<0) 
    {
        Logger::getInstance().LogN("Event",__FUNCTION__,"WARNING",Form("I didn't find two tops for reweighting"),5);
        return;
    }
    //else{
    //    Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("I found two tops for reweighting, pt1=%f, pt2=%f",pt1,pt2));
    //}
    //

    SetPtEtaSF("topreweight",pt1,pt2);
    ApplySF("topreweight");

    if (GetWeight()->GetSF("topreweight")->get() < .001) {
        Logger::getInstance().LogN("Event",__FUNCTION__,"WARNING","Top Reweight very small:",20,Form("sf=%lf, pt1=%f pt2=%f",GetWeight()->GetSF("topreweight")->get(),pt1,pt2));
    }
}

double Event::ApplyL1PreFire(int year){
    //SF_TH2F *sf=(SF_TH2F*)GetWeight()->GetSF("prefire");
    if (IsRealData() ) return 1.;
    
    string name = "prefire";
    if (year==2016) name="prefire2016";
    else if (year==2017) name="prefire2017";
    else if (year==2018) return 1.0;

    SF* dummy=GetWeight()->GetSF("dummy");
    float eventsf = 1.;
    bool changed=false;

    for(int i=0;;++i)
    {
        Jet*j=GetBareJet(i); // or bare?-> Probably PU Id clean the EG ones
        if(j==NULL) break;
        // pt w/o systematics
        float eta= j->GetP4Dirty().Eta();
        float pt = j->GetP4Dirty().Pt();
        float ptem= j->GetP4Dirty().Pt() * (j->GetCEMF() + j->GetNEMF());
    
        if (ptem<40.) continue; // only < 40 has prefire information:
        SetPtEtaSF(name,ptem,fabs(eta));
        eventsf *= (1.-GetWeight()->GetSF(name)->get() );
        changed=true;
    }
    //if (changed)eventsf= 1.-eventsf;
    if (changed)eventsf= eventsf;  // I want to apply the efficiency that all not prefire

    dummy->sf=eventsf;
    dummy->syst=0;
    dummy->err=0;

    ApplySF("dummy");
    return eventsf;
}

//#define VERBOSE 2
double Event::ApplyBTagSF(int wp,int year)
{
    double sf=1.;
    string name="bdeep";
    if (year==2017) name="bdeep2017";
    else if (year==2018) name="bdeep2018";
    else if (year==2016) name ="bdeep2016";
    SetWPSF(name,wp); // loose, for sf


#ifdef VERBOSE
    if (VERBOSE>1)Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG","Starting BTAG SF"); 
#endif
    //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("-> Considering NCJ=%d",NcentralJets() ));
    for (int i=0;i<NcentralJets() ;++i)
    {
        Jet *j=GetCentralJet(i);
        SetJetFlavorSF(name,j->hadFlavor());

        if (wp == 3) // reshaping
            SetDiscrSF(name,j->GetDeepB());

        if (not j->IsBJet() and wp !=3) continue;

        SetPtEtaSF(name,j->Pt(), j->Eta() );

#ifdef VERBOSE
        if(VERBOSE>1)Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("Applying bdeep sf '%s' for jet: %f,%f,%d = %f",name.c_str(),j->Pt(),j->Eta(),j->Flavor(),GetWeight()->GetSF(name)->get()));
#endif
        
        //if (wp==3)
        //{
        //    float newvalue= GetWeight()->GetSF("bdeep")->get();
        //    j->SetDeepBSF(newvalue);
        //    Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("Old value was %f new value is %f",j->GetBareDeepB(),newvalue));
        //}
        //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("SF For jet %d with pt=%f eta=%f hf=%d is %lf",i,j->Pt(),j->Eta(),j->hadFlavor(),GetWeight()->GetSF("bdeep")->get()));

        if (GetWeight()->GetSF(name)->get() <.2 or GetWeight()->GetSF(name)->get()>2.) continue; // not believable

        ApplySF(name);
        sf *= GetWeight()->GetSF(name)->get();
    }
#ifdef VERBOSE
   if(VERBOSE>1)Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG","End BTAG SF"); 
#endif
   return sf;
}

void Event::ApplyTauSF(Tau*t,bool prongs,const string& extra)
{
    if (IsRealData()) return;
    string sfname="tauLeg";//

    if (prongs)
    {
        if (t->GetNProng() ==1 ) sfname="tauLeg1p";
        else sfname="tauLeg3p";
    }

    sfname +=extra;
    if( not ExistSF(sfname) ) Logger::getInstance().Log("Event",__FUNCTION__,"WARING" ,"No Tau Trigger SF"+sfname);  

    if (t->Pt() <10 ) return; // no sf for pt<10 taus

    SetPtEtaSF(sfname,t->Pt(),t->Eta());
    ApplySF(sfname);

}

#include "interface/MetNoJec.hpp"
void Event::ApplyMetNoJEC(double eta0,double eta1)
{
    // I want the possibility to apply it at some point.
    if (not metnojec_) metnojec_.reset(new MetNoJEC());

    //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("Met Was %f",GetMet().Pt()));

    metnojec_->correct(this);

    //Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("Met Now is %f",GetMet().Pt()));

}

bool Event::GenParticleDecayedFrom( int iGenPar, int apdgid ,int& idx){
    //if( apdgid==25) Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("* Called GenParticleDecaydFrom %d",iGenPar) );//DEBUG
    GenParticle *g = GetGenParticle(iGenPar);
    if (g ==NULL)return false;
    //if( apdgid==25) Logger::getInstance().Log("Event",__FUNCTION__,"DEBUG",Form("*  ---> Current pdgId is %d",g->GetPdgId()) );//DEBUG
    if (abs(g->GetPdgId()) == apdgid) { idx=iGenPar; return true;}
    int moidx = g->GetParentIdx();
    if (moidx == iGenPar) return false;
    return GenParticleDecayedFrom( moidx, apdgid,idx);
}


pair<int,float> Event::softVariables(Object *j1, Object *j2, float cut,float dR)
{
    int NJets=0;
    float HT=0.;
    float maxeta = std::max(j1->Eta(),j2->Eta());
    float mineta = std::min(j1->Eta(),j2->Eta());
    for(auto j : tracks_ )
    {
        if (j->Eta()> maxeta ) continue;
        if (j->Eta()< mineta ) continue;
        if (j->Pt() < cut ) continue;
        if (j->DeltaR(j1) < dR) continue;
        if (j->DeltaR(j2) < dR) continue;
        bool islep=false;
        for( int il = 0; ;++il)
        {
            Lepton *l= GetLepton(il);
            if (l==NULL or islep) break;
            if (j->DeltaR(l) < dR) islep=true;
        }
        if (islep) continue;
        HT+=j->Pt();
        NJets+=1;
    }
    return pair<int,float>(NJets,HT);
}



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
