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

#include "interface/BaseFitter.hpp"



class Fitter : virtual public BaseFitter{
    /* Original Author: Andrea C. Marini 
     * Date: 8th June 2015
     * This class provide an interface to create 
     * smooth workspace for combine
     */

    // private members end with _
    //
    RooWorkspace *w_{0};
    RooRealVar *mh_{0}; // truth
    RooRealVar *x_{0}; // observable (mt)

    map< string, RooDataHist*> hist_;

    // save fit parameters vs mass category 
    map<string, float> fitParameters_;
    // interpolates fit parameters and norm vs mass
    map<string, RooSpline1D*> splines_;
    // save RooRealVars used for fit
    map<string, RooRealVar*> vars_;

    public:
    string datasetMask_ ;
    string xsecMask_;
    string eaMask_;
    string massMask_;
    string normMask_;
    string modelMask_;
    string systLabel_{""}; //_JesUp, _JesDown .., mainly internal, but should be uniq
    vector<string> systIn; 

    void info();

    bool writeDatasets_ ; // write the RooDatasets into the ws


    // --- objects that can be set	
    vector<float> mIn; // input masses
    vector<string> inputMasks; // input FileName Mask. Must contain a replacement for float. One for each cat
    string outname; // 
    string inname;
    string plotDir;
    bool plot; // make plot fit
    bool verbose;

    float xmin;
    float xmax;

    // --- objects that can be called
    Fitter();
    void init();	
    void fit() ;
    void write();
    void end();
    const string name() const override { return "Fitter";}

    map<string,float> initPars_;

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
