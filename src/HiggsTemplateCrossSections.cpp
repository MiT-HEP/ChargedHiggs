#ifndef TRUTHRIVETTOOLS_HIGGSTEMPLATECROSSSECTIONS_CC
#define TRUTHRIVETTOOLS_HIGGSTEMPLATECROSSSECTIONS_CC

/* Original code: https://svnweb.cern.ch/cern/wsvn/lhchiggsxs/repository/TemplateXS/HiggsTemplateCrossSections.h
 * modified by Andrea Carlo Marini in order to be ROOT/C++ only based
 * Date: Wed May 31 10:36:35 CEST 2017
 */

// Definition of the StatusCode and Category enums
#include "interface/HiggsTemplateCrossSections.hpp"

using namespace std;
using namespace HTXS;


int HTXS::vbfTopology(const vector<TLorentzVector> &jets, const TLorentzVector &higgs) {
    if (jets.size()<2) return 0;
    const auto & j1 = jets[0];
    const auto & j2 = jets[1];
    //const FourMomentum &j1=jets[0].momentum(), &j2=jets[1].momentum();
    bool VBFtopo = (j1+j2).M() > 400.0 && std::abs(j1.Rapidity()-j2.Rapidity()) > 2.8;
    return VBFtopo ? (j1+j2+higgs).Pt()<25 ? 2 : 1 : 0;
}

bool HTXS::isVH(HTXS::HiggsProdMode p) { return p==HTXS::WH || p==HTXS::QQ2ZH || p==HTXS::GG2ZH; }

HTXS::Stage1::Category HTXS::getStage1Category(const HTXS::HiggsProdMode prodMode,
        const TLorentzVector &higgs,
        const std::vector<TLorentzVector> &jets,
        const TLorentzVector &V, bool quarkDecayed) {
    using namespace HTXS::Stage1;
    int Njets=jets.size(), ctrlHiggs = std::abs(higgs.Rapidity())<2.5, fwdHiggs = !ctrlHiggs;
    double pTj1 = jets.size() ? jets[0].Pt() : 0;
    int vbfTopo = vbfTopology(jets,higgs);

    // 1. GGF Stage 1 categories
    //    Following YR4 write-up: XXXXX
    if (prodMode==HTXS::GGF || (prodMode==HTXS::GG2ZH && quarkDecayed) ) {
        if (fwdHiggs)        return GG2H_FWDH;
        if (Njets==0)        return GG2H_0J;
        else if (Njets==1)   return Category(GG2H_1J_PTH_0_60+getBin(higgs.Pt(),{0,60,120,200}));
        else if (Njets>=2) {
            // events with pT_H>200 get priority over VBF cuts
            if(higgs.Pt()<=200){
                if      (vbfTopo==2) return GG2H_VBFTOPO_JET3VETO;
                else if (vbfTopo==1) return GG2H_VBFTOPO_JET3;
            }
            // Njets >= 2jets without VBF topology
            return Category(GG2H_GE2J_PTH_0_60+getBin(higgs.Pt(),{0,60,120,200}));
        }
    }
    // 2. Electroweak qq->Hqq Stage 1 categories
    else if (prodMode==HTXS::VBF || ( isVH(prodMode) && quarkDecayed) ) {
        if (std::abs(higgs.Rapidity())>2.5) return QQ2HQQ_FWDH;
        if (pTj1>200) return QQ2HQQ_PTJET1_GT200;
        if (vbfTopo==2) return QQ2HQQ_VBFTOPO_JET3VETO;
        if (vbfTopo==1) return QQ2HQQ_VBFTOPO_JET3;
        double mjj = jets.size()>1 ? (jets[0]+jets[1]).M():0;
        if ( 60 < mjj && mjj < 120 ) return QQ2HQQ_VH2JET;
        return QQ2HQQ_REST;
    }
    // 3. WH->Hlv categories
    else if (prodMode==HTXS::WH) {
        if (fwdHiggs) return QQ2HLNU_FWDH;
        else if (V.Pt()<150) return QQ2HLNU_PTV_0_150;
        else if (V.Pt()>250) return QQ2HLNU_PTV_GT250;
        // 150 < pTV/GeV < 250
        return jets.size()==0 ? QQ2HLNU_PTV_150_250_0J : QQ2HLNU_PTV_150_250_GE1J;
    }
    // 4. qq->ZH->llH categories
    else if (prodMode==HTXS::QQ2ZH) {
        if (fwdHiggs) return QQ2HLL_FWDH;
        else if (V.Pt()<150) return QQ2HLL_PTV_0_150;
        else if (V.Pt()>250) return QQ2HLL_PTV_GT250;
        // 150 < pTV/GeV < 250
        return jets.size()==0 ? QQ2HLL_PTV_150_250_0J : QQ2HLL_PTV_150_250_GE1J;
    }
    // 5. gg->ZH->llH categories
    else if (prodMode==HTXS::GG2ZH ) {
        if (fwdHiggs) return GG2HLL_FWDH;
        if      (V.Pt()<150) return GG2HLL_PTV_0_150;
        else if (jets.size()==0) return GG2HLL_PTV_GT150_0J;
        return GG2HLL_PTV_GT150_GE1J;
    }
    // 6.ttH,bbH,tH categories
    else if (prodMode==HTXS::TTH) return Category(TTH_FWDH+ctrlHiggs);
    else if (prodMode==HTXS::BBH) return Category(BBH_FWDH+ctrlHiggs);
    else if (prodMode==HTXS::TH ) return Category(TH_FWDH+ctrlHiggs);
    return Category::UNKNOWN;
}

int HTXS::getBin(double x, std::vector<double> bins) {                                          
    if (bins.size()==0||x<bins[0]) return -1; // should not happen!                         
    for (size_t i=1;i<bins.size();++i)                                                      
        if (x<bins[i]) return i-1;                                                            
    return bins.size()-1;                                                                   
}   

#endif
