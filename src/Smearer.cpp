#include "interface/Smearer.hpp"
#include "interface/Handlers.hpp"

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

int SmearJesPuppiMet::smear(Event *e)
{
    // only on MC
    if( e->IsRealData() ) return SMEAR_NA;
    // this is not const like in e->GetMet()
    //#warning NO MET IN JES
    GetMet(e) . puppiMetSyst . syst = syst_;
    GetMet(e) . puppiMetSyst . SetSmearType(Smearer::JES);
    
    if ( not GetMet(e) . puppiMetSyst . IsFilled() ) Log(__FUNCTION__,"WARNING","JES Smearing not filled in puppi MetMET");

    return SMEAR_OK;
}

int SmearJerPuppiMet::smear(Event *e)
{
    // only on MC
    if( e->IsRealData() ) return SMEAR_NA;
    // this is not const like in e->GetMet()
    //#warning NO MET IN JES
    GetMet(e) . puppiMetSyst . syst = syst_;
    GetMet(e) . puppiMetSyst . SetSmearType(Smearer::JER);
    
    if ( not GetMet(e) . puppiMetSyst . IsFilled() ) Log(__FUNCTION__,"WARNING","JER Smearing not filled in puppi MetMET");

    return SMEAR_OK;
}

int SmearUnclusteredPuppiMet::smear(Event *e)
{
    // only on MC
    if( e->IsRealData() ) return SMEAR_NA;
    // this is not const like in e->GetMet()
    //#warning NO MET IN JES
    GetMet(e) . puppiMetSyst . syst = syst_;
    GetMet(e) . puppiMetSyst . SetSmearType(Smearer::UNCLUSTER);
    
    if ( not GetMet(e) . puppiMetSyst . IsFilled() ) Log(__FUNCTION__,"WARNING","JES Smearing not filled in puppi MetMET");

    return SMEAR_OK;
}

int SmearJesAndCSV::smear(Event *e)
{
    // right correlations with jes, if wanted
    if (doSmearJes_){ 
        if (num_==Systematics::JESup and syst_>0)
            jes->SetSyst(1);
        else if (num_==Systematics::JESup and syst_<0)
            jes->SetSyst(-1);
        else jes->SetSyst(0);

        jes->smear(e);
    }

    if (bsystType==0)
    {
        SF_CSVReweight * sf= dynamic_cast<SF_CSVReweight*>(e->GetWeight()->GetSF("btag-reweight"));

        switch( num_ )
        {
            case Systematics::JESup :
                if (syst_>0) sf->systType = Systematics::JESup;
                else         sf->systType = Systematics::JESdown;
                break;
            case Systematics::CSVLFup :
                if (syst_>0) sf->systType = Systematics::CSVLFup;
                else         sf->systType = Systematics::CSVLFdown;
                break;
            case Systematics::CSVHFup :
                if (syst_>0) sf->systType = Systematics::CSVHFup;
                else         sf->systType = Systematics::CSVHFdown;
                break;
            case Systematics::CSVHFStats1up :
                if (syst_>0) sf->systType = Systematics::CSVHFStats1up;
                else         sf->systType = Systematics::CSVHFStats1down;
                break;
            case Systematics::CSVHFStats2up :
                if (syst_>0) sf->systType = Systematics::CSVHFStats2up;
                else         sf->systType = Systematics::CSVHFStats2down;
                break;
            case Systematics::CSVLFStats1up :
                if (syst_>0) sf->systType = Systematics::CSVLFStats1up;
                else         sf->systType = Systematics::CSVLFStats1down;
                break;
            case Systematics::CSVLFStats2up :
                if (syst_>0) sf->systType = Systematics::CSVLFStats2up;
                else         sf->systType = Systematics::CSVLFStats2down;
                break;
            case Systematics::CSVCErr1up :
                if (syst_>0) sf->systType = Systematics::CSVCErr1up;
                else         sf->systType = Systematics::CSVCErr1down;
                break;
            case Systematics::CSVCErr2up :
                if (syst_>0) sf->systType = Systematics::CSVCErr2up;
                else         sf->systType = Systematics::CSVCErr2down;
                break;
            case Systematics::NA : 
                sf->systType = Systematics::NA;
                break;
            default : 
                sf->systType = Systematics::NA;
                break;
        }
    }
    else if (bsystType==1)
    {
        SF_CSV * sf= dynamic_cast<SF_CSV*>(e->GetWeight()->GetSF(bdeep_));
        switch( num_ )
        {
            case Systematics::JESup :
                if (syst_>0) sf->systType = "up_jes";
                else         sf->systType = "down_jes";
                break;
            case Systematics::CSVLFup :
                if (syst_>0) sf->systType = "up_lf";
                else         sf->systType = "down_lf";
                break;
            case Systematics::CSVHFup :
                if (syst_>0) sf->systType = "up_hf";
                else         sf->systType = "down_hf";
                break;
            case Systematics::CSVHFStats1up :
                if (syst_>0) sf->systType = "up_hfstats1";
                else         sf->systType = "down_hfstats1";
                break;
            case Systematics::CSVHFStats2up :
                if (syst_>0) sf->systType = "up_hfstats2";
                else         sf->systType = "down_hfstats2";
                break;
            case Systematics::CSVLFStats1up :
                if (syst_>0) sf->systType = "up_lfstats1";
                else         sf->systType = "down_hfstats1";
                break;
            case Systematics::CSVLFStats2up :
                if (syst_>0) sf->systType = "up_hfstats2";
                else         sf->systType = "down_hfstats2";
                break;
            case Systematics::CSVCErr1up :
                if (syst_>0) sf->systType = "up_cferr1";
                else         sf->systType = "down_cferr1";
                break;
            case Systematics::CSVCErr2up :
                if (syst_>0) sf->systType = "up_cferr2";
                else         sf->systType = "down_cferr1";
                break;
            case Systematics::NA : 
                sf->systType = "";
                break;
            default : 
                sf->systType = Systematics::NA;
                break;
        }
    }
    else
    {cout <<" UNKNOWN bsystType:"<<bsystType<<endl;}

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

    //#warning NO MET IN JER
    GetMet(e) . syst = syst_;
    GetMet(e) . SetSmearType(Smearer::JER);
    if ( not GetMet(e) . IsFilled() ) Log(__FUNCTION__,"WARNING","JER Smearing not filled in MET");


    return SMEAR_OK;
}

int SmearUncluster::smear(Event *e)
{
    GetMet(e) . syst = syst_;
    GetMet(e) . SetSmearType(Smearer::UNCLUSTER);
    
    if ( not GetMet(e) . IsFilled() ) Log(__FUNCTION__,"WARNING","UNCLUSTER Smearing not filled in MET");

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

int SmearTauScale::smear(Event *e)
{
    // only on data apply Jes
    //if( not e->IsRealData() ) return SMEAR_NA;
    TLorentzVector metSystShift;

    for (auto t : GetTaus(e))
    {
        t->syst = syst_;
    }


    // this is not const like in e->GetMet()
    //#warning NO MET IN JES
    GetMet(e) . syst = syst_;
    GetMet(e) . SetSmearType(Smearer::TAUESCALE);
    
    if ( not GetMet(e) . IsFilled() ) Log(__FUNCTION__,"WARNING","JES Smearing not filled in MET");

    return SMEAR_OK;
}

int SmearBjets::smear(Event *e)
{
    //e -> GetWeight()-> SetSystSF (sfname_,syst_);
    SF_CSV* sf = dynamic_cast<SF_CSV*>(e -> GetWeight()-> GetSF (sfname_) );
    if (sf==NULL)
    {
        Log(__FUNCTION__,"ERROR","Unable to locate SF with name "+sfname_);
        throw abort;
    }
    if (sf->simpleError){
        Log(__FUNCTION__,"ERROR","SF" + sf->name() +" has simpleErrors");
        throw abort;
    }

    if (doB) sf->systB = syst_;
    if (doL) sf->systL = syst_;
    if (not doB and not doL) { Log(__FUNCTION__,"ERROR","Should smear something"); throw abort; }

    return SMEAR_OK;

}

// Smear WG1
int SmearWG1::smear(Event *e){
    SF_WG1* sf = dynamic_cast<SF_WG1*>(e -> GetWeight()-> GetSF (sfname_) );
    if (sf==NULL)
    {
        Log(__FUNCTION__,"ERROR","Unable to locate SF with name "+sfname_);
        throw abort;
    }

    if (num_ >=0 )
    {
        sf->type = SF_WG1::THU_WG1;
        sf->nuisance = num_;
        sf->syst = syst_;
    }
    return SMEAR_OK;

}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
