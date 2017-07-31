#ifndef CORRECTOR_H
#define CORRECTOR_H

#include "interface/Event.hpp"
#include "interface/Named.hpp"

class CorrectorBase : public Named {
	public:
		virtual int correct(Event *e) {return 0;};
		const string name() const override { return "CorrectorBase";}
		virtual void Init() {}
	
		// implement read/write access to event	
		Met & GetMet(Event *e) { return e->met_ ;}
		vector<Tau *> & GetTauVector(Event *e) { return e->taus_;}
        vector<Lepton *>& GetLepVector(Event *e) { return e->leps_;}
        vector<GenParticle *>& GetGenVector(Event *e) { return e->genparticles_;}
		vector<Jet *> & GetJetVector(Event *e) { return e->jets_;}
        
        // R/W Access to Objects, private/protected members
        inline void Scale( Object&o, float value) { o.Scale(value);}
        inline void Add(Object &o, TLorentzVector&v, float c){o.Add(v,c);}
        inline void SetP4(Object&o, TLorentzVector &v){o.p4 = v; o.Scale(1.) ; } // the last set is Correct
        inline void ResetUncorr(Object&o){o.ResetUncorr();}

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
