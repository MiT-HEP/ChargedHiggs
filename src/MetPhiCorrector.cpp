#include "interface/MetPhiCorrector.hpp"
#include <iostream>
using namespace std;

#define VERBOSE 1

void MetPhiCorrector::Init(){
    return ;
}

// 2016-2017-2018
//The correction is applied by subtracting from the px or py value, 
//respectively.
//
//For instance:
//  xyCorrMet.SetPx(METsT1XYCor.px() - metXcorr);
//  xyCorrMet.SetPy(METsT1XYCor.py() - metYcorr);
double MetPhiCorrector::MetPhiCorr(int Era, int npv, bool  runOnData, int metCoord)
{
   double corr = 0.0;

   if (Era == 2016 && runOnData == true && metCoord == 0)
   {
     corr = 1.01096e+00 + 1.42312e-01 * npv;
   }
   else if (Era == 2016 && runOnData == true && metCoord == 1)
   {
     corr = 2.89820e-01 + 4.64643e-02 * npv;
   }
   else if (Era == 2016 && runOnData == false && metCoord == 0)
   {
     corr = -1.11824e-01 - 2.19837e-01 * npv;
   }
   else if (Era == 2016 && runOnData == false && metCoord == 1)
   {
     corr = 8.03025e-01 - 3.89610e-02 * npv;
   }
   else if (Era == 2017 && runOnData == true && metCoord == 0)
   {
     corr = 3.15705e-01 - 1.77717e-01 * npv;
   }
   else if (Era == 2017 && runOnData == true && metCoord == 1)
   {
     corr = 4.48257e-01 + 2.36009e-01 * npv;
   }
   else if (Era == 2017 && runOnData == false && metCoord == 0)
   {
     corr = 3.61535e-01 - 2.30149e-01 * npv;
   }
   else if (Era == 2017 && runOnData == false && metCoord == 1)
   {
     corr = -1.96483e-01 + 1.81803e-01 * npv;
   }
   else if (Era == 2018 && runOnData == true && metCoord == 0)
   {
     corr = 1.09883e+00 + 4.23750e-01 * npv;
   }
   else if (Era == 2018 && runOnData == true && metCoord == 1)
   {
     corr = -1.14832e+00 + 1.14040e-01 * npv;
   }
   else if (Era == 2018 && runOnData == false && metCoord == 0)
   {
     corr = -1.60410e-01 + 3.41247e-01 * npv;
   }
   else if (Era == 2018 && runOnData == false && metCoord == 1)
   {
     corr = 2.05019e-01 + 1.37506e-01 * npv;
   }
   else
   {
     printf("WRONG CHOICE in METphiCorr!\n");
   }

   return corr;
}

int MetPhiCorrector::correct(Event *e)
{
    //GetMet(); // I need write access to met
    
    float px = GetMet(e).GetP4().Px();
    float py = GetMet(e).GetP4().Py();

    float corrx=0.,corry=0.;
    
    corrx= MetPhiCorr(year,e->Npv(),e->IsRealData(),0);
    corry= MetPhiCorr(year,e->Npv(),e->IsRealData(),1);

#ifdef DEBUG
    if(DEBUG>0)cout <<"[MetPhiCorrector]::[correct] Met was "<<e->GetMet().Pt() <<" UNCOR="<<e->GetMet().PtUncorr()<<endl;
#endif
   
    SetPx(GetMet(e),px-corrx);
    SetPy(GetMet(e),py-corry);

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
