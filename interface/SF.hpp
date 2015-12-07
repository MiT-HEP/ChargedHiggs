#ifndef SF_H
#define SF_H

#include <string>
#include <map>
#include <iostream>

using namespace std;

class  SF{
    // Base class for implement SF 
    // this will be used by the Weight class to 
    // change the event weight 
    public:
        SF(){ syst=0; sf=1.0; err=0; label = "";}
        ~SF(){}
        string label;
        double sf;
        double err;
        int syst;
        //
        virtual double get(){ return sf + err*syst ; }
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
};

const bool operator<( const SF_PtEta::range&r1 , const SF_PtEta::range &r2);

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

};



#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
