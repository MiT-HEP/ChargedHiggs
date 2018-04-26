#include "interface/SmearJesSources.hpp"
#include "interface/Smearable.hpp"
#include "interface/Handlers.hpp"
#include <cmath>

SmearJesSource::SmearJesSource(const string &n) : SmearBase(){
	name_ = "JES_"+n;
	Log(__FUNCTION__,"INFO","Constructing smear function using definitions in: "+fname_ + " and section: "+n);
	params_ . reset ( new JetCorrectorParameters(fname_,n) ) ;
	jecUnc_ . reset ( new JetCorrectionUncertainty( *params_) );
}

int SmearJesSource::smear(Event *e)
{
    TLorentzVector unsmeared;
    TLorentzVector smearedUp;
    TLorentzVector smearedDown;

    for (auto j : GetJets(e))
    {
        j->syst = 0; // use nominal point while setting
	j->SetFilled(Smearer::SOURCES,1) ;
	unsmeared += j->GetP4();

	//Log(__FUNCTION__,"DEBUG",Form("Setting pt=%f eta=%f",j->Pt(),j->Eta()));

	jecUnc_->setJetPt(j->Pt());
	jecUnc_->setJetEta(j->Eta());
	float sup =  jecUnc_->getUncertainty(true);

	jecUnc_->setJetPt(j->Pt()); // need to be reset
	jecUnc_->setJetEta(j->Eta());
	float sdw =  jecUnc_->getUncertainty(false);

	//Log(__FUNCTION__,"DEBUG",Form("Getting Uncertainties up=%f down=%d",sup,sdw));

	j->SetValueUp(Smearer::SOURCES,j->Pt() * (1+ sup) ) ;
	j->SetValueDown(Smearer::SOURCES,j->Pt() * (1 - sdw) ) ;

	TLorentzVector sUp (j->GetP4() ) ; sUp *= (1+sup);
	TLorentzVector sDown (j->GetP4() ) ; sDown *= (1 - sdw);

	smearedUp += sUp;
	smearedDown += sDown;

        j->SetSmearType(Smearer::SOURCES);
        j->syst = syst_;
    }

    GetMet(e) . syst = 0;
    // new met is  = smeared - unsmeared + metOld
    smearedUp += GetMet(e).GetP4() - unsmeared;
    smearedDown += GetMet(e).GetP4() - unsmeared;

    GetMet(e) . SetValueUp(Smearer::SOURCES,smearedUp.Pt() ) ;
    GetMet(e) . SetValueDown(Smearer::SOURCES, smearedDown.Pt()) ;
    GetMet(e) . SetSmearType(Smearer::SOURCES);
    GetMet(e) . syst = syst_;
    
    return SMEAR_OK;
}
