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
#include "TRandom3.h"
// do not use namespace for dictionaries
//using namespace RooFit;
class RooSpline1D;

class PdfModelBuilder
{
    RooRealVar *obs_var;
    map<string,RooRealVar*> params;
    map<string,RooFormulaVar*> prods;
    map<string,RooAbsPdf*> pdfs;
    map<string,RooDataHist*> hists;
    TRandom3 *RandomGen{NULL} ;
    private:
    bool replace(std::string& str, const std::string& from, const std::string& to) { // from SO
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos) return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    public:
        PdfModelBuilder(){};
        ~PdfModelBuilder(){};
        //
        void setObsVar(RooRealVar *var){obs_var=var; prods["obs_range"] = new RooFormulaVar(Form("%s_range",obs_var->GetName()),"normalized obs var",Form("(@0-%f)/(%f)",obs_var->getMin(),obs_var->getMax()-obs_var->getMin()),*obs_var);};
        RooAbsPdf* getBernstein(string prefix, int order,bool positive=true);
        RooAbsPdf* getModBernstein(string prefix, int order);
        //RooAbsPdf* getChebychev(string prefix, int order);
        //RooAbsPdf* getPowerLaw(string prefix, int order);
        //RooAbsPdf* getPowerLawSingle(string prefix, int order);
        RooAbsPdf* getPowerLawGeneric(string prefix, int order);
        //RooAbsPdf* getExponential(string prefix, int order);
        RooAbsPdf* getExponentialSingle(string prefix, int order);
        RooAbsPdf* getLaurentSeries(string prefix, int order);
        RooAbsPdf* getLogPol(string prefix, int order);
        RooAbsPdf* getExpPol(string prefix, int order); // e^pol
        RooAbsPdf* getDYBernstein(string prefix, int order,TH1D*dy);
        RooAbsPdf* getZPhotonRun1(string prefix, int order);
        RooAbsPdf* getZModExp(string prefix, int order); // BWZ * (1+x)
        RooAbsPdf* getZModExp2(string prefix, int order); /// BWZ * (1+x + x(1-x))
        RooAbsPdf* getBWZ(string prefix, int order);
        RooAbsPdf* getPolyTimesFewz(string prefix,int order,string fname);
        RooAbsPdf* getCorePdf(string prefix, int order);

        // get the order for the ftest
        // prefix are bern, powlaw, exp, lau
        // return the relevalt to be included in the multipdf
        RooAbsPdf* getPdf(const string& prefix,int order );
        RooAbsPdf* getPdf(const string& prefix,int order, TH1D*h ) { 
            if (prefix.find( "dybern")!=string::npos) return getDYBernstein(prefix+ Form("_ord%d",order),order,h); else return getPdf(prefix,order);
        }
        RooAbsPdf* fTest(const string& prefix,RooDataHist*,int *ord=NULL,const string&plotDir="",TH1D*h=NULL);
        void runFit(RooAbsPdf *pdf, RooDataHist *data, double *NLL, int *stat_t, int MaxTries);
        double getProbabilityFtest(double chi2, int ndof);
        double getGoodnessOfFit(RooRealVar *mass, RooAbsPdf *mpdf, RooDataHist *data, std::string name, bool blind=true);


};


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

    int fitStrategy{0}; // 0 hmm, 1 vbs -> for selecting functions

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
    string xname{"mmm"};


    // --- objects that can be called
    BackgroundFitter();
    void init() override;	
    void fit() override;
    void write();
    //void end() {}; // not needed here,
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
