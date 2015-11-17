#include "interface/RegressionReader.hpp"
//#define DEBUG 1

void RegressionReaderBase::AddVariable(string name,char type)
{
	varsValues_.Add(name,type);
	if (type =='F') reader_->AddVariable(name.c_str(), (float*) varsValues_.GetPointer(name) );
	if (type =='I') reader_->AddVariable(name.c_str(), (int*) varsValues_.GetPointer(name) );
}

// --------------------------------------
//
float TauRegression::Eval(){
	return reader_ ->EvaluateMVA("BDT");
}


int TauRegression::correct(Event*e){
	// keep track of the total correction
	TLorentzVector corr;

	for(unsigned iT=0;iT< GetTauVector(e).size() ; ++iT)
	{
		Tau* t= GetTauVector(e)[iT];

	    if ( t->IsTau() and t->regVars_ .jetPt <0 ) 
	    {
	    	cout <<"[TauRegression]::[coorrect]::[WARNING] Tau w/o associated jets ... not correcting it;"<<endl;
            continue;
	    }

		SetVariable("nvtx",t->regVars_ . nvtx);
		SetVariable("tauPt",t->regVars_ . tauPt);
		SetVariable("tauEta",t->regVars_ . tauEta);
		SetVariable("tauM",t->regVars_ . tauM);
		SetVariable("tauQ",t->regVars_ . tauQ);
		SetVariable("tauIso",t->regVars_ . tauIso);
		SetVariable("tauIso2",t->regVars_ . tauIso2);
		SetVariable("tauChargedIsoPtSum",t->regVars_ . tauChargedIsoPtSum);
		SetVariable("tauNeutralIsoPtSum",t->regVars_ . tauNeutralIsoPtSum);
		SetVariable("jetPt",t->regVars_ . jetPt);
		SetVariable("jetEta",t->regVars_ . jetEta);

		// p4 is uncorrected. Pt() is corrected
		//
        float ptcorr = t->Pt();
		float ptraw = t-> PtUncorr();
		float regfact = Eval(); // tauParton/tauPt
		float ptreg = regfact * ptraw;

		// HOW ABOUT SYST SCALE ENERGY ? Correctors are after syst -> this will 
		Scale(*t, regfact  ); 
		// track the total correction
		TLorentzVector c;
		c.SetPtEtaPhiM( t->Pt()-ptcorr, t->Eta(), t->Phi(), 0);

		// if it is a selected tau
		if( t->IsTau() ) corr += c;
	}

	// correct met for selected taus
#ifdef DEBUG
    if(DEBUG>0)cout << "[TauRegression]::[correct]::[DEBUG]      MET was ="<<e->GetMet().Pt() <<endl;
    if(DEBUG>0)cout << "[TauRegression]::[correct]::[DEBUG]  UNC MET was ="<<e->GetMet().PtUncorr() <<endl;
#endif

	Add(GetMet(e), corr, -1 ) ; 

#ifdef DEBUG
    if(DEBUG>0)cout << "[TauRegression]::[correct]::[DEBUG]  NEW MET  is ="<<e->GetMet().Pt() <<endl;
    if(DEBUG>0)cout << "[TauRegression]::[correct]::[DEBUG]  OLD MET was ="<<e->GetMet().PtUncorr() <<endl;

    if (e->Ntaus() == 1) {
        if(DEBUG>0)cout <<"[TauRegression]::[correct]::[DEBUG] Leading Tau is = "<<e->GetTau(0)->Pt() <<endl;
        if(DEBUG>0)cout <<"[TauRegression]::[correct]::[DEBUG] Leading Tau was = "<<e->GetTau(0)->PtUncorr() <<endl;
    }
#endif

}
void TauRegression::Init(){
	if(reader_==NULL)reader_  = new TMVA::Reader();
	cout<<"[TauRegression]::[Init]::[INFO] Booking regression: '"<<fileName<<"'"<<endl;

	AddVariable("nvtx",'F');
	AddVariable("tauPt",'F');
	AddVariable("tauM",'F');
	AddVariable("tauQ",'F');
	AddVariable("tauEta",'F');
	AddVariable("tauIso",'F');
	AddVariable("tauIso2",'F');
	AddVariable("tauChargedIsoPtSum",'F');
	AddVariable("tauNeutralIsoPtSum",'F');
	AddVariable("jetPt",'F');
	AddVariable("jetEta",'F');

	reader_->BookMVA("BDT",fileName.c_str());
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
