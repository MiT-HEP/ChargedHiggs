// Authors:
//    - Claudia Bertella
//    - Yacine Haddad
//
// email:  yhaddad@cern.ch
// modified by: 
//    - Andrea Carlo Marini
//     Wed Sep 25 18:08:25 CEST 2019

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <cmath>

#include "interface/qq2Hqq_uncert_scheme.hpp"

// STXS bin definition can be found in the HXSWG gitlab respository:
// https://gitlab.cern.ch/LHCHIGGSXS/LHCHXSWG2/STXS/blob/master/HiggsTemplateCrossSections.h

// bin acceptances extracted from POWHEG VBFH (NLO) + PYTHIA8 for showering
// note that the dipoleShower option in pythia8 is turned on 

static std::map<int, std::vector<double> > stxs_acc =
  {//STXS   TOT   ,  PTH200,  Mjj60 , Mjj120 , Mjj350 , Mjj700 ,Mjj1000 ,Mjj1500  ,  25       , JET01
   { 200 , {0.07  ,  0     , 0      , 0      , 0      , 0      , 0      , 0       ,        0  ,    0   }},
   { 201 , {0.0744,  0     , 0      , 0      , 0      , 0      , 0      , 0       ,        0  ,-0.1649 }}, // Jet0
   { 202 , {0.3367,  0     , 0      , 0      , 0      , 0      , 0      , 0       ,        0  ,-0.7464 }}, // Jet1
   { 203 , {0.0092,  0     ,-0.6571 , 0      , 0      , 0      , 0      , 0       ,   -0.0567 ,  0.0178}}, // Mjj 0-60,      PTHjj 0-25
   { 204 , {0.0143,  0     , 0.0282 ,-0.5951 , 0      , 0      , 0      , 0       ,   -0.0876 ,  0.0275}}, // Mjj 60-120,    PTHjj 0-25
   { 205 , {0.0455,  0     , 0.0902 , 0.0946 ,-0.3791 , 0      , 0      , 0       ,   -0.2799 ,  0.0877}}, // Mjj 120-350,   PTHjj 0-25
   { 206 , {0.0048,  0     ,-0.3429 , 0      , 0      , 0      , 0      , 0       ,   +0.0567 ,  0.0093}}, // Mjj 0-60,      PTHjj 25-inf
   { 207 , {0.0097,  0     , 0.0192 ,-0.4049 , 0      , 0      , 0      , 0       ,   +0.0876 ,  0.0187}}, // Mjj 60-120,    PTHjj 25-inf
   { 208 , {0.0746,  0     , 0.1477 , 0.0155 ,-0.6209 , 0      , 0      , 0       ,   +0.2799 ,  0.1437}}, // Mjj 120-350,   PTHjj 25-inf
   { 209 , {0.0375, 0.1166 , 0.0743 , 0.078  , 0.1039 ,-0.2757 , 0      , 0       ,   -0.2306 ,  0.0723}}, // Mjj 350-700,   PTHjj 0-25    , pTH 0-200
   { 210 , {0.0985, 0.3062 , 0.1951 , 0.2048 , 0.273  ,-0.7243 , 0      , 0       ,   +0.2306 ,  0.1898}}, // Mjj 350-700,   PTHjj 25-inf  , pTH 0-200
   { 211 , {0.0166, 0.0515 , 0.0328 , 0.0345 , 0.0459 , 0.0773 ,-0.2473 , 0       ,   -0.1019 ,  0.0319}}, // Mjj 700-1000,  PTHjj 0-25    , pTH 0-200
   { 212 , {0.0504, 0.1568 , 0.0999 , 0.1049 , 0.1398 , 0.2353 ,-0.7527 , 0       ,   +0.1019 ,  0.0972}}, // Mjj 700-1000,  PTHjj 25-inf  , pTH 0-200
   { 213 , {0.0137, 0.0426 , 0.0271 , 0.0285 , 0.0379 , 0.0639 , 0.0982 ,-0.2274  ,   -0.0842 ,  0.0264}}, // Mjj 1000-1500, PTHjj 0-25    , pTH 0-200
   { 214 , {0.0465, 0.1446 , 0.0922 , 0.0967 , 0.1289 , 0.2171 , 0.3335 ,-0.7726  ,   +0.0842 ,  0.0897}}, // Mjj 1000-1500, PTHjj 25-inf  , pTH 0-200
   { 215 , {0.0105, 0.0327 , 0.0208 , 0.0219 , 0.0291 , 0.0491 , 0.0754 , 0.1498  ,   -0.0647 ,  0.0203}}, // Mjj 1500-inf , PTHjj 0-25    , pTH 0-200
   { 216 , {0.048 , 0.1491 , 0.095  , 0.0998 , 0.133  , 0.2239 , 0.344  , 0.6836  ,   +0.0647 ,  0.0925}}, // Mjj 1500-inf , PTHjj 25-inf  , pTH 0-200
   { 217 , {0.0051,-0.1304 , 0.0101 , 0.0106 , 0.0141 , 0.0238 , 0.0366 , 0.0727  ,   -0.0314 ,  0.0098}}, // Mjj 350-700,   PTHjj 0-25    , pTH 200-inf
   { 218 , {0.0054,-0.1378 , 0.0107 , 0.0112 , 0.0149 , 0.0251 , 0.0386 , 0.0768  ,   +0.0314 ,  0.0104}}, // Mjj 350-700,   PTHjj 25-inf  , pTH 200-inf
   { 219 , {0.0032,-0.0816 , 0.0063 , 0.0066 , 0.0088 , 0.0149 , 0.0229 , 0.0455  ,   -0.0196 ,  0.0062}}, // Mjj 700-1000,  PTHjj 0-25    , pTH 200-inf
   { 220 , {0.0047,-0.1190 , 0.0092 , 0.0097 , 0.0129 , 0.0217 , 0.0334 , 0.0663  ,   +0.0196 ,  0.0090}}, // Mjj 700-1000,  PTHjj 25-inf  , pTH 200-inf
   { 221 , {0.0034,-0.0881 , 0.0068 , 0.0072 , 0.0096 , 0.0161 , 0.0247 , 0.0491  ,   -0.0212 ,  0.0066}}, // Mjj 1000-1500, PTHjj 0-25    , pTH 200-inf
   { 222 , {0.0056,-0.1440 , 0.0112 , 0.0117 , 0.0156 , 0.0263 , 0.0404 , 0.0802  ,   +0.0212 ,  0.0109}}, // Mjj 1000-1500, PTHjj 25-inf  , pTH 200-inf
   { 223 , {0.0036,-0.0929 , 0.0072 , 0.0076 , 0.0101 , 0.0169 , 0.026  , 0.0518  ,   -0.0223 ,  0.0070}}, // Mjj 1500-inf , PTHjj 0-25    , pTH 200-inf
   { 224 , {0.0081,-0.2062 , 0.016  , 0.0168 , 0.0223 , 0.0376 , 0.0578 , 0.1149  ,   +0.0223 ,  0.0155}}  // Mjj 1500-inf , PTHjj 25-inf  , pTH 200-inf
   
};

// uncertainty sources extracted from proVBF NNLO
// 10 nuissances:  1 x yield, 1 x 3rd jet veto, 6 x Mjj cuts, 1 x 01->2 jetBin, 1 x PTH cut
//+--------------------+--------+-------+-------+--------+--------+--------+---------+---------+--------+--------+
//|                    |  tot   |  200  | Mjj60 | Mjj120 | Mjj350 | Mjj700 | Mjj1000 | Mjj1500 |   25   |  2jet  |
//+--------------------+--------+-------+-------+--------+--------+--------+---------+---------+--------+--------+
//|   DELTA (POWHEG)   | 14.872 | 1.275 | 6.421 | 5.572  | 3.516  | 7.881  |  5.721  |  4.579  |  4.66  | 1.866  |
//|     DELTA (FO)     | 14.867 | 0.394 | 9.762 | 6.788  | 7.276  | 3.645  |  2.638  |  1.005  | 20.073 | 18.094 |
//+--------------------+--------+-------+-------+--------+--------+--------+---------+---------+--------+--------+
std::vector<double> uncert_deltas({14.867, 0.394, 9.762, 6.788, 7.276, 3.645, 2.638, 1.005, 20.073, 18.094});

// cross sections from different STXS bins
// prediction at NLO from POWEHG VBFH + PYTHIA8(dipoleShower=on)

std::map<int, double> powheg_xsec
  {{200,  273.952 },
   {201,  291.030 },
   {202, 1317.635 },
   {203,   36.095 },
   {204,   55.776 },
   {205,  178.171 },
   {206,   18.839 },
   {207,   37.952 },
   {208,  291.846 },
   {209,  146.782 },
   {210,  385.566 },
   {211,   64.859 },
   {212,  197.414 },
   {213,   53.598 },
   {214,  182.107 },
   {215,   41.167 },
   {216,  187.823 },
   {217,   19.968 },
   {218,   21.092 },
   {219,   12.496 },
   {220,   18.215 },
   {221,   13.490 },
   {222,   22.044 },
   {223,   14.220 },
   {224,   31.565 }};



// Propagation function
double LHCHXSWG::vbf_uncert_stage_1_1(int source, int event_STXS, double Nsigma){
  // return a single weight for a given souce
  if(source < 10){
    double delta_var = stxs_acc[event_STXS][source] * uncert_deltas[source];
    return  1.0 + Nsigma * (delta_var/powheg_xsec[event_STXS]);
  }else{
    return 0.0;
  }
};

// print big table
void print_bigtable(bool relative=true){
  std::cout << " ======================================================== " << std::endl;
  if (relative)
    std::cout << " === relative uncertainties [%] " << std::endl;
  else
    std::cout << " === absolute uncertainties [fb] " << std::endl;

  std::cout << " BIN: "    << std::setw(8)
            << "Yield    | "  << std::setw(8)
            << "PTH200   | "  << std::setw(8)
            << "Mjj60    | "  << std::setw(8)
            << "Mjj120   | "  << std::setw(8)
            << "Mjj350   | "  << std::setw(8)
            << "Mjj700   | "  << std::setw(8)
            << "Mjj1000  | "  << std::setw(8)
            << "Mjj1500  | "  << std::setw(8)
            << "PTH25    | "  << std::setw(8)
            << "JET01    | "  << std::setw(8)
            << "TOT        "  << std::endl;
  for (auto& bin : powheg_xsec) {
    std::cout <<" "<< bin.first << ": ";
    double tot = 0;
    for (int s=0; s < 10; s++) {
      double uncert = 0;
      if (relative){
        uncert = LHCHXSWG::vbf_uncert_stage_1_1(s, bin.first) - 1.0;
        std::cout << std::setw(8) << std::setprecision(5) << 100*uncert << " | ";
      }else{
        uncert = (LHCHXSWG::vbf_uncert_stage_1_1(s, bin.first) - 1.0) * bin.second;
        std::cout << std::setw(8) << std::setprecision(4) << uncert << " | ";
      }
      tot += std::pow(uncert, 2);
    }
    if (relative)
      std::cout<< std::setw(8) << std::setprecision(5) << 100 * std::sqrt(tot) << std::endl;
    else
      std::cout<< std::setw(8) << std::setprecision(4) << std::sqrt(tot) << std::endl;
  }
  std::cout << " ======================================================== " << std::endl;
}

// correlation matrix
double _cov(int ibin, int jbin) {
  double cov_ij=0;
  for (int is=0; is < uncert_deltas.size(); ++is)
    cov_ij+=(LHCHXSWG::vbf_uncert_stage_1_1(is,ibin)-1)*(LHCHXSWG::vbf_uncert_stage_1_1(is,jbin)-1)*powheg_xsec[ibin]*powheg_xsec[jbin];
  return cov_ij;
}

// correlation matrix
double _corr(int ibin, int jbin) {
  if (ibin==jbin) return 1.0;
  return _cov(ibin,jbin)/sqrt(_cov(ibin,ibin)*_cov(jbin,jbin));
}
void print_corr(){
  std::cout << std::setw(8) << " --- ";
  for (auto& ibin: powheg_xsec){
    std::cout << std::setw(8) << ibin.first ;
  }
  std::cout << std::endl;
  std::cout << std::setw(8) << " --- ";
  for (auto& ibin: powheg_xsec){
    std::cout << std::setw(8) << "-----" ;
  }
  std::cout << std::endl;
  for (auto& ibin: powheg_xsec){
    std::cout << std::setw(8) << ibin.first;
    for (auto& jbin: powheg_xsec){
      std::cout << std::setw(8) << std::setprecision(2) << _corr(ibin.first, jbin.first) ;
    }
    std::cout << std::endl;
  }
}

// testing function
//int main(){
//  print_bigtable();
//  print_bigtable(false);
//  print_corr();
//}
