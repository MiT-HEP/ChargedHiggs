#include "interface/Smearer.hpp"
int SmearJes::smear(Event *e)
{
    // only on data apply Jes
    //if( not e->IsRealData() ) return SMEAR_NA;

    for (auto j : GetJets(e))
    {
        j->syst = syst_;
        j->SetSmearType(Smearer::JES);
        if (not j->IsFilled() ) Log(__FUNCTION__,"WARNING","JES Smearings values not filled in JET");
    }

    // this is not const like in e->GetMet()
    //#warning NO MET IN JES
    GetMet(e) . syst = syst_;
    GetMet(e) . SetSmearType(Smearer::JES);
    
    if ( not GetMet(e) . IsFilled() ) Log(__FUNCTION__,"WARNING","JES Smearing not filled in MET");

    return SMEAR_OK;
}

int SmearJer::smear(Event *e)
{
    // only on mc
    if( e->IsRealData() ) return SMEAR_NA;
    //for (auto j : e->jets_)
    for (auto j : GetJets(e))
    {
        j->syst = syst_;
        j->SetSmearType(Smearer::JER);
        if (not j->IsFilled() ) Log(__FUNCTION__,"WARNING","JER Smearings values not filled in JET");
    }

#warning NO MET IN JER
    //GetMet(e) . syst = syst_;
    //GetMet(e) . SetSmearType(Smearer::JER);
    //if ( not GetMet(e) . IsFilled() ) Log(__FUNCTION__,"WARNING","JER Smearing not filled in MET");


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

int SmearSF::smear(Event *e)
{
    //if( e->IsRealData() ) return SMEAR_NA;

    //Log(__FUNCTION__,"DEBUG", "Setting Systematics SF for "+ sfname_ + Form(" to %d",syst_));
    e -> GetWeight()-> SetSystSF (sfname_,syst_);
    return SMEAR_OK;

}

// -----------------------------------
int SmearScales::smear(Event*e)
{
    if ( e->IsRealData() ) return SMEAR_NA;

    // syst in Weight here will have the values of MC::SCALES
    if (syst_ == 1 )
    {
        e->GetWeight() -> SetSystPdf(-1);
        if (doRen and doFac) e->GetWeight() -> SetSyst( MC::r2f2);
        else if (doRen) e->GetWeight() -> SetSyst ( MC::r2f1) ;
        else if (doFac) e->GetWeight() -> SetSyst (MC::r1f2);
        else e->GetWeight() -> SetSyst( MC::none);
    }
    if (syst_ == -1)
    {
        e->GetWeight() -> SetSystPdf(-1);
        if (doRen and doFac) e->GetWeight() -> SetSyst( MC::r5f5);
        else if (doRen) e->GetWeight() -> SetSyst ( MC::r5f1) ;
        else if (doFac) e->GetWeight() -> SetSyst (MC::r1f5);
        else e->GetWeight() -> SetSyst( MC::none);
    }
    if (syst_ == 0 ) e->GetWeight() ->SetSyst( MC::none);
    return SMEAR_OK;
}

int SmearPdfs::smear(Event*e)
{
    if ( e->IsRealData() ) return SMEAR_NA;

    // syst in Weight here will have the values of MC::SCALES
    if (syst_ != 0 )
    {
        e->GetWeight() -> SetSystPdf( GetPos());
        e->GetWeight() ->SetSyst( MC::none);
    }
    else e->GetWeight() -> SetSystPdf(-1);
    return SMEAR_OK;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
