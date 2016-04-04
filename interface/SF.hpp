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
        virtual double get(){ if (veto_) return 1.0 - (sf + err*syst); else return sf + err*syst ; }
        const string name() const {return "SF";}
        virtual void SetVeto(int x=1){veto_=x;}
        // TODO print
};

class SF_Asymm : public SF
{
    public:
        double errUp,errDown;
        SF_Asymm() :SF() { errUp=0.0; errDown=0.0;}
        virtual double get() { if (syst==0 ) return sf; else if (syst>0) return sf + errUp *syst ; else if (syst<0) return sf + errDown * syst ; else return 0.; }
        const string name() const { return "SF_Asymm";}
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
        void print();
        const string name() const {return "SF_PtEta";}
};

const bool operator<( const SF_PtEta::range&r1 , const SF_PtEta::range &r2);


class SF_TH2F : virtual public SF_PtEta
{
    public:
        SF_TH2F() : SF_PtEta(),SF(){}
        SF_TH2F(string filename) : SF_PtEta(),SF(){ init(filename); }
        void init(string filename,string histname="EGamma_SF2D");
        const string name() const {return "SF_TH2F";}
};

#include "TSpline.h"

class SF_PtSpline : virtual public SF
{
    private:

        TSpline3 *spline_{0};
        TSpline3 *errSpline_{0};

    public :
        SF_PtSpline() : SF() {}
        ~SF_PtSpline(){clear();}
        
        inline void clear(){delete spline_; delete errSpline_;}

        void add (double pt, double sf ,double err);
        void init (string filename, string obj,string obj2);
        void init();
        void set (double pt);
        void print();
        const string name() const {return "SF_PtSpline";}

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
    BTagCalibrationReader *readerL{0}, *readerL_up={0}, *readerL_down={0};
    BTagCalibrationReader *readerM{0}, *readerM_up={0}, *readerM_down={0};
    BTagCalibrationReader *readerT{0}, *readerT_up={0}, *readerT_down={0};
    constexpr static float MaxBJetPt = 670.;
    constexpr static float MaxLJetPt = 1000.;
    constexpr static float MinBJetPt = 30.;
    constexpr static float MinLJetPt = 20.;

    float cached_pt{30.};
    float cached_eta{0};
    int cached_wp{0};
    int cached_flavor{0};

    public:
        SF_CSV() {};
        ~SF_CSV(){ delete readerL;delete readerL_up;delete readerL_down;delete readerM;delete readerM_up;delete readerM_down;delete readerT;delete readerT_up;delete readerT_down;delete calib;}
        void init (string filename);
        // flavor is pdgid, wp 0==Loose, 1==Medium, 2 = Tight
        void set (float pt, float eta, int wp,int flavor );
        void set (float pt, float eta); // use cached for the others
        void setWP(int wp);
        void setJetFlavor(int flavor);
        void print(){};
        const string name() const {return "SF_CSV";}

};



#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
