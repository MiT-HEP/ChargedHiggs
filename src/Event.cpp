#include "interface/Event.hpp"
#include "interface/GeneralFunctions.hpp"
#include "TMath.h"
#include <iostream>


const bool operator<( const SF_PtEta::range&r1 , const SF_PtEta::range &r2)
{
	// on pt1
	if (r1.pt1 < r2.pt1) return true;
	if (r1.pt1 > r2.pt1) return false;
	// -- pt2
	if (r1.pt2 < r2.pt2) return true;
	if (r1.pt2 > r2.pt2) return false;
	// -- eta1
	if (r1.eta1 < r2.eta1) return true;
	if (r1.eta1 > r2.eta1) return false;
	// -- eta2
	if (r1.eta2 < r2.eta2) return true;
	if (r1.eta2 > r2.eta2) return false;
	// they are equal
	return false;
}

void Event::ClearEvent(){

	for (auto o :  jets_ ) delete o;
	for (auto o :  leps_ ) delete o;
	for (auto o :  taus_ ) delete o;

	jets_ . clear();
	leps_ . clear();
	taus_ . clear();

	weight_ . clearSF( );

}

void Event::clearSyst(){
	for ( auto o: jets_) o->clearSyst();
	for ( auto o: taus_) o->clearSyst();
	for ( auto o: leps_) o->clearSyst();
	met_ . clearSyst();
	// clear SF syst
	weight_ . clearSF();
	weight_ . resetSystSF();
}


float Event::Mt() { 
	if ( Ntaus() <=0 ) return -1; 
	float pt_t  = taus_[ LeadTau() ] -> Pt();
	float phi_t = taus_[ LeadTau() ] -> Phi();
	float pt_m = met_ . Pt(); 
	float phi_m= met_. Phi(); 
	return TMath::Sqrt( 2* pt_t * pt_m * TMath::Cos(ChargedHiggs::deltaPhi(phi_t,phi_m) ) );
       	} 

double Event::weight(){
	if (isRealData_ ) return 1;
	return weight_ . weight();
	}

void Event::validate(){
	//reject all the jets that are identified as : lepton or tau
    	for ( auto j : jets_ )
	{
	j-> resetValidity();
	for(auto l : leps_)
		if(l->IsLep() )j-> computeValidity(l);
	for(auto t: taus_)
		if(t->IsTau() )j-> computeValidity(t);
	}
    return ;
}

void Weight::AddMC( string label, string dir, double xsec, double nevents)
{
	mc_db[label] = new MC();
	mc_db[label]->dir =dir;
	mc_db[label]->xsec = xsec;
	mc_db[label]->nevents = nevents;
}

void Weight::AddSF( string label, double sf, double err){
	sf_db[label] = new SF();
	sf_db[label]->sf = sf;
	sf_db[label]->err = err;
	sf_db[label]->syst = 0;
}

void Weight::AddPtEtaSF( string label,double pt1, double pt2 , double eta1, double eta2, double sf, double err){
	if ( sf_db.find(label) == sf_db.end() )
		{
		sf_db[label] = new SF_PtEta();
		}
	SF_PtEta *p =  dynamic_cast<SF_PtEta*> ( sf_db[label] );
	if (p == NULL)
		cout <<"[Weight]::[AddPtEtaSF]::[ERROR] SF "<<label<<" is not Pt Eta dependent"<<endl;

	p->add(pt1, pt2, eta1,eta2,sf,err);
	return;
}

void Weight::resetSystSF(){
	for (auto o : sf_db)
		o.second->syst = 0;
}

void Weight::SetPtEtaSF(string label,double pt, double eta)
{
	 SF_PtEta *p =  dynamic_cast<SF_PtEta*> ( sf_db[label] );
         if (p == NULL)
                 cout <<"[Weight]::[SetPtEtaSF]::[ERROR] SF "<<label<<" is not Pt Eta dependent"<<endl;
	 p->set(pt,eta);
	 return;
}


string Weight::LoadMC( string label) 
{ 
	if( mc_db.find(label) == mc_db.end() )
		return "";
	mcName_ = label; 
	mcXsec_= mc_db[label]->xsec; 
	nEvents_ = mc_db[label] -> nevents; 
	return mc_db[label]->dir;
}
string Weight::LoadMCbyDir( string dir )	 // return "" if failed otherwise label
{
	string label = "";
	for( auto p : mc_db )
		{
		if(p.second -> dir == dir) 
			{
			label = p.first;
			break;
			}
		}
	LoadMC(label);
	return label;
}

void SF_PtEta::add(double pt1, double pt2,double eta1, double eta2, double sf, double err)
{
	range r;
	r.pt1 = pt1;
	r.pt2 = pt2;
	r.eta1 = eta1;
	r.eta2 = eta2;

	store[r].first = sf;
	store[r].second = err;
}

void SF_PtEta::set( double pt, double eta)
{
	int change = 0;
	for(auto s : store)
	{
	if ( s.first.pt1  < pt ) continue;
	if ( s.first.pt2  >= pt ) continue;
	if ( s.first.eta1  < eta ) continue;
	if ( s.first.eta2  >= eta ) continue;
	sf = s.second.first;	
	err = s.second.second;
	change = 1;
	break;
	}
	if (not change)
		{
		cout<<"[SF_PtEta]::[set]::[ERROR] no PT ETA RANGE for SF '"<<label<<"' in pt="<<pt<<" eta="<<eta<<endl;
		sf = 1.0;
		err = 0.0;
		}
}


