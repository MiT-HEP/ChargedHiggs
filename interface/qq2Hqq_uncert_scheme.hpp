#ifndef QQ2HQQ_H
#define QQ2HQQ_H

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <cmath>
// changes:
// * static maps will go into a namespace. static definition of global variables and functions only limit accessibility from a specific file
namespace LHCHXSWG{
    double vbf_uncert_stage_1_1(int source, int event_STXS, double Nsigma=1.0);

};

#endif
