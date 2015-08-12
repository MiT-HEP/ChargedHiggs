#ifndef PURITY_FIT_H
#define PURITY_FIT_H

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

class PurityFit{
    protected:
        float fit_specific( TH1* h, TH1* sig, TH1* bkg, 
            string name, // unique name of the result
            string outname="" , // output file name, where to save results
            map<string,float> *pars	=NULL // to gather additional params
            );

        TFile *fIn_;

    public:
        PurityFit(){outname="";inname="";verbose_=0;};
        ~PurityFit(){};

        vector<float> PtBins;
        string outname;
        string inname;

        int verbose_;

        virtual void init();
        virtual void fit();
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
