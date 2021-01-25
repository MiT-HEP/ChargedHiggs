#ifndef FITTER_H
#define FITTER_H


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

//#ifdef HAVE_COMBINE
class Fitter : virtual public BaseFitter{
    /* Original Author: Andrea C. Marini 
     * Date: 8th June 2015
     * This class provide an interface to create 
     * smooth workspace for combine
     */
    //
    RooWorkspace *w_{0};
    RooRealVar *mh_{0}; // truth
    RooRealVar *x_{0}; // observable (mt)

    map< string, RooDataHist*> hist_;

    // save fit parameters vs mass category 
    map<string, float> fitParameters_;
    // interpolates fit parameters and norm vs mass
    map<string, RooAbsReal*> splines_;
    // save RooRealVars used for fit
    map<string, RooRealVar*> vars_;

    float lastMass{-10}; //last Mass point fitted

    public:
    string datasetMask_ ;
    string xsecMask_;
    string eaMask_;
    string massMask_;
    string normMask_;
    string modelMask_;
    string uniq_{""}; // 2016 vs 2017 vs 2018

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
    bool doXsec {false}; // do xsec 
    bool saveFit{true};
    bool useDCB{false}; //use a DBCB as first pdf

    string xname{"mmm"};
    float xmin;
    float xmax;

    float mhmin{120};
    float mhmax{130};

    // --- objects that can be called
    Fitter(string uniq="");
    void init() override;	
    void fit() override ;
    void write() ;
    void end() override ;
    const string name() const override { return "Fitter";}

    map<string,float> initPars_;

    //vector<int> nGaussians;
    map< pair<int,string>, int > nGaussians;
    map< pair<int,string>, pair<int,string> >  replace;
    void SetGaussians(int cat, const string& proc, int nG ) { nGaussians[pair<int,string>(cat,proc) ] = nG;} // easy access with pyroot
    vector<string> processes;

    map< pair<int,string>, float>  scaleUnc;
    map< pair<int,string>, pair<int,string> >  scaleCorr;
    map< pair<int,string>, float>  smearUnc;
    map< pair<int,string>, pair<int,string> >  smearCorr;

    void SetScaleUnc(int cat, const string& proc, float value ) { scaleUnc[pair<int,string>(cat,proc) ] = value;} // easy access with pyroot
    void SetScaleUncCorr(int cat, const string& proc, int cat2, const string&proc2 ) { scaleCorr[pair<int,string>(cat,proc) ] = pair<int,string>(cat2,proc2);} // easy access with pyroot
    void SetSmearUnc(int cat, const string& proc, float value ) { smearUnc[pair<int,string>(cat,proc) ] = value;} // easy access with pyroot
    void SetSmearUncCorr(int cat, const string& proc, int cat2, const string&proc2 ) { smearCorr[pair<int,string>(cat,proc) ] = pair<int,string>(cat2,proc2);} // easy access with pyroot
    void SetReplace(int cat, const string& proc, int cat2,const string& proc2){ replace[pair<int,string>(cat,proc)] = pair<int,string>(cat2,proc2);}

    RooAbsReal* getMeanWithSyst(int cat, string proc,int gaus);
    RooAbsReal* getSigmaWithSyst(int cat, string proc,int gaus);
};


#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
