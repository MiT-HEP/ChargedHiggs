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

#define MAX_NUIS 20
#define MAX_BINS 1000

//#define DEBUG_RooBinnedUncertainties 1

class RooBinnedUncertainties : public RooAbsPdf
{
    int _absolute{0}; //shifts are absolute or relative. (TODO: Absolute, figure out norm) 
    int _nbins{0}; 
    double _xmin{0.0};
    double _xmax{0.0}; // in this way it is faster to figure out the bin
    RooRealProxy _x; // observables
    RooRealProxy _pdf; //pdf

    RooListProxy _coefList ; //list of nuisances/morphers. No constrain is put on these parameters. Use gaussian priors, if like.
    //std::vector<std::vector< float> > shiftUp{}; // shifts up, [nuis][bin]
    //std::vector<std::vector< float> > shiftDown{};

    Double_t shiftUp[MAX_NUIS][MAX_BINS]; // huge ? 
    Double_t shiftDown[MAX_NUIS][MAX_BINS];

    
    public:
    // TODO: Analytic integrals
    // if pdf has analitic integrals, integrals is the pdf integral, plus the sum over the nuisances.
        RooBinnedUncertainties() : RooAbsPdf(){}
        RooBinnedUncertainties(const char* name, const char *title) : RooAbsPdf(name,title){}
        RooBinnedUncertainties(const char* name, const char *title, RooRealVar& obs,RooAbsPdf&pdf,int nbins, double xmin,double xmax) :
            RooAbsPdf(name,title),
             _x("x", "Dependent", this, obs),
             _pdf("pdf", "Dependent", this, pdf),
             _nbins(nbins), _xmin(xmin), _xmax(xmax),
             _coefList("coef","Dependent",this)
            {
            //#ifdef DEBUG_RooBinnedUncertainties
            //std::cout<< "[DEBUG]"<<" Called nominal constructor: "<< GetName()<< std::endl;
            //#endif
            }

        //copy constructor and clone
        RooBinnedUncertainties(const RooBinnedUncertainties&other,const char *name=0) :
             RooAbsPdf(other, name), 
             _x("x", this, other._x), 
             _pdf("pdf",this,other._pdf),
             _coefList("coefList",this,other._coefList),
             _nbins(other._nbins), _xmin(other._xmin),_xmax(other._xmax)
        {
            //#ifdef DEBUG_RooBinnedUncertainties
            //std::cout<< "[DEBUG]"<<" Called copy constructor:"<< GetName()<<"<-"<< other.GetName()<<std::endl;
            //#endif
            // mem cp should be faster
            for(unsigned inuis=0;inuis < _coefList.getSize() ; ++inuis)
            for(unsigned ibin=0;ibin<other._nbins;++ibin)
            {
                shiftUp[inuis][ibin] = other.shiftUp[inuis][ibin];
                shiftDown[inuis][ibin] = other.shiftDown[inuis][ibin];
            }
        }

        virtual TObject* clone(const char* newname) const override { return new RooBinnedUncertainties(*this, newname); }
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
            int nNuis = _coefList.getSize();
            assert(nNuis <= MAX_NUIS);

            //coefList[nNuis] = RooRealProxy("proxy",( std::string("Proxy for ") + nuis.GetName() ).c_str(),this,nuis);

            //#ifdef DEBUG_RooBinnedUncertainties
            //std::cout<<"[DEBUG]"<<"Adding Proxy:"<<nNuis<<std::endl;
            //_coefList.Print("VA");
            //std::cout<<"[DEBUG]"<<"   --- _x Proxy:"<<nNuis<<std::endl;
            //_x.Print("VA");
            //#endif



            for(unsigned i=0; i< _nbins;++i) {  
                shiftUp[nNuis-1][i] = up[i]; 
                shiftDown[nNuis-1][i] = down[i];
            }
            //shiftUp.push_back(up);
            //shiftDown.push_back(down);

            //_coefList.print(std::cout);
        }

        //symmetric
        void addUncertainty(RooAbsReal& nuis,const std::vector<float> &val)
            {
            return addUncertainty(nuis,val,val);
            }

        void info(){
            std::cout<< "------------------ "<<this->GetName() << " ----------------"<<std::endl;
            std::cout<< ((_absolute)? "Absolute Shifts" : "Relative Shifts") <<std::endl;
            std::cout<< "Binning: "<< _nbins <<" in ["<< _xmin<<","<< _xmax<<"]"<<std::endl;
            std::cout<< "Underlying pdf: "<< _pdf.arg().GetName()<<std::endl;
            std::cout<< "Morphers: "<<std::endl;
            for (unsigned inuis=0;inuis<_coefList.getSize(); ++inuis)
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
                    std::cout <<"[WARNING] ibin found to be (<0) for x ("<<x<<") in ["<<_xmin<<","<<_xmax<<"]. Setting to 0."<<std::endl;
                    ibin=0;
                }
                if (ibin>=_nbins)
                {
                    if ( fabs(_xmax-x)>0.001) // last bin is close
                        std::cout <<"[WARNING] ibin found to be (>="<<_nbins<<") for x ("<<x<<") in ["<<_xmin<<","<<_xmax<<"]. Setting to "<<_nbins-1<<"."<<std::endl;
                    ibin=_nbins-1;
                }
                
                // un-normalized 
                //double c = static_cast<const RooAbsPdf*>(&_pdf.arg())->getValV(NULL); // central value of the pdf 
                double c = _pdf;
                double d=0.; //delta induced by nuisances in bin, ibin

                //#ifdef DEBUG_RooBinnedUncertainties
                //std::cout<<"[DEBUG]"<<"Evaluating at val:"<<x<<std::endl;
                //std::cout<<"[DEBUG]"<<"  --- pdf: "<< c <<std::endl;
                //#endif

                for(unsigned inuis =0 ;inuis< _coefList.getSize() ;++inuis)
                {
                    double p=static_cast<RooAbsReal*>(_coefList.at(inuis))->getVal(); 

                    //#ifdef DEBUG_RooBinnedUncertainties
                    //std::cout<<"[DEBUG]"<<"  --- nuis: "<<inuis<< ": "<< static_cast<RooAbsReal*>(_coefList.at(inuis))->GetName() <<" "<< p <<std::endl;
                    //std::cout<<"[DEBUG]"<<"          : "<< shiftDown[inuis][ibin]<<"/"<<shiftUp[inuis][ibin] <<std::endl;
                    //#endif

                    if (p>=0) d+= p*shiftUp[inuis][ibin];
                    else      d+= p*shiftDown[inuis][ibin]; // p carries the sign

                }
                //#ifdef DEBUG_RooBinnedUncertainties
                //std::cout<<"[DEBUG]"<<" --- Returning value"<< ((_absolute)?(c+d):(c*(d+1.0))) <<std::endl;
                //#endif
                return (_absolute)?(c+d):(c*(d+1.0));
        }
    private:
        ClassDef(RooBinnedUncertainties,1) 
};

#endif
