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

void SF_TF1::print(){
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

void SF_TF2::print(){
    SF::print();
    if (f_sf_)Log(__FUNCTION__,"INFO",f_sf_->GetTitle());
    else Log(__FUNCTION__,"INFO","TF2 not set");
    Log(__FUNCTION__,"INFO","------------------------------");
}

// ---------------- PT ETA TIME
void SF_PtEtaTime::add(double pt1, double pt2,double eta1, double eta2,long run1,long run2, long lumi1,long lumi2, double sf, double err)
{
    range r;
    r.pt1 = pt1;
    r.pt2 = pt2;
    r.eta1 = eta1;
    r.eta2 = eta2;
    r.run1= run1;
    r.run2= run2;
    r.lumi1= lumi1;
    r.lumi2= lumi2;

    store[r].first = sf;
    store[r].second = err;

#ifdef VERBOSE
    if(VERBOSE>0)cout <<"[SF_PtEtaTime]::[add]::[DEBUG] Adding SF Pt Eta:"<<pt1<<":"<<pt2<<"|"<<eta1<<":"<<eta2<<":"<<run1<<":"<<run2<<":"<<lumi1<<":"<<lumi2<<"|"<<sf<<":"<<err<<endl;
#endif
}

void SF_PtEtaTime::set( double pt, double eta, long run ,long lumi )
{
    int change = 0;
    for(auto s : store)
    {
        if ( pt < s.first.pt1 ) continue;
        if ( s.first.pt2  <= pt ) continue;
        if ( eta < s.first.eta1 ) continue;
        if ( s.first.eta2  <= eta ) continue;
        // run range selection is [run1,run2]
        if ( s.first.run2 >=0 and run > s.first.run2  ) continue;
        if ( s.first.run1 >=0 and run < s.first.run1  ) continue;
        // lumi range selection is [lumi1,lumi2], these checks needs to be made only if in the run boundaries
        if ( s.first.run1 == run and s.first.lumi1 >=0 and lumi < s.first.lumi1  ) continue;
        if ( s.first.run2 == run and s.first.lumi2 >=0 and lumi > s.first.lumi2  ) continue;
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

const bool operator<( const SF_PtEtaTime::range&r1 , const SF_PtEtaTime::range &r2)
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
    // -- run1
    if (r1.run1 < r2.run1) return true;
    if (r1.run1 > r2.run1) return false;
    // -- run2
    if (r1.run2 < r2.run2) return true;
    if (r1.run2 > r2.run2) return false;
    // -- lumi1
    if (r1.lumi1 < r2.lumi1) return true;
    if (r1.lumi1 > r2.lumi1) return false;
    // -- lumi2
    if (r1.lumi2 < r2.lumi2) return true;
    if (r1.lumi2 > r2.lumi2) return false;
    // they are equal
    return false;
}

void SF_PtEtaTime::print(){
    cout <<" ----- SF Pt Eta ------"<<endl;
    cout <<"label='"<<label<<"'"<<endl;
    for(auto& p : store ) 
    {
    cout << p.first.pt1<<":"<<p.first.pt2<<"|"<<p.first.eta1<<":"<<p.first.eta2<<"|"<<p.first.run1<<":"<<p.first.run2<<"|"<<p.first.lumi1<<":"<<p.first.lumi2<<"||"<< p.second.first<<":"<<p.second.second<<endl;
    }
    cout <<" ----------------------"<<endl;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
