#ifndef EVENT_H
#define EVENT_H

#include "TLorentzVector.h"
#include <vector>
using namespace std;

// -----
class Object
{
protected:
	// this class provide the really basic information used at analysis level
	TLorentzVector p4;
public:
	//---
	Object(){};
	~Object(){};
	//---
	virtual inline void  clearSyst(){}; // reset smearing
	virtual inline float Pt(){return p4.Pt() ;}
	virtual inline float Eta(){ return p4.Eta(); } 
	virtual inline float Phi(){ return p4.Phi(); }
	virtual inline int   IsObject(){return 0;}
	virtual inline float DeltaR(Object &o){ return p4.DeltaR(o.GetP4()); }
	virtual inline TLorentzVector & GetP4(){ return p4;}
	virtual inline void SetP4(TLorentzVector &x){p4 = x;}
};


// ---
class Jet : virtual public Object
{
	float ptcut_; // ** pt cut on the accepted jets
	float bcut_; /// ** bcut on the bJets discr
public:
	Jet() : Object() {syst = 0;unc =0; bdiscr = 0; bsyst = 0; bunc=0; bcut_=0.5;ptcut_=30.;isValid=1;}

	int isValid; // rejected by DR

	float unc; // TOFILL
	int syst ;

	float bdiscr; // TOFILL
	float bunc; // TOFILL
	int bsyst ;
	// ---
	inline float Pt(){ if (syst ==0) return p4.Pt(); return p4.Pt() *(1.0  + unc*syst );}
	virtual inline void  clearSyst(){syst = 0;bsyst=0; isValid=1;}; // reset smearing
	// ---
	virtual inline int   IsObject(){return IsJet();}
	inline int IsJet() { if (not isValid) return 0 ; 
			     if( Pt() < ptcut_ ) return 0; 
			     return 1;
			   }
	inline int IsBJet(){ if( bdiscr > bcut_ + bsyst*bunc and IsJet() )   return 1; return 0;}

	inline void computeValidity( vector<Object*> &a, float dR = 0.4)
		{
			for( Object* o :  a )
				if ( DeltaR (*o) < dR )  isValid = 0;
		}
};

// ----
class Lepton : virtual public Object
{
protected:
	float isocut_;
public:
	Lepton() : Object() { iso =-1; charge = 0 ;isocut_ = 10;type=0;}

	float iso; // isolation 
	int charge; // charge +1 -1
	int type;// abspdgid 11 o 13 
	
	virtual inline int IsLep(){ 
		if (iso> isocut_) return 0;
	       	return 1;
		}
	virtual inline int   IsObject(){return IsLep();}
};

class Tau: virtual public Object,
	virtual public Lepton
{
public:
	virtual inline int IsTau(){ return 0; }
	virtual inline int IsObject(){ return IsTau(); }
};

class Met: virtual public Object
{
public:
	Met(){syst = 0 ;}
	float ptUp; // TOFILL
	float ptDown; // TOFILL
	int syst ;

	virtual inline int IsMet(){ return 1; }
	virtual inline int IsObject(){ return IsMet(); }
	virtual inline void  clearSyst(){syst = 0;}; // reset smearing
	virtual inline float Pt(){ if ( syst == 0 ) return p4.Pt();
		else if (syst >0 ) return ptUp * syst ;
		else return ptDown * syst;
		}
};

// FWD Declaration
class Looper;
class SmearBase;

class Event{
	friend class Looper;
	friend class SmearBase;
protected:
	vector<Lepton*> leps_;
	vector<Jet*>    jets_;
	vector<Tau*>    taus_;
	Met met_;
	int isRealData_;

public:
	inline float Ht()   { float ht=0 ; for(auto j : jets_ ) if( j->IsJet()  ) ht+= j->Pt() ; return ht;}
	inline int   Njets(){ int   n=0  ; for(auto j : jets_ ) if( j->IsJet()  ) n+=1; return n; }
	inline int   Bjets(){ int   n=0  ; for(auto j : jets_ ) if( j->IsBJet() ) n+=1; return n; }
	inline float Mvis() {  TLorentzVector e(0,0,0,0); 
				for(auto l : leps_ )  if ( l->IsLep() ) e += l->GetP4(); 
				for(auto j : jets_ )  if ( j->IsJet() ) e += j->GetP4();
				for(auto t : taus_ )  if ( t->IsTau() ) e += t->GetP4();
				return e.M();
			}
	virtual void ClearEvent();
	virtual inline int IsRealData(){ return isRealData_ ;} 
};

#endif
