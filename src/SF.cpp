#include "interface/SF.hpp"
#include "interface/Handlers.hpp"
#include "TFile.h"
#include "TH2F.h"
#include "TMVA/TSpline1.h"
//#define VERBOSE 2
//
void SF::print() const
{
    Log(__FUNCTION__,"INFO","------ SF: "+ label + "-------");
    Log(__FUNCTION__,"INFO",Form("sf=%f",sf));
    Log(__FUNCTION__,"INFO",Form("err=%f",err));
    Log(__FUNCTION__,"INFO",Form("syst=%d",syst));
    Log(__FUNCTION__,"INFO","------------------------------");
}

void SF_PtEta::add(double pt1, double pt2,double eta1, double eta2, double sf, double err)
{
    range r;
    r.pt1 = pt1;
    r.pt2 = pt2;
    r.eta1 = eta1;
    r.eta2 = eta2;

    store[r].first = sf;
    store[r].second = err;

#ifdef VERBOSE
    if(VERBOSE>0)cout <<"[SF_PtEta]::[add]::[DEBUG] Adding SF Pt Eta:"<<pt1<<":"<<pt2<<"|"<<eta1<<":"<<eta2<<"|"<<sf<<":"<<err<<endl;
#endif
}

void SF_PtEta::set( double pt, double eta)
{
    int change = 0;
    for(auto s : store)
    {
        if ( pt < s.first.pt1 ) continue;
        if ( s.first.pt2  <= pt ) continue;
        if ( eta < s.first.eta1 ) continue;
        if ( s.first.eta2  <= eta ) continue;
        sf = s.second.first;	
        err = s.second.second;
        change = 1;
        break;
    }
    if (not change)
    {
        cout<<"[SF_PtEta]::[set]::[ERROR] no PT ETA RANGE for SF '"<<label<<"' in pt="<<pt<<" eta="<<eta<<endl;
        sf = 1.0;
        err = 0.0;
    }
}

const bool operator<( const SF_PtEta::range&r1 , const SF_PtEta::range &r2)
{
    // on pt1
    if (r1.pt1 < r2.pt1) return true;
    if (r1.pt1 > r2.pt1) return false;
    // -- pt2
    if (r1.pt2 < r2.pt2) return true;
    if (r1.pt2 > r2.pt2) return false;
    // -- eta1
    if (r1.eta1 < r2.eta1) return true;
    if (r1.eta1 > r2.eta1) return false;
    // -- eta2
    if (r1.eta2 < r2.eta2) return true;
    if (r1.eta2 > r2.eta2) return false;
    // they are equal
    return false;
}

void SF_PtEta::print() const {
    cout <<" ----- SF Pt Eta ------"<<endl;
    cout <<"label='"<<label<<"'"<<endl;
    for(auto& p : store ) 
    {
        cout << p.first.pt1<<":"<<p.first.pt2<<"|"<<p.first.eta1<<":"<<p.first.eta2<<"||"<< p.second.first<<":"<<p.second.second<<endl;
    }
    cout <<" ----------------------"<<endl;
}

void SF_PtEta_And_Eff::add(double pt1,double pt2 ,double eta1, double eta2, double dataEff, double mcEff, double dataErr, double mcErr )
{
    //SF_PtEta::add(pt1,pt2,eta1,eta2, dataEff/mcEff, (dataErr/dataEff + mcErr/mcEff) * dataEff/mcEff); 
    Log(__FUNCTION__,"INFO",Form("* Adding sf in range pt [%f,%f), eta[%f,%f) deff=%f (+/-%f) mceff=%f (+/-%f)",pt1,pt2,eta1,eta2,dataEff,dataErr,mcEff ,mcErr) );
    range r;
    r.pt1 = pt1;
    r.pt2 = pt2;
    r.eta1 = eta1;
    r.eta2 = eta2;

    storeData_[r].first = dataEff;
    storeData_[r].second = dataErr;
    storeMC_[r].first = mcEff;
    storeMC_[r].second = mcErr;
}
void SF_PtEta_And_Eff::set(double pt, double eta)
{
    //SF_PtEta::set(pt,eta); // set sf and err
    //if (not veto_) { return;}
    int change = 0;
    range r;
    effData_=1,errData_=0;

    for(auto s : storeData_)
    {
        // DEBUG
        //cout <<"Considering element: ["<<s.first.pt1<<","<<s.first.pt2<<") ["<<s.first.eta1<<","<<s.first.eta2<<") against "<<pt<<","<<eta<<endl;
        if ( pt < s.first.pt1 ) continue;
        if ( s.first.pt2  <= pt ) continue;
        if ( eta < s.first.eta1 ) continue;
        if ( s.first.eta2  <= eta ) continue;
        //cout <<" * OK"<<endl;
        effData_ = s.second.first;
        errData_ = s.second.second;
        r = s.first;
        change = 1;
        break;
    }

    if (not change)
    {
        Log(__FUNCTION__,"ERROR","no Pt eta range for SF " + label + Form( " in pt %f eta %f ",pt,eta));
        sf = 1.0;
        err = 0.0;
        effMC_ = 1.0; errMC_=0.0;
        return;
    }

    effMC_ = storeMC_[r].first;
    errMC_ = storeMC_[r].second;


}

double SF_PtEta_And_Eff::get()
{
    if (veto_)
    {
        sf = (1.-effData_) / (1.-effMC_);

        if (dataErr_)
            err = fabs(errData_ / (1.-effData_) ) * sf;
        else if (sqrErr_)
            err = std::sqrt( std::pow(errData_ / (1.-effData_),2) + std::pow(errMC_ / (1.-effMC_),2)) *sf;
        else
            err = (fabs(errData_ / (1.-effData_)) + fabs(errMC_ / (1.-effMC_)) ) * sf;
    }
    else{
        sf = effData_ / effMC_;
        if (dataErr_)
            err = fabs(errData_ / effData_ ) * sf;
        else if (sqrErr_)
            err = std::sqrt((errData_/effData_)*(errData_/effData_)+ (errMC_/effMC_)*(errMC_/effMC_)  ) * sf ;
        else
            err = fabs(errData_/effData_) * sf  + fabs(errMC_/effMC_) * sf ;
    }

    return sf + err*syst;
}

// --- TH1F 

void SF_TH2F::init(string filename,string histname,string errorname)
{
    TFile *f = TFile::Open(filename.c_str() ) ;
    if (f == NULL){
        Log(__FUNCTION__,"ERROR","file '" + filename + "' does not exist");
        throw abortException() ;
    }
    TH2F *h=(TH2F*)f->Get(histname.c_str());
    if (h==NULL){
        Log(__FUNCTION__,"ERROR","h '"+histname+"' does not exist");
        throw abortException() ;
    }

    TH2F *errHist=NULL;

    if (errorname!="")
    {
        errHist=(TH2F*)f->Get(errorname.c_str());
        if (errHist==NULL){
            Log(__FUNCTION__,"ERROR","error name '"+errorname+"' does not exist");
            throw abortException() ;
        }
    }

    for( int aetabin =1; aetabin <= h->GetNbinsX() ; ++aetabin)
    for( int ptbin =1; ptbin <= h->GetNbinsY() ; ++ptbin)
    {
        float ptmin = h->GetYaxis()->GetBinLowEdge(ptbin); 
        float ptmax = h->GetYaxis()->GetBinLowEdge(ptbin+1); 
        float aetamin = h->GetXaxis()->GetBinLowEdge(aetabin); 
        float aetamax = h->GetXaxis()->GetBinLowEdge(aetabin+1); 
        float sf = h->GetBinContent(aetabin,ptbin);
        float err = h->GetBinError(aetabin,ptbin);
        if (errHist)
        {
            err=errHist->GetBinContent(aetabin,ptbin);
        }
        if (ptbin == h->GetNbinsY() ) ptmax = 8000.; // highest is open, current recommendation
        add(ptmin,ptmax,aetamin,aetamax,sf,err);
    }
}

TH2* SF_TH2F_And_Eff::getHisto(const string & name)
{
    TH2* r = (TH2*)f->Get(name.c_str());
    if (r == NULL)
    {
        Log(__FUNCTION__,"ERROR","hist '"+name+"' does not exist");
        throw abortException() ;
    }
    return r;
}

void SF_TH2F_And_Eff::init(string filename,string effdata,string effmc,string errordata,string errormc)
{
    f = TFile::Open(filename.c_str() ) ;

    if (f == NULL){
        Log(__FUNCTION__,"ERROR","file '" + filename + "' does not exist");
        throw abortException() ;
    }

    TH2 * hDataEff = getHisto(effdata);
    TH2 * hDataErr = NULL;
    if (errordata != "" ) hDataErr = getHisto(errordata);
    TH2 * hMcEff = getHisto(effmc);
    TH2 * hMcErr = NULL;
    if (errormc != "" ) hMcErr = getHisto(errormc);


    for( int aetabin =1; aetabin <= hDataEff->GetNbinsX() ; ++aetabin)
    for( int ptbin =1; ptbin <= hDataEff->GetNbinsY() ; ++ptbin)
    {
        float ptmin = hDataEff->GetYaxis()->GetBinLowEdge(ptbin); 
        float ptmax = hDataEff->GetYaxis()->GetBinLowEdge(ptbin+1); 
        float aetamin = hDataEff->GetXaxis()->GetBinLowEdge(aetabin); 
        float aetamax = hDataEff->GetXaxis()->GetBinLowEdge(aetabin+1); 
        float effData = hDataEff->GetBinContent(aetabin,ptbin);
        float errData = 0.0 ;
        if (hDataErr) err=hDataErr->GetBinContent(aetabin,ptbin);
        else errData = hDataEff->GetBinError(aetabin,ptbin);

        float effMc = hMcEff->GetBinContent(aetabin,ptbin);
        float errMc = 0.0; 
        if (hMcEff) errMc = hMcEff->GetBinContent(aetabin,ptbin);
        else errMc = hMcEff->GetBinError(aetabin,ptbin);

        if (ptbin == hDataEff->GetNbinsY() ) ptmax = 8000.; // highest is open, current recommendation
        if (aetabin == hDataEff->GetNbinsX()  ) aetamax = aetamax+0.0001; // put it slightly larger  to get 2.4 as well
        add(ptmin,ptmax,aetamin,aetamax,effData,effMc,errData,errMc);
    }
    f->Close(); // delete?
    delete f;
    f = NULL;
}

// ------------- SPLINE -------------
//
#include "TFile.h"
void SF_PtSpline::set( double pt){
#ifdef VERBOSE
    if(VERBOSE>1)Log(__FUNCTION__,"DEBUG", Form("is spline_ null ? %ld and err? %ld",spline_,errSpline_) );
    if(VERBOSE>1) print();
#endif
    if (spline_ == NULL) init();
    sf = spline_ -> Eval(pt);
    err = errSpline_ -> Eval(pt);
    return;
}

void SF_PtSpline::init(){ // init splines from TGraph
#ifdef VERBOSE
    if(VERBOSE>1)Log(__FUNCTION__,"DEBUG", Form("is spline_ null ? %ld and err? %ld",spline_,errSpline_) );
#endif
    if (spline_==NULL and g_ ==NULL) Log(__FUNCTION__,"ERROR","Unable to init spline");
    if (errSpline_==NULL and ge_ ==NULL) Log(__FUNCTION__,"ERROR","Unable to init Error Spline");
    // are G and GE sorted ?
    if (spline_==NULL){
            spline_=new TMVA::TSpline1( (label+"Spline").c_str(),g_);
            spline_->SetName( (label+"spline").c_str());
    }
    if (errSpline_==NULL){
        errSpline_=new TMVA::TSpline1( (label+"errSpline").c_str(),ge_); 
        errSpline_->SetName( (label+"errSpline").c_str());
    }

}

void SF_PtSpline::init(string filename, string obj,string obj2)
{
#ifdef VERBOSE
    if(VERBOSE>1)Log(__FUNCTION__,"DEBUG"," --- ");
#endif
    clear();
    TFile *f = TFile::Open(filename.c_str()); // TODO: make it able to read TGraph too
    if (f->Get(obj.c_str())->InheritsFrom("TSpline") ) spline_= (TSpline*)f->Get(obj.c_str()) -> Clone( (obj + "clone").c_str() );
    else if ( f->Get(obj.c_str())->InheritsFrom("TGraph") ) 
            { 
                g_=(TGraph*)f->Get(obj.c_str())->Clone();
            }
    else Log(__FUNCTION__,"ERROR","Type not supported for obj "+ obj + " in " + filename);

    if (f->Get(obj2.c_str())->InheritsFrom("TSpline") ) errSpline_= (TSpline*)f->Get(obj2.c_str()) -> Clone( (obj2 + "clone").c_str() );
    else if (f->Get(obj2.c_str())->InheritsFrom("TGraph") )
            {
                ge_=(TGraph*)f->Get(obj.c_str())->Clone();
            }
    else Log(__FUNCTION__,"ERROR","Type not supported for obj "+ obj2 + " in " + filename);
    f->Close();
}

void SF_PtSpline::add(double pt, double sf, double err)
{
#ifdef VERBOSE
    if(VERBOSE>1)Log(__FUNCTION__,"DEBUG"," --- ");
#endif
    if (g_==NULL) {g_ = new TGraph();}
    if (ge_==NULL) {ge_ = new TGraph();}
    g_->SetPoint( g_ ->GetN(), pt , sf );
    ge_->SetPoint( ge_->GetN(), pt, err);

}

void SF_PtSpline::print() const{
    cout <<" ------ SF Pt Spline --------"<<endl;
    cout <<"label="<<label<<endl;
    if(spline_)spline_->Print("V");
    if(errSpline_)errSpline_->Print("V");
    cout <<"---------------"<<endl;
}

// -----------------------CSV-------------------------- 
//
void SF_CSV::init(string filename)
{
    //---
    Log(__FUNCTION__,"INFO",string("Loading '") +filename+"' in SF CSV " + label );
    //string meas="incl";
    //    string measB="mujets";
    string measB="comb";
    string measL="incl";
    calib=new BTagCalibration("CSVv2",filename);
    readerL=new BTagCalibrationReader( BTagEntry::OP_LOOSE,  // operating point 
                    "central"           // systematics type
                     );
    readerL->load( *calib, BTagEntry::FLAV_B,measB); readerL->load( *calib, BTagEntry::FLAV_C,measB); readerL->load( *calib, BTagEntry::FLAV_UDSG,measL);

    readerL_up=new BTagCalibrationReader( BTagEntry::OP_LOOSE,"up" );
    readerL_down=new BTagCalibrationReader( BTagEntry::OP_LOOSE, "down" );

    readerL_up->load( *calib, BTagEntry::FLAV_B,measB); readerL_up->load( *calib, BTagEntry::FLAV_C,measB); readerL_up->load( *calib, BTagEntry::FLAV_UDSG,measL);
    readerL_down->load( *calib, BTagEntry::FLAV_B,measB); readerL_down->load( *calib, BTagEntry::FLAV_C,measB); readerL_down->load( *calib, BTagEntry::FLAV_UDSG,measL);

    readerM=new BTagCalibrationReader( BTagEntry::OP_MEDIUM, "central" );
    readerM_up=new BTagCalibrationReader(  BTagEntry::OP_MEDIUM,  "up" );
    readerM_down=new BTagCalibrationReader(  BTagEntry::OP_MEDIUM,  "down" );

    readerM->load( *calib, BTagEntry::FLAV_B,measB); readerM->load( *calib, BTagEntry::FLAV_C,measB); readerM->load( *calib, BTagEntry::FLAV_UDSG,measL);
    readerM_up->load( *calib, BTagEntry::FLAV_B,measB); readerM_up->load( *calib, BTagEntry::FLAV_C,measB); readerM_up->load( *calib, BTagEntry::FLAV_UDSG,measL);
    readerM_down->load( *calib, BTagEntry::FLAV_B,measB); readerM_down->load( *calib, BTagEntry::FLAV_C,measB); readerM_down->load( *calib, BTagEntry::FLAV_UDSG,measL);

    readerT=new BTagCalibrationReader( BTagEntry::OP_TIGHT, "central" );
    readerT_up=new BTagCalibrationReader( BTagEntry::OP_TIGHT,  "up" );
    readerT_down=new BTagCalibrationReader( BTagEntry::OP_TIGHT,  "down" );

    readerT->load( *calib, BTagEntry::FLAV_B,measB); readerT->load( *calib, BTagEntry::FLAV_C,measB); readerT->load( *calib, BTagEntry::FLAV_UDSG,measL);
    readerT_up->load( *calib, BTagEntry::FLAV_B,measB); readerT_up->load( *calib, BTagEntry::FLAV_C,measB); readerT_up->load( *calib, BTagEntry::FLAV_UDSG,measL);
    readerT_down->load( *calib, BTagEntry::FLAV_B,measB); readerT_down->load( *calib, BTagEntry::FLAV_C,measB); readerT_down->load( *calib, BTagEntry::FLAV_UDSG,measL);

    return;
}

void SF_CSV::set( float pt, float eta)
{
   set(pt,eta,cached_wp,cached_flavor); 
}

void SF_CSV::setWP(int wp){
   set(cached_pt,cached_eta,wp,cached_flavor); 
}

void SF_CSV::setJetFlavor(int flavor){
   set(cached_pt,cached_eta,cached_wp,flavor); 
}


void SF_CSV::set( float pt, float eta, int wp, int flavor)
{
    // -- cache --
    cached_pt=pt;
    cached_eta=eta;
    cached_wp=wp;
    cached_flavor=flavor;

    // change flavor to be easy to parse later
    flavor = abs(flavor);
    if (flavor == 21 ) flavor = 0;
    if (flavor <4) flavor = 0;
    
    bool isLJet = (flavor == 0 );
    bool isBJet = (flavor == 5 );
    bool isCJet = (flavor == 4 );
    float scaleSyst = 1.0;

    auto BTEFlav = BTagEntry::FLAV_B;
    if (isCJet) BTEFlav = BTagEntry::FLAV_C;
    if (isLJet) BTEFlav = BTagEntry::FLAV_UDSG;
    
    if ( (isBJet or isCJet) and pt> MaxBJetPt ) { pt=MaxBJetPt; scaleSyst=2.0;}
    if (isLJet and pt> MaxLJetPt ) { pt=MaxLJetPt; scaleSyst=2.0;}
    if ( (isBJet or isCJet) and pt< MinBJetPt ) { pt=MinBJetPt; scaleSyst=2.0;}
    if (isLJet and pt< MinLJetPt ) { pt=MinLJetPt; scaleSyst=2.0;}
    
    BTagCalibrationReader *nominal{0};
    BTagCalibrationReader *up{0};
    BTagCalibrationReader *down{0};

    if (wp ==0)
    {
        nominal = readerL; up = readerL_up ; down=readerL_down;
    }
    else if (wp ==1)
    {
        nominal = readerM; up = readerM_up ; down=readerM_down;
    }
    else if (wp ==2)
    {
        nominal = readerT; up = readerT_up ; down=readerT_down;
    }
    else { Log(__FUNCTION__,"ERROR","Unsupported WP"); throw abortException() ;}

    sf=nominal->eval(BTEFlav, eta, pt);

    if (simpleError){
        errUp=(up->eval(BTEFlav, eta, pt) -sf ) * scaleSyst;
        errDown=(sf - down->eval(BTEFlav, eta, pt) ) * scaleSyst;
    }
    else // independent errors for L and B/C
    {
        double active=1.0;

        if (BTEFlav == BTagEntry::FLAV_B or BTEFlav== BTagEntry::FLAV_C and systB==0) active=0.0;
        if (BTEFlav == BTagEntry::FLAV_UDSG and systL==0) active=0.0;


        errUp=(up->eval(BTEFlav, eta, pt) -sf ) * scaleSyst * active ;
        errDown=(sf - down->eval(BTEFlav, eta, pt) ) * scaleSyst *active ;
    }

    return;
}

double SF_CSV::get() { 
    if (simpleError) return SF_Asymm::get() ;

    if (systL==0  and systB==0) return sf; 
    if (systL !=0  and systB !=0 ) Log(__FUNCTION__,"ERROR","SystL and SystB cannot be both !=0 simultaneously");
    if (systL>0 ) return sf + errUp *systL  ; 
    if (systL<0) return sf + errDown * systL ;
    if (systB>0 ) return sf + errUp *systB  ; 
    if (systB<0) return sf + errDown * systB ;

    return 0.; 
}



// ------------ TF1 -----------------

void SF_TF1::set(double pt)
{
    sf = f_sf_->Eval(pt);
    if(f_err_) err = f_err_->Eval(pt);
    else err=0;

    //if (get()< 1.e-5)
    //    Log(__FUNCTION__,"DEBUG",Form("Very little scale factor: pt=%.5f sf=%g",pt,get()));
}

void SF_TF1::print() const{
    SF::print();
    if (f_sf_)Log(__FUNCTION__,"INFO",f_sf_->GetTitle());
    else Log(__FUNCTION__,"INFO","TF1 not set");
    Log(__FUNCTION__,"INFO","------------------------------");
}

// -----
void SF_TF2::set(double x,double y)
{
    sf = f_sf_->Eval(x,y);
    if(f_err_) err = f_err_->Eval(x,y);
    else err=0;
}

void SF_TF2::print() const{
    SF::print();
    if (f_sf_)Log(__FUNCTION__,"INFO",f_sf_->GetTitle());
    else Log(__FUNCTION__,"INFO","TF2 not set");
    Log(__FUNCTION__,"INFO","------------------------------");
}

#include "interface/ggF_qcd_uncertainty_2017.hpp"

void SF_WG1::print() const{
    SF::print();
    Log(__FUNCTION__,"INFO",Form("nuisance=%d",nuisance));
    Log(__FUNCTION__,"INFO",Form("pTH=%.3lf",pTH));
    Log(__FUNCTION__,"INFO",Form("STXS_Stage1=%d",STXS_Stage1));
    Log(__FUNCTION__,"INFO",Form("type=%d",int(type)));
    Log(__FUNCTION__,"INFO","------------------------------");
}

double SF_WG1::get() {
    if (nuisance == 0) {
        return 1.0;
    }
    if (syst == 0 ) {
        return 1.0;
    }
    if (type == THU_WG1){
        if (syst >0) return LHCHXSWG::qcd_ggF_uncertSF_wg1(Njets30,pTH,STXS_Stage1,1.0)[nuisance-1];
        else return LHCHXSWG::qcd_ggF_uncertSF_wg1(Njets30,pTH,STXS_Stage1,-1.0)[nuisance-1];
    }
    else if (type == THU_2017){
        if (syst >0) return LHCHXSWG::qcd_ggF_uncertSF_2017(Njets30,pTH,STXS_Stage1,1.0)[nuisance-1];
        else return LHCHXSWG::qcd_ggF_uncertSF_2017(Njets30,pTH,STXS_Stage1,-1.0)[nuisance-1];
    }
}

// -------------------- NNLO PS Reweight -------------------------
void SF_NNLOPS_Reweight::init(const string & fname)
{
    TFile *fIn = TFile::Open(fname.c_str());
    if (fIn == NULL) Log(__FUNCTION__,"ERROR","Unable to open file:"+fname);

    for(unsigned nj = 0 ; nj<=3; ++nj)
    {
        w.push_back( (TGraph*)fIn->Get(Form("gr_NNLOPSratio_pt_mcatnlo_%djet",nj))->Clone() );
    }
}
double SF_NNLOPS_Reweight::get()
{
    if (syst !=0 ){
        Log(__FUNCTION__,"ERROR","Systematics shifts not implemented for NNLOPS Reweight");
        throw abortException() ;
    }
    double weight{1};

    if (njets < 0) return 1.0;
    switch (njets){
    case 0: return w[0] -> Eval(min(Hpt,125.0)); break;
    case 1: return w[1] -> Eval(min(Hpt,625.0)); break;
    case 2: return w[2] -> Eval(min(Hpt,800.0)); break;
    default: return w[3] ->Eval(min(Hpt,925.0)); break;
    }
    return 1.0;
}
void SF_NNLOPS_Reweight::print() const
{
    SF::print();
    Log(__FUNCTION__,"INFO",Form("wsize=%u",w.size()));
    Log(__FUNCTION__,"INFO",Form("njets=%d",njets));
    Log(__FUNCTION__,"INFO",Form("Hpt=%f",Hpt));
    Log(__FUNCTION__,"INFO","------------------------------");
}
// ---------------------------------------------------------------

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
