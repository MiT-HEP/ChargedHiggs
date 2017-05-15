#include "interface/SF_CSVReweight.hpp"

void SF_CSVReweight::init(){
	helper_ . reset ( new CSVHelper() ) ;	
	helper_ ->init("aux/CSV/csv_rwt_fit_hf_v2_final_2017_1_10test.root","aux/CSV/csv_rwt_fit_lf_v2_final_2017_1_10test.root",5); //??
    Log(__FUNCTION__,"INFO","Init CSV Reweight with aux/CSV/csv_rwt_fit_hf_v2_final_2017_1_10test.root and aux/CSV/csv_rwt_fit_lf_v2_final_2017_1_10test.root and 5" );
    err= 1.0;
}

void SF_CSVReweight::set(){
    //throw 2; // UPDATE to use the helper and set the right things to call it
    //
    //syst= 7 -> JES UP, 8 JES Down, 9 LF UP 10,11/12/HF UP/Down, LF Down, 13/14 Stat1, 15,16 Stat2
    sf = helper_->getCSVWeight(pt_,eta_,csv_,flavor_,syst,csvWgtHF,csvWgtLF,csvWgtCF);
    errUp= 0.0; // TODO
    errDown= 0.0;
}

void SF_CSVReweight::add_to_sf(float pt, float eta, float csv, int flavor)
{
    pt_.push_back( pt );
    eta_.push_back( eta );
    csv_.push_back( csv );
    flavor_.push_back( flavor );
}



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
