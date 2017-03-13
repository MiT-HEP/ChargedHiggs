#include "interface/SF_CSVReweight.hpp"

void SF_CSVReweight::init(){
	helper_ . reset ( new CSVHelper() ) ;	
	helper_ ->init("aux/CSV/csv_rwt_fit_hf_v2_final_2017_1_10test.root","aux/CSV/csv_rwt_fit_lf_v2_final_2017_1_10test.root",5); //??
    Log(__FUNCTION__,"INFO","Init CSV Reweight with aux/CSV/csv_rwt_fit_hf_v2_final_2017_1_10test.root and aux/CSV/csv_rwt_fit_lf_v2_final_2017_1_10test.root and 5" );
    err= 1.0;
}

void SF_CSVReweight::set(){
    throw 2; // UPDATE to use the helper and set the right things to call it
    sf = 1.0;
    errUp= 1.0;
    errDown= 1.0;
}
//  double getCSVWeight(const std::vector<double>& jetPts,
//		      const std::vector<double>& jetEtas,
//		      const std::vector<double>& jetCSVs,
//		      const std::vector<int>& jetFlavors,
//		      const int iSys,
//		      double &csvWgtHF,
//		      double &csvWgtLF,
//		      double &csvWgtCF) const;





// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
