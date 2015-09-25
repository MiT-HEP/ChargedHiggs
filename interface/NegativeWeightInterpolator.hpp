#ifndef NEG_WEIGHT_INTERPOLATOR_H
#define NEG_WEIGHT_INTERPOLATOR_H
// --- STD ---
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
// --- ROOT ---
#include "TFile.h"
#include "TDirectory.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h" 
#include "TLatex.h"
#include "TLegend.h"

// --- ROOFIT ---
#include "RooGlobalFunc.h"
#include "RooPlot.h"
#include "RooWorkspace.h"
#include "RooGaussian.h"
#include "RooVoigtian.h"
#include "RooProduct.h"
#include "RooAddition.h"
#include "RooConstVar.h"
#include "RooFormulaVar.h"
#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooAddPdf.h"
#include "RooHistPdf.h"
#include "RooFitResult.h"
#include "RooExponential.h"
#include "RooGenericPdf.h"


class NegativeWeightInterpolator
{
public:
	NegativeWeightInterpolator() { constructModel() ; w_ = new RooWorkspace("w","workspace");}
	TH1 * add( const TH1* pos, const TH1* neg); // assume that the second one is negative
	TH1 * fit( const TH1*) ;

	void constructModel();
	void reset();
    void guess(const TH1*); // guess initial parameters

    void inline SetVerbosity(int n) {verbose_= n;}
    void print();
private:
	map<string, RooRealVar*> vars;
	map<string, RooAbsPdf*> pdfs;

	RooAbsPdf *model;
	RooFitResult *result=NULL;
    int verbose_ = 0;
    RooWorkspace *w_;
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
