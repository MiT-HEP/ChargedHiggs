#ifndef CHARGEDHIGGSHW_H
#define CHARGEDHIGGSHW_H
#include "interface/AnalysisBase.hpp"

class ChargedHiggsHW:  virtual public AnalysisBase
{
    public:
        virtual void Init();
        virtual int analyze(Event*,string);
        virtual int VetoFirst(Event*,string);
        virtual int VetoSecond(Event*,string);
        //Double_t deltaR(Double_t, Double_t, Double_t, Double_t);
        Double_t neutrinoPz(TLorentzVector, TLorentzVector, Int_t);
        virtual const string name(){return "ChargedHiggsHW";}

        static constexpr float Mw = 80.385;
        static constexpr float Mh = 125.;
        static constexpr float Mtop = 173.34;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
