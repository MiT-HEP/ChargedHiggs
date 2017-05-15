
#ifndef CHARGEDHIGGS_EWKPURITY_H
#define CHARGEDHIGGS_EWKPURITY_H

#include "interface/AnalysisBase.hpp"
#include "interface/GeneralFunctions.hpp"
#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include <memory>

// FWD Declaration
namespace Pythia8{
    class Pythia;
};

namespace CLHEP {
  class HepRandomEngine;
}

#include "TRandom.h"
#include "TRandom3.h"

class ChargedHiggsEWKPurity:  virtual public ChargedHiggsTauNu
{
    public:
        void Init() override;
        int analyze(Event*,string systname) override;
        const string name() const override{return "ChargedHiggsEWKPurity";}
        const string dir="ChargedHiggsEWKPurity/Vars/";
        const string none="ChargedHiggsEWKPurity/NOne/";

        bool doPythia{false};

        // Isolation as for taus -- hard coded. The value here is used by the jet rejection
        void SetLeptonCuts(Lepton *l) override { l->SetIsoCut(10); l->SetPtCut(10);l->SetIsoRelCut(-1);l->SetEtaCut(2.4); l->SetTightCut(false);}

    private:
        std::unique_ptr<Pythia8::Pythia> fMasterGen;
        std::unique_ptr<TRandom> random;
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
