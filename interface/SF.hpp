#ifndef SF_H
#define SF_H

#include <string>
#include <map>
#include <iostream>

#include "interface/Named.hpp"

using namespace std;

class  SF : public Named{
    // Base class for implement SF 
    // this will be used by the Weight class to 
    // change the event weight 
    protected:
        int veto_{0}; // flip to veto ; make sure is set even if constructor is not called
    public:
        SF(){ syst=0; sf=1.0; err=0; label = ""; veto_=0;}
        ~SF(){}
        string label;
        double sf;
        double err;
        int syst;
        //
        virtual double get(){ 
            //                1.0 - DELTA
            if (veto_) Log(__FUNCTION__,"ERROR","Probably unsupported");
            return sf + err*syst;
            //if (veto_) return 1.0 + 1.0 - (sf + err*syst); else return sf + err*syst ; 
        }


        const string name() const {return "SF";}
        virtual void SetVeto(int x=1){veto_=x;}
        virtual void print() const ;
        virtual void clearSyst(){ syst=0;} // reset systs. called in clearSyst
        virtual void clearEvent(){} //clear event specific. called in clearSF
};

class SF_Asymm : public SF
{
    public:
        double errUp,errDown;
        SF_Asymm() :SF() { errUp=0.0; errDown=0.0;}
        double get() override { if (syst==0 ) return sf; else if (syst>0) return sf + errUp *syst ; else if (syst<0) return sf + errDown * syst ; else return 0.; }
        const string name() const override { return "SF_Asymm";}
};

class SF_PtEta : virtual public SF
{
    // this class implements a SF that it is pt and eta dependent (2D dependent) on something
    public:
        struct range{
            double pt1; double pt2; double eta1 ; double eta2;
        };

    private:
        map < range, pair<double,double> >  store;
    public:
        SF_PtEta() : SF() {}
        // add to the db a sf, and error in this range [pt1,pt2) [eta1,eta2)
        virtual void add(double pt1, double pt2 , double eta1, double eta2, double sf, double err);
        // will copy the right SF and err in the mother members sf,err
        virtual void set(double pt,double eta);
        void print() const override;
        const string name() const override {return "SF_PtEta";}
};

const bool operator<( const SF_PtEta::range&r1 , const SF_PtEta::range &r2);

class SF_PtEta_And_Eff : virtual public SF_PtEta
{
    // this implements the reading of the efficiency files for pass and veto sf
    // sf err syst
    // use range for PtEta
    private:
        map < range, pair<double,double> >  storeData_;
        map < range, pair<double,double> >  storeMC_;
        double effData_{1.},effMC_{1.},errData_{0},errMC_{0};

        bool sqrErr_ {true};
        bool dataErr_ {true};

    public:
        using SF_PtEta::range;
        double get() override; //{  return sf + err*syst;}
        const string name() const override { return "SF_PtEta_And_Eff";}
        virtual void add(double pt1, double pt2 , double eta1, double eta2, double dataEff,double mcEff, double dataErr,double mcErr);
        void set(double pt , double eta) override;

        double getDataEff() const { return effData_ ;}
        double getDataErr() const { return errData_;}
        double getMCEff() const { return effMC_ ;}
        double getMCErr() const { return errMC_;}
        
};

class SF_TH2F : virtual public SF_PtEta
{
    public:
        SF_TH2F() : SF_PtEta(),SF(){}
        SF_TH2F(string filename) : SF_PtEta(),SF(){ init(filename); }
        void init(string filename,string histname="EGamma_SF2D",string errorhist=""); // ""== binerror
        const string name() const override {return "SF_TH2F";}
};

class TH2;
class TFile;

class SF_TH2F_And_Eff : virtual public SF_PtEta_And_Eff
{
    public:
        SF_TH2F_And_Eff() : SF_PtEta_And_Eff(),SF(){}
        SF_TH2F_And_Eff( string filename, string effdata,string effmc, string errordata="",string errormc="") : SF_PtEta_And_Eff(),SF(){ init(filename,effdata,effmc,errordata,errormc); }
        void init(string filename,string effdata,string effmc, string errordata="",string errormc=""); // ""== no error
        const string name() const override {return "SF_TH2F_And_Eff";}

    private:
        TFile *f;
        TH2* getHisto( const string & name);
};

#include "TSpline.h"
#include "TGraph.h"
#include "TF1.h"
#include "TF2.h"

class SF_TF1 : virtual public SF
{
    private:
        TF1 *f_sf_{0};
        TF1 *f_err_{0};

    public :
        SF_TF1() : SF() {} 
        void init(string formula,string errFormula="") 
            {
            f_sf_ =new TF1( ("SF_TF1"+ label).c_str(),formula.c_str(),0,1000);
            if(errFormula!="") f_err_=new TF1(("SF_TF1_ERR_"+ label).c_str(), errFormula.c_str(),0,1000);
            }
        ~SF_TF1(){clear();}
        
        inline void clear(){delete f_sf_; delete f_err_; }

        void set (double pt);
        void print() const override;
        const string name() const override {return "SF_TF1";}
};

class SF_TF2 : virtual public SF
{
    private:
        TF2 *f_sf_{0};
        TF2 *f_err_{0};

    public :
        SF_TF2() : SF() {} 
        void init(string formula,string errFormula="") 
            {
            f_sf_ =new TF2( ("SF_TF2"+ label).c_str(),formula.c_str(),0,1000,0,1000);
            if(errFormula!="") f_err_=new TF2(("SF_TF2_ERR_"+ label).c_str(), errFormula.c_str(),0,1000,0,1000);
            }
        ~SF_TF2(){clear();}
        
        inline void clear(){delete f_sf_; delete f_err_; }

        void set (double x,double y);
        void print() const override;
        const string name() const override {return "SF_TF2";}
};

class SF_PtSpline : virtual public SF
{
    private:
        TGraph *g_{0};
        TGraph *ge_{0};
        TSpline *spline_{0};
        TSpline *errSpline_{0};

    public :
        SF_PtSpline() : SF() {spline_=NULL;errSpline_=NULL;}
        ~SF_PtSpline(){clear();}
        
        inline void clear(){delete spline_; delete errSpline_; delete g_; delete ge_;}

        void add (double pt, double sf ,double err);
        void init (string filename, string obj,string obj2);
        void init();
        void set (double pt);
        void print() const override;
        const string name() const override {return "SF_PtSpline";}

};

#include "interface/BTagCalibrationStandalone.hpp"

class SF_CSV : public SF_Asymm
{
    //SF(){ syst=0; sf=1.0; err=0; label = "";}
    //~SF(){}
    //string label;
    //double sf;
    //double errUp;
    //double errDown;
    //int syst;
    //
    BTagCalibration *calib {0};
    BTagCalibrationReader *readerR{0};
    BTagCalibrationReader *readerL{0};
    BTagCalibrationReader *readerM{0};
    BTagCalibrationReader *readerT{0};

    float cached_pt{30.};
    float cached_eta{0};
    int cached_wp{0};
    int cached_flavor{0};
    float cached_discr{0.};

    public:
        SF_CSV() {};
        ~SF_CSV(){ delete readerL;
            delete readerM;
            delete readerT;
            delete readerR;
            delete calib;
        }
        void init (string filename);
        // flavor is pdgid, wp 0==Loose, 1==Medium, 2 = Tight
        void set (float pt, float eta, int wp,int flavor, float discr );
        void set (float pt, float eta); // use cached for the others
        void setDiscr(float discr);
        void setWP(int wp);
        void setJetFlavor(int flavor);
        void print() const override{};
        const string name() const override {return "SF_CSV";}
        

        /* Correlations between the SF measurements: SFlight is mostly uncorrelated to SFc and SFb, while SFc is conservatively taken as correlated to SFb. When varying SFb by +/- 1 sigma, one has to vary SFc by +/- 1 sigma simultaneously. SFlight can be independently varied by +/-1 sigma and the resulting uncertainty added in quadrature to that of SFc and SFb. The SF +/- 1 sigma are obtained by retrieving the "up" and "down" values as usual. Additional breakdown of the systematics is also available for combination of 2015 and 2016 data (see below) . 
         */

        bool simpleError{true};
        int systL{0},systB{0};
        std::string systType="";

        double get() override;
        void clearSyst() override { SF_Asymm::clearSyst(); systL=0; systB=0;systType="";}
};

class SF_WG1: virtual public SF
{
        int Njets30{0};
        double pTH{0.};
        int STXS_Stage1{0};

    public:
        enum TYPES { THU_WG1=0,THU_2017=1};
        TYPES type{THU_2017};
        int nuisance{0};
        //
        //
        inline void SetNjets30(int n){Njets30=n;}
        inline void SetPTH(double pt){pTH=pt;}
        inline void SetSTXS(int i){STXS_Stage1=i;}

        SF_WG1() : SF(){}
        const string name() const override {return "SF_WG1";}
        //void init();

        void print() const override;
        double get() override ;

};

class SF_NNLOPS_Reweight: virtual public SF
{
    // these graphs, are in number of jets
    vector<TGraph*> w;

    int njets {-1};
    double Hpt{0};

    public:
        SF_NNLOPS_Reweight(){};
        SF_NNLOPS_Reweight(const string& fname):SF(){init(fname);};
        const string name() const override {return "SF_NNLOPS_Reweight";}
        virtual void print() const  override;
        //virtual void clearSyst(){ syst=0;}
        void init(const string& fname);
        /// @brief set the pt and njets 
        virtual void set(double pt,int nj){Hpt=pt; njets=nj;}
        double get() override ;

};



#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
