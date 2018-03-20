#ifndef ROO_BINNED_UNCERTAINTIES_H
#define ROO_BINNED_UNCERTAINTIES_H
#include "RooAbsReal.h"
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooChangeTracker.h"
#include "RooRealVar.h"

#include "TMath.h"
#include <vector>
#include <map>
#include <iostream>

/* Original Author: Andrea Carlo Marini
 * Date: 20/03/2018
 * Licence: gpl-3.0
 */

class RooBinnedUncertainties : public RooAbsPdf
{
    int _nbins{0}; 
    double _xmin{0.0};
    double _xmax{0.0}; // in this way it is faster to figure out the bin
    RooRealProxy _x; // observables
    RooRealProxy _pdf; //pdf

    RooListProxy _coefList ; //list of nuisances/morphers. No constrain is put on these parameters. Use gaussian priors, if like.
    std::vector<std::vector< float> > shiftUp; // shifts up, [nuis][bin]
    std::vector<std::vector< float> > shiftDown;

    
    public:
    // TODO: Analytic integrals
    // if pdf has analitic integrals, integrals is the pdf integral, plus the sum over the nuisances.
        RooBinnedUncertainties() : RooAbsPdf(){}
        RooBinnedUncertainties(const char* name, const char *title) : RooAbsPdf(name,title){}
        RooBinnedUncertainties(const char* name, const char *title, RooRealVar& obs,RooAbsPdf&pdf,int nbins, double xmin,double xmax) :
            RooAbsPdf(name,title),
             _x("x", "Dependent", this, obs),
             _pdf("pdf", "Dependent", this, pdf),
             _nbins(nbins), _xmin(xmin), _xmax(xmax)
            {}

        //copy constructor and clone
        RooBinnedUncertainties(const RooBinnedUncertainties&other,const char *name=0) :
             RooAbsPdf(other, name), 
             _x("x", this, other._x), 
             _pdf("pdf",this,other._pdf),
             _coefList("coefList",this,other._coefList),
             _nbins(other._nbins), _xmin(other._xmin),_xmax(other._xmax)
        {}

        virtual TObject* clone(const char* newname) const { return new RooBinnedUncertainties(*this, newname); }
        virtual ~RooBinnedUncertainties() {}

        // asymmetric shifts
        void addUncertainty(RooAbsReal& nuis,const std::vector<float> &up, const std::vector<float> &down)
        {
            std::cout<< "[INFO]"<<" Adding uncertainty:"<<nuis.GetName()<<std::endl;
            if (up.size() != _nbins or down.size() !=_nbins)
            {
                std::cout<<"[ERROR] can't add a nuisance with number of entries "<< up.size()<<","<<down.size()<<" different from "<<_nbins<<". Nothing done."<<std::endl;
                return;
            }

            _coefList.add(nuis) ; // not owned
            shiftUp.push_back(up);
            shiftDown.push_back(down);

            //_coefList.print(std::cout);
        }

        //symmetric
        void addUncertainty(RooAbsReal& nuis,const std::vector<float> &val)
            {
            return addUncertainty(nuis,val,val);
            }

        void info(){
            std::cout<< "------------------ "<<this->GetName() << " ----------------"<<std::endl;
            std::cout<< "Binning: "<< _nbins <<" in ["<< _xmin<<","<< _xmax<<"]"<<std::endl;
            std::cout<< "Underlying pdf: "<< _pdf.arg().GetName()<<std::endl;
            std::cout<< "Morphers: "<<std::endl;
            for (unsigned inuis=0;inuis<shiftUp.size(); ++inuis)
            {
                std::cout<< "    *) "<<static_cast<RooAbsReal*>(_coefList.at(inuis))->GetName()<<":";
                for(int i=0;i<_nbins;++i)
                    std::cout<< shiftDown[inuis][i]<<"/"<<shiftUp[inuis][i]<<" ";
                std::cout<<std::endl;
            }
            std::cout<< "-----------------------------------------------------------"<<std::endl;
        }

    protected:
        Double_t evaluate   ()   const override {
                double x = _x; // observable value
                int ibin= int(std::floor( (x-_xmin)/(_xmax-_xmin) * _nbins ));
                if (ibin<0) { 
                    std::cout <<"[WARINING] ibin found to be (<0) for x ("<<x<<") in ["<<_xmin<<","<<_xmax<<"]. Setting to 0."<<std::endl;
                    ibin=0;}
                if (ibin>=_nbins)
                {
                    std::cout <<"[WARINING] ibin found to be (>="<<_nbins<<") for x ("<<x<<") in ["<<_xmin<<","<<_xmax<<"]. Setting to "<<_nbins-1<<"."<<std::endl;
                    ibin=_nbins-1;
                }
                
                double c = _pdf; // central value of the pdf 
                double d=0.; //delta induced by nuisances in bin, ibin

                for(unsigned inuis =0 ;inuis< shiftUp.size() ;++inuis)
                {
                    double p=static_cast<RooAbsReal*>(_coefList.at(inuis))->getVal(); 
                    if (p>=0) d+= p*shiftUp[inuis][ibin];
                    else      d+= p*shiftDown[inuis][ibin]; // p carries the sign

                }
                return c+d;
        }
    private:
        ClassDef(RooBinnedUncertainties,1) 
};

#endif
