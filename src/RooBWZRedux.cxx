/*******************************************
 * RooBWZRedux2 : with polynomial functions
 * Andrea Carlo Marini (MIT)
 *
 ******************************************/
#ifndef ROO_BWZREDUX
#define ROO_BWZREDUX

#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooChangeTracker.h"
#include "TMath.h"

class RooBWZRedux : public RooAbsPdf
{
    public:
        RooBWZRedux(){}
        // nominal constructor
        RooBWZRedux(const char *name, const char*title, RooAbsReal*x, const RooArgList& coefList);
        //copy constructor
        RooBWZRedux(const RooBWZRedux & other, const char* name=0):
            RooAbsPdf(other, name), 
            _x("x", this, other._x), 
            _coefList("coefList",this,other._coefList) {}
        // clone
        virtual inline TObject *clone(const char*newname) const { return new RooBWZRedux(*this,newname);}

        //destructors
        virtual ~RooBWZRedux() {} 

        // analysic integral
        Int_t getAnalyticalIntegral(RooArgSet&allVars, RooArgSet & analVars, const char* rangeName=0) const 
        {
            // No analytical calculation available (yet) of integrals over subranges.
            if (rangeName && strlen(rangeName)){return 0;} 
            if (matchArgs(allVars,analVars,_x)){return 1;}
            return 0;
        }

        Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

        void printMultiline (std::ostream &os, Int_t contents, Bool_t verbose=kFALSE, TString indent="") const override;

    protected:
        int order_{0}; // order of the polinomial function
        RooRealProxy _x;
        RooListProxy _coefList;
        Double_t evaluate() const {}
        RooRealVar Zmass_,Zwidth_;

    ClassDef(RooBWZRedux,1) // Polynomial PDF

};

//RooBWZRedux(const char *name, const char*title, RooAbsReal*x, const RooArgList& coefList);
RooBWZRedux::RooBWZRedux(const char *name, const char*title, RooAbsReal*x, const RooArgList& coefList):
    RooAbsPdf(name, title),
    _x("x", "Dependent", this, x),
    _coefList("coefList","List of coefficients",this),
    Zmass_("zmass","zmass",91.2), // for the moment constant.
    Zwidth_("zwidth","zwidth",2.5)
{
    int n = coefList.getSize();
    if (n < 3 ) cout <<"ERROR: need more parameters (at least 3: a,b,c)!"<<endl;
    else order_=n-3;
    _coefList.add(coefList);
}

void RooBWZRedux::printMultiline (std::ostream &os, Int_t contents, Bool_t verbose=kFALSE, TString indent="") const
{
    RooAbsPdf::printMultiline(os,contents,verbose,indent);
    os<< indent<<"order="<<order_<<endl;
}


Double_t RooBWZRedux::evaluate() const 
{
    using std::exp;
    using std::pow;
    //double x100 = _x / 100.;
    double a =  static_cast<RooAbsReal*>(_coefList.at(0))->getVal();
    double b =  static_cast<RooAbsReal*>(_coefList.at(1))->getVal();
    double c =  static_cast<RooAbsReal*>(_coefList.at(2))->getVal();
    double zm = Zmass_.getVal();
    double zw = Zwidth_.getVal();
    double x = _x/100.; // WARNING. x != _x !!! numerical stability
    double xmin = _x.min();
    double xmax = _x.max();
    double n = (_x -xmin)/(xmax-xmin); 
    //e^(b x/100 + x^2 c) / ((x-91.2)^a + (2.5/2.)^a)
    double bwzr = exp( x *b + x*x*c) / (pow(_x-zm,a) + pow((zw/2.),a));

    double pol = 1.;
    switch (order_)
    {
        case 0: { pol = 1.; break; }
        case 1: { double p1=static_cast<RooAbsReal*>(_coefList.at(3))->getVal(); pol = 1. + p1*(n-.5); break; }
        case 2: { 
                    double p1=static_cast<RooAbsReal*>(_coefList.at(3))->getVal(); 
                    double p2=static_cast<RooAbsReal*>(_coefList.at(4))->getVal(); 
                    pol = 1. + p1*3.* n *n + p2 *6.* n *(1.-n) + (-p1-p2)*3.*(1.-n)*(1.-n);
                    break;
                }
        case 3: { 
                    double p1=static_cast<RooAbsReal*>(_coefList.at(3))->getVal(); 
                    double p2=static_cast<RooAbsReal*>(_coefList.at(4))->getVal(); 
                    double p3=static_cast<RooAbsReal*>(_coefList.at(5))->getVal(); 
                    pol = 1. + p1*4.* n*n*n + p2 * n*n *(1.-n) + (p3)*n*(1.-n)*(1.-n) + (-p1-p2-p3) *(1-n)*(1-n)*(1-n);
                    break;
                }
        case 4:{
                    double p1=static_cast<RooAbsReal*>(_coefList.at(3))->getVal(); 
                    double p2=static_cast<RooAbsReal*>(_coefList.at(4))->getVal(); 
                    double p3=static_cast<RooAbsReal*>(_coefList.at(5))->getVal(); 
                    double p4=static_cast<RooAbsReal*>(_coefList.at(6))->getVal(); 
                    //(1+ @5*5*pow(@4,4)+20*@6*pow(@4,3)*(1-@4)+@7*30*@4*@4*(1-@4)*(1-@4) + @8 *20*@4*pow(1-@4,3)+(-@5-@6-@7-@8)*5*pow(1-@4,4))",*plist);
                    pol = 1.+p1*5*pow(n,4) + 20*p2*pow(n,3)*(1.-n) +p3*30*pow(n,2)*pow(1.-n,2) + p4*20*n*pow(1-n,3) + (-p1-p2-p3-p4)*5*pow(1-n,4);
                    break;
                }
    }
    return bwzr * pol;
}

Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const {
    //  integrate from xmin to xmax
    double xmin = _x.min();
    double xmax = _x.max();
}

