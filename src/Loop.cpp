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
	event_= new Event(); 
	dump_ = new Dumper();
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


int Looper::InitAnalysis() { 
	for(auto a : analysis_ ) { a->SetOutput(output_); a->doInit() ;}  
	return 0;
}

int Looper::PrintInfo(){
	string analysis_string="Analysis";
	string systs_string="Systs";
	string corr_string="Correctors";
	for(auto a : analysis_ ) analysis_string += string(":") +a->name();
	for(auto s : systs_ ) systs_string += string(":") +s->name();
	for(auto c : correctors_) corr_string += string(":")+c->name();

	Log(__FUNCTION__,"INFO","-------------------------------------");
	Log(__FUNCTION__,"INFO",analysis_string);
	Log(__FUNCTION__,"INFO",systs_string);
	Log(__FUNCTION__,"INFO",corr_string);
	Log(__FUNCTION__,"INFO","-------------------------------------");

}

int Looper::InitTree()
{
#ifdef VERBOSE
	if(VERBOSE>0)Log(__FUNCTION__,"DEBUG","Init Tree");
#endif
	loader_->SetTree(tree_);
	loader_->SetEventPtr(event_);

	loader_->InitTree();

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
				//LogErr(__FUNCTION__,"INFO",Form("Getting Entry %lu / %lu in (Cpu) %.4f (Real) %.4f",iEntry,nEntries, sw_.CpuTime(),sw_.RealTime()) );
				static int slow_machine=0;
				// min entries per second
				if (iEntry >30000 and minEntries_>0 and 10000./sw_.RealTime() < minEntries_){
					Log(__FUNCTION__,"WARNING",Form("Machine is too slow e/s=%.3f",10000./sw_.RealTime()));
					++slow_machine;
					if (slow_machine> 3)throw slow();
				}
				// check that real time is within a ord of magn of cpu time
				else if (iEntry >30000 and minEntries_>0 and sw_.RealTime() > 100* sw_.CpuTime()){
					Log(__FUNCTION__,"WARNING","Real time too big wrt cpu time. I/O Problems?");
					++slow_machine;
					if(slow_machine>3)throw slow();
				}
				else
				{
					slow_machine=0;
				}
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
					{
						s->smear(c);
						c->correct(event_);
					}

					//do the analysis
					for(auto a : analysis_)
					{
#ifdef VERBOSE
						if (VERBOSE > 1) Log(__FUNCTION__,"DEBUG", string("Doing Analysis") + a->name());
#endif
						event_->validate(); // validate the objects
						// each analysis step will apply the SF accordingly to the object it is using
						event_ -> GetWeight() -> clearSF() ;
						event_ -> GetWeight() -> clearPU() ; // for target
						if ( a->doAnalyze(event_,s->name()) > 0 ) break; // go on analyzing event, if no analysis returns >0
					}
				}
				// necessary for corrector smearer!
				s->SetSyst(0); // not necessary, but cleaner in this way

			}
		}
	}
	catch( sigint_exception &e)
	{
		Log(__FUNCTION__,"SIGNAL"," Caught SIGINT/SIGTERM: exiting! ");
		Write();
		Close();
		dump_->Close();
		throw e; 
	}
	//call end procedures for the analyis
	for(auto a : analysis_)
		a->doEnd();
	// save output

	dump_->Close();
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
	if (eos==string::npos)
		eos=fname.find("/eos/"); // this make it working for eosuser as well
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
			if (token.find("user") != string::npos) continue;
			if (token == "" ) continue;
			dirs.push_back(token); 
		} 
	}// scope loop

	if ( event_->IsRealData() ) {  
		cout<<"[Looper]::[NewFile]::[INFO] Data file found"<<endl;;
		event_ -> GetWeight() -> LoadMC("data"); 
	}
	// -- Load current MC --
	else {
		// try as labels all the directories in the given order
		//
		bool success = false; // should load a MC

		string savedDir= "" ;
		int iDir= dirs.size()-1;
		while ( savedDir == "" and iDir>=0 )
		{
			label = dirs[iDir];
			savedDir=event_ -> GetWeight() -> LoadMC( label );
			--iDir;
		}

		if (savedDir != "") success = true;
 
		// last change
		if (savedDir =="")
		{
			cout<<"[Looper]::[NewFile]::[WARNING] failed to search MC by LABEL '"<<label<<"' search by dir '"<<dir<<"'"<<endl;
			// search for dir
			label = event_ -> GetWeight() -> LoadMCbyDir(dir);
			savedDir = dir;
			cout<<"[Looper]::[NewFile]::[WARNING] label found '"<<label<<"'"<<endl;
			if (label != "") success=true;
		}


		if ( dir != savedDir or label == "")
			{
			cout<<"[Looper]::[NewFile]::[WARNING] saved dir '"<<savedDir<<"' and current dir '"<< dir <<"' label '"<<label<<"'"<<endl;
			}
		if (not success)
		{
			Log(__FUNCTION__,"ERROR","Unable to find correct MC in MCDB. Normalization uncorrect");	
			throw abortException();
		}

	} // end MC

	loader_->NewFile();

	// Dumper
	dump_ -> NewFile( event_-> GetWeight() -> GetMC() ) ;
	dump_ -> TriggerNames(event_->triggerNames_);

	if (dynamic_cast<LoadNero*>(loader_) != NULL)
		dump_ -> InitTree(dynamic_cast<LoadNero*>(loader_)->GetBare());
	return;
}


void Looper::FillEvent(){

	loader_->FillEventInfo();

	if ( tree_ -> GetTreeNumber() != fNumber)
	{
		NewFile();
	}

	loader_->FillEvent();

	// fead the dumper before clearing the collections
	dump_->Fill();
#ifdef VERBOSE
	if(VERBOSE>1)Log(__FUNCTION__,"DEBUG","Clearing collections");
#endif
	loader_->Clear();
#ifdef VERBOSE
	if(VERBOSE>0) Log(__FUNCTION__,"DONE","");
#endif 
}

int Looper::AddSmear(SmearBase*s)
{ 
	SmearBjets* bj= dynamic_cast<SmearBjets*>(s);
	if (bj!=NULL ) 
	{ 
		SF_CSV * sf = dynamic_cast<SF_CSV*>(event_->GetWeight()->GetSF(bj->sfname()));
		if (sf == NULL) 
		{
			Log(__FUNCTION__,"ERROR","SF"+bj->sfname()+"does not exist or is not SF_CSV");
			throw abort;
		}
		sf->simpleError=false; 
		Log(__FUNCTION__ ,"INFO","Setting SF simpleErrors to false for " + bj->sfname()) ;
	}  
	systs_ .push_back(s) ; 
	return 0; 
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:

