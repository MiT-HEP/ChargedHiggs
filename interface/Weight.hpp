#ifndef WEIGHT_H
#define WEIGHT_H

#include "interface/SF.hpp"
#include "interface/MC.hpp"

class SmearBase;
class Event;

class Weight{
	friend class Event;
	friend class SmearBase;
	
	string mcName_;
	double mcXsec_;
	double nEvents_;
	double lumi_;
	double sf_;
protected:

	map<string, MC*> mc_db;
	map<string, SF*> sf_db;

public:
	Weight(){ clear(); }
	~Weight(){}

	double mcWeight_; // this is set in FillEvent

	void clear(){ mcName_= "";
		mcXsec_ = 1.0; 
		mcWeight_=1.0; 
		nEvents_=1000. ; 
		lumi_=1.0; 
		sf_ = 1.0;}
	//
	string GetMC(){ return mcName_; }
	// ---
	void SetLumi(double l) {lumi_= l;}
	void AddMC( string label, string dir, double xsec, double nevents);
	string LoadMC( string label) ;	 // return "" if failed otherwise dir
	string LoadMCbyDir( string dir ) ;	 // return "" if failed otherwise label

	void AddSF( string label, double sf, double err);
	void AddPtEtaSF( string label, double pt1,double pt2 ,double eta1 ,double eta2,double sf, double err);

	void clearSF( ){ sf_ =1.0;}
	void SetSystSF(string label, int s ) { sf_db[label] -> syst = s;}
	void resetSystSF( ) ;
	void SetPtEtaSF(string label,double pt , double eta);
	void ApplySF(string label){ sf_ *= sf_db[label] -> get(); }

	
	// ---
	double weight(){ return mcWeight_* mcXsec_ * lumi_ * sf_ / nEvents_; }
};

#endif
