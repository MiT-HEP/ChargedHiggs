#ifndef LOADNANO_H
#define LOADNANO_H

#include "interface/Loader.hpp"
#include <map>
#include <string>

#define MAX_MUON

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
