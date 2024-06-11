#ifndef JET_FILTER_2016_H
#define JET_FILTER_2016_H

#include "interface/Corrector.hpp"

#include "TH2D.h"
#include <memory>

class JetFilter2016: public CorrectorBase
{
    // kill jets accordingly to JetMet Map
    std::unique_ptr<TH2D> h; 
    std::unique_ptr<TH2D> hMC; 

    bool useMC_{false};

    std::string filename_{"aux/hotjets-runBCDEFGH.root"};
    std::string histname_{"h2jet"};

    std::string filenameMC_{"aux/hotjets-runBCDEFGH.root"};
    std::string histnameMC_{"h2jet"};

    public:
        JetFilter2016() : CorrectorBase() {}
        JetFilter2016(const string &fname, const string &hname) : CorrectorBase(){filename_=fname; histname_=hname;}
        JetFilter2016(const string &fname, const string &hname, const string &fname2, const string &hname2) : CorrectorBase(){
            filename_=fname; histname_=hname;
            filenameMC_=fname2; histnameMC_=hname2;
            useMC_=true;
        }
        void Init() override;
	    const string name() const override { return "JetFilter2016";}
	    int correct(Event *e) override ;
	    //vector<Jet *> & GetJetVector(Event *e) { return e->jets_;}
        //inline void Scale( Object&o, float value) { o.Scale(value);}
};

#endif
