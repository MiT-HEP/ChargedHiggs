#ifndef PURITY_FIT_ANALYTIC_H
#define PURITY_FIT_ANALYTIC_H

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
#include "RooExtendPdf.h"
#include "RooExponential.h"
#include "RooGaussian.h"
#include "RooGenericPdf.h"
#include "RooFFTConvPdf.h"

#include "interface/BaseFitter.hpp"

class PurityFitAnalytic : public BaseFitter{
    protected:
        float fit_specific( const TH1* h, const TH1* sig, const TH1* bkg, 
            TH1* bkgInv,
            string name, // unique name of the result
            string outname="" , // output file name, where to save results
            map<string,float> *pars	=NULL // to gather additional params
            );

        TFile *fIn_;


    public:
        PurityFitAnalytic(){outname="";inname="";txtoutname="";verbose_=0;};
        ~PurityFitAnalytic(){};

        vector<float> PtBins;
        vector<string> bkglabels;

        string outname;
        string inname;

        string txtoutname;
        map<string,float> initvalues;

        int verbose_;
        double lumi{2318};

        virtual void init();
        virtual void fit();
        const string name() const override { return "PurityFitAnalytic";}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
