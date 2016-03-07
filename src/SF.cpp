#include "interface/SF.hpp"
#include "interface/Handlers.hpp"
#include "TFile.h"
#include "TH2F.h"
#define VERBOSE 2

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

void SF_PtEta::print(){
    cout <<" ----- SF Pt Eta ------"<<endl;
    cout <<"label='"<<label<<"'"<<endl;
    for(auto& p : store ) 
    {
    cout << p.first.pt1<<":"<<p.first.pt2<<"|"<<p.first.eta1<<":"<<p.first.eta2<<"||"<< p.second.first<<":"<<p.second.second<<endl;
    }
    cout <<" ----------------------"<<endl;
}
// --- TH1F 

void SF_TH2F::init(string filename,string histname)
{
    TFile *f = TFile::Open(filename.c_str() ) ;
    if (f == NULL){
        Log(__FUNCTION__,"ERROR","file '" + filename + "' does not exist");
        throw abort ;
    }
    TH2F *h=(TH2F*)f->Get(histname.c_str());
    if (h==NULL){
        Log(__FUNCTION__,"ERROR","h '"+histname+"' does not exist");
        throw abort ;
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
        if (ptbin == h->GetNbinsY() ) ptmax = 8000.; // highest is open, current recommendation
        add(ptmin,ptmax,aetamin,aetamax,sf,err);
    }
}

// ------------- SPLINE -------------
//
#include "TFile.h"
void SF_PtSpline::set( double pt){
    sf = spline_ -> Eval(pt);
    err = errSpline_ -> Eval(pt);
    return;
}

void SF_PtSpline::init(){
    if (spline_==NULL){spline_=new TSpline3();spline_->SetName( (label+"spline").c_str());}
    if (errSpline_==NULL){errSpline_=new TSpline3(); errSpline_->SetName( (label+"errSpline").c_str());}

}

void SF_PtSpline::init(string filename, string obj,string obj2)
{
    clear();
    TFile *f = TFile::Open(filename.c_str());
    spline_= (TSpline3*)f->Get(obj.c_str()) -> Clone( (obj + "clone").c_str() );
    errSpline_= (TSpline3*)f->Get(obj2.c_str()) -> Clone( (obj2 + "clone").c_str() );
    f->Close();
}

void SF_PtSpline::add(double pt, double sf, double err)
{
    init();
    spline_->SetPoint( spline_ ->GetNp(), pt , sf );
    errSpline_->SetPoint(errSpline_->GetNp(), pt, err);

}

void SF_PtSpline::print(){
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
    string meas="incl";
    calib=new BTagCalibration("CSVv2",filename);
    readerL=new BTagCalibrationReader( BTagEntry::OP_LOOSE,  // operating point 
                    "central"           // systematics type
                     );
    readerL->load( *calib, BTagEntry::FLAV_B,meas); readerL->load( *calib, BTagEntry::FLAV_C,meas); readerL->load( *calib, BTagEntry::FLAV_UDSG,meas);

    readerL_up=new BTagCalibrationReader( BTagEntry::OP_LOOSE,"up" );
    readerL_down=new BTagCalibrationReader( BTagEntry::OP_LOOSE, "down" );

    readerL_up->load( *calib, BTagEntry::FLAV_B,meas); readerL_up->load( *calib, BTagEntry::FLAV_C,meas); readerL_up->load( *calib, BTagEntry::FLAV_UDSG,meas);
    readerL_down->load( *calib, BTagEntry::FLAV_B,meas); readerL_down->load( *calib, BTagEntry::FLAV_C,meas); readerL_down->load( *calib, BTagEntry::FLAV_UDSG,meas);

    readerM=new BTagCalibrationReader( BTagEntry::OP_MEDIUM, "central" );
    readerM_up=new BTagCalibrationReader(  BTagEntry::OP_MEDIUM,  "up" );
    readerM_down=new BTagCalibrationReader(  BTagEntry::OP_MEDIUM,  "down" );

    readerM->load( *calib, BTagEntry::FLAV_B,meas); readerM->load( *calib, BTagEntry::FLAV_C,meas); readerM->load( *calib, BTagEntry::FLAV_UDSG,meas);
    readerM_up->load( *calib, BTagEntry::FLAV_B,meas); readerM_up->load( *calib, BTagEntry::FLAV_C,meas); readerM_up->load( *calib, BTagEntry::FLAV_UDSG,meas);
    readerM_down->load( *calib, BTagEntry::FLAV_B,meas); readerM_down->load( *calib, BTagEntry::FLAV_C,meas); readerM_down->load( *calib, BTagEntry::FLAV_UDSG,meas);

    readerT=new BTagCalibrationReader( BTagEntry::OP_TIGHT, "central" );
    readerT_up=new BTagCalibrationReader( BTagEntry::OP_TIGHT,  "up" );
    readerT_down=new BTagCalibrationReader( BTagEntry::OP_TIGHT,  "down" );

    readerT->load( *calib, BTagEntry::FLAV_B,meas); readerT->load( *calib, BTagEntry::FLAV_C,meas); readerT->load( *calib, BTagEntry::FLAV_UDSG,meas);
    readerT_up->load( *calib, BTagEntry::FLAV_B,meas); readerT_up->load( *calib, BTagEntry::FLAV_C,meas); readerT_up->load( *calib, BTagEntry::FLAV_UDSG,meas);
    readerT_down->load( *calib, BTagEntry::FLAV_B,meas); readerT_down->load( *calib, BTagEntry::FLAV_C,meas); readerT_down->load( *calib, BTagEntry::FLAV_UDSG,meas);

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
    else { Log(__FUNCTION__,"ERROR","Unsupported WP"); throw abort ;}

    sf=nominal->eval(BTEFlav, eta, pt);
    errUp=(up->eval(BTEFlav, eta, pt) -sf ) * scaleSyst;
    errDown=(sf - down->eval(BTEFlav, eta, pt) ) * scaleSyst;

    return;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
