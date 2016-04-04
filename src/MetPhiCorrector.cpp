#include "interface/MetPhiCorrector.hpp"
#include <iostream>
using namespace std;

#define VERBOSE 1

void MetPhiCorrector::getFunction(TFile *f, TF1* &func, const string& name)
{
    func = (TF1*)f->Get(name.c_str()) -> Clone();
    if (func == NULL ) cout <<"[MetPhiCorrector]::[getFunction]::[ERROR] Unable to get "<<name<<" from "<<fileName<<endl;
    return;
}

void MetPhiCorrector::Init(){
    isInit_=false;
    #ifdef VERBOSE
    if (VERBOSE >0 )  cout <<"[MetPhiCorrector]::[Init]::[INFO] Opening file "<<fileName<<endl;
    #endif 
    TFile * f = TFile::Open(fileName.c_str()); 

    if (f == NULL) cout<<"[MetPhiCorrector]::[Init]::[ERROR] Unable to open file "<<fileName<<endl;

    getFunction( f, metPhiPxMC , "fx_mc" ) ;
    getFunction( f, metPhiPyMC , "fy_mc" ) ;
    getFunction( f, metPhiPxDATA , "fx_data" ) ;
    getFunction( f, metPhiPyDATA , "fy_data" ) ;

    #ifdef VERBOSE
    if (VERBOSE >0 )  cout <<"[MetPhiCorrector]::[Init]::[INFO]Closing file "<<endl;
    #endif 
    f->Close();
   
    if ( metPhiPxMC and metPhiPyMC and metPhiPxDATA and metPhiPyDATA) 
        isInit_=true;

    return ;
}

int MetPhiCorrector::correct(Event *e)
{
    if (not isInit_) Init();

    if( not isInit_ ) return 1;

    //GetMet(); // I need write access to met
    float px = GetMet(e).GetP4().Px();
    float py = GetMet(e).GetP4().Py();
    
    if ( e->IsRealData() )
    {
        px -= metPhiPxDATA -> Eval( e->Npv() ) ;
        py -= metPhiPyDATA -> Eval( e->Npv() ) ;
    }
    else {
        px -= metPhiPxMC -> Eval( e->Npv() ) ;
        py -= metPhiPyMC -> Eval( e->Npv() ) ;
    }
    TLorentzVector newmet(px,py,0,0);
    //GetMet(e).GetP4().SetXYZT( px,py,0,0);
    //
#ifdef DEBUG
    if(DEBUG>0)cout <<"[MetPhiCorrector]::[correct] Met was "<<e->GetMet().Pt() <<" UNCOR="<<e->GetMet().PtUncorr()<<endl;
#endif
   
    SetP4(GetMet(e), newmet);

#ifdef DEBUG
    if(DEBUG>0)cout <<"[MetPhiCorrector]::[correct] Met is "<<e->GetMet().Pt() <<" UNCOR="<<e->GetMet().PtUncorr()<<endl;
#endif
    return 0;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
