#ifndef SMEAR_H
#define SMEAR_H

#include "interface/Event.hpp"

#define SMEAR_OK 0
#define SMEAR_NA 1
#define SMEAR_UNKOWN_FAIL 2

class SmearBase{
public:
	
	SmearBase(){};
	~SmearBase(){};

	// ---
	string name_ = "NONE";
	int syst_;
	const inline string name() {
		if (syst_ == 0 ) return name_;
		else if (syst_ >0 ) return name_ + "_UP" ;
		else  return name_ + "_DOWN" ;
		}
	//const inline string operator(){return name();};
	virtual void inline SetSyst(int s){syst_=s;}
	// ---
	virtual int smear(Event *e){return 0;}; // 0 = success; 1 not applicable to the event
	virtual inline vector<Jet*> &GetJets(Event *e){return e->jets_;}
	virtual inline Met &GetMet(Event *e){return e->met_;}
};

class SmearLepSF : virtual public SmearBase
{
public:
	SmearLepSF() :SmearBase(){ name_= "LEPSF";}
	virtual int smear(Event *e);
};

class SmearJes : virtual public SmearBase
{
public:
	SmearJes() : SmearBase(){ name_ = "JES";}
	virtual int smear(Event*e);	
};

class SmearJer:virtual public SmearBase
{
public:
	SmearJer() : SmearBase(){ name_ = "JER";}
	virtual int smear(Event*e);	
	
};

#endif
