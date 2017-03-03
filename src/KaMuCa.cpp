#include "interface/KaMuCa.hpp"
#include "interface/Handlers.hpp"
#include <cmath>

void KaMuCa::Init(){
    Log(__FUNCTION__,"INFO","Init KaMuCa Corrections with MC_80X_13TeV and DATA_80X_13TeV");
    correctorMC_.reset(new KalmanMuonCalibrator("MC_80X_13TeV") ) ;
    correctorDATA_.reset(new KalmanMuonCalibrator("DATA_80X_13TeV") ) ;
    if (doResolution) resolution_.reset(new KalmanMuonResolution("aux/KaMuCa/"));
    rnd_ . reset(new TRandom3());
    //rnd_ . SetSeed(123456);
}

int KaMuCa::correct(Event *e){

    correctorDATA_ -> reset();
    correctorMC_ -> reset();

    if (syst!=0)
    {
        // check
        if (syst >= correctorDATA_->getN()+1 or syst < -correctorDATA_->getN())
        {
            Log(__FUNCTION__,"ERROR",Form("CorrectorData has %d variations (2n+1), and position %d has been requested ",correctorDATA_->getN(),syst));
            throw abortException();
        }
    
        // check
        if (syst >= correctorMC_->getN()+1 or syst < -correctorMC_->getN())
        {
            Log(__FUNCTION__,"ERROR",Form("CorrectorData has %d variations (2n+1), and position %d has been requested ",correctorMC_->getN(),syst));
            throw abortException();
        }

        // set syst N+1 = closure
        if (e->IsRealData() and syst ==  correctorDATA_->getN()+1)
        {
            correctorDATA_->varyClosure(+1) ;
        }
        else if (not e->IsRealData() and syst ==  correctorMC_->getN()+1)
        {
            correctorMC_->varyClosure(+1) ;
        }
        else if ( syst >0 )
        {
            if ( e->IsRealData() ) correctorDATA_->vary(syst,+1); 
            else correctorMC_ -> vary ( syst , + 1);
        }
        else //(syst <0)
        {
            if ( e->IsRealData() ) correctorDATA_->vary(syst,-1); 
            else correctorMC_ -> vary ( syst , -1);
        }

    }


    for (auto & lep : GetLepVector(e))
    {
        if (not lep->IsMuonDirty() ) continue; // check only it's muon w/o any Id
        // reset uncorrected value
        ResetUncorr(*lep);

        if (doResolution and not e->IsRealData())
        {
            resolution_ -> setPtEta(lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta());
            float genPt=-1;
            // find match gen particle
            for (int ig=0;/*empty*/;++ig)
            {
                GenParticle * gp = e->GetGenStable(ig,13); // apdgid
                if (gp==NULL) break;
                if ((lep->DeltaR(gp)<.3) and genPt<gp->Pt() ) genPt=gp->Pt();
            }

            float sf = resolution_->getSF() ;
            if (genPt>0)
            {
                double corr = (( lep->GetP4Dirty().Pt()  - genPt ) *sf  + genPt) / lep->GetP4Dirty().Pt();
                Scale( *lep, corr );
            }
            else
            {
                float res = resolution_->getResData();  
                float sigma=std::sqrt( sf*sf -1) * res * lep->GetP4Dirty().Pt(); // res is for dp/p
                float s = rnd_->Gaus(0,1);
                float newpt = s * sigma + lep->GetP4Dirty().Pt();
                Scale(*lep,newpt/lep->GetP4Dirty().Pt());
            }
        }

        //
        if ( e->IsRealData()){
            double dataSF = correctorDATA_ ->getCorrectedPt(
                    lep->GetP4Dirty().Pt(), // bypass syst
                    lep->GetP4Dirty().Eta(),
                    lep->GetP4Dirty().Phi(),
                    lep->Charge()) / lep->GetP4Dirty().Pt();
            Scale( *lep, dataSF);
        }
        else {
            // correcting scale in MC, they should align everything to the best knowledge
            double mcSF = correctorMC_ ->getCorrectedPt(
                    lep->GetP4Dirty().Pt(), // bypass syst
                    lep->GetP4Dirty().Eta(),
                    lep->GetP4Dirty().Phi(),
                    lep->Charge()) / lep->GetP4Dirty().Pt();
            mcSF *= correctorMC_ -> smear ( lep->GetP4Dirty().Pt(),lep->GetP4Dirty().Eta()) /lep->GetP4Dirty().Pt(); 
            Scale( *lep, mcSF);
        }

        //Uncertainties https://github.com/bachtis/Analysis/blob/KaMuCa_V3/Calibration/test/test.py
        //print 'propagate the statistical error of the calibratio to a + 40 GeV muon at eta=phi=0'
        //print 'first get number of parameters'
        //N=c.getN()
        //print N,'parameters'
        //for i in range(0,N):
        //    c.vary(i,+1)
        //      print 'variation',i,'ptUp', c.getCorrectedPt(40,0.0,0.0,1)
        //      c.vary(i,-1)
        //      print 'variation',i,'ptDwn', c.getCorrectedPt(40,0.0,0.0,1)
        //
        //
        //c.reset()
        //
        //print 'propagate the closure error to a + 40 GeV muon at eta=phi=0'
        //
        //c.varyClosure(+1)
        //print 'After closure shift pt=', c.getCorrectedPt(40,0.0,0.0,1)

    }
}

// ------------ SMEAR ------------
int SmearKaMuCa::smear(CorrectorBase*c)
{
    //syst_
    //check that the corrector is right, first should be faster
    if (dynamic_cast<KaMuCa*>(c) == NULL ) return 0;
    if (c->name() != "KaMuCa") return 0; // nothing to correct
    dynamic_cast<KaMuCa*>(c)->syst = syst_;
}

// ------------- RESOLUTION -- From Elisabetta
float KalmanMuonResolution::getRes(float a, float b, float c, float e, float p2, float L){

	//const float L0=103.8;

	return sqrt(a*L0*L0/L/L+b*L0*L0/L/L/(1+370./p2/(L0*L0/L/L))+c*L0*L0/L/L*L0*L0/L/L*p2+e*L0*L0/L/L*L0*L0/L/L*p2);
}

float KalmanMuonResolution::getL(float eta){

	//const float L0=103.8;
	//const float rmax=108.0;
	float r=-99, L=-99;
	float ttheta=2/(expf(eta)-expf(-eta));

	if(fabs(eta)<1.4){
		L=L0;
	}
	else{
		if(eta>1.4){

			r=(267*ttheta);
		}
		else if(eta<-1.4){
			r=(-267*ttheta);
		}

		L=(std::min(r,rmax))-4.4;
	}
	return L;
}

void KalmanMuonResolution::init(const string& dir)
{
	files_["mc_a"]      .reset( TFile::Open((dir + "resultsJMC50_80X.root"  ).c_str()) );
	files_["mcBinn"]    .reset( TFile::Open((dir + "mcResolution50_80X.root").c_str()) );
	files_["mc_e"]      .reset( TFile::Open((dir + "resultsZMCBWm40.root"   ).c_str()) );
	files_["data_a"]    .reset( TFile::Open((dir + "resultsJDATA50_80X.root").c_str()) );
	files_["data_e"]    .reset( TFile::Open((dir + "resultsZDATABWm40.root" ).c_str()) );
	files_["data_e_old"].reset( TFile::Open((dir + "resultsZDATABWnew.root" ).c_str()) );
	files_["mc_e_old"]  .reset( TFile::Open((dir + "resultsZMCBWnew.root"   ).c_str()) );
    
    for(auto&p : files_) 
    {
        if (p.second==NULL) cerr<<"File: "<< p.first <<" not open correctly"<<endl;
    }

    // INIT Histos
	hists_["c_mcbinn"] . reset ( (TH1*)files_["mcBinn"]->Get("c_0")->Clone());
	hists_["b_mcbinn"] . reset ( (TH1*)files_["mcBinn"]->Get("b_0")->Clone());

	TH3 *e_mc=(TH3*)files_["mc_e"]->Get("e_0");
	hists_["e_mc1D"] . reset( e_mc->ProjectionY("mc_e",1,1,1,1) );

	TH3 *a_mc=(TH3*)files_["mc_a"]->Get("a_0");
	hists_["a_mc1D"] . reset ( a_mc->ProjectionY("mc_a",1,1,1,1)) ;

	TH3 *e_data=(TH3*)files_["data_e"]->Get("e_0");
	hists_["e_data1D"] . reset ( e_data->ProjectionY("data_e",1,1,1,1) );

	TH3 *a_data=(TH3*)files_["data_a"]->Get("a_0");
	hists_["a_data1D"] . reset ( a_data->ProjectionY("data_a",1,1,1,1) );

	TH3 *e_mc_old=(TH3*)files_["mc_e_old"]->Get("e_0");
	hists_["e_mc1D_old"] . reset (e_mc_old->ProjectionY("mc_e_old",1,1,1,1) );

	TH3 *e_data_old=(TH3*) files_["data_e_old"]->Get("e_0");
	hists_["e_data1D_old"] .reset ( e_data_old->ProjectionY("data_e_old",1,1,1,1) );

}

void KalmanMuonResolution::setPtEta( float pt, float eta )  
{
    #warning fix_for_bad_endcap
    if (eta<-2) eta=fabs(eta);

    float t = std::exp(-eta); 
    float sin_theta = 2*t / (1+ t*t);
    float p2 = std::pow( t/sin_theta,2);
    float L = getL(eta);
	int bin=hists_["e_mc1D"]->FindBin(eta);
	float err_amc  = std::pow(hists_["a_mc1D"]->GetBinError(bin),2)   * std::pow(L0/L,4);
	float err_adata= std::pow(hists_["a_data1D"]->GetBinError(bin),2) * std::pow(L0/L,4);
	float err_emc  = std::pow(hists_["e_mc1D"]->GetBinError(bin),2) * std::pow(L0/L,4);
	float err_edata= std::pow(hists_["e_data1D"]->GetBinError(bin),2) * std::pow(L0/L,4);

	float a_mc_DL   = hists_["a_mc1D"]->GetBinContent(bin);
	float a_data    = hists_["a_data1D"]->GetBinContent(bin);
	float b         = hists_["b_mcbinn"]->GetBinContent(bin);
	float c         = hists_["c_mcbinn"]->GetBinContent(bin);
	float e_mc      = hists_["e_mc1D"]->GetBinContent(bin);
	float e_data    = hists_["e_data1D"]->GetBinContent(bin);

	res_mc_    = getRes(a_mc_DL, b, c, e_mc, p2, L);
	res_data_  = getRes(a_data, b, c, e_data, p2, L);
    res_mc_error_   = std::sqrt((1./4/(res_mc_*res_mc_))*(err_amc+err_emc*p2*p2));
    res_data_error_ = std::sqrt((1./4/(res_data_*res_data_))*(err_adata+err_edata*p2*p2));

}



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
