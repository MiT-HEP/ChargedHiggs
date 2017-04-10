#ifndef PURITY_FIT_BASE_H
#define PURITY_FIT_BASE_H

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

#include "interface/BaseFitter.hpp"

class PurityFitBase : public BaseFitter{
    protected:
        void fit_specific();
	float computeR();

	// For reads
        TFile *fIn_;

	// histos -- for Fit Specific
	TH1D *h_;
	TH1D* sig_;
	TH1D* bkg_;
	TH1D* bkgInv_;
	TH1D* hFullInv_;
	string name_; // current name

	// output of fit specific
	map<string,float> pars_;
	float frac_{1};


    public:
        PurityFitBase(){outname="";inname="";txtoutname="";verbose_=0;};
        ~PurityFitBase(){};

        vector<float> PtBins;
        vector<string> bkglabels;

        string outname;
        string inname;

        string txtoutname;
        map<string,float> initvalues;

        int verbose_;

        void init() override;
        void fit() override;
        const string name() const override { return "PurityFitBase";}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
