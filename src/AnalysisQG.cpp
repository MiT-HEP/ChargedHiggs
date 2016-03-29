#include "interface/AnalysisQG.hpp"


void QGAnalysis::SetLeptonCuts(Lepton *l){ 
    l->SetIsoCut(-1); 
    l->SetPtCut(25); 
    l->SetIsoRelCut(0.15);
    l->SetEtaCut(2.4);
}

void QGAnalysis::Init(){

    Log(__FUNCTION__,"INFO","Booking Histos");

    for ( string l : AllLabel()  ) {
        Book ("QGAnalysis/CutFlow/CutFlow_"+ l ,"CutFlow;Events",10,-.5,10-.5);
        Book ("QGAnalysis/Vars/Mmm_"+ l ,"Mmm;m^{#mu#mu} [GeV];Events", 100,50,200);
        // 
        Book ("QGAnalysis/Vars/Ptmm_"+ l ,"Ptmm;p_{T}^{#mu#mu} [GeV];Events", 1000,0,1000);
        Book ("QGAnalysis/Vars/Npv_"+ l ,"Npvmm", 50,0,50);
        //
        for (const string& t : jetTypes ) {
            for (const string& v : jetVars ){

                if ( l == "Data" and t != "U") continue; // Data have no MC info

                float xmin=-1, xmax=1.0;
                int nbins=200;

                if ( v == "axis2") { xmin=0; xmax=10;}
                if ( v == "mult") { xmin=0; xmax=100;nbins=100;}

                Book ("QGAnalysis/Vars/"+v +"_"+ t+"_"+ l ,v+"_" + t, 200,-1,1.0);
                for(int ptb=0;ptb < ptBins.size()-1; ++ptb)         
                for(int aetab=0;aetab < aetaBins.size()-1; ++aetab)
                {
                    Book ("QGAnalysis/Vars/"+v+"_"+t+Form("_pt%.0f_%.0f",ptBins[ptb],ptBins[ptb+1]) + Form("_eta%.1f_%.1f",aetaBins[aetab],aetaBins[aetab+1])+"_"+ l ,v +"_" + t, nbins,xmin,xmax);
                } // pt-eta
            } // vars
        } // types
    } // labels

}

int QGAnalysis::analyzeMM(Event *e, string systname)
{
    string label = GetLabel(e);
    cut.reset();
    cut.SetMask(MaxCut-1) ;
    cut.SetCutBit( Total ) ;
    Fill("QGAnalysis/CutFlow/CutFlow_"+label,systname,Total,e->weight());

    Lepton*mu0 = e->GetMuon(0);
    Lepton*mu1 = e->GetMuon(1);
    Jet *j0 = e->GetJet(0);
    Jet *j1 = e->GetJet(1);

    if (mu0 == NULL or mu1 == NULL) return 0;

    cut.SetCutBit(Leptons);
    if (e->IsTriggered("HLT_IsoMu20") ) cut.SetCutBit(Trigger);
    Fill("QGAnalysis/CutFlow/CutFlow_"+label,systname,Trigger,e->weight());

    Object Z(*mu0);
    Z += *mu1;
    if (cut.passAllUpTo(Trigger) ){
        Fill("QGAnalysis/Vars/Mmm_"+ label,systname, Z.M(),e->weight()) ;
    }

    if (Z.M()> 70 && Z.M()<110) cut.SetCutBit(Mass);

    Fill("QGAnalysis/CutFlow/CutFlow_"+label,systname,Mass,e->weight());

    if ( j0 == NULL ) return 0; // 1 Jet is needed

    if ( j1 == NULL ) cut.SetCutBit(OneJet);
    if ( j1 != NULL and j1->Pt() < Z.Pt() * 0.3 ) cut.SetCutBit(OneJet);

    Fill("QGAnalysis/CutFlow/CutFlow_"+label,systname,OneJet,e->weight());
    if ( j0->DeltaPhi(Z) > 2.5 ) cut.SetCutBit(DeltaPhi);

    Fill("QGAnalysis/CutFlow/CutFlow_"+label,systname,DeltaPhi,e->weight());

    if (cut.passAll() ){
        Fill("QGAnalysis/CutFlow/CutFlow_"+label,systname,MaxCut,e->weight());
        Fill("QGAnalysis/Vars/Ptmm_"+ label,systname, Z.Pt(),e->weight()) ;
        Fill("QGAnalysis/Vars/Npv_"+ label,systname, e->Npv(),e->weight()) ;
        string etaStr=Binning::findBinStr( aetaBins, fabs(j0->Eta()), "eta%.1f_%.1f");
        string ptStr=Binning::findBinStr( ptBins, j0->Pt(), "pt%.0f_%.0f");
        int flavor = abs(j0->Flavor());
        string type="U";
        if (flavor == 21) type="G";
        else if (flavor < 6 and flavor != 0) type="Q"; 
           
        // global 
        Fill("QGAnalysis/Vars/QGL_" +type +"_"+ label, systname, j0->QGL(),e->weight()) ;
         
        Fill("QGAnalysis/Vars/mult_" +type +"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
        Fill("QGAnalysis/Vars/ptD_" +type +"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
        Fill("QGAnalysis/Vars/axis2_" +type +"_"+ label, systname, j0->QGLVar("axis2"),e->weight()) ;
        // binned
        if (etaStr != "NotFound" and ptStr != "NotFound")
        {
            Fill("QGAnalysis/Vars/QGL_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGL(),e->weight()) ;
            Fill("QGAnalysis/Vars/mult_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("mult"),e->weight()) ;
            Fill("QGAnalysis/Vars/ptD_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, j0->QGLVar("ptD"),e->weight()) ;
            Fill("QGAnalysis/Vars/axis2_" +type +"_" + ptStr + "_" + etaStr+"_"+ label, systname, -TMath::Log(j0->QGLVar("axis2")),e->weight()) ;
        }

    }

    return 0;
}


int QGAnalysis::analyze(Event *e, string systname)
{

    if (e->weight() == 0.) Log(__FUNCTION__,"WARNING","Event Weight is NULL");
    analyzeMM(e,systname);
    return 0;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
