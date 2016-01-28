#include "interface/Smearer.hpp"
int SmearJes::smear(Event *e)
{
    // only on data apply Jes
    if( not e->IsRealData() ) return SMEAR_NA;

    //for (auto j : e->jets_)
    for (auto j : GetJets(e))
        j->syst = syst_;
    //e->met_ .syst = syst_;
    GetMet(e) . syst = syst_;

    return SMEAR_OK;
}

int SmearJer::smear(Event *e)
{
    // only on data apply Jes
    if( e->IsRealData() ) return SMEAR_NA;
    //for (auto j : e->jets_)
    for (auto j : GetJets(e))
        j->syst = syst_;
    return SMEAR_OK;
}

int SmearLepSF::smear(Event *e)
{
    if( e->IsRealData() ) return SMEAR_NA;

    e -> GetWeight()-> SetSystSF ("LEP",syst_);
    return SMEAR_OK;

}

int SmearPu::smear(Event *e)
{
    if( e->IsRealData() ) return SMEAR_NA;

    e -> GetWeight() -> SetSystPU ( syst_ );

    return SMEAR_OK;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
