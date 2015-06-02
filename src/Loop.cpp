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

#define VERBOSE


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
	return 0;
}

void Looper::Loop()
{
	// TODO
	unsigned long nEntries = tree_->GetEntries();

	for(unsigned long iEntry = 0 ;iEntry< nEntries ;++iEntry)
	{
	// load tree
	tree_ -> GetEntry(iEntry);		
	//move content into the event
	FillEvent();	
	// for each smearing
		//reset 	
		//smear
		//do the analysis
			for(auto a : analysis_)
				a->analyze();
		// save the output
	}
	return;	
}

void Looper::ClearEvent(){

	event_ -> ClearEvent();

}

void Looper::FillEvent(){

	BareEvent *e = dynamic_cast<BareEvent*> ( bare_ [names_["Event"] ] ) ; assert(e!=NULL);
#ifdef VERBOSE
	cout <<"[Looper]::[FillEvent]::[INFO] Processing "<<e->runNum<<":"<<e->lumiNum<<":"<<e->eventNum<<endl;
#endif
	event_ -> isRealData_ = e->isRealData;
	//fill Jets
	BareJets *bj = dynamic_cast<BareJets*> ( bare_ [ names_[ "Jets" ] ] ); assert (bj !=NULL);
	for (int iJet=0;iJet< bj -> p4 ->GetEntries() ; ++iJet)
		{
		Jet *j =new Jet();
		j->SetP4( *(TLorentzVector*) ((*bj->p4)[iJet]) );
		j->unc = bj -> unc -> at(iJet);
		j->bdiscr = bj -> bDiscr -> at(iJet);
		event_ -> jets_ . push_back(j);
		}
	// Fill Leptons
	BareLeptons *bl = dynamic_cast<BareLeptons*> ( bare_[ names_["Leptons"] ]); assert(bl != NULL ) ;
	for (int iL = 0;iL<bl->p4->GetEntries() ;++iL)
		{
		Lepton *l = new Lepton();
		l->SetP4( *(TLorentzVector*) ((*bl->p4)[iL]) );
		l-> iso = (*bl->iso) [iL];
		l-> charge = ((*bl->pdgId)[iL] >0) ?  -1: 1; 
		l-> type = abs((*bl->pdgId)[iL]);
		event_ -> leps_ . push_back(l);
		}
	//Fill Tau
	BareTaus *bt = dynamic_cast<BareTaus*> ( bare_[ names_["Taus"] ]); assert (bt != NULL ) ;
	for (int iL = 0;iL<bt->p4->GetEntries() ;++iL)
		{
		Tau *t = new Tau();
		t->SetP4( *(TLorentzVector*) ((*bt->p4)[iL]) );
		t-> iso = (*bt->iso) [iL];
		t-> charge = bt -> Q -> at(iL);
		t-> type = 15;
		event_ -> taus_ . push_back(t);
		}
	// FillMEt
	BareMet * met = dynamic_cast<BareMet*> ( bare_[ names_["Met"]]);
	event_ -> met_ . SetP4 ( *(TLorentzVector*)(*met -> p4) [0]) ;
	event_ -> met_ . ptUp = met-> ptJESUP -> at(0);
	event_ -> met_ . ptDown = met-> ptJESDOWN -> at(0);
#ifdef VERBOSE
	cout <<"[Looper]::[FillEvent]::[DONE]"<<endl;
#endif 
}
