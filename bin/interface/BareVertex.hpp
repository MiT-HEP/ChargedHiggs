#ifndef BARE_VERTEX_H
#define BARE_VERTEX_H

#include "NeroProducer/Core/interface/BareCollection.hpp"


class BareVertex : virtual public BareCollection
{
    public:
        BareVertex();
        ~BareVertex();
        void clear() override;
        void defineBranches(TTree *t) override;
        void setBranchAddresses(TTree*) override;
        inline string name() override { return "BareVertex"; }
        inline unsigned size() const override { return npv; }

        int npv;

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
