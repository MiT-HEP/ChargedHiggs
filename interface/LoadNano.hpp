#ifndef LOADNANO_H
#define LOADNANO_H

#include "interface/Loader.hpp"
#include <map>
#include <string>

#define MAX_MUON 10
#define MAX_JET 20

class LoadNano : public Loader
{
    protected:
        // event info
        int run,lumi;
        long event;
        float rho;
        int npv;
        // met
        float met_pt, met_phi;
        //muon
        int nMuon;
        float Muon_eta[MAX_MUON];
        float Muon_pt[MAX_MUON];
        float Muon_phi[MAX_MUON];
        float Muon_mass[MAX_MUON];
        int Muon_charge[MAX_MUON];
        int Muon_nTrackerLayers[MAX_MUON];
        float Muon_miniPFRelIso_all[MAX_MUON];
        float Muon_pfRelIso04_all[MAX_MUON];
        bool Muon_mediumId[MAX_MUON];
        bool Muon_tightId[MAX_MUON];
        bool Muon_isPFcand[MAX_MUON];

        // jet
        int nJet;
        float  Jet_pt[MAX_JET];
        float  Jet_eta[MAX_JET];
        float  Jet_phi[MAX_JET];
        float  Jet_mass[MAX_JET];

        float  Jet_chEmEF[MAX_JET];
        float  Jet_neEmEF[MAX_JET];
        float  Jet_chHEF[MAX_JET];
        float  Jet_neHEF[MAX_JET];
        float  Jet_qgl[MAX_JET];
        float  Jet_bReg[MAX_JET];
        float  Jet_puId[MAX_JET];

        float  Jet_jetId[MAX_JET];
        float  Jet_btagDeepB[MAX_JET];
        float  Jet_btagCSVV2[MAX_JET];

        
    public:
    const string name() const override { return "LoadNano" ;} 
    const string chain() const override{ return "Events";} // name of the tree in the file

    int InitTree()  override ;
    int FillEvent() override  ;
    void FillEventInfo() override  ; // pre new file check: fill isReal data
    void NewFile() override ;
    void Clear() override;
};

#endif
