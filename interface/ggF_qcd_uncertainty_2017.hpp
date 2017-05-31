#ifndef LHCHXSWG_H
#define LHCHXSWG_H

#include <vector>

namespace LHCHXSWG{

    typedef std::vector<double> NumV;

    // The input kinematics should be based on the truth quantites of
    // defined according to:
    // https://svnweb.cern.ch/cern/wsvn/lhchiggsxs/repository/TemplateXS/HiggsTemplateCrossSections.h
    // namely Higgs boson pT (in GeV!), jet multiplicity with 30 GeV pT threshold
    // of jets built ignoring all stable particles originating from the Higgs boson decay
    // and the Stage1 STXS category
    // In the code above, these quanties are part of the HiggsClassification struct and called:
    //   higgs.Pt(), jets30.size(), stage1_cat_pTjet30GeV
    //
    // Note: the stage 1 STXS index is only used to determine if the current event fulfil the
    //       VBF topology selection. I.e. only categories
    //         GG2H_VBFTOPO_JET3VETO = 101, GG2H_VBFTOPO_JET3 = 102,
    //       are checked
    //
    // Dag Gillberg, March 21, 2017
    
    //
    // Fractional uncertainty amplitudes of the "WG1 scheme", the "STXS scheme" and the merged "2017 scheme"
    // The six first numbers are the same from each method below, namely the uncertainty amplitude of the jet bins:
    // mu, res, mig01, mig12, vbf2j, vbf3j
    // The last numbers are pT dependent uncertainies
    NumV qcd_ggF_uncert_wg1(int Njets30, double pTH, int STXS);  // 7 nuisances, 5 x jetbin, pTH, qm_t
    NumV qcd_ggF_uncert_stxs(int Njets30, double pTH, int STXS); // 8 nuisances, 5 x jetbin, D60, D120, D200
    NumV qcd_ggF_uncert_2017(int Njets30, double pTH, int STXS); // 8 nuisances, 5 x jetbin, pT60, pT120, qm_t
    NumV qcd_ggF_uncert_jve(int Njets30, double pTH, int STXS);  // 7 nuisances, 4 x jetbin, pT60, pT120, qm_t
    
    //
    // Scale factors defined as "1+uncert", where uncert is the fractional uncertainty amplitude
    // This can be treated as an event weight to propagate the uncertainty to any observable/distribution.
    NumV qcd_ggF_uncertSF_wg1(int Njets30, double pTH, int STXS_Stage1, double Nsigma=1.0);
    NumV qcd_ggF_uncertSF_stxs(int Njets30, double pTH, int STXS_Stage1, double Nsigma=1.0);
    NumV qcd_ggF_uncertSF_2017(int Njets30, double pTH, int STXS_Stage1, double Nsigma=1.0);
    NumV qcd_ggF_uncertSF_jve(int Njets30, double pTH, int STXS_Stage1, double Nsigma=1.0);
    
    // Cross sections of ggF with =0, =1, and >=2 jets
    // Obtained from Powheg NNLOPS. Scaled to sigma(N3LO) @125.09 = 48.52 pb
    // set as global variables (sorry!) since used both by BLPTW and JVE
    static double g_sig0=30.117, g_sig1=12.928, g_sig_ge2=5.475,
      g_sig_ge1 = g_sig1+g_sig_ge2, g_sig_tot=g_sig0+g_sig_ge1, g_sig_vbfTopo = 0.630,
      g_sig_ge2noVBF=g_sig_ge2-g_sig_vbfTopo, g_sig_ge1noVBF=g_sig_ge1-g_sig_vbfTopo;

    NumV blptw(int Njets30) ;
    double vbf_2j(int STXS) ;
    double vbf_3j(int STXS) ;
    double interpol(double x, double x1, double y1, double x2, double y2) ;
    double qm_t(double pT) ;
    double pT120(double pT, int Njets30) ;
    double pT60(double pT, int Njets30) ;
    NumV jetBinUnc(int Njets30, int STXS) ;
    NumV unc2sf(const NumV &unc, double Nsigma) ;
};

#endif
