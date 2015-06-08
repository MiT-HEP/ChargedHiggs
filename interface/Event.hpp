#ifndef EVENT_H
#define EVENT_H

#include "TLorentzVector.h"
#include <vector>
#include <map>
using namespace std;

// -----
#include "interface/Object.hpp"
#include "interface/Jet.hpp"
#include "interface/Lepton.hpp"
#include "interface/Tau.hpp"
#include "interface/Met.hpp"

// ----
#include "interface/Weight.hpp"
 
class Looper;
class AnalysisBase;

class Event{
	friend class Looper;
	friend class SmearBase;
	friend class AnalysisBase;
protected:
	vector<Lepton*> leps_;
	vector<Jet*>    jets_;
	vector<Tau*>    taus_;
	Met met_;
	int isRealData_;
	int runNum_;

public:
	Weight weight_;

	inline int runNum(){return runNum_; }

	 inline Jet * GetJet( int iJet ) { return jets_.at(iJet);}
	 inline Tau * GetTau( int iTau ) { return taus_.at(iTau);}
	 inline Lepton * GetLepton( int iLep ) { return leps_.at(iLep);}

	inline float Ht()   { float ht=0 ; for(auto j : jets_ ) if( j->IsJet()  ) ht+= j->Pt() ; return ht;}
	inline int   Njets(){ int   n=0  ; for(auto j : jets_ ) if( j->IsJet()  ) n+=1; return n; }
	inline int   Bjets(){ int   n=0  ; for(auto j : jets_ ) if( j->IsBJet() ) n+=1; return n; }
	inline int   Ntaus(){ int   n=0  ; for(auto t : taus_ ) if( t->IsTau()  )  n+=1; return n; }
	inline int   LeadTau(){ int  n=0 ; for(auto t : taus_ ) { if( t->IsTau() ) return n; ++n;} return -1; }
	inline int   LeadJet(){ int  n=0 ; for(auto j : jets_ ) { if( j->IsJet() ) return n; ++n;} return -1; }
	inline float Mvis() {  TLorentzVector e(0,0,0,0); 
				for(auto l : leps_ )  if ( l->IsLep() ) e += l->GetP4(); 
				for(auto j : jets_ )  if ( j->IsJet() ) e += j->GetP4();
				for(auto t : taus_ )  if ( t->IsTau() ) e += t->GetP4();
				return e.M();
			}
	float Mt() ;
	virtual void ClearEvent();
	virtual inline int IsRealData(){ return isRealData_ ;} 
	virtual void clearSyst(); // call clearSyst in all object it posses

	double weight();
	// update objects that can be invalid (jets)
	virtual void validate();
};

#endif
