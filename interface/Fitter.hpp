#ifndef FITTER_H
#define FITTER_H

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
// do not use namespace for dictionaries
//using namespace RooFit;
class RooSpline1D;



class Fitter{
    /* Original Author: Andrea C. Marini 
     * Date: 8th June 2015
     * This class provide an interface to create 
     * smooth workspace for combine
     */

    // private members end with _
    //
    RooWorkspace *w_;
    RooRealVar *mh_; // truth
    RooRealVar *x_; // observable

    map< string, RooDataHist*> hist_;

    string datasetMask_ ;
    string xsecMask_;
    string massMask_;

    bool writeDatasets_ ; // write the RooDatasets into the ws

    vector<float> startMean_;
    vector<float> startSigma_;
    vector<float> startFraction_;
    vector<float> startBern_;

    // save fit parameters vs mass category 
    map<string, float> fitParameters_;
    // interpolates fit parameters and norm vs mass
    map<string, RooSpline1D*> splines_;
    // save RooRealVars used for fit
    map<string, RooRealVar*> vars_;

    // -- RooAbsReal* getMeanWithSyst(string name, RooAbsReal*mean);
    void info();

    // ------------ INIT -------
    void initGaus();
    void initBern();


    // ------------SIGNAL MODEL AND FIT -----------
    // add Bern - Gaus To the fit Model
    void addBernFitModel(RooArgList *pdfs,RooArgList *coeffs,bool isLast);
    void addGausFitModel(RooArgList *pdfs,RooArgList *coeffs,bool isLast);

    // -- save Coeff of the fit in fitParameters_
    void saveCoefficientsGaus( int cat,string mass,bool isLast);
    void saveCoefficientsBern( int cat,string mass,bool isLast);

    void interpolateBern(int cat,bool isLast);
    void interpolateGaus(int cat,bool isLast);

    // --- FINAL MODEL ---

    public:

    // --- objects that can be set	
    vector<float> mIn; // input masses
    vector<string> inputMasks; // input FileName Mask. Must contain a replacement for float. One for each cat
    string outputFileName; // 
    string inputFileName;
    string plotDir;
    bool plot; // make plot fit
    bool verbose;

    int nGaussians;
    int nBernstein;

    float xmin;
    float xmax;

    // --- objects that can be called
    Fitter();
    void init();	
    void fitSignal();
    void write();
    void finalModel();

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
