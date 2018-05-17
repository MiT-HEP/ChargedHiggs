#ifndef BARE_P4_OBJ_H
#define BARE_P4_OBJ_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareP4 : virtual public BareCollection
{
    // just a container to identify all the objects that are based on p4
    bool doMatch_{false};
    public:
        BareP4();
        ~BareP4();

        inline string name() override { return "BareP4"; }
        inline unsigned size() const override { if (p4) return p4->GetEntries(); else return 0; }

        virtual inline TLorentzVector const& momentum(unsigned idx) const { return *static_cast<TLorentzVector*>((*p4)[idx]); }
        virtual inline double px(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->Px(); }
        virtual inline double py(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->Py(); }
        virtual inline double pz(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->Pz(); }
        virtual inline double energy(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->Energy(); }
        virtual inline double pt(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->Pt(); }
        virtual inline double eta(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->Eta(); }
        virtual inline double phi(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->Phi(); }
        virtual inline double mass(unsigned idx) const { return static_cast<TLorentzVector*>((*p4)[idx])->M(); }

        TClonesArray  *p4{0};

        virtual bool isInit() { return p4 != NULL;}

        void init() override;
        void clear() override;
        virtual void defineBranches(TTree* t, std::string prefix);
        virtual void setBranchAddresses(TTree* t, std::string prefix);
        void defineBranches(TTree* t) override { defineBranches(t, ""); }
        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }

        void compress() override;
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
