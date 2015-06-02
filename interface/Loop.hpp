#ifndef LOOP_H
#define LOOP_H

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"


// Charged Higgs
#include "interface/AnalysisBase.hpp"
#include "interface/Event.hpp"

// Bare Structures
#include "NeroProducer/Core/interface/BareCollection.hpp"

class Looper{

private:
	TChain *tree_; 
	vector<AnalysisBase*> analysis_;
	vector<BareCollection*> bare_;
	map<string,int> names_;
	Event * event_;

public:
	// -- constructor
	Looper(){}
	~Looper(){ClearEvent();}
	// ---
	inline int AddToChain( string name ){ return tree_ -> Add( name.c_str() ) ; }
	int InitTree () ;
	void Loop();
	void FillEvent();
	void ClearEvent();
	
};

#endif
