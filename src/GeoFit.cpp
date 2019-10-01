#include <math.h>

namespace PtGeoCor{

  float PtGeoFit(float d0, float pt, float eta, int year) { 
    float pt_cor = 0.0;
    if (year == 2016) {   // for 2016 it is applied to KaMu pt
      if      (abs(eta) < 0.9) pt_cor = ( 1.116 * tanh(679.40*d0) + 171.84 * d0 - 0.2034 ) * pt * pt / 10000.0;
      else if (abs(eta) < 1.7) pt_cor = ( 1.294 * tanh(646.23*d0) + 390.31 * d0 - 0.3462 ) * pt * pt / 10000.0;
      else                     pt_cor = ( 0.851 * tanh(595.69*d0) + 925.38 * d0 - 0.7122 ) * pt * pt / 10000.0;
    }
    else if (year == 2017) {
    }
    else if (year == 2018) {  // for 2018 it is applied to Roch pt
      if      (abs(eta) < 0.9) pt_cor = ( 1.036 * tanh(660.86*d0) + 275.22 * d0 - 0.1305 ) * pt * pt / 10000.0;
      else if (abs(eta) < 1.7) pt_cor = ( 1.385 * tanh(520.61*d0) + 494.80 * d0 - 0.2535 ) * pt * pt / 10000.0;
      else                     pt_cor = ( 0.784 * tanh(772.87*d0) + 1122.9 * d0 - 0.4349 ) * pt * pt / 10000.0;
    }
    return pt_cor;
  } // end of float PtGeoFit(float d0, float pt, float eta, int year)

  float PtGeoFit_mod(float d0, float pt, float eta, int year) { 
    float pt_cor = 0.0;
    if (year == 2016) {  // for 2016 it is applied to KaMu pt
      if      (abs(eta) < 0.9) pt_cor = ( 1.116 * tanh(679.40*d0) + 171.84 * d0 ) * pt * pt / 10000.0;
      else if (abs(eta) < 1.7) pt_cor = ( 1.294 * tanh(646.23*d0) + 390.31 * d0 ) * pt * pt / 10000.0;
      else                     pt_cor = ( 0.851 * tanh(595.69*d0) + 925.38 * d0 ) * pt * pt / 10000.0;
    }
    else if (year == 2017) {
    }
    else if (year == 2018) {  // for 2018 it is applied to Roch pt
      if      (abs(eta) < 0.9) pt_cor = ( 1.036 * tanh(660.86*d0) + 275.22 * d0 ) * pt * pt / 10000.0;
      else if (abs(eta) < 1.7) pt_cor = ( 1.385 * tanh(520.61*d0) + 494.80 * d0 ) * pt * pt / 10000.0;
      else                     pt_cor = ( 0.784 * tanh(772.87*d0) + 1122.9 * d0 ) * pt * pt / 10000.0;
    }
    return pt_cor;
  } // end of float PtGeoFit_mod(float d0, float pt, float eta, int year)

} // end namespace PtGeoCor

