#ifndef BACKGRONUD_FITTER_H
#define BACKGRONUD_FITTER_H


// --- STD ---
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
using namespace std;
// --- ROOT ---
#include "TFile.h"
#include "TDirectory.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h" 
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
#include "RooAbsPdf.h"
#include "RooDataSet.h"
#include "RooExtendPdf.h"
// do not use namespace for dictionaries
//using namespace RooFit;
class RooSpline1D;


#include "interface/BaseFitter.hpp"

//#ifdef HAVE_COMBINE
class BackgroundFitter : virtual public BaseFitter{
    /* Original Author: Andrea C. Marini 
     * Date: 28th Feb 2017
     * This class provide an interface to create 
     * smooth workspace for combine
     */
    //
    RooWorkspace *w_{0};
    RooRealVar *x_{0}; // observable (mt)

    map< string, RooDataHist*> hist_;

    // save fit parameters vs mass category 
    map<string, float> fitParameters_;
    // interpolates fit parameters and norm vs mass
    map<string, RooAbsReal*> splines_;
    // save RooRealVars used for fit
    map<string, RooRealVar*> vars_;


    public:

    string datasetMask_ ;
    string normMask_;
    string modelMask_;

    void info();

    bool writeDatasets_ ; // write the RooDatasets into the ws

    // --- objects that can be set	
    vector<string> inputMasks; // input FileName Mask. Must contain a replacement for each cat
    string outname; // 
    string inname;
    string plotDir;
    bool plot; // make plot fit
    bool blind{true};
    bool verbose;

    float xmin;
    float xmax;


    // --- objects that can be called
    BackgroundFitter();
    void init();	
    void fit() ;
    void write();
    void end() {}; // not needed here,
    const string name() const override { return "BackgroundFitter";}

    map<string,float> initPars_;

    int rebin{1};

    void plotOnFrame(RooPlot *p, RooAbsPdf*pdf, int color, int style=kSolid,const string& legend="", TLegend* leg =NULL);

};

//#else
//#include "interface/Handlers.hpp"
//class BackgroundFitter: public BaseFitter
//{
//    public:
//        void init(){ throw abortException(); };
//        void fit(){ throw abortException() ;};
//        void end(){ throw abortException(); };
//        const name() const override { return "BackgroundFitter_NotImplemeted";};
//};
//#endif


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
