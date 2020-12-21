//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Dec 18 15:27:30 2020 by ROOT version 6.12/07
// from TTree Events/Events
// found on file: /eos/cms/store/mc/RunIISummer20UL16NanoAODv2/tZq_ll_4f_ckm_NLO_TuneCP5_13TeV-amcatnlo-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v15-v1/50000/1391F854-3216-B248-AC30-7E1581BB4816.root
// modified by Andrea Carlo Marini
//////////////////////////////////////////////////////////

#ifndef nanov8_h
#define nanov8_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class nanov8 {
public :
   int year=2016;

   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   UInt_t          run;
   UInt_t          luminosityBlock;
   ULong64_t       event;
   Float_t         HTXS_Higgs_pt;
   Float_t         HTXS_Higgs_y;
   Int_t           HTXS_stage1_1_cat_pTjet25GeV;
   Int_t           HTXS_stage1_1_cat_pTjet30GeV;
   Int_t           HTXS_stage1_1_fine_cat_pTjet25GeV;
   Int_t           HTXS_stage1_1_fine_cat_pTjet30GeV;
   Int_t           HTXS_stage1_2_cat_pTjet25GeV;
   Int_t           HTXS_stage1_2_cat_pTjet30GeV;
   Int_t           HTXS_stage1_2_fine_cat_pTjet25GeV;
   Int_t           HTXS_stage1_2_fine_cat_pTjet30GeV;
   Int_t           HTXS_stage_0;
   Int_t           HTXS_stage_1_pTjet25;
   Int_t           HTXS_stage_1_pTjet30;
   UChar_t         HTXS_njets25;
   UChar_t         HTXS_njets30;
   Float_t         btagWeight_CSVV2;
   Float_t         btagWeight_DeepCSVB;
   Float_t         CaloMET_phi;
   Float_t         CaloMET_pt;
   Float_t         CaloMET_sumEt;
   Float_t         ChsMET_phi;
   Float_t         ChsMET_pt;
   Float_t         ChsMET_sumEt;
   UInt_t          nCorrT1METJet;
   Float_t         CorrT1METJet_area[23];   //[nCorrT1METJet]
   Float_t         CorrT1METJet_eta[23];   //[nCorrT1METJet]
   Float_t         CorrT1METJet_muonSubtrFactor[23];   //[nCorrT1METJet]
   Float_t         CorrT1METJet_phi[23];   //[nCorrT1METJet]
   Float_t         CorrT1METJet_rawPt[23];   //[nCorrT1METJet]
   Float_t         DeepMETResolutionTune_phi;
   Float_t         DeepMETResolutionTune_pt;
   Float_t         DeepMETResponseTune_phi;
   Float_t         DeepMETResponseTune_pt;
   UInt_t          nElectron;
   Float_t         Electron_deltaEtaSC[9];   //[nElectron]
   Float_t         Electron_dr03EcalRecHitSumEt[9];   //[nElectron]
   Float_t         Electron_dr03HcalDepth1TowerSumEt[9];   //[nElectron]
   Float_t         Electron_dr03TkSumPt[9];   //[nElectron]
   Float_t         Electron_dr03TkSumPtHEEP[9];   //[nElectron]
   Float_t         Electron_dxy[9];   //[nElectron]
   Float_t         Electron_dxyErr[9];   //[nElectron]
   Float_t         Electron_dz[9];   //[nElectron]
   Float_t         Electron_dzErr[9];   //[nElectron]

   Float_t         Electron_eInvMinusPInv[9];   //[nElectron]
   Float_t         Electron_energyErr[9];   //[nElectron]
   Float_t         Electron_eta[9];   //[nElectron]
   Float_t         Electron_hoe[9];   //[nElectron]
   Float_t         Electron_ip3d[9];   //[nElectron]
   Float_t         Electron_jetPtRelv2[9];   //[nElectron]
   Float_t         Electron_jetRelIso[9];   //[nElectron]
   Float_t         Electron_mass[9];   //[nElectron]
   Float_t         Electron_miniPFRelIso_all[9];   //[nElectron]
   Float_t         Electron_miniPFRelIso_chg[9];   //[nElectron]
   Float_t         Electron_mvaFall17V1Iso[9];   //[nElectron]
   Float_t         Electron_mvaFall17V1noIso[9];   //[nElectron]
   Float_t         Electron_mvaFall17V2Iso[9];   //[nElectron]
   Float_t         Electron_mvaFall17V2noIso[9];   //[nElectron]
   Float_t         Electron_pfRelIso03_all[9];   //[nElectron]
   Float_t         Electron_pfRelIso03_chg[9];   //[nElectron]
   Float_t         Electron_phi[9];   //[nElectron]
   Float_t         Electron_pt[9];   //[nElectron]
   Float_t         Electron_r9[9];   //[nElectron]
   Float_t         Electron_scEtOverPt[9];   //[nElectron]
   Float_t         Electron_sieie[9];   //[nElectron]
   Float_t         Electron_sip3d[9];   //[nElectron]
   Float_t         Electron_mvaTTH[9];   //[nElectron]
   Int_t           Electron_charge[9];   //[nElectron]
   Int_t           Electron_cutBased[9];   //[nElectron]
   Int_t           Electron_cutBased_Fall17_V1[9];   //[nElectron]
   Int_t           Electron_jetIdx[9];   //[nElectron]
   Int_t           Electron_pdgId[9];   //[nElectron]
   Int_t           Electron_photonIdx[9];   //[nElectron]
   Int_t           Electron_tightCharge[9];   //[nElectron]
   Int_t           Electron_vidNestedWPBitmap[9];   //[nElectron]
   Int_t           Electron_vidNestedWPBitmapHEEP[9];   //[nElectron]
   Bool_t          Electron_convVeto[9];   //[nElectron]
   Bool_t          Electron_cutBased_HEEP[9];   //[nElectron]
   Bool_t          Electron_isPFcand[9];   //[nElectron]
   UChar_t         Electron_jetNDauCharged[9];   //[nElectron]
   UChar_t         Electron_lostHits[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V1Iso_WP80[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V1Iso_WP90[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V1Iso_WPL[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V1noIso_WP80[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V1noIso_WP90[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V1noIso_WPL[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V2Iso_WP80[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V2Iso_WP90[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V2Iso_WPL[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V2noIso_WP80[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V2noIso_WP90[9];   //[nElectron]
   Bool_t          Electron_mvaFall17V2noIso_WPL[9];   //[nElectron]
   UChar_t         Electron_seedGain[9];   //[nElectron]
   UInt_t          nFatJet;
   Float_t         FatJet_area[6];   //[nFatJet]
   Float_t         FatJet_btagCMVA[6];   //[nFatJet]
   Float_t         FatJet_btagCSVV2[6];   //[nFatJet]
   Float_t         FatJet_btagDDBvL[6];   //[nFatJet]
   Float_t         FatJet_btagDDBvLV2[6];   //[nFatJet]
   Float_t         FatJet_btagDDBvL_noMD[6];   //[nFatJet]
   Float_t         FatJet_btagDDCvB[6];   //[nFatJet]
   Float_t         FatJet_btagDDCvBV2[6];   //[nFatJet]
   Float_t         FatJet_btagDDCvB_noMD[6];   //[nFatJet]
   Float_t         FatJet_btagDDCvL[6];   //[nFatJet]
   Float_t         FatJet_btagDDCvLV2[6];   //[nFatJet]
   Float_t         FatJet_btagDDCvL_noMD[6];   //[nFatJet]
   Float_t         FatJet_btagDeepB[6];   //[nFatJet]
   Float_t         FatJet_btagHbb[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_H4qvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_HbbvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_TvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_WvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_ZHbbvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_ZHccvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_ZbbvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_ZvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_bbvsLight[6];   //[nFatJet]
   Float_t         FatJet_deepTagMD_ccvsLight[6];   //[nFatJet]
   Float_t         FatJet_deepTag_H[6];   //[nFatJet]
   Float_t         FatJet_deepTag_QCD[6];   //[nFatJet]
   Float_t         FatJet_deepTag_QCDothers[6];   //[nFatJet]
   Float_t         FatJet_deepTag_TvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTag_WvsQCD[6];   //[nFatJet]
   Float_t         FatJet_deepTag_ZvsQCD[6];   //[nFatJet]
   Float_t         FatJet_eta[6];   //[nFatJet]
   Float_t         FatJet_mass[6];   //[nFatJet]
   Float_t         FatJet_msoftdrop[6];   //[nFatJet]
   Float_t         FatJet_n2b1[6];   //[nFatJet]
   Float_t         FatJet_n3b1[6];   //[nFatJet]
   Float_t         FatJet_particleNetMD_QCD[6];   //[nFatJet]
   Float_t         FatJet_particleNetMD_Xbb[6];   //[nFatJet]
   Float_t         FatJet_particleNetMD_Xcc[6];   //[nFatJet]
   Float_t         FatJet_particleNetMD_Xqq[6];   //[nFatJet]
   Float_t         FatJet_particleNet_H4qvsQCD[6];   //[nFatJet]
   Float_t         FatJet_particleNet_HbbvsQCD[6];   //[nFatJet]
   Float_t         FatJet_particleNet_HccvsQCD[6];   //[nFatJet]
   Float_t         FatJet_particleNet_QCD[6];   //[nFatJet]
   Float_t         FatJet_particleNet_TvsQCD[6];   //[nFatJet]
   Float_t         FatJet_particleNet_WvsQCD[6];   //[nFatJet]
   Float_t         FatJet_particleNet_ZvsQCD[6];   //[nFatJet]
   Float_t         FatJet_phi[6];   //[nFatJet]
   Float_t         FatJet_pt[6];   //[nFatJet]
   Float_t         FatJet_rawFactor[6];   //[nFatJet]
   Float_t         FatJet_tau1[6];   //[nFatJet]
   Float_t         FatJet_tau2[6];   //[nFatJet]
   Float_t         FatJet_tau3[6];   //[nFatJet]
   Float_t         FatJet_tau4[6];   //[nFatJet]
   Float_t         FatJet_lsf3[6];   //[nFatJet]
   Int_t           FatJet_jetId[6];   //[nFatJet]
   Int_t           FatJet_subJetIdx1[6];   //[nFatJet]
   Int_t           FatJet_subJetIdx2[6];   //[nFatJet]
   Int_t           FatJet_electronIdx3SJ[6];   //[nFatJet]
   Int_t           FatJet_muonIdx3SJ[6];   //[nFatJet]
   UInt_t          nFsrPhoton;
   Float_t         FsrPhoton_dROverEt2[3];   //[nFsrPhoton]
   Float_t         FsrPhoton_eta[3];   //[nFsrPhoton]
   Float_t         FsrPhoton_phi[3];   //[nFsrPhoton]
   Float_t         FsrPhoton_pt[3];   //[nFsrPhoton]
   Float_t         FsrPhoton_relIso03[3];   //[nFsrPhoton]
   Int_t           FsrPhoton_muonIdx[3];   //[nFsrPhoton]
   UInt_t          nGenJetAK8;
   Float_t         GenJetAK8_eta[7];   //[nGenJetAK8]
   Float_t         GenJetAK8_mass[7];   //[nGenJetAK8]
   Float_t         GenJetAK8_phi[7];   //[nGenJetAK8]
   Float_t         GenJetAK8_pt[7];   //[nGenJetAK8]
   UInt_t          nGenJet;
   Float_t         GenJet_eta[22];   //[nGenJet]
   Float_t         GenJet_mass[22];   //[nGenJet]
   Float_t         GenJet_phi[22];   //[nGenJet]
   Float_t         GenJet_pt[22];   //[nGenJet]
   UInt_t          nGenPart;
   Float_t         GenPart_eta[131];   //[nGenPart]
   Float_t         GenPart_mass[131];   //[nGenPart]
   Float_t         GenPart_phi[131];   //[nGenPart]
   Float_t         GenPart_pt[131];   //[nGenPart]
   Int_t           GenPart_genPartIdxMother[131];   //[nGenPart]
   Int_t           GenPart_pdgId[131];   //[nGenPart]
   Int_t           GenPart_status[131];   //[nGenPart]
   Int_t           GenPart_statusFlags[131];   //[nGenPart]
   UInt_t          nSubGenJetAK8;
   Float_t         SubGenJetAK8_eta[14];   //[nSubGenJetAK8]
   Float_t         SubGenJetAK8_mass[14];   //[nSubGenJetAK8]
   Float_t         SubGenJetAK8_phi[14];   //[nSubGenJetAK8]
   Float_t         SubGenJetAK8_pt[14];   //[nSubGenJetAK8]
   Float_t         Generator_binvar;
   Float_t         Generator_scalePDF;
   Float_t         Generator_weight;
   Float_t         Generator_x1;
   Float_t         Generator_x2;
   Float_t         Generator_xpdf1;
   Float_t         Generator_xpdf2;
   Int_t           Generator_id1;
   Int_t           Generator_id2;
   Float_t         GenVtx_x;
   Float_t         GenVtx_y;
   Float_t         GenVtx_z;
   UInt_t          nGenVisTau;
   Float_t         GenVisTau_eta[4];   //[nGenVisTau]
   Float_t         GenVisTau_mass[4];   //[nGenVisTau]
   Float_t         GenVisTau_phi[4];   //[nGenVisTau]
   Float_t         GenVisTau_pt[4];   //[nGenVisTau]
   Int_t           GenVisTau_charge[4];   //[nGenVisTau]
   Int_t           GenVisTau_genPartIdxMother[4];   //[nGenVisTau]
   Int_t           GenVisTau_status[4];   //[nGenVisTau]
   Float_t         genWeight;
   Float_t         LHEWeight_originalXWGTUP;
   UInt_t          nLHEPdfWeight;
   Float_t         LHEPdfWeight[101];   //[nLHEPdfWeight]
   UInt_t          nLHEReweightingWeight;
   Float_t         LHEReweightingWeight[1];   //[nLHEReweightingWeight]
   UInt_t          nLHEScaleWeight;
   Float_t         LHEScaleWeight[9];   //[nLHEScaleWeight]
   UInt_t          nPSWeight;
   Float_t         PSWeight[4];   //[nPSWeight]
   UInt_t          nIsoTrack;
   Float_t         IsoTrack_dxy[6];   //[nIsoTrack]
   Float_t         IsoTrack_dz[6];   //[nIsoTrack]
   Float_t         IsoTrack_eta[6];   //[nIsoTrack]
   Float_t         IsoTrack_pfRelIso03_all[6];   //[nIsoTrack]
   Float_t         IsoTrack_pfRelIso03_chg[6];   //[nIsoTrack]
   Float_t         IsoTrack_phi[6];   //[nIsoTrack]
   Float_t         IsoTrack_pt[6];   //[nIsoTrack]
   Float_t         IsoTrack_miniPFRelIso_all[6];   //[nIsoTrack]
   Float_t         IsoTrack_miniPFRelIso_chg[6];   //[nIsoTrack]
   Int_t           IsoTrack_fromPV[6];   //[nIsoTrack]
   Int_t           IsoTrack_pdgId[6];   //[nIsoTrack]
   Bool_t          IsoTrack_isHighPurityTrack[6];   //[nIsoTrack]
   Bool_t          IsoTrack_isPFcand[6];   //[nIsoTrack]
   Bool_t          IsoTrack_isFromLostTrack[6];   //[nIsoTrack]
   UInt_t          nJet;
   Float_t         Jet_area[27];   //[nJet]
   Float_t         Jet_btagCMVA[27];   //[nJet]
   Float_t         Jet_btagCSVV2[27];   //[nJet]
   Float_t         Jet_btagDeepB[27];   //[nJet]
   Float_t         Jet_btagDeepC[27];   //[nJet]
   Float_t         Jet_btagDeepCvB[27];   //[nJet]
   Float_t         Jet_btagDeepCvL[27];   //[nJet]
   Float_t         Jet_btagDeepFlavB[27];   //[nJet]
   Float_t         Jet_btagDeepFlavC[27];   //[nJet]
   Float_t         Jet_btagDeepFlavCvB[27];   //[nJet]
   Float_t         Jet_btagDeepFlavCvL[27];   //[nJet]
   Float_t         Jet_btagDeepFlavQG[27];   //[nJet]
   Float_t         Jet_chEmEF[27];   //[nJet]
   Float_t         Jet_chFPV0EF[27];   //[nJet]
   Float_t         Jet_chFPV1EF[27];   //[nJet]
   Float_t         Jet_chFPV2EF[27];   //[nJet]
   Float_t         Jet_chFPV3EF[27];   //[nJet]
   Float_t         Jet_chHEF[27];   //[nJet]
   Float_t         Jet_eta[27];   //[nJet]
   Float_t         Jet_hfsigmaEtaEta[27];   //[nJet]
   Float_t         Jet_hfsigmaPhiPhi[27];   //[nJet]
   Float_t         Jet_mass[27];   //[nJet]
   Float_t         Jet_muEF[27];   //[nJet]
   Float_t         Jet_muonSubtrFactor[27];   //[nJet]
   Float_t         Jet_neEmEF[27];   //[nJet]
   Float_t         Jet_neHEF[27];   //[nJet]
   Float_t         Jet_phi[27];   //[nJet]
   Float_t         Jet_pt[27];   //[nJet]
   Float_t         Jet_puIdDisc[27];   //[nJet]
   Float_t         Jet_qgl[27];   //[nJet]
   Float_t         Jet_rawFactor[27];   //[nJet]
   Float_t         Jet_bRegCorr[27];   //[nJet]
   Float_t         Jet_bRegRes[27];   //[nJet]
   Float_t         Jet_cRegCorr[27];   //[nJet]
   Float_t         Jet_cRegRes[27];   //[nJet]
   Int_t           Jet_electronIdx1[27];   //[nJet]
   Int_t           Jet_electronIdx2[27];   //[nJet]
   Int_t           Jet_hfadjacentEtaStripsSize[27];   //[nJet]
   Int_t           Jet_hfcentralEtaStripSize[27];   //[nJet]
   Int_t           Jet_jetId[27];   //[nJet]
   Int_t           Jet_muonIdx1[27];   //[nJet]
   Int_t           Jet_muonIdx2[27];   //[nJet]
   Int_t           Jet_nConstituents[27];   //[nJet]
   Int_t           Jet_nElectrons[27];   //[nJet]
   Int_t           Jet_nMuons[27];   //[nJet]
   Int_t           Jet_puId[27];   //[nJet]
   Float_t         L1PreFiringWeight_Dn;
   Float_t         L1PreFiringWeight_Nom;
   Float_t         L1PreFiringWeight_Up;
   Float_t         LHE_HT;
   Float_t         LHE_HTIncoming;
   Float_t         LHE_Vpt;
   Float_t         LHE_AlphaS;
   UChar_t         LHE_Njets;
   UChar_t         LHE_Nb;
   UChar_t         LHE_Nc;
   UChar_t         LHE_Nuds;
   UChar_t         LHE_Nglu;
   UChar_t         LHE_NpNLO;
   UChar_t         LHE_NpLO;
   UInt_t          nLHEPart;
   Float_t         LHEPart_pt[10];   //[nLHEPart]
   Float_t         LHEPart_eta[10];   //[nLHEPart]
   Float_t         LHEPart_phi[10];   //[nLHEPart]
   Float_t         LHEPart_mass[10];   //[nLHEPart]
   Float_t         LHEPart_incomingpz[10];   //[nLHEPart]
   Int_t           LHEPart_pdgId[10];   //[nLHEPart]
   Int_t           LHEPart_status[10];   //[nLHEPart]
   Int_t           LHEPart_spin[10];   //[nLHEPart]
   Float_t         GenMET_phi;
   Float_t         GenMET_pt;
   Float_t         MET_MetUnclustEnUpDeltaX;
   Float_t         MET_MetUnclustEnUpDeltaY;
   Float_t         MET_covXX;
   Float_t         MET_covXY;
   Float_t         MET_covYY;
   Float_t         MET_phi;
   Float_t         MET_pt;
   Float_t         MET_significance;
   Float_t         MET_sumEt;
   Float_t         MET_sumPtUnclustered;
   UInt_t          nMuon;
   Float_t         Muon_dxy[9];   //[nMuon]
   Float_t         Muon_dxyErr[9];   //[nMuon]
   Float_t         Muon_dxybs[9];   //[nMuon]
   Float_t         Muon_dz[9];   //[nMuon]
   Float_t         Muon_dzErr[9];   //[nMuon]
   Float_t         Muon_eta[9];   //[nMuon]
   Float_t         Muon_ip3d[9];   //[nMuon]
   Float_t         Muon_jetPtRelv2[9];   //[nMuon]
   Float_t         Muon_jetRelIso[9];   //[nMuon]
   Float_t         Muon_mass[9];   //[nMuon]
   Float_t         Muon_miniPFRelIso_all[9];   //[nMuon]
   Float_t         Muon_miniPFRelIso_chg[9];   //[nMuon]
   Float_t         Muon_pfRelIso03_all[9];   //[nMuon]
   Float_t         Muon_pfRelIso03_chg[9];   //[nMuon]
   Float_t         Muon_pfRelIso04_all[9];   //[nMuon]
   Float_t         Muon_phi[9];   //[nMuon]
   Float_t         Muon_pt[9];   //[nMuon]
   Float_t         Muon_ptErr[9];   //[nMuon]
   Float_t         Muon_segmentComp[9];   //[nMuon]
   Float_t         Muon_sip3d[9];   //[nMuon]
   Float_t         Muon_softMva[9];   //[nMuon]
   Float_t         Muon_tkRelIso[9];   //[nMuon]
   Float_t         Muon_tunepRelPt[9];   //[nMuon]
   Float_t         Muon_mvaLowPt[9];   //[nMuon]
   Float_t         Muon_mvaTTH[9];   //[nMuon]
   Int_t           Muon_charge[9];   //[nMuon]
   Int_t           Muon_jetIdx[9];   //[nMuon]
   Int_t           Muon_nStations[9];   //[nMuon]
   Int_t           Muon_nTrackerLayers[9];   //[nMuon]
   Int_t           Muon_pdgId[9];   //[nMuon]
   Int_t           Muon_tightCharge[9];   //[nMuon]
   Int_t           Muon_fsrPhotonIdx[9];   //[nMuon]
   UChar_t         Muon_highPtId[9];   //[nMuon]
   Bool_t          Muon_highPurity[9];   //[nMuon]
   Bool_t          Muon_inTimeMuon[9];   //[nMuon]
   Bool_t          Muon_isGlobal[9];   //[nMuon]
   Bool_t          Muon_isPFcand[9];   //[nMuon]
   Bool_t          Muon_isTracker[9];   //[nMuon]
   UChar_t         Muon_jetNDauCharged[9];   //[nMuon]
   Bool_t          Muon_looseId[9];   //[nMuon]
   Bool_t          Muon_mediumId[9];   //[nMuon]
   Bool_t          Muon_mediumPromptId[9];   //[nMuon]
   UChar_t         Muon_miniIsoId[9];   //[nMuon]
   UChar_t         Muon_multiIsoId[9];   //[nMuon]
   UChar_t         Muon_mvaId[9];   //[nMuon]
   UChar_t         Muon_mvaLowPtId[9];   //[nMuon]
   UChar_t         Muon_pfIsoId[9];   //[nMuon]
   UChar_t         Muon_puppiIsoId[9];   //[nMuon]
   Bool_t          Muon_softId[9];   //[nMuon]
   Bool_t          Muon_softMvaId[9];   //[nMuon]
   Bool_t          Muon_tightId[9];   //[nMuon]
   UChar_t         Muon_tkIsoId[9];   //[nMuon]
   Bool_t          Muon_triggerIdLoose[9];   //[nMuon]
   UInt_t          nPhoton;
   Float_t         Photon_energyErr[10];   //[nPhoton]
   Float_t         Photon_eta[10];   //[nPhoton]
   Float_t         Photon_hoe[10];   //[nPhoton]
   Float_t         Photon_mass[10];   //[nPhoton]
   Float_t         Photon_mvaID[10];   //[nPhoton]
   Float_t         Photon_mvaID_Fall17V1p1[10];   //[nPhoton]
   Float_t         Photon_pfRelIso03_all[10];   //[nPhoton]
   Float_t         Photon_pfRelIso03_chg[10];   //[nPhoton]
   Float_t         Photon_phi[10];   //[nPhoton]
   Float_t         Photon_pt[10];   //[nPhoton]
   Float_t         Photon_r9[10];   //[nPhoton]
   Float_t         Photon_sieie[10];   //[nPhoton]
   Int_t           Photon_charge[10];   //[nPhoton]
   Int_t           Photon_cutBased[10];   //[nPhoton]
   Int_t           Photon_cutBased_Fall17V1Bitmap[10];   //[nPhoton]
   Int_t           Photon_electronIdx[10];   //[nPhoton]
   Int_t           Photon_jetIdx[10];   //[nPhoton]
   Int_t           Photon_pdgId[10];   //[nPhoton]
   Int_t           Photon_vidNestedWPBitmap[10];   //[nPhoton]
   Bool_t          Photon_electronVeto[10];   //[nPhoton]
   Bool_t          Photon_isScEtaEB[10];   //[nPhoton]
   Bool_t          Photon_isScEtaEE[10];   //[nPhoton]
   Bool_t          Photon_mvaID_WP80[10];   //[nPhoton]
   Bool_t          Photon_mvaID_WP90[10];   //[nPhoton]
   Bool_t          Photon_pixelSeed[10];   //[nPhoton]
   UChar_t         Photon_seedGain[10];   //[nPhoton]
   Float_t         Pileup_nTrueInt;
   Float_t         Pileup_pudensity;
   Float_t         Pileup_gpudensity;
   Int_t           Pileup_nPU;
   Int_t           Pileup_sumEOOT;
   Int_t           Pileup_sumLOOT;
   Float_t         PuppiMET_phi;
   Float_t         PuppiMET_phiJERDown;
   Float_t         PuppiMET_phiJERUp;
   Float_t         PuppiMET_phiJESDown;
   Float_t         PuppiMET_phiJESUp;
   Float_t         PuppiMET_phiUnclusteredDown;
   Float_t         PuppiMET_phiUnclusteredUp;
   Float_t         PuppiMET_pt;
   Float_t         PuppiMET_ptJERDown;
   Float_t         PuppiMET_ptJERUp;
   Float_t         PuppiMET_ptJESDown;
   Float_t         PuppiMET_ptJESUp;
   Float_t         PuppiMET_ptUnclusteredDown;
   Float_t         PuppiMET_ptUnclusteredUp;
   Float_t         PuppiMET_sumEt;
   Float_t         RawMET_phi;
   Float_t         RawMET_pt;
   Float_t         RawMET_sumEt;
   Float_t         RawPuppiMET_phi;
   Float_t         RawPuppiMET_pt;
   Float_t         RawPuppiMET_sumEt;
   Float_t         fixedGridRhoFastjetAll;
   Float_t         fixedGridRhoFastjetCentral;
   Float_t         fixedGridRhoFastjetCentralCalo;
   Float_t         fixedGridRhoFastjetCentralChargedPileUp;
   Float_t         fixedGridRhoFastjetCentralNeutral;
   UInt_t          nGenDressedLepton;
   Float_t         GenDressedLepton_eta[5];   //[nGenDressedLepton]
   Float_t         GenDressedLepton_mass[5];   //[nGenDressedLepton]
   Float_t         GenDressedLepton_phi[5];   //[nGenDressedLepton]
   Float_t         GenDressedLepton_pt[5];   //[nGenDressedLepton]
   Int_t           GenDressedLepton_pdgId[5];   //[nGenDressedLepton]
   Bool_t          GenDressedLepton_hasTauAnc[5];   //[nGenDressedLepton]
   UInt_t          nGenIsolatedPhoton;
   Float_t         GenIsolatedPhoton_eta[3];   //[nGenIsolatedPhoton]
   Float_t         GenIsolatedPhoton_mass[3];   //[nGenIsolatedPhoton]
   Float_t         GenIsolatedPhoton_phi[3];   //[nGenIsolatedPhoton]
   Float_t         GenIsolatedPhoton_pt[3];   //[nGenIsolatedPhoton]
   UInt_t          nSoftActivityJet;
   Float_t         SoftActivityJet_eta[6];   //[nSoftActivityJet]
   Float_t         SoftActivityJet_phi[6];   //[nSoftActivityJet]
   Float_t         SoftActivityJet_pt[6];   //[nSoftActivityJet]
   Float_t         SoftActivityJetHT;
   Float_t         SoftActivityJetHT10;
   Float_t         SoftActivityJetHT2;
   Float_t         SoftActivityJetHT5;
   Int_t           SoftActivityJetNjets10;
   Int_t           SoftActivityJetNjets2;
   Int_t           SoftActivityJetNjets5;
   UInt_t          nSubJet;
   Float_t         SubJet_btagCMVA[10];   //[nSubJet]
   Float_t         SubJet_btagCSVV2[10];   //[nSubJet]
   Float_t         SubJet_btagDeepB[10];   //[nSubJet]
   Float_t         SubJet_eta[10];   //[nSubJet]
   Float_t         SubJet_mass[10];   //[nSubJet]
   Float_t         SubJet_n2b1[10];   //[nSubJet]
   Float_t         SubJet_n3b1[10];   //[nSubJet]
   Float_t         SubJet_phi[10];   //[nSubJet]
   Float_t         SubJet_pt[10];   //[nSubJet]
   Float_t         SubJet_rawFactor[10];   //[nSubJet]
   Float_t         SubJet_tau1[10];   //[nSubJet]
   Float_t         SubJet_tau2[10];   //[nSubJet]
   Float_t         SubJet_tau3[10];   //[nSubJet]
   Float_t         SubJet_tau4[10];   //[nSubJet]
   UInt_t          nTau;
   Float_t         Tau_chargedIso[8];   //[nTau]
   Float_t         Tau_dxy[8];   //[nTau]
   Float_t         Tau_dz[8];   //[nTau]
   Float_t         Tau_eta[8];   //[nTau]
   Float_t         Tau_leadTkDeltaEta[8];   //[nTau]
   Float_t         Tau_leadTkDeltaPhi[8];   //[nTau]
   Float_t         Tau_leadTkPtOverTauPt[8];   //[nTau]
   Float_t         Tau_mass[8];   //[nTau]
   Float_t         Tau_neutralIso[8];   //[nTau]
   Float_t         Tau_phi[8];   //[nTau]
   Float_t         Tau_photonsOutsideSignalCone[8];   //[nTau]
   Float_t         Tau_pt[8];   //[nTau]
   Float_t         Tau_puCorr[8];   //[nTau]
   Float_t         Tau_rawAntiEle[8];   //[nTau]
   Float_t         Tau_rawAntiEle2018[8];   //[nTau]
   Float_t         Tau_rawDeepTau2017v2p1VSe[8];   //[nTau]
   Float_t         Tau_rawDeepTau2017v2p1VSjet[8];   //[nTau]
   Float_t         Tau_rawDeepTau2017v2p1VSmu[8];   //[nTau]
   Float_t         Tau_rawIso[8];   //[nTau]
   Float_t         Tau_rawIsodR03[8];   //[nTau]
   Float_t         Tau_rawMVAnewDM2017v2[8];   //[nTau]
   Float_t         Tau_rawMVAoldDM[8];   //[nTau]
   Float_t         Tau_rawMVAoldDM2017v1[8];   //[nTau]
   Float_t         Tau_rawMVAoldDM2017v2[8];   //[nTau]
   Float_t         Tau_rawMVAoldDMdR032017v2[8];   //[nTau]
   Int_t           Tau_charge[8];   //[nTau]
   Int_t           Tau_decayMode[8];   //[nTau]
   Int_t           Tau_jetIdx[8];   //[nTau]
   Int_t           Tau_rawAntiEleCat[8];   //[nTau]
   Int_t           Tau_rawAntiEleCat2018[8];   //[nTau]
   UChar_t         Tau_idAntiEle[8];   //[nTau]
   UChar_t         Tau_idAntiEle2018[8];   //[nTau]
   Bool_t          Tau_idAntiEleDeadECal[8];   //[nTau]
   UChar_t         Tau_idAntiMu[8];   //[nTau]
   Bool_t          Tau_idDecayMode[8];   //[nTau]
   Bool_t          Tau_idDecayModeNewDMs[8];   //[nTau]
   UChar_t         Tau_idDeepTau2017v2p1VSe[8];   //[nTau]
   UChar_t         Tau_idDeepTau2017v2p1VSjet[8];   //[nTau]
   UChar_t         Tau_idDeepTau2017v2p1VSmu[8];   //[nTau]
   UChar_t         Tau_idMVAnewDM2017v2[8];   //[nTau]
   UChar_t         Tau_idMVAoldDM[8];   //[nTau]
   UChar_t         Tau_idMVAoldDM2017v1[8];   //[nTau]
   UChar_t         Tau_idMVAoldDM2017v2[8];   //[nTau]
   UChar_t         Tau_idMVAoldDMdR032017v2[8];   //[nTau]
   Float_t         TkMET_phi;
   Float_t         TkMET_pt;
   Float_t         TkMET_sumEt;
   UInt_t          nTrigObj;
   Float_t         TrigObj_pt[49];   //[nTrigObj]
   Float_t         TrigObj_eta[49];   //[nTrigObj]
   Float_t         TrigObj_phi[49];   //[nTrigObj]
   Float_t         TrigObj_l1pt[49];   //[nTrigObj]
   Float_t         TrigObj_l1pt_2[49];   //[nTrigObj]
   Float_t         TrigObj_l2pt[49];   //[nTrigObj]
   Int_t           TrigObj_id[49];   //[nTrigObj]
   Int_t           TrigObj_l1iso[49];   //[nTrigObj]
   Int_t           TrigObj_l1charge[49];   //[nTrigObj]
   Int_t           TrigObj_filterBits[49];   //[nTrigObj]
   Int_t           genTtbarId;
   UInt_t          nOtherPV;
   Float_t         OtherPV_z[3];   //[nOtherPV]
   Float_t         PV_ndof;
   Float_t         PV_x;
   Float_t         PV_y;
   Float_t         PV_z;
   Float_t         PV_chi2;
   Float_t         PV_score;
   Int_t           PV_npvs;
   Int_t           PV_npvsGood;
   UInt_t          nSV;
   Float_t         SV_dlen[11];   //[nSV]
   Float_t         SV_dlenSig[11];   //[nSV]
   Float_t         SV_dxy[11];   //[nSV]
   Float_t         SV_dxySig[11];   //[nSV]
   Float_t         SV_pAngle[11];   //[nSV]
   Int_t           Electron_genPartIdx[9];   //[nElectron]
   UChar_t         Electron_genPartFlav[9];   //[nElectron]
   Int_t           FatJet_genJetAK8Idx[6];   //[nFatJet]
   Int_t           FatJet_hadronFlavour[6];   //[nFatJet]
   UChar_t         FatJet_nBHadrons[6];   //[nFatJet]
   UChar_t         FatJet_nCHadrons[6];   //[nFatJet]
   Int_t           GenJetAK8_partonFlavour[7];   //[nGenJetAK8]
   UChar_t         GenJetAK8_hadronFlavour[7];   //[nGenJetAK8]
   Int_t           GenJet_partonFlavour[22];   //[nGenJet]
   UChar_t         GenJet_hadronFlavour[22];   //[nGenJet]
   Float_t         GenVtx_t0;
   Int_t           Jet_genJetIdx[27];   //[nJet]
   Int_t           Jet_hadronFlavour[27];   //[nJet]
   Int_t           Jet_partonFlavour[27];   //[nJet]
   Int_t           Muon_genPartIdx[9];   //[nMuon]
   UChar_t         Muon_genPartFlav[9];   //[nMuon]
   Int_t           Photon_genPartIdx[10];   //[nPhoton]
   UChar_t         Photon_genPartFlav[10];   //[nPhoton]
   Float_t         MET_fiducialGenPhi;
   Float_t         MET_fiducialGenPt;
   UChar_t         Electron_cleanmask[9];   //[nElectron]
   UChar_t         Jet_cleanmask[27];   //[nJet]
   UChar_t         Muon_cleanmask[9];   //[nMuon]
   UChar_t         Photon_cleanmask[10];   //[nPhoton]
   UChar_t         Tau_cleanmask[8];   //[nTau]
   Int_t           SubJet_hadronFlavour[10];   //[nSubJet]
   UChar_t         SubJet_nBHadrons[10];   //[nSubJet]
   UChar_t         SubJet_nCHadrons[10];   //[nSubJet]
   Float_t         SV_chi2[11];   //[nSV]
   Float_t         SV_eta[11];   //[nSV]
   Float_t         SV_mass[11];   //[nSV]
   Float_t         SV_ndof[11];   //[nSV]
   Float_t         SV_phi[11];   //[nSV]
   Float_t         SV_pt[11];   //[nSV]
   Float_t         SV_x[11];   //[nSV]
   Float_t         SV_y[11];   //[nSV]
   Float_t         SV_z[11];   //[nSV]
   UChar_t         SV_ntracks[11];   //[nSV]
   Int_t           Tau_genPartIdx[8];   //[nTau]
   UChar_t         Tau_genPartFlav[8];   //[nTau]
   Bool_t          L1_AlwaysTrue;

   /*
   Bool_t          L1_BRIL_TRIG0_AND;
   Bool_t          L1_BRIL_TRIG0_FstBunchInTrain;
   Bool_t          L1_BRIL_TRIG0_OR;
   Bool_t          L1_BRIL_TRIG0_delayedAND;
   Bool_t          L1_BeamGasB1;
   Bool_t          L1_BeamGasB2;
   Bool_t          L1_BeamGasMinus;
   Bool_t          L1_BeamGasPlus;
   Bool_t          L1_BptxMinus;
   Bool_t          L1_BptxOR;
   Bool_t          L1_BptxPlus;
   Bool_t          L1_BptxXOR;
   Bool_t          L1_DoubleEG6_HTT255;
   Bool_t          L1_DoubleEG_15_10;
   Bool_t          L1_DoubleEG_18_17;
   Bool_t          L1_DoubleEG_20_18;
   Bool_t          L1_DoubleEG_22_10;
   Bool_t          L1_DoubleEG_22_12;
   Bool_t          L1_DoubleEG_22_15;
   Bool_t          L1_DoubleEG_23_10;
   Bool_t          L1_DoubleEG_24_17;
   Bool_t          L1_DoubleEG_25_12;
   Bool_t          L1_DoubleIsoTau28er;
   Bool_t          L1_DoubleIsoTau30er;
   Bool_t          L1_DoubleIsoTau32er;
   Bool_t          L1_DoubleIsoTau33er;
   Bool_t          L1_DoubleIsoTau34er;
   Bool_t          L1_DoubleIsoTau35er;
   Bool_t          L1_DoubleIsoTau36er;
   Bool_t          L1_DoubleJet12_ForwardBackward;
   Bool_t          L1_DoubleJet16_ForwardBackward;
   Bool_t          L1_DoubleJet8_ForwardBackward;
   Bool_t          L1_DoubleJetC100;
   Bool_t          L1_DoubleJetC112;
   Bool_t          L1_DoubleJetC120;
   Bool_t          L1_DoubleJetC40;
   Bool_t          L1_DoubleJetC50;
   Bool_t          L1_DoubleJetC60;
   Bool_t          L1_DoubleJetC60_ETM60;
   Bool_t          L1_DoubleJetC80;
   Bool_t          L1_DoubleMu0;
   Bool_t          L1_DoubleMu0_ETM40;
   Bool_t          L1_DoubleMu0_ETM55;
   Bool_t          L1_DoubleMu0er1p4_dEta_Max1p8_OS;
   Bool_t          L1_DoubleMu0er1p6_dEta_Max1p8;
   Bool_t          L1_DoubleMu0er1p6_dEta_Max1p8_OS;
   Bool_t          L1_DoubleMu7_EG14;
   Bool_t          L1_DoubleMu7_EG7;
   Bool_t          L1_DoubleMuOpen;
   Bool_t          L1_DoubleMu_10_0_dEta_Max1p8;
   Bool_t          L1_DoubleMu_10_3p5;
   Bool_t          L1_DoubleMu_10_Open;
   Bool_t          L1_DoubleMu_11_4;
   Bool_t          L1_DoubleMu_12_5;
   Bool_t          L1_DoubleMu_12_8;
   Bool_t          L1_DoubleMu_13_6;
   Bool_t          L1_DoubleMu_15_5;
   Bool_t          L1_DoubleTau50er;
   Bool_t          L1_EG25er_HTT125;
   Bool_t          L1_EG27er_HTT200;
   Bool_t          L1_ETM100;
   Bool_t          L1_ETM120;
   Bool_t          L1_ETM30;
   Bool_t          L1_ETM40;
   Bool_t          L1_ETM50;
   Bool_t          L1_ETM60;
   Bool_t          L1_ETM70;
   Bool_t          L1_ETM75;
   Bool_t          L1_ETM75_Jet60_dPhi_Min0p4;
   Bool_t          L1_ETM80;
   Bool_t          L1_ETM85;
   Bool_t          L1_ETM90;
   Bool_t          L1_ETM95;
   Bool_t          L1_ETT25;
   Bool_t          L1_ETT35_BptxAND;
   Bool_t          L1_ETT40_BptxAND;
   Bool_t          L1_ETT50_BptxAND;
   Bool_t          L1_ETT60_BptxAND;
   Bool_t          L1_FirstBunchAfterTrain;
   Bool_t          L1_FirstBunchInTrain;
   Bool_t          L1_HTM100;
   Bool_t          L1_HTM120;
   Bool_t          L1_HTM130;
   Bool_t          L1_HTM140;
   Bool_t          L1_HTM150;
   Bool_t          L1_HTM50;
   Bool_t          L1_HTM60_HTT260;
   Bool_t          L1_HTM70;
   Bool_t          L1_HTM80;
   Bool_t          L1_HTM80_HTT220;
   Bool_t          L1_HTT120;
   Bool_t          L1_HTT160;
   Bool_t          L1_HTT200;
   Bool_t          L1_HTT220;
   Bool_t          L1_HTT240;
   Bool_t          L1_HTT255;
   Bool_t          L1_HTT270;
   Bool_t          L1_HTT280;
   Bool_t          L1_HTT300;
   Bool_t          L1_HTT320;
   Bool_t          L1_IsoEG18er_IsoTau24er_dEta_Min0p2;
   Bool_t          L1_IsoEG20er_IsoTau25er_dEta_Min0p2;
   Bool_t          L1_IsoEG22er_IsoTau26er_dEta_Min0p2;
   Bool_t          L1_IsoEG22er_Tau20er_dEta_Min0p2;
   Bool_t          L1_IsolatedBunch;
   Bool_t          L1_Jet32_DoubleMu_10_0_dPhi_Jet_Mu0_Max0p4_dPhi_Mu_Mu_Min1p0;
   Bool_t          L1_Jet32_Mu0_EG10_dPhi_Jet_Mu_Max0p4_dPhi_Mu_EG_Min1p0;
   Bool_t          L1_MU20_EG15;
   Bool_t          L1_MinimumBiasHF0_AND;
   Bool_t          L1_MinimumBiasHF0_AND_BptxAND;
   Bool_t          L1_MinimumBiasHF0_OR;
   Bool_t          L1_MinimumBiasHF0_OR_BptxAND;
   Bool_t          L1_MinimumBiasHF1_AND;
   Bool_t          L1_MinimumBiasHF1_AND_BptxAND;
   Bool_t          L1_MinimumBiasHF1_OR;
   Bool_t          L1_MinimumBiasHF1_OR_BptxAND;
   Bool_t          L1_Mu0er_ETM40;
   Bool_t          L1_Mu0er_ETM55;
   Bool_t          L1_Mu10er_ETM30;
   Bool_t          L1_Mu10er_ETM50;
   Bool_t          L1_Mu12_EG10;
   Bool_t          L1_Mu14er_ETM30;
   Bool_t          L1_Mu16er_Tau20er;
   Bool_t          L1_Mu16er_Tau24er;
   Bool_t          L1_Mu18er_IsoTau26er;
   Bool_t          L1_Mu18er_Tau20er;
   Bool_t          L1_Mu18er_Tau24er;
   Bool_t          L1_Mu20_EG10;
   Bool_t          L1_Mu20_EG17;
   Bool_t          L1_Mu20_IsoEG6;
   Bool_t          L1_Mu20er_IsoTau26er;
   Bool_t          L1_Mu22er_IsoTau26er;
   Bool_t          L1_Mu23_EG10;
   Bool_t          L1_Mu23_IsoEG10;
   Bool_t          L1_Mu25er_IsoTau26er;
   Bool_t          L1_Mu3_JetC120;
   Bool_t          L1_Mu3_JetC120_dEta_Max0p4_dPhi_Max0p4;
   Bool_t          L1_Mu3_JetC16;
   Bool_t          L1_Mu3_JetC16_dEta_Max0p4_dPhi_Max0p4;
   Bool_t          L1_Mu3_JetC60;
   Bool_t          L1_Mu3_JetC60_dEta_Max0p4_dPhi_Max0p4;
   Bool_t          L1_Mu5_EG15;
   Bool_t          L1_Mu5_EG20;
   Bool_t          L1_Mu5_EG23;
   Bool_t          L1_Mu5_IsoEG18;
   Bool_t          L1_Mu5_IsoEG20;
   Bool_t          L1_Mu6_DoubleEG10;
   Bool_t          L1_Mu6_DoubleEG17;
   Bool_t          L1_Mu6_HTT200;
   Bool_t          L1_Mu8_HTT150;
   Bool_t          L1_NotBptxOR;
   Bool_t          L1_QuadJetC36_Tau52;
   Bool_t          L1_QuadJetC40;
   Bool_t          L1_QuadJetC50;
   Bool_t          L1_QuadJetC60;
   Bool_t          L1_QuadMu0;
   Bool_t          L1_SingleEG10;
   Bool_t          L1_SingleEG15;
   Bool_t          L1_SingleEG18;
   Bool_t          L1_SingleEG24;
   Bool_t          L1_SingleEG26;
   Bool_t          L1_SingleEG28;
   Bool_t          L1_SingleEG2_BptxAND;
   Bool_t          L1_SingleEG30;
   Bool_t          L1_SingleEG32;
   Bool_t          L1_SingleEG34;
   Bool_t          L1_SingleEG36;
   Bool_t          L1_SingleEG38;
   Bool_t          L1_SingleEG40;
   Bool_t          L1_SingleEG45;
   Bool_t          L1_SingleEG5;
   Bool_t          L1_SingleIsoEG18;
   Bool_t          L1_SingleIsoEG18er;
   Bool_t          L1_SingleIsoEG20;
   Bool_t          L1_SingleIsoEG20er;
   Bool_t          L1_SingleIsoEG22;
   Bool_t          L1_SingleIsoEG22er;
   Bool_t          L1_SingleIsoEG24;
   Bool_t          L1_SingleIsoEG24er;
   Bool_t          L1_SingleIsoEG26;
   Bool_t          L1_SingleIsoEG26er;
   Bool_t          L1_SingleIsoEG28;
   Bool_t          L1_SingleIsoEG28er;
   Bool_t          L1_SingleIsoEG30;
   Bool_t          L1_SingleIsoEG30er;
   Bool_t          L1_SingleIsoEG32;
   Bool_t          L1_SingleIsoEG32er;
   Bool_t          L1_SingleIsoEG34;
   Bool_t          L1_SingleIsoEG34er;
   Bool_t          L1_SingleIsoEG36;
   Bool_t          L1_SingleJet120;
   Bool_t          L1_SingleJet12_BptxAND;
   Bool_t          L1_SingleJet140;
   Bool_t          L1_SingleJet150;
   Bool_t          L1_SingleJet16;
   Bool_t          L1_SingleJet160;
   Bool_t          L1_SingleJet170;
   Bool_t          L1_SingleJet180;
   Bool_t          L1_SingleJet20;
   Bool_t          L1_SingleJet200;
   Bool_t          L1_SingleJet35;
   Bool_t          L1_SingleJet60;
   Bool_t          L1_SingleJet8_BptxAND;
   Bool_t          L1_SingleJet90;
   Bool_t          L1_SingleJetC20_NotBptxOR;
   Bool_t          L1_SingleJetC20_NotBptxOR_3BX;
   Bool_t          L1_SingleJetC32_NotBptxOR;
   Bool_t          L1_SingleJetC32_NotBptxOR_3BX;
   Bool_t          L1_SingleJetC36_NotBptxOR_3BX;
   Bool_t          L1_SingleMu10_LowQ;
   Bool_t          L1_SingleMu12;
   Bool_t          L1_SingleMu14;
   Bool_t          L1_SingleMu14er;
   Bool_t          L1_SingleMu16;
   Bool_t          L1_SingleMu16er;
   Bool_t          L1_SingleMu18;
   Bool_t          L1_SingleMu18er;
   Bool_t          L1_SingleMu20;
   Bool_t          L1_SingleMu20er;
   Bool_t          L1_SingleMu22;
   Bool_t          L1_SingleMu22er;
   Bool_t          L1_SingleMu25;
   Bool_t          L1_SingleMu25er;
   Bool_t          L1_SingleMu3;
   Bool_t          L1_SingleMu30;
   Bool_t          L1_SingleMu30er;
   Bool_t          L1_SingleMu5;
   Bool_t          L1_SingleMu7;
   Bool_t          L1_SingleMuCosmics;
   Bool_t          L1_SingleMuOpen;
   Bool_t          L1_SingleMuOpen_NotBptxOR;
   Bool_t          L1_SingleMuOpen_NotBptxOR_3BX;
   Bool_t          L1_SingleTau100er;
   Bool_t          L1_SingleTau120er;
   Bool_t          L1_SingleTau80er;
   Bool_t          L1_TripleEG_14_10_8;
   Bool_t          L1_TripleEG_18_17_8;
   Bool_t          L1_TripleJet_84_68_48_VBF;
   Bool_t          L1_TripleJet_88_72_56_VBF;
   Bool_t          L1_TripleJet_92_76_64_VBF;
   Bool_t          L1_TripleMu0;
   Bool_t          L1_TripleMu_5_0_0;
   Bool_t          L1_TripleMu_5_5_3;
   Bool_t          L1_ZeroBias;
   Bool_t          L1_ZeroBias_FirstCollidingBunch;
   Bool_t          L1_ZeroBias_copy;
   Bool_t          L1_UnprefireableEvent;
   */
   Bool_t          Flag_HBHENoiseFilter;
   Bool_t          Flag_HBHENoiseIsoFilter;
   Bool_t          Flag_CSCTightHaloFilter;
   Bool_t          Flag_CSCTightHaloTrkMuUnvetoFilter;
   Bool_t          Flag_CSCTightHalo2015Filter;
   Bool_t          Flag_globalTightHalo2016Filter;
   Bool_t          Flag_globalSuperTightHalo2016Filter;
   Bool_t          Flag_HcalStripHaloFilter;
   Bool_t          Flag_hcalLaserEventFilter;
   Bool_t          Flag_EcalDeadCellTriggerPrimitiveFilter;
   Bool_t          Flag_EcalDeadCellBoundaryEnergyFilter;
   Bool_t          Flag_ecalBadCalibFilter;
   Bool_t          Flag_goodVertices;
   Bool_t          Flag_eeBadScFilter;
   Bool_t          Flag_ecalLaserCorrFilter;
   Bool_t          Flag_trkPOGFilters;
   Bool_t          Flag_chargedHadronTrackResolutionFilter;
   Bool_t          Flag_muonBadTrackFilter;
   Bool_t          Flag_BadChargedCandidateFilter;
   Bool_t          Flag_BadPFMuonFilter;
   Bool_t          Flag_BadChargedCandidateSummer16Filter;
   Bool_t          Flag_BadPFMuonSummer16Filter;
   Bool_t          Flag_trkPOG_manystripclus53X;
   Bool_t          Flag_trkPOG_toomanystripclus53X;
   Bool_t          Flag_trkPOG_logErrorTooManyClusters;
   Bool_t          Flag_METFilters;
   /*
   Bool_t          L1Reco_step;
   Bool_t          HLTriggerFirstPath;
   Bool_t          HLT_AK8PFJet360_TrimMass30;
   Bool_t          HLT_AK8PFJet400_TrimMass30;
   Bool_t          HLT_AK8PFHT750_TrimMass50;
   Bool_t          HLT_AK8PFHT800_TrimMass50;
   Bool_t          HLT_AK8DiPFJet300_200_TrimMass30_BTagCSV_p20;
   Bool_t          HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p087;
   Bool_t          HLT_AK8DiPFJet300_200_TrimMass30_BTagCSV_p087;
   Bool_t          HLT_AK8DiPFJet300_200_TrimMass30;
   Bool_t          HLT_AK8PFHT700_TrimR0p1PT0p03Mass50;
   Bool_t          HLT_AK8PFHT650_TrimR0p1PT0p03Mass50;
   Bool_t          HLT_AK8PFHT600_TrimR0p1PT0p03Mass50_BTagCSV_p20;
   Bool_t          HLT_AK8DiPFJet280_200_TrimMass30;
   Bool_t          HLT_AK8DiPFJet250_200_TrimMass30;
   Bool_t          HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20;
   Bool_t          HLT_AK8DiPFJet250_200_TrimMass30_BTagCSV_p20;
   Bool_t          HLT_CaloJet260;
   Bool_t          HLT_CaloJet500_NoJetID;
   Bool_t          HLT_Dimuon13_PsiPrime;
   Bool_t          HLT_Dimuon13_Upsilon;
   Bool_t          HLT_Dimuon20_Jpsi;
   Bool_t          HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf;
   Bool_t          HLT_DoubleEle25_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_DoubleEle33_CaloIdL;
   Bool_t          HLT_DoubleEle33_CaloIdL_MW;
   Bool_t          HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW;
   Bool_t          HLT_DoubleEle33_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_DoubleMediumCombinedIsoPFTau35_Trk1_eta2p1_Reg;
   Bool_t          HLT_DoubleTightCombinedIsoPFTau35_Trk1_eta2p1_Reg;
   Bool_t          HLT_DoubleMediumCombinedIsoPFTau40_Trk1_eta2p1_Reg;
   Bool_t          HLT_DoubleTightCombinedIsoPFTau40_Trk1_eta2p1_Reg;
   Bool_t          HLT_DoubleMediumCombinedIsoPFTau40_Trk1_eta2p1;
   Bool_t          HLT_DoubleTightCombinedIsoPFTau40_Trk1_eta2p1;
   Bool_t          HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_Reg;
   Bool_t          HLT_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg;
   Bool_t          HLT_DoubleMediumIsoPFTau40_Trk1_eta2p1;
   Bool_t          HLT_DoubleEle37_Ele27_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_DoubleMu33NoFiltersNoVtx;
   Bool_t          HLT_DoubleMu38NoFiltersNoVtx;
   Bool_t          HLT_DoubleMu23NoFiltersNoVtxDisplaced;
   Bool_t          HLT_DoubleMu28NoFiltersNoVtxDisplaced;
   Bool_t          HLT_DoubleMu0;
   Bool_t          HLT_DoubleMu4_3_Bs;
   Bool_t          HLT_DoubleMu4_3_Jpsi_Displaced;
   Bool_t          HLT_DoubleMu4_JpsiTrk_Displaced;
   Bool_t          HLT_DoubleMu4_LowMassNonResonantTrk_Displaced;
   Bool_t          HLT_DoubleMu3_Trk_Tau3mu;
   Bool_t          HLT_DoubleMu4_PsiPrimeTrk_Displaced;
   Bool_t          HLT_Mu7p5_L2Mu2_Jpsi;
   Bool_t          HLT_Mu7p5_L2Mu2_Upsilon;
   Bool_t          HLT_Mu7p5_Track2_Jpsi;
   Bool_t          HLT_Mu7p5_Track3p5_Jpsi;
   Bool_t          HLT_Mu7p5_Track7_Jpsi;
   Bool_t          HLT_Mu7p5_Track2_Upsilon;
   Bool_t          HLT_Mu7p5_Track3p5_Upsilon;
   Bool_t          HLT_Mu7p5_Track7_Upsilon;
   Bool_t          HLT_Dimuon0er16_Jpsi_NoOS_NoVertexing;
   Bool_t          HLT_Dimuon0er16_Jpsi_NoVertexing;
   Bool_t          HLT_Dimuon6_Jpsi_NoVertexing;
   Bool_t          HLT_Photon150;
   Bool_t          HLT_Photon90_CaloIdL_HT300;
   Bool_t          HLT_HT250_CaloMET70;
   Bool_t          HLT_DoublePhoton60;
   Bool_t          HLT_DoublePhoton85;
   Bool_t          HLT_Ele17_Ele8_Gsf;
   Bool_t          HLT_Ele20_eta2p1_WPLoose_Gsf_LooseIsoPFTau28;
   Bool_t          HLT_Ele22_eta2p1_WPLoose_Gsf_LooseIsoPFTau29;
   Bool_t          HLT_Ele22_eta2p1_WPLoose_Gsf;
   Bool_t          HLT_Ele22_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_Ele23_WPLoose_Gsf;
   Bool_t          HLT_Ele23_WPLoose_Gsf_WHbbBoost;
   Bool_t          HLT_Ele24_eta2p1_WPLoose_Gsf;
   Bool_t          HLT_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau20;
   Bool_t          HLT_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau30;
   Bool_t          HLT_Ele25_WPTight_Gsf;
   Bool_t          HLT_Ele25_eta2p1_WPLoose_Gsf;
   Bool_t          HLT_Ele25_eta2p1_WPTight_Gsf;
   Bool_t          HLT_Ele27_WPLoose_Gsf;
   Bool_t          HLT_Ele27_WPLoose_Gsf_WHbbBoost;
   Bool_t          HLT_Ele27_WPTight_Gsf;
   Bool_t          HLT_Ele27_WPTight_Gsf_L1JetTauSeeded;
   Bool_t          HLT_Ele27_eta2p1_WPLoose_Gsf;
   Bool_t          HLT_Ele27_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_Ele27_eta2p1_WPTight_Gsf;
   Bool_t          HLT_Ele30_WPTight_Gsf;
   Bool_t          HLT_Ele30_eta2p1_WPLoose_Gsf;
   Bool_t          HLT_Ele30_eta2p1_WPTight_Gsf;
   Bool_t          HLT_Ele32_WPTight_Gsf;
   Bool_t          HLT_Ele32_eta2p1_WPLoose_Gsf;
   Bool_t          HLT_Ele32_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_Ele32_eta2p1_WPTight_Gsf;
   Bool_t          HLT_Ele35_WPLoose_Gsf;
   Bool_t          HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50;
   Bool_t          HLT_Ele36_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_Ele45_WPLoose_Gsf;
   Bool_t          HLT_Ele45_WPLoose_Gsf_L1JetTauSeeded;
   Bool_t          HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50;
   Bool_t          HLT_Ele105_CaloIdVT_GsfTrkIdT;
   Bool_t          HLT_Ele30WP60_SC4_Mass55;
   Bool_t          HLT_Ele30WP60_Ele8_Mass55;
   Bool_t          HLT_HT200;
   Bool_t          HLT_HT275;
   Bool_t          HLT_HT325;
   Bool_t          HLT_HT425;
   Bool_t          HLT_HT575;
   Bool_t          HLT_HT410to430;
   Bool_t          HLT_HT430to450;
   Bool_t          HLT_HT450to470;
   Bool_t          HLT_HT470to500;
   Bool_t          HLT_HT500to550;
   Bool_t          HLT_HT550to650;
   Bool_t          HLT_HT650;
   Bool_t          HLT_Mu16_eta2p1_MET30;
   Bool_t          HLT_IsoMu16_eta2p1_MET30;
   Bool_t          HLT_IsoMu16_eta2p1_MET30_LooseIsoPFTau50_Trk30_eta2p1;
   Bool_t          HLT_IsoMu17_eta2p1;
   Bool_t          HLT_IsoMu17_eta2p1_LooseIsoPFTau20;
   Bool_t          HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_DoubleIsoMu17_eta2p1;
   Bool_t          HLT_DoubleIsoMu17_eta2p1_noDzCut;
   Bool_t          HLT_IsoMu18;
   Bool_t          HLT_IsoMu19_eta2p1_LooseIsoPFTau20;
   Bool_t          HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_IsoMu19_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg;
   Bool_t          HLT_IsoMu19_eta2p1_LooseCombinedIsoPFTau20;
   Bool_t          HLT_IsoMu19_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg;
   Bool_t          HLT_IsoMu19_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg;
   Bool_t          HLT_IsoMu21_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg;
   Bool_t          HLT_IsoMu21_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg;
   Bool_t          HLT_IsoMu20;
   Bool_t          HLT_IsoMu21_eta2p1_LooseIsoPFTau20_SingleL1;
   Bool_t          HLT_IsoMu21_eta2p1_LooseIsoPFTau50_Trk30_eta2p1_SingleL1;
   Bool_t          HLT_IsoMu21_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg;
   Bool_t          HLT_IsoMu22;
   Bool_t          HLT_IsoMu22_eta2p1;
   Bool_t          HLT_IsoMu24;
   Bool_t          HLT_IsoMu27;
   Bool_t          HLT_IsoTkMu18;
   Bool_t          HLT_IsoTkMu20;
   Bool_t          HLT_IsoTkMu22;
   Bool_t          HLT_IsoTkMu22_eta2p1;
   Bool_t          HLT_IsoTkMu24;
   Bool_t          HLT_IsoTkMu27;
   Bool_t          HLT_JetE30_NoBPTX3BX;
   Bool_t          HLT_JetE30_NoBPTX;
   Bool_t          HLT_JetE50_NoBPTX3BX;
   Bool_t          HLT_JetE70_NoBPTX3BX;
   Bool_t          HLT_L1SingleMu18;
   Bool_t          HLT_L2Mu10;
   Bool_t          HLT_L1SingleMuOpen;
   Bool_t          HLT_L1SingleMuOpen_DT;
   Bool_t          HLT_L2DoubleMu23_NoVertex;
   Bool_t          HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10;
   Bool_t          HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10;
   Bool_t          HLT_L2Mu10_NoVertex_NoBPTX3BX;
   Bool_t          HLT_L2Mu10_NoVertex_NoBPTX;
   Bool_t          HLT_L2Mu45_NoVertex_3Sta_NoBPTX3BX;
   Bool_t          HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX;
   Bool_t          HLT_LooseIsoPFTau50_Trk30_eta2p1;
   Bool_t          HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80;
   Bool_t          HLT_LooseIsoPFTau50_Trk30_eta2p1_MET90;
   Bool_t          HLT_LooseIsoPFTau50_Trk30_eta2p1_MET110;
   Bool_t          HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120;
   Bool_t          HLT_PFTau120_eta2p1;
   Bool_t          HLT_PFTau140_eta2p1;
   Bool_t          HLT_VLooseIsoPFTau120_Trk50_eta2p1;
   Bool_t          HLT_VLooseIsoPFTau140_Trk50_eta2p1;
   Bool_t          HLT_Mu17_Mu8;
   Bool_t          HLT_Mu17_Mu8_DZ;
   Bool_t          HLT_Mu17_Mu8_SameSign;
   Bool_t          HLT_Mu17_Mu8_SameSign_DZ;
   Bool_t          HLT_Mu20_Mu10;
   Bool_t          HLT_Mu20_Mu10_DZ;
   Bool_t          HLT_Mu20_Mu10_SameSign;
   Bool_t          HLT_Mu20_Mu10_SameSign_DZ;
   Bool_t          HLT_Mu17_TkMu8_DZ;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
   Bool_t          HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL;
   Bool_t          HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;
   Bool_t          HLT_Mu25_TkMu0_dEta18_Onia;
   Bool_t          HLT_Mu27_TkMu8;
   Bool_t          HLT_Mu30_TkMu11;
   Bool_t          HLT_Mu30_eta2p1_PFJet150_PFJet50;
   Bool_t          HLT_Mu40_TkMu11;
   Bool_t          HLT_Mu40_eta2p1_PFJet200_PFJet50;
   Bool_t          HLT_Mu20;
   Bool_t          HLT_TkMu17;
   Bool_t          HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL;
   Bool_t          HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;
   Bool_t          HLT_TkMu20;
   Bool_t          HLT_Mu24_eta2p1;
   Bool_t          HLT_TkMu24_eta2p1;
   Bool_t          HLT_Mu27;
   Bool_t          HLT_TkMu27;
   Bool_t          HLT_Mu45_eta2p1;
   Bool_t          HLT_Mu50;
   Bool_t          HLT_TkMu50;
   Bool_t          HLT_Mu38NoFiltersNoVtx_Photon38_CaloIdL;
   Bool_t          HLT_Mu42NoFiltersNoVtx_Photon42_CaloIdL;
   Bool_t          HLT_Mu28NoFiltersNoVtxDisplaced_Photon28_CaloIdL;
   Bool_t          HLT_Mu33NoFiltersNoVtxDisplaced_Photon33_CaloIdL;
   Bool_t          HLT_Mu23NoFiltersNoVtx_Photon23_CaloIdL;
   Bool_t          HLT_DoubleMu18NoFiltersNoVtx;
   Bool_t          HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Tight;
   Bool_t          HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Loose;
   Bool_t          HLT_Mu28NoFiltersNoVtx_DisplacedJet40_Loose;
   Bool_t          HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Tight;
   Bool_t          HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Loose;
   Bool_t          HLT_Mu38NoFiltersNoVtx_DisplacedJet60_Loose;
   Bool_t          HLT_Mu28NoFiltersNoVtx_CentralCaloJet40;
   Bool_t          HLT_PFHT300_PFMET100;
   Bool_t          HLT_PFHT300_PFMET110;
   Bool_t          HLT_PFHT550_4JetPt50;
   Bool_t          HLT_PFHT650_4JetPt50;
   Bool_t          HLT_PFHT750_4JetPt50;
   Bool_t          HLT_PFHT750_4JetPt70;
   Bool_t          HLT_PFHT750_4JetPt80;
   Bool_t          HLT_PFHT800_4JetPt50;
   Bool_t          HLT_PFHT850_4JetPt50;
   Bool_t          HLT_PFJet15_NoCaloMatched;
   Bool_t          HLT_PFJet25_NoCaloMatched;
   Bool_t          HLT_DiPFJet15_NoCaloMatched;
   Bool_t          HLT_DiPFJet25_NoCaloMatched;
   Bool_t          HLT_DiPFJet15_FBEta3_NoCaloMatched;
   Bool_t          HLT_DiPFJet25_FBEta3_NoCaloMatched;
   Bool_t          HLT_DiPFJetAve15_HFJEC;
   Bool_t          HLT_DiPFJetAve25_HFJEC;
   Bool_t          HLT_DiPFJetAve35_HFJEC;
   Bool_t          HLT_AK8PFJet40;
   Bool_t          HLT_AK8PFJet60;
   Bool_t          HLT_AK8PFJet80;
   Bool_t          HLT_AK8PFJet140;
   Bool_t          HLT_AK8PFJet200;
   Bool_t          HLT_AK8PFJet260;
   Bool_t          HLT_AK8PFJet320;
   Bool_t          HLT_AK8PFJet400;
   Bool_t          HLT_AK8PFJet450;
   Bool_t          HLT_AK8PFJet500;
   Bool_t          HLT_PFJet40;
   Bool_t          HLT_PFJet60;
   Bool_t          HLT_PFJet80;
   Bool_t          HLT_PFJet140;
   Bool_t          HLT_PFJet200;
   Bool_t          HLT_PFJet260;
   Bool_t          HLT_PFJet320;
   Bool_t          HLT_PFJet400;
   Bool_t          HLT_PFJet450;
   Bool_t          HLT_PFJet500;
   Bool_t          HLT_DiPFJetAve40;
   Bool_t          HLT_DiPFJetAve60;
   Bool_t          HLT_DiPFJetAve80;
   Bool_t          HLT_DiPFJetAve140;
   Bool_t          HLT_DiPFJetAve200;
   Bool_t          HLT_DiPFJetAve260;
   Bool_t          HLT_DiPFJetAve320;
   Bool_t          HLT_DiPFJetAve400;
   Bool_t          HLT_DiPFJetAve500;
   Bool_t          HLT_DiPFJetAve60_HFJEC;
   Bool_t          HLT_DiPFJetAve80_HFJEC;
   Bool_t          HLT_DiPFJetAve100_HFJEC;
   Bool_t          HLT_DiPFJetAve160_HFJEC;
   Bool_t          HLT_DiPFJetAve220_HFJEC;
   Bool_t          HLT_DiPFJetAve300_HFJEC;
   Bool_t          HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu140;
   Bool_t          HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu80;
   Bool_t          HLT_DiCentralPFJet170;
   Bool_t          HLT_SingleCentralPFJet170_CFMax0p1;
   Bool_t          HLT_DiCentralPFJet170_CFMax0p1;
   Bool_t          HLT_DiCentralPFJet220_CFMax0p3;
   Bool_t          HLT_DiCentralPFJet330_CFMax0p5;
   Bool_t          HLT_DiCentralPFJet430;
   Bool_t          HLT_PFHT125;
   Bool_t          HLT_PFHT200;
   Bool_t          HLT_PFHT250;
   Bool_t          HLT_PFHT300;
   Bool_t          HLT_PFHT350;
   Bool_t          HLT_PFHT400;
   Bool_t          HLT_PFHT475;
   Bool_t          HLT_PFHT600;
   Bool_t          HLT_PFHT650;
   Bool_t          HLT_PFHT800;
   Bool_t          HLT_PFHT900;
   Bool_t          HLT_PFHT200_PFAlphaT0p51;
   Bool_t          HLT_PFHT200_DiPFJetAve90_PFAlphaT0p57;
   Bool_t          HLT_PFHT200_DiPFJetAve90_PFAlphaT0p63;
   Bool_t          HLT_PFHT250_DiPFJetAve90_PFAlphaT0p55;
   Bool_t          HLT_PFHT250_DiPFJetAve90_PFAlphaT0p58;
   Bool_t          HLT_PFHT300_DiPFJetAve90_PFAlphaT0p53;
   Bool_t          HLT_PFHT300_DiPFJetAve90_PFAlphaT0p54;
   Bool_t          HLT_PFHT350_DiPFJetAve90_PFAlphaT0p52;
   Bool_t          HLT_PFHT350_DiPFJetAve90_PFAlphaT0p53;
   Bool_t          HLT_PFHT400_DiPFJetAve90_PFAlphaT0p51;
   Bool_t          HLT_PFHT400_DiPFJetAve90_PFAlphaT0p52;
   Bool_t          HLT_MET60_IsoTrk35_Loose;
   Bool_t          HLT_MET75_IsoTrk50;
   Bool_t          HLT_MET90_IsoTrk50;
   Bool_t          HLT_PFMET120_BTagCSV_p067;
   Bool_t          HLT_PFMET120_Mu5;
   Bool_t          HLT_PFMET170_NotCleaned;
   Bool_t          HLT_PFMET170_NoiseCleaned;
   Bool_t          HLT_PFMET170_HBHECleaned;
   Bool_t          HLT_PFMET170_JetIdCleaned;
   Bool_t          HLT_PFMET170_BeamHaloCleaned;
   Bool_t          HLT_PFMET170_HBHE_BeamHaloCleaned;
   Bool_t          HLT_PFMETTypeOne190_HBHE_BeamHaloCleaned;
   Bool_t          HLT_PFMET90_PFMHT90_IDTight;
   Bool_t          HLT_PFMET100_PFMHT100_IDTight;
   Bool_t          HLT_PFMET100_PFMHT100_IDTight_BeamHaloCleaned;
   Bool_t          HLT_PFMET110_PFMHT110_IDTight;
   Bool_t          HLT_PFMET120_PFMHT120_IDTight;
   Bool_t          HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_BTagCSV_p067;
   Bool_t          HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight;
   Bool_t          HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200;
   Bool_t          HLT_QuadPFJet_BTagCSV_p016_VBF_Mqq460;
   Bool_t          HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq240;
   Bool_t          HLT_QuadPFJet_BTagCSV_p016_VBF_Mqq500;
   Bool_t          HLT_QuadPFJet_VBF;
   Bool_t          HLT_L1_TripleJet_VBF;
   Bool_t          HLT_QuadJet45_TripleBTagCSV_p087;
   Bool_t          HLT_QuadJet45_DoubleBTagCSV_p087;
   Bool_t          HLT_DoubleJet90_Double30_TripleBTagCSV_p087;
   Bool_t          HLT_DoubleJet90_Double30_DoubleBTagCSV_p087;
   Bool_t          HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160;
   Bool_t          HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6;
   Bool_t          HLT_DoubleJetsC112_DoubleBTagCSV_p026_DoublePFJetsC172;
   Bool_t          HLT_DoubleJetsC112_DoubleBTagCSV_p014_DoublePFJetsC112MaxDeta1p6;
   Bool_t          HLT_DoubleJetsC100_SingleBTagCSV_p026;
   Bool_t          HLT_DoubleJetsC100_SingleBTagCSV_p014;
   Bool_t          HLT_DoubleJetsC100_SingleBTagCSV_p026_SinglePFJetC350;
   Bool_t          HLT_DoubleJetsC100_SingleBTagCSV_p014_SinglePFJetC350;
   Bool_t          HLT_Photon135_PFMET100;
   Bool_t          HLT_Photon20_CaloIdVL_IsoL;
   Bool_t          HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40;
   Bool_t          HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_VBF;
   Bool_t          HLT_Photon250_NoHE;
   Bool_t          HLT_Photon300_NoHE;
   Bool_t          HLT_Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60;
   Bool_t          HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15;
   Bool_t          HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40;
   Bool_t          HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_VBF;
   Bool_t          HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40;
   Bool_t          HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_VBF;
   Bool_t          HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40;
   Bool_t          HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_VBF;
   Bool_t          HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40;
   Bool_t          HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_VBF;
   Bool_t          HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_PFMET40;
   Bool_t          HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_VBF;
   Bool_t          HLT_Mu8_TrkIsoVVL;
   Bool_t          HLT_Mu17_TrkIsoVVL;
   Bool_t          HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30;
   Bool_t          HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30;
   Bool_t          HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30;
   Bool_t          HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30;
   Bool_t          HLT_BTagMu_DiJet20_Mu5;
   Bool_t          HLT_BTagMu_DiJet40_Mu5;
   Bool_t          HLT_BTagMu_DiJet70_Mu5;
   Bool_t          HLT_BTagMu_DiJet110_Mu5;
   Bool_t          HLT_BTagMu_DiJet170_Mu5;
   Bool_t          HLT_BTagMu_Jet300_Mu5;
   Bool_t          HLT_BTagMu_AK8Jet300_Mu5;
   Bool_t          HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_L1JetTauSeeded;
   Bool_t          HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Mu30_Ele30_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_Mu33_Ele33_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_Mu37_Ele27_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_Mu27_Ele37_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_Mu8_DiEle12_CaloIdL_TrackIdL;
   Bool_t          HLT_Mu12_Photon25_CaloIdL;
   Bool_t          HLT_Mu12_Photon25_CaloIdL_L1ISO;
   Bool_t          HLT_Mu12_Photon25_CaloIdL_L1OR;
   Bool_t          HLT_Mu17_Photon22_CaloIdL_L1ISO;
   Bool_t          HLT_Mu17_Photon30_CaloIdL_L1ISO;
   Bool_t          HLT_Mu17_Photon35_CaloIdL_L1ISO;
   Bool_t          HLT_DiMu9_Ele9_CaloIdL_TrackIdL;
   Bool_t          HLT_TripleMu_5_3_3;
   Bool_t          HLT_TripleMu_12_10_5;
   Bool_t          HLT_Mu3er_PFHT140_PFMET125;
   Bool_t          HLT_Mu6_PFHT200_PFMET80_BTagCSV_p067;
   Bool_t          HLT_Mu6_PFHT200_PFMET100;
   Bool_t          HLT_Mu14er_PFMET100;
   Bool_t          HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Ele12_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Ele17_CaloIdL_GsfTrkIdVL;
   Bool_t          HLT_Ele17_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Ele23_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_PFHT650_WideJetMJJ900DEtaJJ1p5;
   Bool_t          HLT_PFHT650_WideJetMJJ950DEtaJJ1p5;
   Bool_t          HLT_Photon22;
   Bool_t          HLT_Photon30;
   Bool_t          HLT_Photon36;
   Bool_t          HLT_Photon50;
   Bool_t          HLT_Photon75;
   Bool_t          HLT_Photon90;
   Bool_t          HLT_Photon120;
   Bool_t          HLT_Photon175;
   Bool_t          HLT_Photon165_HE10;
   Bool_t          HLT_Photon22_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon30_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon36_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon50_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon75_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon90_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon120_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon165_R9Id90_HE10_IsoM;
   Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90;
   Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70;
   Bool_t          HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55;
   Bool_t          HLT_Diphoton30_18_Solid_R9Id_AND_IsoCaloId_AND_HE_R9Id_Mass55;
   Bool_t          HLT_Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55;
   Bool_t          HLT_Dimuon0_Jpsi_Muon;
   Bool_t          HLT_Dimuon0_Upsilon_Muon;
   Bool_t          HLT_QuadMuon0_Dimuon0_Jpsi;
   Bool_t          HLT_QuadMuon0_Dimuon0_Upsilon;
   Bool_t          HLT_Rsq0p25_Calo;
   Bool_t          HLT_RsqMR240_Rsq0p09_MR200_4jet_Calo;
   Bool_t          HLT_RsqMR240_Rsq0p09_MR200_Calo;
   Bool_t          HLT_Rsq0p25;
   Bool_t          HLT_Rsq0p30;
   Bool_t          HLT_RsqMR240_Rsq0p09_MR200;
   Bool_t          HLT_RsqMR240_Rsq0p09_MR200_4jet;
   Bool_t          HLT_RsqMR270_Rsq0p09_MR200;
   Bool_t          HLT_RsqMR270_Rsq0p09_MR200_4jet;
   Bool_t          HLT_Rsq0p02_MR300_TriPFJet80_60_40_BTagCSV_p063_p20_Mbb60_200;
   Bool_t          HLT_Rsq0p02_MR400_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;
   Bool_t          HLT_Rsq0p02_MR450_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;
   Bool_t          HLT_Rsq0p02_MR500_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;
   Bool_t          HLT_Rsq0p02_MR550_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;
   Bool_t          HLT_HT200_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT250_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT350_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT350_DisplacedDijet80_DisplacedTrack;
   Bool_t          HLT_HT350_DisplacedDijet80_Tight_DisplacedTrack;
   Bool_t          HLT_HT350_DisplacedDijet40_Inclusive;
   Bool_t          HLT_HT400_DisplacedDijet40_Inclusive;
   Bool_t          HLT_HT500_DisplacedDijet40_Inclusive;
   Bool_t          HLT_HT550_DisplacedDijet40_Inclusive;
   Bool_t          HLT_HT550_DisplacedDijet80_Inclusive;
   Bool_t          HLT_HT650_DisplacedDijet80_Inclusive;
   Bool_t          HLT_HT750_DisplacedDijet80_Inclusive;
   Bool_t          HLT_VBF_DisplacedJet40_DisplacedTrack;
   Bool_t          HLT_VBF_DisplacedJet40_DisplacedTrack_2TrackIP2DSig5;
   Bool_t          HLT_VBF_DisplacedJet40_TightID_DisplacedTrack;
   Bool_t          HLT_VBF_DisplacedJet40_Hadronic;
   Bool_t          HLT_VBF_DisplacedJet40_Hadronic_2PromptTrack;
   Bool_t          HLT_VBF_DisplacedJet40_TightID_Hadronic;
   Bool_t          HLT_VBF_DisplacedJet40_VTightID_Hadronic;
   Bool_t          HLT_VBF_DisplacedJet40_VVTightID_Hadronic;
   Bool_t          HLT_VBF_DisplacedJet40_VTightID_DisplacedTrack;
   Bool_t          HLT_VBF_DisplacedJet40_VVTightID_DisplacedTrack;
   Bool_t          HLT_PFMETNoMu90_PFMHTNoMu90_IDTight;
   Bool_t          HLT_PFMETNoMu100_PFMHTNoMu100_IDTight;
   Bool_t          HLT_PFMETNoMu110_PFMHTNoMu110_IDTight;
   Bool_t          HLT_PFMETNoMu120_PFMHTNoMu120_IDTight;
   Bool_t          HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90_IDTight;
   Bool_t          HLT_MonoCentralPFJet80_PFMETNoMu100_PFMHTNoMu100_IDTight;
   Bool_t          HLT_MonoCentralPFJet80_PFMETNoMu110_PFMHTNoMu110_IDTight;
   Bool_t          HLT_MonoCentralPFJet80_PFMETNoMu120_PFMHTNoMu120_IDTight;
   Bool_t          HLT_Ele27_eta2p1_WPLoose_Gsf_HT200;
   Bool_t          HLT_Photon90_CaloIdL_PFHT500;
   Bool_t          HLT_DoubleMu8_Mass8_PFHT250;
   Bool_t          HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT250;
   Bool_t          HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT250;
   Bool_t          HLT_DoubleMu8_Mass8_PFHT300;
   Bool_t          HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT300;
   Bool_t          HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT300;
   Bool_t          HLT_Mu10_CentralPFJet30_BTagCSV_p13;
   Bool_t          HLT_DoubleMu3_PFMET50;
   Bool_t          HLT_Ele10_CaloIdM_TrackIdM_CentralPFJet30_BTagCSV_p13;
   Bool_t          HLT_Ele15_IsoVVVL_BTagCSV_p067_PFHT400;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT350_PFMET50;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT600;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT350;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT400_PFMET50;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT400;
   Bool_t          HLT_Ele50_IsoVVVL_PFHT400;
   Bool_t          HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60;
   Bool_t          HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60;
   Bool_t          HLT_Mu15_IsoVVVL_BTagCSV_p067_PFHT400;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT350_PFMET50;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT600;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT350;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT400_PFMET50;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT400;
   Bool_t          HLT_Mu50_IsoVVVL_PFHT400;
   Bool_t          HLT_Dimuon16_Jpsi;
   Bool_t          HLT_Dimuon10_Jpsi_Barrel;
   Bool_t          HLT_Dimuon8_PsiPrime_Barrel;
   Bool_t          HLT_Dimuon8_Upsilon_Barrel;
   Bool_t          HLT_Dimuon0_Phi_Barrel;
   Bool_t          HLT_Mu16_TkMu0_dEta18_Onia;
   Bool_t          HLT_Mu16_TkMu0_dEta18_Phi;
   Bool_t          HLT_TrkMu15_DoubleTrkMu5NoFiltersNoVtx;
   Bool_t          HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx;
   Bool_t          HLT_Mu8;
   Bool_t          HLT_Mu17;
   Bool_t          HLT_Mu3_PFJet40;
   Bool_t          HLT_Ele8_CaloIdM_TrackIdM_PFJet30;
   Bool_t          HLT_Ele12_CaloIdM_TrackIdM_PFJet30;
   Bool_t          HLT_Ele17_CaloIdM_TrackIdM_PFJet30;
   Bool_t          HLT_Ele23_CaloIdM_TrackIdM_PFJet30;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet140;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;
   Bool_t          HLT_PFHT400_SixJet30_DoubleBTagCSV_p056;
   Bool_t          HLT_PFHT450_SixJet40_BTagCSV_p056;
   Bool_t          HLT_PFHT400_SixJet30;
   Bool_t          HLT_PFHT450_SixJet40;
   Bool_t          HLT_Ele115_CaloIdVT_GsfTrkIdT;
   Bool_t          HLT_Mu55;
   Bool_t          HLT_Photon42_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon25_AND_HE10_R9Id65_Eta2_Mass15;
   Bool_t          HLT_Photon90_CaloIdL_PFHT600;
   Bool_t          HLT_PixelTracks_Multiplicity60ForEndOfFill;
   Bool_t          HLT_PixelTracks_Multiplicity85ForEndOfFill;
   Bool_t          HLT_PixelTracks_Multiplicity110ForEndOfFill;
   Bool_t          HLT_PixelTracks_Multiplicity135ForEndOfFill;
   Bool_t          HLT_PixelTracks_Multiplicity160ForEndOfFill;
   Bool_t          HLT_FullTracks_Multiplicity80;
   Bool_t          HLT_FullTracks_Multiplicity100;
   Bool_t          HLT_FullTracks_Multiplicity130;
   Bool_t          HLT_FullTracks_Multiplicity150;
   Bool_t          HLT_ECALHT800;
   Bool_t          HLT_DiSC30_18_EIso_AND_HE_Mass70;
   Bool_t          HLT_Photon125;
   Bool_t          HLT_MET100;
   Bool_t          HLT_MET150;
   Bool_t          HLT_MET200;
   Bool_t          HLT_Ele27_HighEta_Ele20_Mass55;
   Bool_t          HLT_L1FatEvents;
   Bool_t          HLT_Physics;
   Bool_t          HLT_L1FatEvents_part0;
   Bool_t          HLT_L1FatEvents_part1;
   Bool_t          HLT_L1FatEvents_part2;
   Bool_t          HLT_L1FatEvents_part3;
   Bool_t          HLT_Random;
   Bool_t          HLT_ZeroBias;
   Bool_t          HLT_AK4CaloJet30;
   Bool_t          HLT_AK4CaloJet40;
   Bool_t          HLT_AK4CaloJet50;
   Bool_t          HLT_AK4CaloJet80;
   Bool_t          HLT_AK4CaloJet100;
   Bool_t          HLT_AK4PFJet30;
   Bool_t          HLT_AK4PFJet50;
   Bool_t          HLT_AK4PFJet80;
   Bool_t          HLT_AK4PFJet100;
   Bool_t          HLT_HISinglePhoton10;
   Bool_t          HLT_HISinglePhoton15;
   Bool_t          HLT_HISinglePhoton20;
   Bool_t          HLT_HISinglePhoton40;
   Bool_t          HLT_HISinglePhoton60;
   Bool_t          HLT_EcalCalibration;
   Bool_t          HLT_HcalCalibration;
   Bool_t          HLT_GlobalRunHPDNoise;
   Bool_t          HLT_L1BptxMinus;
   Bool_t          HLT_L1BptxPlus;
   Bool_t          HLT_L1NotBptxOR;
   Bool_t          HLT_L1BeamGasMinus;
   Bool_t          HLT_L1BeamGasPlus;
   Bool_t          HLT_L1BptxXOR;
   Bool_t          HLT_L1MinimumBiasHF_OR;
   Bool_t          HLT_L1MinimumBiasHF_AND;
   Bool_t          HLT_HcalNZS;
   Bool_t          HLT_HcalPhiSym;
   Bool_t          HLT_HcalIsolatedbunch;
   Bool_t          HLT_ZeroBias_FirstCollisionAfterAbortGap;
   Bool_t          HLT_ZeroBias_FirstCollisionAfterAbortGap_copy;
   Bool_t          HLT_ZeroBias_FirstCollisionAfterAbortGap_TCDS;
   Bool_t          HLT_ZeroBias_IsolatedBunches;
   Bool_t          HLT_ZeroBias_FirstCollisionInTrain;
   Bool_t          HLT_ZeroBias_FirstBXAfterTrain;
   Bool_t          HLT_Photon500;
   Bool_t          HLT_Photon600;
   Bool_t          HLT_Mu300;
   Bool_t          HLT_Mu350;
   Bool_t          HLT_MET250;
   Bool_t          HLT_MET300;
   Bool_t          HLT_MET600;
   Bool_t          HLT_MET700;
   Bool_t          HLT_PFMET300;
   Bool_t          HLT_PFMET400;
   Bool_t          HLT_PFMET500;
   Bool_t          HLT_PFMET600;
   Bool_t          HLT_Ele250_CaloIdVT_GsfTrkIdT;
   Bool_t          HLT_Ele300_CaloIdVT_GsfTrkIdT;
   Bool_t          HLT_HT2000;
   Bool_t          HLT_HT2500;
   Bool_t          HLT_IsoTrackHE;
   Bool_t          HLT_IsoTrackHB;
   Bool_t          HLTriggerFinalPath;
   Bool_t          L1simulation_step;
*/
   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_luminosityBlock;   //!
   TBranch        *b_event;   //!
   TBranch        *b_HTXS_Higgs_pt;   //!
   TBranch        *b_HTXS_Higgs_y;   //!
   TBranch        *b_HTXS_stage1_1_cat_pTjet25GeV;   //!
   TBranch        *b_HTXS_stage1_1_cat_pTjet30GeV;   //!
   TBranch        *b_HTXS_stage1_1_fine_cat_pTjet25GeV;   //!
   TBranch        *b_HTXS_stage1_1_fine_cat_pTjet30GeV;   //!
   TBranch        *b_HTXS_stage1_2_cat_pTjet25GeV;   //!
   TBranch        *b_HTXS_stage1_2_cat_pTjet30GeV;   //!
   TBranch        *b_HTXS_stage1_2_fine_cat_pTjet25GeV;   //!
   TBranch        *b_HTXS_stage1_2_fine_cat_pTjet30GeV;   //!
   TBranch        *b_HTXS_stage_0;   //!
   TBranch        *b_HTXS_stage_1_pTjet25;   //!
   TBranch        *b_HTXS_stage_1_pTjet30;   //!
   TBranch        *b_HTXS_njets25;   //!
   TBranch        *b_HTXS_njets30;   //!
   TBranch        *b_btagWeight_CSVV2;   //!
   TBranch        *b_btagWeight_DeepCSVB;   //!
   TBranch        *b_CaloMET_phi;   //!
   TBranch        *b_CaloMET_pt;   //!
   TBranch        *b_CaloMET_sumEt;   //!
   TBranch        *b_ChsMET_phi;   //!
   TBranch        *b_ChsMET_pt;   //!
   TBranch        *b_ChsMET_sumEt;   //!
   TBranch        *b_nCorrT1METJet;   //!
   TBranch        *b_CorrT1METJet_area;   //!
   TBranch        *b_CorrT1METJet_eta;   //!
   TBranch        *b_CorrT1METJet_muonSubtrFactor;   //!
   TBranch        *b_CorrT1METJet_phi;   //!
   TBranch        *b_CorrT1METJet_rawPt;   //!
   TBranch        *b_DeepMETResolutionTune_phi;   //!
   TBranch        *b_DeepMETResolutionTune_pt;   //!
   TBranch        *b_DeepMETResponseTune_phi;   //!
   TBranch        *b_DeepMETResponseTune_pt;   //!
   TBranch        *b_nElectron;   //!
   TBranch        *b_Electron_deltaEtaSC;   //!
   TBranch        *b_Electron_dr03EcalRecHitSumEt;   //!
   TBranch        *b_Electron_dr03HcalDepth1TowerSumEt;   //!
   TBranch        *b_Electron_dr03TkSumPt;   //!
   TBranch        *b_Electron_dr03TkSumPtHEEP;   //!
   TBranch        *b_Electron_dxy;   //!
   TBranch        *b_Electron_dxyErr;   //!
   TBranch        *b_Electron_dz;   //!
   TBranch        *b_Electron_dzErr;   //!
   TBranch        *b_Electron_eInvMinusPInv;   //!
   TBranch        *b_Electron_energyErr;   //!
   TBranch        *b_Electron_eta;   //!
   TBranch        *b_Electron_hoe;   //!
   TBranch        *b_Electron_ip3d;   //!
   TBranch        *b_Electron_jetPtRelv2;   //!
   TBranch        *b_Electron_jetRelIso;   //!
   TBranch        *b_Electron_mass;   //!
   TBranch        *b_Electron_miniPFRelIso_all;   //!
   TBranch        *b_Electron_miniPFRelIso_chg;   //!
   TBranch        *b_Electron_mvaFall17V1Iso;   //!
   TBranch        *b_Electron_mvaFall17V1noIso;   //!
   TBranch        *b_Electron_mvaFall17V2Iso;   //!
   TBranch        *b_Electron_mvaFall17V2noIso;   //!
   TBranch        *b_Electron_pfRelIso03_all;   //!
   TBranch        *b_Electron_pfRelIso03_chg;   //!
   TBranch        *b_Electron_phi;   //!
   TBranch        *b_Electron_pt;   //!
   TBranch        *b_Electron_r9;   //!
   TBranch        *b_Electron_scEtOverPt;   //!
   TBranch        *b_Electron_sieie;   //!
   TBranch        *b_Electron_sip3d;   //!
   TBranch        *b_Electron_mvaTTH;   //!
   TBranch        *b_Electron_charge;   //!
   TBranch        *b_Electron_cutBased;   //!
   TBranch        *b_Electron_cutBased_Fall17_V1;   //!
   TBranch        *b_Electron_jetIdx;   //!
   TBranch        *b_Electron_pdgId;   //!
   TBranch        *b_Electron_photonIdx;   //!
   TBranch        *b_Electron_tightCharge;   //!
   TBranch        *b_Electron_vidNestedWPBitmap;   //!
   TBranch        *b_Electron_vidNestedWPBitmapHEEP;   //!
   TBranch        *b_Electron_convVeto;   //!
   TBranch        *b_Electron_cutBased_HEEP;   //!
   TBranch        *b_Electron_isPFcand;   //!
   TBranch        *b_Electron_jetNDauCharged;   //!
   TBranch        *b_Electron_lostHits;   //!
   TBranch        *b_Electron_mvaFall17V1Iso_WP80;   //!
   TBranch        *b_Electron_mvaFall17V1Iso_WP90;   //!
   TBranch        *b_Electron_mvaFall17V1Iso_WPL;   //!
   TBranch        *b_Electron_mvaFall17V1noIso_WP80;   //!
   TBranch        *b_Electron_mvaFall17V1noIso_WP90;   //!
   TBranch        *b_Electron_mvaFall17V1noIso_WPL;   //!
   TBranch        *b_Electron_mvaFall17V2Iso_WP80;   //!
   TBranch        *b_Electron_mvaFall17V2Iso_WP90;   //!
   TBranch        *b_Electron_mvaFall17V2Iso_WPL;   //!
   TBranch        *b_Electron_mvaFall17V2noIso_WP80;   //!
   TBranch        *b_Electron_mvaFall17V2noIso_WP90;   //!
   TBranch        *b_Electron_mvaFall17V2noIso_WPL;   //!
   TBranch        *b_Electron_seedGain;   //!
   TBranch        *b_nFatJet;   //!
   TBranch        *b_FatJet_area;   //!
   TBranch        *b_FatJet_btagCMVA;   //!
   TBranch        *b_FatJet_btagCSVV2;   //!
   TBranch        *b_FatJet_btagDDBvL;   //!
   TBranch        *b_FatJet_btagDDBvLV2;   //!
   TBranch        *b_FatJet_btagDDBvL_noMD;   //!
   TBranch        *b_FatJet_btagDDCvB;   //!
   TBranch        *b_FatJet_btagDDCvBV2;   //!
   TBranch        *b_FatJet_btagDDCvB_noMD;   //!
   TBranch        *b_FatJet_btagDDCvL;   //!
   TBranch        *b_FatJet_btagDDCvLV2;   //!
   TBranch        *b_FatJet_btagDDCvL_noMD;   //!
   TBranch        *b_FatJet_btagDeepB;   //!
   TBranch        *b_FatJet_btagHbb;   //!
   TBranch        *b_FatJet_deepTagMD_H4qvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_HbbvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_TvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_WvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_ZHbbvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_ZHccvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_ZbbvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_ZvsQCD;   //!
   TBranch        *b_FatJet_deepTagMD_bbvsLight;   //!
   TBranch        *b_FatJet_deepTagMD_ccvsLight;   //!
   TBranch        *b_FatJet_deepTag_H;   //!
   TBranch        *b_FatJet_deepTag_QCD;   //!
   TBranch        *b_FatJet_deepTag_QCDothers;   //!
   TBranch        *b_FatJet_deepTag_TvsQCD;   //!
   TBranch        *b_FatJet_deepTag_WvsQCD;   //!
   TBranch        *b_FatJet_deepTag_ZvsQCD;   //!
   TBranch        *b_FatJet_eta;   //!
   TBranch        *b_FatJet_mass;   //!
   TBranch        *b_FatJet_msoftdrop;   //!
   TBranch        *b_FatJet_n2b1;   //!
   TBranch        *b_FatJet_n3b1;   //!
   TBranch        *b_FatJet_particleNetMD_QCD;   //!
   TBranch        *b_FatJet_particleNetMD_Xbb;   //!
   TBranch        *b_FatJet_particleNetMD_Xcc;   //!
   TBranch        *b_FatJet_particleNetMD_Xqq;   //!
   TBranch        *b_FatJet_particleNet_H4qvsQCD;   //!
   TBranch        *b_FatJet_particleNet_HbbvsQCD;   //!
   TBranch        *b_FatJet_particleNet_HccvsQCD;   //!
   TBranch        *b_FatJet_particleNet_QCD;   //!
   TBranch        *b_FatJet_particleNet_TvsQCD;   //!
   TBranch        *b_FatJet_particleNet_WvsQCD;   //!
   TBranch        *b_FatJet_particleNet_ZvsQCD;   //!
   TBranch        *b_FatJet_phi;   //!
   TBranch        *b_FatJet_pt;   //!
   TBranch        *b_FatJet_rawFactor;   //!
   TBranch        *b_FatJet_tau1;   //!
   TBranch        *b_FatJet_tau2;   //!
   TBranch        *b_FatJet_tau3;   //!
   TBranch        *b_FatJet_tau4;   //!
   TBranch        *b_FatJet_lsf3;   //!
   TBranch        *b_FatJet_jetId;   //!
   TBranch        *b_FatJet_subJetIdx1;   //!
   TBranch        *b_FatJet_subJetIdx2;   //!
   TBranch        *b_FatJet_electronIdx3SJ;   //!
   TBranch        *b_FatJet_muonIdx3SJ;   //!
   TBranch        *b_nFsrPhoton;   //!
   TBranch        *b_FsrPhoton_dROverEt2;   //!
   TBranch        *b_FsrPhoton_eta;   //!
   TBranch        *b_FsrPhoton_phi;   //!
   TBranch        *b_FsrPhoton_pt;   //!
   TBranch        *b_FsrPhoton_relIso03;   //!
   TBranch        *b_FsrPhoton_muonIdx;   //!
   TBranch        *b_nGenJetAK8;   //!
   TBranch        *b_GenJetAK8_eta;   //!
   TBranch        *b_GenJetAK8_mass;   //!
   TBranch        *b_GenJetAK8_phi;   //!
   TBranch        *b_GenJetAK8_pt;   //!
   TBranch        *b_nGenJet;   //!
   TBranch        *b_GenJet_eta;   //!
   TBranch        *b_GenJet_mass;   //!
   TBranch        *b_GenJet_phi;   //!
   TBranch        *b_GenJet_pt;   //!
   TBranch        *b_nGenPart;   //!
   TBranch        *b_GenPart_eta;   //!
   TBranch        *b_GenPart_mass;   //!
   TBranch        *b_GenPart_phi;   //!
   TBranch        *b_GenPart_pt;   //!
   TBranch        *b_GenPart_genPartIdxMother;   //!
   TBranch        *b_GenPart_pdgId;   //!
   TBranch        *b_GenPart_status;   //!
   TBranch        *b_GenPart_statusFlags;   //!
   TBranch        *b_nSubGenJetAK8;   //!
   TBranch        *b_SubGenJetAK8_eta;   //!
   TBranch        *b_SubGenJetAK8_mass;   //!
   TBranch        *b_SubGenJetAK8_phi;   //!
   TBranch        *b_SubGenJetAK8_pt;   //!
   TBranch        *b_Generator_binvar;   //!
   TBranch        *b_Generator_scalePDF;   //!
   TBranch        *b_Generator_weight;   //!
   TBranch        *b_Generator_x1;   //!
   TBranch        *b_Generator_x2;   //!
   TBranch        *b_Generator_xpdf1;   //!
   TBranch        *b_Generator_xpdf2;   //!
   TBranch        *b_Generator_id1;   //!
   TBranch        *b_Generator_id2;   //!
   TBranch        *b_GenVtx_x;   //!
   TBranch        *b_GenVtx_y;   //!
   TBranch        *b_GenVtx_z;   //!
   TBranch        *b_nGenVisTau;   //!
   TBranch        *b_GenVisTau_eta;   //!
   TBranch        *b_GenVisTau_mass;   //!
   TBranch        *b_GenVisTau_phi;   //!
   TBranch        *b_GenVisTau_pt;   //!
   TBranch        *b_GenVisTau_charge;   //!
   TBranch        *b_GenVisTau_genPartIdxMother;   //!
   TBranch        *b_GenVisTau_status;   //!
   TBranch        *b_genWeight;   //!
   TBranch        *b_LHEWeight_originalXWGTUP;   //!
   TBranch        *b_nLHEPdfWeight;   //!
   TBranch        *b_LHEPdfWeight;   //!
   TBranch        *b_nLHEReweightingWeight;   //!
   TBranch        *b_LHEReweightingWeight;   //!
   TBranch        *b_nLHEScaleWeight;   //!
   TBranch        *b_LHEScaleWeight;   //!
   TBranch        *b_nPSWeight;   //!
   TBranch        *b_PSWeight;   //!
   TBranch        *b_nIsoTrack;   //!
   TBranch        *b_IsoTrack_dxy;   //!
   TBranch        *b_IsoTrack_dz;   //!
   TBranch        *b_IsoTrack_eta;   //!
   TBranch        *b_IsoTrack_pfRelIso03_all;   //!
   TBranch        *b_IsoTrack_pfRelIso03_chg;   //!
   TBranch        *b_IsoTrack_phi;   //!
   TBranch        *b_IsoTrack_pt;   //!
   TBranch        *b_IsoTrack_miniPFRelIso_all;   //!
   TBranch        *b_IsoTrack_miniPFRelIso_chg;   //!
   TBranch        *b_IsoTrack_fromPV;   //!
   TBranch        *b_IsoTrack_pdgId;   //!
   TBranch        *b_IsoTrack_isHighPurityTrack;   //!
   TBranch        *b_IsoTrack_isPFcand;   //!
   TBranch        *b_IsoTrack_isFromLostTrack;   //!
   TBranch        *b_nJet;   //!
   TBranch        *b_Jet_area;   //!
   TBranch        *b_Jet_btagCMVA;   //!
   TBranch        *b_Jet_btagCSVV2;   //!
   TBranch        *b_Jet_btagDeepB;   //!
   TBranch        *b_Jet_btagDeepC;   //!
   TBranch        *b_Jet_btagDeepCvB;   //!
   TBranch        *b_Jet_btagDeepCvL;   //!
   TBranch        *b_Jet_btagDeepFlavB;   //!
   TBranch        *b_Jet_btagDeepFlavC;   //!
   TBranch        *b_Jet_btagDeepFlavCvB;   //!
   TBranch        *b_Jet_btagDeepFlavCvL;   //!
   TBranch        *b_Jet_btagDeepFlavQG;   //!
   TBranch        *b_Jet_chEmEF;   //!
   TBranch        *b_Jet_chFPV0EF;   //!
   TBranch        *b_Jet_chFPV1EF;   //!
   TBranch        *b_Jet_chFPV2EF;   //!
   TBranch        *b_Jet_chFPV3EF;   //!
   TBranch        *b_Jet_chHEF;   //!
   TBranch        *b_Jet_eta;   //!
   TBranch        *b_Jet_hfsigmaEtaEta;   //!
   TBranch        *b_Jet_hfsigmaPhiPhi;   //!
   TBranch        *b_Jet_mass;   //!
   TBranch        *b_Jet_muEF;   //!
   TBranch        *b_Jet_muonSubtrFactor;   //!
   TBranch        *b_Jet_neEmEF;   //!
   TBranch        *b_Jet_neHEF;   //!
   TBranch        *b_Jet_phi;   //!
   TBranch        *b_Jet_pt;   //!
   TBranch        *b_Jet_puIdDisc;   //!
   TBranch        *b_Jet_qgl;   //!
   TBranch        *b_Jet_rawFactor;   //!
   TBranch        *b_Jet_bRegCorr;   //!
   TBranch        *b_Jet_bRegRes;   //!
   TBranch        *b_Jet_cRegCorr;   //!
   TBranch        *b_Jet_cRegRes;   //!
   TBranch        *b_Jet_electronIdx1;   //!
   TBranch        *b_Jet_electronIdx2;   //!
   TBranch        *b_Jet_hfadjacentEtaStripsSize;   //!
   TBranch        *b_Jet_hfcentralEtaStripSize;   //!
   TBranch        *b_Jet_jetId;   //!
   TBranch        *b_Jet_muonIdx1;   //!
   TBranch        *b_Jet_muonIdx2;   //!
   TBranch        *b_Jet_nConstituents;   //!
   TBranch        *b_Jet_nElectrons;   //!
   TBranch        *b_Jet_nMuons;   //!
   TBranch        *b_Jet_puId;   //!
   TBranch        *b_L1PreFiringWeight_Dn;   //!
   TBranch        *b_L1PreFiringWeight_Nom;   //!
   TBranch        *b_L1PreFiringWeight_Up;   //!
   TBranch        *b_LHE_HT;   //!
   TBranch        *b_LHE_HTIncoming;   //!
   TBranch        *b_LHE_Vpt;   //!
   TBranch        *b_LHE_AlphaS;   //!
   TBranch        *b_LHE_Njets;   //!
   TBranch        *b_LHE_Nb;   //!
   TBranch        *b_LHE_Nc;   //!
   TBranch        *b_LHE_Nuds;   //!
   TBranch        *b_LHE_Nglu;   //!
   TBranch        *b_LHE_NpNLO;   //!
   TBranch        *b_LHE_NpLO;   //!
   TBranch        *b_nLHEPart;   //!
   TBranch        *b_LHEPart_pt;   //!
   TBranch        *b_LHEPart_eta;   //!
   TBranch        *b_LHEPart_phi;   //!
   TBranch        *b_LHEPart_mass;   //!
   TBranch        *b_LHEPart_incomingpz;   //!
   TBranch        *b_LHEPart_pdgId;   //!
   TBranch        *b_LHEPart_status;   //!
   TBranch        *b_LHEPart_spin;   //!
   TBranch        *b_GenMET_phi;   //!
   TBranch        *b_GenMET_pt;   //!
   TBranch        *b_MET_MetUnclustEnUpDeltaX;   //!
   TBranch        *b_MET_MetUnclustEnUpDeltaY;   //!
   TBranch        *b_MET_covXX;   //!
   TBranch        *b_MET_covXY;   //!
   TBranch        *b_MET_covYY;   //!
   TBranch        *b_MET_phi;   //!
   TBranch        *b_MET_pt;   //!
   TBranch        *b_MET_significance;   //!
   TBranch        *b_MET_sumEt;   //!
   TBranch        *b_MET_sumPtUnclustered;   //!
   TBranch        *b_nMuon;   //!
   TBranch        *b_Muon_dxy;   //!
   TBranch        *b_Muon_dxyErr;   //!
   TBranch        *b_Muon_dxybs;   //!
   TBranch        *b_Muon_dz;   //!
   TBranch        *b_Muon_dzErr;   //!
   TBranch        *b_Muon_eta;   //!
   TBranch        *b_Muon_ip3d;   //!
   TBranch        *b_Muon_jetPtRelv2;   //!
   TBranch        *b_Muon_jetRelIso;   //!
   TBranch        *b_Muon_mass;   //!
   TBranch        *b_Muon_miniPFRelIso_all;   //!
   TBranch        *b_Muon_miniPFRelIso_chg;   //!
   TBranch        *b_Muon_pfRelIso03_all;   //!
   TBranch        *b_Muon_pfRelIso03_chg;   //!
   TBranch        *b_Muon_pfRelIso04_all;   //!
   TBranch        *b_Muon_phi;   //!
   TBranch        *b_Muon_pt;   //!
   TBranch        *b_Muon_ptErr;   //!
   TBranch        *b_Muon_segmentComp;   //!
   TBranch        *b_Muon_sip3d;   //!
   TBranch        *b_Muon_softMva;   //!
   TBranch        *b_Muon_tkRelIso;   //!
   TBranch        *b_Muon_tunepRelPt;   //!
   TBranch        *b_Muon_mvaLowPt;   //!
   TBranch        *b_Muon_mvaTTH;   //!
   TBranch        *b_Muon_charge;   //!
   TBranch        *b_Muon_jetIdx;   //!
   TBranch        *b_Muon_nStations;   //!
   TBranch        *b_Muon_nTrackerLayers;   //!
   TBranch        *b_Muon_pdgId;   //!
   TBranch        *b_Muon_tightCharge;   //!
   TBranch        *b_Muon_fsrPhotonIdx;   //!
   TBranch        *b_Muon_highPtId;   //!
   TBranch        *b_Muon_highPurity;   //!
   TBranch        *b_Muon_inTimeMuon;   //!
   TBranch        *b_Muon_isGlobal;   //!
   TBranch        *b_Muon_isPFcand;   //!
   TBranch        *b_Muon_isTracker;   //!
   TBranch        *b_Muon_jetNDauCharged;   //!
   TBranch        *b_Muon_looseId;   //!
   TBranch        *b_Muon_mediumId;   //!
   TBranch        *b_Muon_mediumPromptId;   //!
   TBranch        *b_Muon_miniIsoId;   //!
   TBranch        *b_Muon_multiIsoId;   //!
   TBranch        *b_Muon_mvaId;   //!
   TBranch        *b_Muon_mvaLowPtId;   //!
   TBranch        *b_Muon_pfIsoId;   //!
   TBranch        *b_Muon_puppiIsoId;   //!
   TBranch        *b_Muon_softId;   //!
   TBranch        *b_Muon_softMvaId;   //!
   TBranch        *b_Muon_tightId;   //!
   TBranch        *b_Muon_tkIsoId;   //!
   TBranch        *b_Muon_triggerIdLoose;   //!
   TBranch        *b_nPhoton;   //!
   TBranch        *b_Photon_energyErr;   //!
   TBranch        *b_Photon_eta;   //!
   TBranch        *b_Photon_hoe;   //!
   TBranch        *b_Photon_mass;   //!
   TBranch        *b_Photon_mvaID;   //!
   TBranch        *b_Photon_mvaID_Fall17V1p1;   //!
   TBranch        *b_Photon_pfRelIso03_all;   //!
   TBranch        *b_Photon_pfRelIso03_chg;   //!
   TBranch        *b_Photon_phi;   //!
   TBranch        *b_Photon_pt;   //!
   TBranch        *b_Photon_r9;   //!
   TBranch        *b_Photon_sieie;   //!
   TBranch        *b_Photon_charge;   //!
   TBranch        *b_Photon_cutBased;   //!
   TBranch        *b_Photon_cutBased_Fall17V1Bitmap;   //!
   TBranch        *b_Photon_electronIdx;   //!
   TBranch        *b_Photon_jetIdx;   //!
   TBranch        *b_Photon_pdgId;   //!
   TBranch        *b_Photon_vidNestedWPBitmap;   //!
   TBranch        *b_Photon_electronVeto;   //!
   TBranch        *b_Photon_isScEtaEB;   //!
   TBranch        *b_Photon_isScEtaEE;   //!
   TBranch        *b_Photon_mvaID_WP80;   //!
   TBranch        *b_Photon_mvaID_WP90;   //!
   TBranch        *b_Photon_pixelSeed;   //!
   TBranch        *b_Photon_seedGain;   //!
   TBranch        *b_Pileup_nTrueInt;   //!
   TBranch        *b_Pileup_pudensity;   //!
   TBranch        *b_Pileup_gpudensity;   //!
   TBranch        *b_Pileup_nPU;   //!
   TBranch        *b_Pileup_sumEOOT;   //!
   TBranch        *b_Pileup_sumLOOT;   //!
   TBranch        *b_PuppiMET_phi;   //!
   TBranch        *b_PuppiMET_phiJERDown;   //!
   TBranch        *b_PuppiMET_phiJERUp;   //!
   TBranch        *b_PuppiMET_phiJESDown;   //!
   TBranch        *b_PuppiMET_phiJESUp;   //!
   TBranch        *b_PuppiMET_phiUnclusteredDown;   //!
   TBranch        *b_PuppiMET_phiUnclusteredUp;   //!
   TBranch        *b_PuppiMET_pt;   //!
   TBranch        *b_PuppiMET_ptJERDown;   //!
   TBranch        *b_PuppiMET_ptJERUp;   //!
   TBranch        *b_PuppiMET_ptJESDown;   //!
   TBranch        *b_PuppiMET_ptJESUp;   //!
   TBranch        *b_PuppiMET_ptUnclusteredDown;   //!
   TBranch        *b_PuppiMET_ptUnclusteredUp;   //!
   TBranch        *b_PuppiMET_sumEt;   //!
   TBranch        *b_RawMET_phi;   //!
   TBranch        *b_RawMET_pt;   //!
   TBranch        *b_RawMET_sumEt;   //!
   TBranch        *b_RawPuppiMET_phi;   //!
   TBranch        *b_RawPuppiMET_pt;   //!
   TBranch        *b_RawPuppiMET_sumEt;   //!
   TBranch        *b_fixedGridRhoFastjetAll;   //!
   TBranch        *b_fixedGridRhoFastjetCentral;   //!
   TBranch        *b_fixedGridRhoFastjetCentralCalo;   //!
   TBranch        *b_fixedGridRhoFastjetCentralChargedPileUp;   //!
   TBranch        *b_fixedGridRhoFastjetCentralNeutral;   //!
   TBranch        *b_nGenDressedLepton;   //!
   TBranch        *b_GenDressedLepton_eta;   //!
   TBranch        *b_GenDressedLepton_mass;   //!
   TBranch        *b_GenDressedLepton_phi;   //!
   TBranch        *b_GenDressedLepton_pt;   //!
   TBranch        *b_GenDressedLepton_pdgId;   //!
   TBranch        *b_GenDressedLepton_hasTauAnc;   //!
   TBranch        *b_nGenIsolatedPhoton;   //!
   TBranch        *b_GenIsolatedPhoton_eta;   //!
   TBranch        *b_GenIsolatedPhoton_mass;   //!
   TBranch        *b_GenIsolatedPhoton_phi;   //!
   TBranch        *b_GenIsolatedPhoton_pt;   //!
   TBranch        *b_nSoftActivityJet;   //!
   TBranch        *b_SoftActivityJet_eta;   //!
   TBranch        *b_SoftActivityJet_phi;   //!
   TBranch        *b_SoftActivityJet_pt;   //!
   TBranch        *b_SoftActivityJetHT;   //!
   TBranch        *b_SoftActivityJetHT10;   //!
   TBranch        *b_SoftActivityJetHT2;   //!
   TBranch        *b_SoftActivityJetHT5;   //!
   TBranch        *b_SoftActivityJetNjets10;   //!
   TBranch        *b_SoftActivityJetNjets2;   //!
   TBranch        *b_SoftActivityJetNjets5;   //!
   TBranch        *b_nSubJet;   //!
   TBranch        *b_SubJet_btagCMVA;   //!
   TBranch        *b_SubJet_btagCSVV2;   //!
   TBranch        *b_SubJet_btagDeepB;   //!
   TBranch        *b_SubJet_eta;   //!
   TBranch        *b_SubJet_mass;   //!
   TBranch        *b_SubJet_n2b1;   //!
   TBranch        *b_SubJet_n3b1;   //!
   TBranch        *b_SubJet_phi;   //!
   TBranch        *b_SubJet_pt;   //!
   TBranch        *b_SubJet_rawFactor;   //!
   TBranch        *b_SubJet_tau1;   //!
   TBranch        *b_SubJet_tau2;   //!
   TBranch        *b_SubJet_tau3;   //!
   TBranch        *b_SubJet_tau4;   //!
   TBranch        *b_nTau;   //!
   TBranch        *b_Tau_chargedIso;   //!
   TBranch        *b_Tau_dxy;   //!
   TBranch        *b_Tau_dz;   //!
   TBranch        *b_Tau_eta;   //!
   TBranch        *b_Tau_leadTkDeltaEta;   //!
   TBranch        *b_Tau_leadTkDeltaPhi;   //!
   TBranch        *b_Tau_leadTkPtOverTauPt;   //!
   TBranch        *b_Tau_mass;   //!
   TBranch        *b_Tau_neutralIso;   //!
   TBranch        *b_Tau_phi;   //!
   TBranch        *b_Tau_photonsOutsideSignalCone;   //!
   TBranch        *b_Tau_pt;   //!
   TBranch        *b_Tau_puCorr;   //!
   TBranch        *b_Tau_rawAntiEle;   //!
   TBranch        *b_Tau_rawAntiEle2018;   //!
   TBranch        *b_Tau_rawDeepTau2017v2p1VSe;   //!
   TBranch        *b_Tau_rawDeepTau2017v2p1VSjet;   //!
   TBranch        *b_Tau_rawDeepTau2017v2p1VSmu;   //!
   TBranch        *b_Tau_rawIso;   //!
   TBranch        *b_Tau_rawIsodR03;   //!
   TBranch        *b_Tau_rawMVAnewDM2017v2;   //!
   TBranch        *b_Tau_rawMVAoldDM;   //!
   TBranch        *b_Tau_rawMVAoldDM2017v1;   //!
   TBranch        *b_Tau_rawMVAoldDM2017v2;   //!
   TBranch        *b_Tau_rawMVAoldDMdR032017v2;   //!
   TBranch        *b_Tau_charge;   //!
   TBranch        *b_Tau_decayMode;   //!
   TBranch        *b_Tau_jetIdx;   //!
   TBranch        *b_Tau_rawAntiEleCat;   //!
   TBranch        *b_Tau_rawAntiEleCat2018;   //!
   TBranch        *b_Tau_idAntiEle;   //!
   TBranch        *b_Tau_idAntiEle2018;   //!
   TBranch        *b_Tau_idAntiEleDeadECal;   //!
   TBranch        *b_Tau_idAntiMu;   //!
   TBranch        *b_Tau_idDecayMode;   //!
   TBranch        *b_Tau_idDecayModeNewDMs;   //!
   TBranch        *b_Tau_idDeepTau2017v2p1VSe;   //!
   TBranch        *b_Tau_idDeepTau2017v2p1VSjet;   //!
   TBranch        *b_Tau_idDeepTau2017v2p1VSmu;   //!
   TBranch        *b_Tau_idMVAnewDM2017v2;   //!
   TBranch        *b_Tau_idMVAoldDM;   //!
   TBranch        *b_Tau_idMVAoldDM2017v1;   //!
   TBranch        *b_Tau_idMVAoldDM2017v2;   //!
   TBranch        *b_Tau_idMVAoldDMdR032017v2;   //!
   TBranch        *b_TkMET_phi;   //!
   TBranch        *b_TkMET_pt;   //!
   TBranch        *b_TkMET_sumEt;   //!
   TBranch        *b_nTrigObj;   //!
   TBranch        *b_TrigObj_pt;   //!
   TBranch        *b_TrigObj_eta;   //!
   TBranch        *b_TrigObj_phi;   //!
   TBranch        *b_TrigObj_l1pt;   //!
   TBranch        *b_TrigObj_l1pt_2;   //!
   TBranch        *b_TrigObj_l2pt;   //!
   TBranch        *b_TrigObj_id;   //!
   TBranch        *b_TrigObj_l1iso;   //!
   TBranch        *b_TrigObj_l1charge;   //!
   TBranch        *b_TrigObj_filterBits;   //!
   TBranch        *b_genTtbarId;   //!
   TBranch        *b_nOtherPV;   //!
   TBranch        *b_OtherPV_z;   //!
   TBranch        *b_PV_ndof;   //!
   TBranch        *b_PV_x;   //!
   TBranch        *b_PV_y;   //!
   TBranch        *b_PV_z;   //!
   TBranch        *b_PV_chi2;   //!
   TBranch        *b_PV_score;   //!
   TBranch        *b_PV_npvs;   //!
   TBranch        *b_PV_npvsGood;   //!
   TBranch        *b_nSV;   //!
   TBranch        *b_SV_dlen;   //!
   TBranch        *b_SV_dlenSig;   //!
   TBranch        *b_SV_dxy;   //!
   TBranch        *b_SV_dxySig;   //!
   TBranch        *b_SV_pAngle;   //!
   TBranch        *b_Electron_genPartIdx;   //!
   TBranch        *b_Electron_genPartFlav;   //!
   TBranch        *b_FatJet_genJetAK8Idx;   //!
   TBranch        *b_FatJet_hadronFlavour;   //!
   TBranch        *b_FatJet_nBHadrons;   //!
   TBranch        *b_FatJet_nCHadrons;   //!
   TBranch        *b_GenJetAK8_partonFlavour;   //!
   TBranch        *b_GenJetAK8_hadronFlavour;   //!
   TBranch        *b_GenJet_partonFlavour;   //!
   TBranch        *b_GenJet_hadronFlavour;   //!
   TBranch        *b_GenVtx_t0;   //!
   TBranch        *b_Jet_genJetIdx;   //!
   TBranch        *b_Jet_hadronFlavour;   //!
   TBranch        *b_Jet_partonFlavour;   //!
   TBranch        *b_Muon_genPartIdx;   //!
   TBranch        *b_Muon_genPartFlav;   //!
   TBranch        *b_Photon_genPartIdx;   //!
   TBranch        *b_Photon_genPartFlav;   //!
   TBranch        *b_MET_fiducialGenPhi;   //!
   TBranch        *b_MET_fiducialGenPt;   //!
   TBranch        *b_Electron_cleanmask;   //!
   TBranch        *b_Jet_cleanmask;   //!
   TBranch        *b_Muon_cleanmask;   //!
   TBranch        *b_Photon_cleanmask;   //!
   TBranch        *b_Tau_cleanmask;   //!
   TBranch        *b_SubJet_hadronFlavour;   //!
   TBranch        *b_SubJet_nBHadrons;   //!
   TBranch        *b_SubJet_nCHadrons;   //!
   TBranch        *b_SV_chi2;   //!
   TBranch        *b_SV_eta;   //!
   TBranch        *b_SV_mass;   //!
   TBranch        *b_SV_ndof;   //!
   TBranch        *b_SV_phi;   //!
   TBranch        *b_SV_pt;   //!
   TBranch        *b_SV_x;   //!
   TBranch        *b_SV_y;   //!
   TBranch        *b_SV_z;   //!
   TBranch        *b_SV_ntracks;   //!
   TBranch        *b_Tau_genPartIdx;   //!
   TBranch        *b_Tau_genPartFlav;   //!

   /*
   TBranch        *b_L1_AlwaysTrue;   //!
   TBranch        *b_L1_BRIL_TRIG0_AND;   //!
   TBranch        *b_L1_BRIL_TRIG0_FstBunchInTrain;   //!
   TBranch        *b_L1_BRIL_TRIG0_OR;   //!
   TBranch        *b_L1_BRIL_TRIG0_delayedAND;   //!
   TBranch        *b_L1_BeamGasB1;   //!
   TBranch        *b_L1_BeamGasB2;   //!
   TBranch        *b_L1_BeamGasMinus;   //!
   TBranch        *b_L1_BeamGasPlus;   //!
   TBranch        *b_L1_BptxMinus;   //!
   TBranch        *b_L1_BptxOR;   //!
   TBranch        *b_L1_BptxPlus;   //!
   TBranch        *b_L1_BptxXOR;   //!
   TBranch        *b_L1_DoubleEG6_HTT255;   //!
   TBranch        *b_L1_DoubleEG_15_10;   //!
   TBranch        *b_L1_DoubleEG_18_17;   //!
   TBranch        *b_L1_DoubleEG_20_18;   //!
   TBranch        *b_L1_DoubleEG_22_10;   //!
   TBranch        *b_L1_DoubleEG_22_12;   //!
   TBranch        *b_L1_DoubleEG_22_15;   //!
   TBranch        *b_L1_DoubleEG_23_10;   //!
   TBranch        *b_L1_DoubleEG_24_17;   //!
   TBranch        *b_L1_DoubleEG_25_12;   //!
   TBranch        *b_L1_DoubleIsoTau28er;   //!
   TBranch        *b_L1_DoubleIsoTau30er;   //!
   TBranch        *b_L1_DoubleIsoTau32er;   //!
   TBranch        *b_L1_DoubleIsoTau33er;   //!
   TBranch        *b_L1_DoubleIsoTau34er;   //!
   TBranch        *b_L1_DoubleIsoTau35er;   //!
   TBranch        *b_L1_DoubleIsoTau36er;   //!
   TBranch        *b_L1_DoubleJet12_ForwardBackward;   //!
   TBranch        *b_L1_DoubleJet16_ForwardBackward;   //!
   TBranch        *b_L1_DoubleJet8_ForwardBackward;   //!
   TBranch        *b_L1_DoubleJetC100;   //!
   TBranch        *b_L1_DoubleJetC112;   //!
   TBranch        *b_L1_DoubleJetC120;   //!
   TBranch        *b_L1_DoubleJetC40;   //!
   TBranch        *b_L1_DoubleJetC50;   //!
   TBranch        *b_L1_DoubleJetC60;   //!
   TBranch        *b_L1_DoubleJetC60_ETM60;   //!
   TBranch        *b_L1_DoubleJetC80;   //!
   TBranch        *b_L1_DoubleMu0;   //!
   TBranch        *b_L1_DoubleMu0_ETM40;   //!
   TBranch        *b_L1_DoubleMu0_ETM55;   //!
   TBranch        *b_L1_DoubleMu0er1p4_dEta_Max1p8_OS;   //!
   TBranch        *b_L1_DoubleMu0er1p6_dEta_Max1p8;   //!
   TBranch        *b_L1_DoubleMu0er1p6_dEta_Max1p8_OS;   //!
   TBranch        *b_L1_DoubleMu7_EG14;   //!
   TBranch        *b_L1_DoubleMu7_EG7;   //!
   TBranch        *b_L1_DoubleMuOpen;   //!
   TBranch        *b_L1_DoubleMu_10_0_dEta_Max1p8;   //!
   TBranch        *b_L1_DoubleMu_10_3p5;   //!
   TBranch        *b_L1_DoubleMu_10_Open;   //!
   TBranch        *b_L1_DoubleMu_11_4;   //!
   TBranch        *b_L1_DoubleMu_12_5;   //!
   TBranch        *b_L1_DoubleMu_12_8;   //!
   TBranch        *b_L1_DoubleMu_13_6;   //!
   TBranch        *b_L1_DoubleMu_15_5;   //!
   TBranch        *b_L1_DoubleTau50er;   //!
   TBranch        *b_L1_EG25er_HTT125;   //!
   TBranch        *b_L1_EG27er_HTT200;   //!
   TBranch        *b_L1_ETM100;   //!
   TBranch        *b_L1_ETM120;   //!
   TBranch        *b_L1_ETM30;   //!
   TBranch        *b_L1_ETM40;   //!
   TBranch        *b_L1_ETM50;   //!
   TBranch        *b_L1_ETM60;   //!
   TBranch        *b_L1_ETM70;   //!
   TBranch        *b_L1_ETM75;   //!
   TBranch        *b_L1_ETM75_Jet60_dPhi_Min0p4;   //!
   TBranch        *b_L1_ETM80;   //!
   TBranch        *b_L1_ETM85;   //!
   TBranch        *b_L1_ETM90;   //!
   TBranch        *b_L1_ETM95;   //!
   TBranch        *b_L1_ETT25;   //!
   TBranch        *b_L1_ETT35_BptxAND;   //!
   TBranch        *b_L1_ETT40_BptxAND;   //!
   TBranch        *b_L1_ETT50_BptxAND;   //!
   TBranch        *b_L1_ETT60_BptxAND;   //!
   TBranch        *b_L1_FirstBunchAfterTrain;   //!
   TBranch        *b_L1_FirstBunchInTrain;   //!
   TBranch        *b_L1_HTM100;   //!
   TBranch        *b_L1_HTM120;   //!
   TBranch        *b_L1_HTM130;   //!
   TBranch        *b_L1_HTM140;   //!
   TBranch        *b_L1_HTM150;   //!
   TBranch        *b_L1_HTM50;   //!
   TBranch        *b_L1_HTM60_HTT260;   //!
   TBranch        *b_L1_HTM70;   //!
   TBranch        *b_L1_HTM80;   //!
   TBranch        *b_L1_HTM80_HTT220;   //!
   TBranch        *b_L1_HTT120;   //!
   TBranch        *b_L1_HTT160;   //!
   TBranch        *b_L1_HTT200;   //!
   TBranch        *b_L1_HTT220;   //!
   TBranch        *b_L1_HTT240;   //!
   TBranch        *b_L1_HTT255;   //!
   TBranch        *b_L1_HTT270;   //!
   TBranch        *b_L1_HTT280;   //!
   TBranch        *b_L1_HTT300;   //!
   TBranch        *b_L1_HTT320;   //!
   TBranch        *b_L1_IsoEG18er_IsoTau24er_dEta_Min0p2;   //!
   TBranch        *b_L1_IsoEG20er_IsoTau25er_dEta_Min0p2;   //!
   TBranch        *b_L1_IsoEG22er_IsoTau26er_dEta_Min0p2;   //!
   TBranch        *b_L1_IsoEG22er_Tau20er_dEta_Min0p2;   //!
   TBranch        *b_L1_IsolatedBunch;   //!
   TBranch        *b_L1_Jet32_DoubleMu_10_0_dPhi_Jet_Mu0_Max0p4_dPhi_Mu_Mu_Min1p0;   //!
   TBranch        *b_L1_Jet32_Mu0_EG10_dPhi_Jet_Mu_Max0p4_dPhi_Mu_EG_Min1p0;   //!
   TBranch        *b_L1_MU20_EG15;   //!
   TBranch        *b_L1_MinimumBiasHF0_AND;   //!
   TBranch        *b_L1_MinimumBiasHF0_AND_BptxAND;   //!
   TBranch        *b_L1_MinimumBiasHF0_OR;   //!
   TBranch        *b_L1_MinimumBiasHF0_OR_BptxAND;   //!
   TBranch        *b_L1_MinimumBiasHF1_AND;   //!
   TBranch        *b_L1_MinimumBiasHF1_AND_BptxAND;   //!
   TBranch        *b_L1_MinimumBiasHF1_OR;   //!
   TBranch        *b_L1_MinimumBiasHF1_OR_BptxAND;   //!
   TBranch        *b_L1_Mu0er_ETM40;   //!
   TBranch        *b_L1_Mu0er_ETM55;   //!
   TBranch        *b_L1_Mu10er_ETM30;   //!
   TBranch        *b_L1_Mu10er_ETM50;   //!
   TBranch        *b_L1_Mu12_EG10;   //!
   TBranch        *b_L1_Mu14er_ETM30;   //!
   TBranch        *b_L1_Mu16er_Tau20er;   //!
   TBranch        *b_L1_Mu16er_Tau24er;   //!
   TBranch        *b_L1_Mu18er_IsoTau26er;   //!
   TBranch        *b_L1_Mu18er_Tau20er;   //!
   TBranch        *b_L1_Mu18er_Tau24er;   //!
   TBranch        *b_L1_Mu20_EG10;   //!
   TBranch        *b_L1_Mu20_EG17;   //!
   TBranch        *b_L1_Mu20_IsoEG6;   //!
   TBranch        *b_L1_Mu20er_IsoTau26er;   //!
   TBranch        *b_L1_Mu22er_IsoTau26er;   //!
   TBranch        *b_L1_Mu23_EG10;   //!
   TBranch        *b_L1_Mu23_IsoEG10;   //!
   TBranch        *b_L1_Mu25er_IsoTau26er;   //!
   TBranch        *b_L1_Mu3_JetC120;   //!
   TBranch        *b_L1_Mu3_JetC120_dEta_Max0p4_dPhi_Max0p4;   //!
   TBranch        *b_L1_Mu3_JetC16;   //!
   TBranch        *b_L1_Mu3_JetC16_dEta_Max0p4_dPhi_Max0p4;   //!
   TBranch        *b_L1_Mu3_JetC60;   //!
   TBranch        *b_L1_Mu3_JetC60_dEta_Max0p4_dPhi_Max0p4;   //!
   TBranch        *b_L1_Mu5_EG15;   //!
   TBranch        *b_L1_Mu5_EG20;   //!
   TBranch        *b_L1_Mu5_EG23;   //!
   TBranch        *b_L1_Mu5_IsoEG18;   //!
   TBranch        *b_L1_Mu5_IsoEG20;   //!
   TBranch        *b_L1_Mu6_DoubleEG10;   //!
   TBranch        *b_L1_Mu6_DoubleEG17;   //!
   TBranch        *b_L1_Mu6_HTT200;   //!
   TBranch        *b_L1_Mu8_HTT150;   //!
   TBranch        *b_L1_NotBptxOR;   //!
   TBranch        *b_L1_QuadJetC36_Tau52;   //!
   TBranch        *b_L1_QuadJetC40;   //!
   TBranch        *b_L1_QuadJetC50;   //!
   TBranch        *b_L1_QuadJetC60;   //!
   TBranch        *b_L1_QuadMu0;   //!
   TBranch        *b_L1_SingleEG10;   //!
   TBranch        *b_L1_SingleEG15;   //!
   TBranch        *b_L1_SingleEG18;   //!
   TBranch        *b_L1_SingleEG24;   //!
   TBranch        *b_L1_SingleEG26;   //!
   TBranch        *b_L1_SingleEG28;   //!
   TBranch        *b_L1_SingleEG2_BptxAND;   //!
   TBranch        *b_L1_SingleEG30;   //!
   TBranch        *b_L1_SingleEG32;   //!
   TBranch        *b_L1_SingleEG34;   //!
   TBranch        *b_L1_SingleEG36;   //!
   TBranch        *b_L1_SingleEG38;   //!
   TBranch        *b_L1_SingleEG40;   //!
   TBranch        *b_L1_SingleEG45;   //!
   TBranch        *b_L1_SingleEG5;   //!
   TBranch        *b_L1_SingleIsoEG18;   //!
   TBranch        *b_L1_SingleIsoEG18er;   //!
   TBranch        *b_L1_SingleIsoEG20;   //!
   TBranch        *b_L1_SingleIsoEG20er;   //!
   TBranch        *b_L1_SingleIsoEG22;   //!
   TBranch        *b_L1_SingleIsoEG22er;   //!
   TBranch        *b_L1_SingleIsoEG24;   //!
   TBranch        *b_L1_SingleIsoEG24er;   //!
   TBranch        *b_L1_SingleIsoEG26;   //!
   TBranch        *b_L1_SingleIsoEG26er;   //!
   TBranch        *b_L1_SingleIsoEG28;   //!
   TBranch        *b_L1_SingleIsoEG28er;   //!
   TBranch        *b_L1_SingleIsoEG30;   //!
   TBranch        *b_L1_SingleIsoEG30er;   //!
   TBranch        *b_L1_SingleIsoEG32;   //!
   TBranch        *b_L1_SingleIsoEG32er;   //!
   TBranch        *b_L1_SingleIsoEG34;   //!
   TBranch        *b_L1_SingleIsoEG34er;   //!
   TBranch        *b_L1_SingleIsoEG36;   //!
   TBranch        *b_L1_SingleJet120;   //!
   TBranch        *b_L1_SingleJet12_BptxAND;   //!
   TBranch        *b_L1_SingleJet140;   //!
   TBranch        *b_L1_SingleJet150;   //!
   TBranch        *b_L1_SingleJet16;   //!
   TBranch        *b_L1_SingleJet160;   //!
   TBranch        *b_L1_SingleJet170;   //!
   TBranch        *b_L1_SingleJet180;   //!
   TBranch        *b_L1_SingleJet20;   //!
   TBranch        *b_L1_SingleJet200;   //!
   TBranch        *b_L1_SingleJet35;   //!
   TBranch        *b_L1_SingleJet60;   //!
   TBranch        *b_L1_SingleJet8_BptxAND;   //!
   TBranch        *b_L1_SingleJet90;   //!
   TBranch        *b_L1_SingleJetC20_NotBptxOR;   //!
   TBranch        *b_L1_SingleJetC20_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleJetC32_NotBptxOR;   //!
   TBranch        *b_L1_SingleJetC32_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleJetC36_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleMu10_LowQ;   //!
   TBranch        *b_L1_SingleMu12;   //!
   TBranch        *b_L1_SingleMu14;   //!
   TBranch        *b_L1_SingleMu14er;   //!
   TBranch        *b_L1_SingleMu16;   //!
   TBranch        *b_L1_SingleMu16er;   //!
   TBranch        *b_L1_SingleMu18;   //!
   TBranch        *b_L1_SingleMu18er;   //!
   TBranch        *b_L1_SingleMu20;   //!
   TBranch        *b_L1_SingleMu20er;   //!
   TBranch        *b_L1_SingleMu22;   //!
   TBranch        *b_L1_SingleMu22er;   //!
   TBranch        *b_L1_SingleMu25;   //!
   TBranch        *b_L1_SingleMu25er;   //!
   TBranch        *b_L1_SingleMu3;   //!
   TBranch        *b_L1_SingleMu30;   //!
   TBranch        *b_L1_SingleMu30er;   //!
   TBranch        *b_L1_SingleMu5;   //!
   TBranch        *b_L1_SingleMu7;   //!
   TBranch        *b_L1_SingleMuCosmics;   //!
   TBranch        *b_L1_SingleMuOpen;   //!
   TBranch        *b_L1_SingleMuOpen_NotBptxOR;   //!
   TBranch        *b_L1_SingleMuOpen_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleTau100er;   //!
   TBranch        *b_L1_SingleTau120er;   //!
   TBranch        *b_L1_SingleTau80er;   //!
   TBranch        *b_L1_TripleEG_14_10_8;   //!
   TBranch        *b_L1_TripleEG_18_17_8;   //!
   TBranch        *b_L1_TripleJet_84_68_48_VBF;   //!
   TBranch        *b_L1_TripleJet_88_72_56_VBF;   //!
   TBranch        *b_L1_TripleJet_92_76_64_VBF;   //!
   TBranch        *b_L1_TripleMu0;   //!
   TBranch        *b_L1_TripleMu_5_0_0;   //!
   TBranch        *b_L1_TripleMu_5_5_3;   //!
   TBranch        *b_L1_ZeroBias;   //!
   TBranch        *b_L1_ZeroBias_FirstCollidingBunch;   //!
   TBranch        *b_L1_ZeroBias_copy;   //!
   TBranch        *b_L1_UnprefireableEvent;   //!
   */
   TBranch        *b_Flag_HBHENoiseFilter;   //!
   TBranch        *b_Flag_HBHENoiseIsoFilter;   //!
   TBranch        *b_Flag_CSCTightHaloFilter;   //!
   TBranch        *b_Flag_CSCTightHaloTrkMuUnvetoFilter;   //!
   TBranch        *b_Flag_CSCTightHalo2015Filter;   //!
   TBranch        *b_Flag_globalTightHalo2016Filter;   //!
   TBranch        *b_Flag_globalSuperTightHalo2016Filter;   //!
   TBranch        *b_Flag_HcalStripHaloFilter;   //!
   TBranch        *b_Flag_hcalLaserEventFilter;   //!
   TBranch        *b_Flag_EcalDeadCellTriggerPrimitiveFilter;   //!
   TBranch        *b_Flag_EcalDeadCellBoundaryEnergyFilter;   //!
   TBranch        *b_Flag_ecalBadCalibFilter;   //!
   TBranch        *b_Flag_goodVertices;   //!
   TBranch        *b_Flag_eeBadScFilter;   //!
   TBranch        *b_Flag_ecalLaserCorrFilter;   //!
   TBranch        *b_Flag_trkPOGFilters;   //!
   TBranch        *b_Flag_chargedHadronTrackResolutionFilter;   //!
   TBranch        *b_Flag_muonBadTrackFilter;   //!
   TBranch        *b_Flag_BadChargedCandidateFilter;   //!
   TBranch        *b_Flag_BadPFMuonFilter;   //!
   TBranch        *b_Flag_BadChargedCandidateSummer16Filter;   //!
   TBranch        *b_Flag_BadPFMuonSummer16Filter;   //!
   TBranch        *b_Flag_trkPOG_manystripclus53X;   //!
   TBranch        *b_Flag_trkPOG_toomanystripclus53X;   //!
   TBranch        *b_Flag_trkPOG_logErrorTooManyClusters;   //!
   TBranch        *b_Flag_METFilters;   //!
   /*
   TBranch        *b_L1Reco_step;   //!
   TBranch        *b_HLTriggerFirstPath;   //!
   TBranch        *b_HLT_AK8PFJet360_TrimMass30;   //!
   TBranch        *b_HLT_AK8PFJet400_TrimMass30;   //!
   TBranch        *b_HLT_AK8PFHT750_TrimMass50;   //!
   TBranch        *b_HLT_AK8PFHT800_TrimMass50;   //!
   TBranch        *b_HLT_AK8DiPFJet300_200_TrimMass30_BTagCSV_p20;   //!
   TBranch        *b_HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p087;   //!
   TBranch        *b_HLT_AK8DiPFJet300_200_TrimMass30_BTagCSV_p087;   //!
   TBranch        *b_HLT_AK8DiPFJet300_200_TrimMass30;   //!
   TBranch        *b_HLT_AK8PFHT700_TrimR0p1PT0p03Mass50;   //!
   TBranch        *b_HLT_AK8PFHT650_TrimR0p1PT0p03Mass50;   //!
   TBranch        *b_HLT_AK8PFHT600_TrimR0p1PT0p03Mass50_BTagCSV_p20;   //!
   TBranch        *b_HLT_AK8DiPFJet280_200_TrimMass30;   //!
   TBranch        *b_HLT_AK8DiPFJet250_200_TrimMass30;   //!
   TBranch        *b_HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20;   //!
   TBranch        *b_HLT_AK8DiPFJet250_200_TrimMass30_BTagCSV_p20;   //!
   TBranch        *b_HLT_CaloJet260;   //!
   TBranch        *b_HLT_CaloJet500_NoJetID;   //!
   TBranch        *b_HLT_Dimuon13_PsiPrime;   //!
   TBranch        *b_HLT_Dimuon13_Upsilon;   //!
   TBranch        *b_HLT_Dimuon20_Jpsi;   //!
   TBranch        *b_HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf;   //!
   TBranch        *b_HLT_DoubleEle25_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_DoubleEle33_CaloIdL;   //!
   TBranch        *b_HLT_DoubleEle33_CaloIdL_MW;   //!
   TBranch        *b_HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW;   //!
   TBranch        *b_HLT_DoubleEle33_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_DoubleMediumCombinedIsoPFTau35_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_DoubleTightCombinedIsoPFTau35_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_DoubleMediumCombinedIsoPFTau40_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_DoubleTightCombinedIsoPFTau40_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_DoubleMediumCombinedIsoPFTau40_Trk1_eta2p1;   //!
   TBranch        *b_HLT_DoubleTightCombinedIsoPFTau40_Trk1_eta2p1;   //!
   TBranch        *b_HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_DoubleMediumIsoPFTau40_Trk1_eta2p1;   //!
   TBranch        *b_HLT_DoubleEle37_Ele27_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_DoubleMu33NoFiltersNoVtx;   //!
   TBranch        *b_HLT_DoubleMu38NoFiltersNoVtx;   //!
   TBranch        *b_HLT_DoubleMu23NoFiltersNoVtxDisplaced;   //!
   TBranch        *b_HLT_DoubleMu28NoFiltersNoVtxDisplaced;   //!
   TBranch        *b_HLT_DoubleMu0;   //!
   TBranch        *b_HLT_DoubleMu4_3_Bs;   //!
   TBranch        *b_HLT_DoubleMu4_3_Jpsi_Displaced;   //!
   TBranch        *b_HLT_DoubleMu4_JpsiTrk_Displaced;   //!
   TBranch        *b_HLT_DoubleMu4_LowMassNonResonantTrk_Displaced;   //!
   TBranch        *b_HLT_DoubleMu3_Trk_Tau3mu;   //!
   TBranch        *b_HLT_DoubleMu4_PsiPrimeTrk_Displaced;   //!
   TBranch        *b_HLT_Mu7p5_L2Mu2_Jpsi;   //!
   TBranch        *b_HLT_Mu7p5_L2Mu2_Upsilon;   //!
   TBranch        *b_HLT_Mu7p5_Track2_Jpsi;   //!
   TBranch        *b_HLT_Mu7p5_Track3p5_Jpsi;   //!
   TBranch        *b_HLT_Mu7p5_Track7_Jpsi;   //!
   TBranch        *b_HLT_Mu7p5_Track2_Upsilon;   //!
   TBranch        *b_HLT_Mu7p5_Track3p5_Upsilon;   //!
   TBranch        *b_HLT_Mu7p5_Track7_Upsilon;   //!
   TBranch        *b_HLT_Dimuon0er16_Jpsi_NoOS_NoVertexing;   //!
   TBranch        *b_HLT_Dimuon0er16_Jpsi_NoVertexing;   //!
   TBranch        *b_HLT_Dimuon6_Jpsi_NoVertexing;   //!
   TBranch        *b_HLT_Photon150;   //!
   TBranch        *b_HLT_Photon90_CaloIdL_HT300;   //!
   TBranch        *b_HLT_HT250_CaloMET70;   //!
   TBranch        *b_HLT_DoublePhoton60;   //!
   TBranch        *b_HLT_DoublePhoton85;   //!
   TBranch        *b_HLT_Ele17_Ele8_Gsf;   //!
   TBranch        *b_HLT_Ele20_eta2p1_WPLoose_Gsf_LooseIsoPFTau28;   //!
   TBranch        *b_HLT_Ele22_eta2p1_WPLoose_Gsf_LooseIsoPFTau29;   //!
   TBranch        *b_HLT_Ele22_eta2p1_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele22_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_Ele23_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele23_WPLoose_Gsf_WHbbBoost;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau20;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau30;   //!
   TBranch        *b_HLT_Ele25_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele25_eta2p1_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele25_eta2p1_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele27_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele27_WPLoose_Gsf_WHbbBoost;   //!
   TBranch        *b_HLT_Ele27_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele27_WPTight_Gsf_L1JetTauSeeded;   //!
   TBranch        *b_HLT_Ele27_eta2p1_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele27_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_Ele27_eta2p1_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele30_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele30_eta2p1_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele30_eta2p1_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele32_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele32_eta2p1_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele32_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_Ele32_eta2p1_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele35_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50;   //!
   TBranch        *b_HLT_Ele36_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_Ele45_WPLoose_Gsf;   //!
   TBranch        *b_HLT_Ele45_WPLoose_Gsf_L1JetTauSeeded;   //!
   TBranch        *b_HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50;   //!
   TBranch        *b_HLT_Ele105_CaloIdVT_GsfTrkIdT;   //!
   TBranch        *b_HLT_Ele30WP60_SC4_Mass55;   //!
   TBranch        *b_HLT_Ele30WP60_Ele8_Mass55;   //!
   TBranch        *b_HLT_HT200;   //!
   TBranch        *b_HLT_HT275;   //!
   TBranch        *b_HLT_HT325;   //!
   TBranch        *b_HLT_HT425;   //!
   TBranch        *b_HLT_HT575;   //!
   TBranch        *b_HLT_HT410to430;   //!
   TBranch        *b_HLT_HT430to450;   //!
   TBranch        *b_HLT_HT450to470;   //!
   TBranch        *b_HLT_HT470to500;   //!
   TBranch        *b_HLT_HT500to550;   //!
   TBranch        *b_HLT_HT550to650;   //!
   TBranch        *b_HLT_HT650;   //!
   TBranch        *b_HLT_Mu16_eta2p1_MET30;   //!
   TBranch        *b_HLT_IsoMu16_eta2p1_MET30;   //!
   TBranch        *b_HLT_IsoMu16_eta2p1_MET30_LooseIsoPFTau50_Trk30_eta2p1;   //!
   TBranch        *b_HLT_IsoMu17_eta2p1;   //!
   TBranch        *b_HLT_IsoMu17_eta2p1_LooseIsoPFTau20;   //!
   TBranch        *b_HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_DoubleIsoMu17_eta2p1;   //!
   TBranch        *b_HLT_DoubleIsoMu17_eta2p1_noDzCut;   //!
   TBranch        *b_HLT_IsoMu18;   //!
   TBranch        *b_HLT_IsoMu19_eta2p1_LooseIsoPFTau20;   //!
   TBranch        *b_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_IsoMu19_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_IsoMu19_eta2p1_LooseCombinedIsoPFTau20;   //!
   TBranch        *b_HLT_IsoMu19_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_IsoMu19_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_IsoMu21_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_IsoMu21_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_IsoMu20;   //!
   TBranch        *b_HLT_IsoMu21_eta2p1_LooseIsoPFTau20_SingleL1;   //!
   TBranch        *b_HLT_IsoMu21_eta2p1_LooseIsoPFTau50_Trk30_eta2p1_SingleL1;   //!
   TBranch        *b_HLT_IsoMu21_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg;   //!
   TBranch        *b_HLT_IsoMu22;   //!
   TBranch        *b_HLT_IsoMu22_eta2p1;   //!
   TBranch        *b_HLT_IsoMu24;   //!
   TBranch        *b_HLT_IsoMu27;   //!
   TBranch        *b_HLT_IsoTkMu18;   //!
   TBranch        *b_HLT_IsoTkMu20;   //!
   TBranch        *b_HLT_IsoTkMu22;   //!
   TBranch        *b_HLT_IsoTkMu22_eta2p1;   //!
   TBranch        *b_HLT_IsoTkMu24;   //!
   TBranch        *b_HLT_IsoTkMu27;   //!
   TBranch        *b_HLT_JetE30_NoBPTX3BX;   //!
   TBranch        *b_HLT_JetE30_NoBPTX;   //!
   TBranch        *b_HLT_JetE50_NoBPTX3BX;   //!
   TBranch        *b_HLT_JetE70_NoBPTX3BX;   //!
   TBranch        *b_HLT_L1SingleMu18;   //!
   TBranch        *b_HLT_L2Mu10;   //!
   TBranch        *b_HLT_L1SingleMuOpen;   //!
   TBranch        *b_HLT_L1SingleMuOpen_DT;   //!
   TBranch        *b_HLT_L2DoubleMu23_NoVertex;   //!
   TBranch        *b_HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10;   //!
   TBranch        *b_HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10;   //!
   TBranch        *b_HLT_L2Mu10_NoVertex_NoBPTX3BX;   //!
   TBranch        *b_HLT_L2Mu10_NoVertex_NoBPTX;   //!
   TBranch        *b_HLT_L2Mu45_NoVertex_3Sta_NoBPTX3BX;   //!
   TBranch        *b_HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX;   //!
   TBranch        *b_HLT_LooseIsoPFTau50_Trk30_eta2p1;   //!
   TBranch        *b_HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80;   //!
   TBranch        *b_HLT_LooseIsoPFTau50_Trk30_eta2p1_MET90;   //!
   TBranch        *b_HLT_LooseIsoPFTau50_Trk30_eta2p1_MET110;   //!
   TBranch        *b_HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120;   //!
   TBranch        *b_HLT_PFTau120_eta2p1;   //!
   TBranch        *b_HLT_PFTau140_eta2p1;   //!
   TBranch        *b_HLT_VLooseIsoPFTau120_Trk50_eta2p1;   //!
   TBranch        *b_HLT_VLooseIsoPFTau140_Trk50_eta2p1;   //!
   TBranch        *b_HLT_Mu17_Mu8;   //!
   TBranch        *b_HLT_Mu17_Mu8_DZ;   //!
   TBranch        *b_HLT_Mu17_Mu8_SameSign;   //!
   TBranch        *b_HLT_Mu17_Mu8_SameSign_DZ;   //!
   TBranch        *b_HLT_Mu20_Mu10;   //!
   TBranch        *b_HLT_Mu20_Mu10_DZ;   //!
   TBranch        *b_HLT_Mu20_Mu10_SameSign;   //!
   TBranch        *b_HLT_Mu20_Mu10_SameSign_DZ;   //!
   TBranch        *b_HLT_Mu17_TkMu8_DZ;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;   //!
   TBranch        *b_HLT_Mu25_TkMu0_dEta18_Onia;   //!
   TBranch        *b_HLT_Mu27_TkMu8;   //!
   TBranch        *b_HLT_Mu30_TkMu11;   //!
   TBranch        *b_HLT_Mu30_eta2p1_PFJet150_PFJet50;   //!
   TBranch        *b_HLT_Mu40_TkMu11;   //!
   TBranch        *b_HLT_Mu40_eta2p1_PFJet200_PFJet50;   //!
   TBranch        *b_HLT_Mu20;   //!
   TBranch        *b_HLT_TkMu17;   //!
   TBranch        *b_HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL;   //!
   TBranch        *b_HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ;   //!
   TBranch        *b_HLT_TkMu20;   //!
   TBranch        *b_HLT_Mu24_eta2p1;   //!
   TBranch        *b_HLT_TkMu24_eta2p1;   //!
   TBranch        *b_HLT_Mu27;   //!
   TBranch        *b_HLT_TkMu27;   //!
   TBranch        *b_HLT_Mu45_eta2p1;   //!
   TBranch        *b_HLT_Mu50;   //!
   TBranch        *b_HLT_TkMu50;   //!
   TBranch        *b_HLT_Mu38NoFiltersNoVtx_Photon38_CaloIdL;   //!
   TBranch        *b_HLT_Mu42NoFiltersNoVtx_Photon42_CaloIdL;   //!
   TBranch        *b_HLT_Mu28NoFiltersNoVtxDisplaced_Photon28_CaloIdL;   //!
   TBranch        *b_HLT_Mu33NoFiltersNoVtxDisplaced_Photon33_CaloIdL;   //!
   TBranch        *b_HLT_Mu23NoFiltersNoVtx_Photon23_CaloIdL;   //!
   TBranch        *b_HLT_DoubleMu18NoFiltersNoVtx;   //!
   TBranch        *b_HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Tight;   //!
   TBranch        *b_HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Loose;   //!
   TBranch        *b_HLT_Mu28NoFiltersNoVtx_DisplacedJet40_Loose;   //!
   TBranch        *b_HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Tight;   //!
   TBranch        *b_HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Loose;   //!
   TBranch        *b_HLT_Mu38NoFiltersNoVtx_DisplacedJet60_Loose;   //!
   TBranch        *b_HLT_Mu28NoFiltersNoVtx_CentralCaloJet40;   //!
   TBranch        *b_HLT_PFHT300_PFMET100;   //!
   TBranch        *b_HLT_PFHT300_PFMET110;   //!
   TBranch        *b_HLT_PFHT550_4JetPt50;   //!
   TBranch        *b_HLT_PFHT650_4JetPt50;   //!
   TBranch        *b_HLT_PFHT750_4JetPt50;   //!
   TBranch        *b_HLT_PFHT750_4JetPt70;   //!
   TBranch        *b_HLT_PFHT750_4JetPt80;   //!
   TBranch        *b_HLT_PFHT800_4JetPt50;   //!
   TBranch        *b_HLT_PFHT850_4JetPt50;   //!
   TBranch        *b_HLT_PFJet15_NoCaloMatched;   //!
   TBranch        *b_HLT_PFJet25_NoCaloMatched;   //!
   TBranch        *b_HLT_DiPFJet15_NoCaloMatched;   //!
   TBranch        *b_HLT_DiPFJet25_NoCaloMatched;   //!
   TBranch        *b_HLT_DiPFJet15_FBEta3_NoCaloMatched;   //!
   TBranch        *b_HLT_DiPFJet25_FBEta3_NoCaloMatched;   //!
   TBranch        *b_HLT_DiPFJetAve15_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve25_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve35_HFJEC;   //!
   TBranch        *b_HLT_AK8PFJet40;   //!
   TBranch        *b_HLT_AK8PFJet60;   //!
   TBranch        *b_HLT_AK8PFJet80;   //!
   TBranch        *b_HLT_AK8PFJet140;   //!
   TBranch        *b_HLT_AK8PFJet200;   //!
   TBranch        *b_HLT_AK8PFJet260;   //!
   TBranch        *b_HLT_AK8PFJet320;   //!
   TBranch        *b_HLT_AK8PFJet400;   //!
   TBranch        *b_HLT_AK8PFJet450;   //!
   TBranch        *b_HLT_AK8PFJet500;   //!
   TBranch        *b_HLT_PFJet40;   //!
   TBranch        *b_HLT_PFJet60;   //!
   TBranch        *b_HLT_PFJet80;   //!
   TBranch        *b_HLT_PFJet140;   //!
   TBranch        *b_HLT_PFJet200;   //!
   TBranch        *b_HLT_PFJet260;   //!
   TBranch        *b_HLT_PFJet320;   //!
   TBranch        *b_HLT_PFJet400;   //!
   TBranch        *b_HLT_PFJet450;   //!
   TBranch        *b_HLT_PFJet500;   //!
   TBranch        *b_HLT_DiPFJetAve40;   //!
   TBranch        *b_HLT_DiPFJetAve60;   //!
   TBranch        *b_HLT_DiPFJetAve80;   //!
   TBranch        *b_HLT_DiPFJetAve140;   //!
   TBranch        *b_HLT_DiPFJetAve200;   //!
   TBranch        *b_HLT_DiPFJetAve260;   //!
   TBranch        *b_HLT_DiPFJetAve320;   //!
   TBranch        *b_HLT_DiPFJetAve400;   //!
   TBranch        *b_HLT_DiPFJetAve500;   //!
   TBranch        *b_HLT_DiPFJetAve60_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve80_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve100_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve160_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve220_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve300_HFJEC;   //!
   TBranch        *b_HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu140;   //!
   TBranch        *b_HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu80;   //!
   TBranch        *b_HLT_DiCentralPFJet170;   //!
   TBranch        *b_HLT_SingleCentralPFJet170_CFMax0p1;   //!
   TBranch        *b_HLT_DiCentralPFJet170_CFMax0p1;   //!
   TBranch        *b_HLT_DiCentralPFJet220_CFMax0p3;   //!
   TBranch        *b_HLT_DiCentralPFJet330_CFMax0p5;   //!
   TBranch        *b_HLT_DiCentralPFJet430;   //!
   TBranch        *b_HLT_PFHT125;   //!
   TBranch        *b_HLT_PFHT200;   //!
   TBranch        *b_HLT_PFHT250;   //!
   TBranch        *b_HLT_PFHT300;   //!
   TBranch        *b_HLT_PFHT350;   //!
   TBranch        *b_HLT_PFHT400;   //!
   TBranch        *b_HLT_PFHT475;   //!
   TBranch        *b_HLT_PFHT600;   //!
   TBranch        *b_HLT_PFHT650;   //!
   TBranch        *b_HLT_PFHT800;   //!
   TBranch        *b_HLT_PFHT900;   //!
   TBranch        *b_HLT_PFHT200_PFAlphaT0p51;   //!
   TBranch        *b_HLT_PFHT200_DiPFJetAve90_PFAlphaT0p57;   //!
   TBranch        *b_HLT_PFHT200_DiPFJetAve90_PFAlphaT0p63;   //!
   TBranch        *b_HLT_PFHT250_DiPFJetAve90_PFAlphaT0p55;   //!
   TBranch        *b_HLT_PFHT250_DiPFJetAve90_PFAlphaT0p58;   //!
   TBranch        *b_HLT_PFHT300_DiPFJetAve90_PFAlphaT0p53;   //!
   TBranch        *b_HLT_PFHT300_DiPFJetAve90_PFAlphaT0p54;   //!
   TBranch        *b_HLT_PFHT350_DiPFJetAve90_PFAlphaT0p52;   //!
   TBranch        *b_HLT_PFHT350_DiPFJetAve90_PFAlphaT0p53;   //!
   TBranch        *b_HLT_PFHT400_DiPFJetAve90_PFAlphaT0p51;   //!
   TBranch        *b_HLT_PFHT400_DiPFJetAve90_PFAlphaT0p52;   //!
   TBranch        *b_HLT_MET60_IsoTrk35_Loose;   //!
   TBranch        *b_HLT_MET75_IsoTrk50;   //!
   TBranch        *b_HLT_MET90_IsoTrk50;   //!
   TBranch        *b_HLT_PFMET120_BTagCSV_p067;   //!
   TBranch        *b_HLT_PFMET120_Mu5;   //!
   TBranch        *b_HLT_PFMET170_NotCleaned;   //!
   TBranch        *b_HLT_PFMET170_NoiseCleaned;   //!
   TBranch        *b_HLT_PFMET170_HBHECleaned;   //!
   TBranch        *b_HLT_PFMET170_JetIdCleaned;   //!
   TBranch        *b_HLT_PFMET170_BeamHaloCleaned;   //!
   TBranch        *b_HLT_PFMET170_HBHE_BeamHaloCleaned;   //!
   TBranch        *b_HLT_PFMETTypeOne190_HBHE_BeamHaloCleaned;   //!
   TBranch        *b_HLT_PFMET90_PFMHT90_IDTight;   //!
   TBranch        *b_HLT_PFMET100_PFMHT100_IDTight;   //!
   TBranch        *b_HLT_PFMET100_PFMHT100_IDTight_BeamHaloCleaned;   //!
   TBranch        *b_HLT_PFMET110_PFMHT110_IDTight;   //!
   TBranch        *b_HLT_PFMET120_PFMHT120_IDTight;   //!
   TBranch        *b_HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_BTagCSV_p067;   //!
   TBranch        *b_HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight;   //!
   TBranch        *b_HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200;   //!
   TBranch        *b_HLT_QuadPFJet_BTagCSV_p016_VBF_Mqq460;   //!
   TBranch        *b_HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq240;   //!
   TBranch        *b_HLT_QuadPFJet_BTagCSV_p016_VBF_Mqq500;   //!
   TBranch        *b_HLT_QuadPFJet_VBF;   //!
   TBranch        *b_HLT_L1_TripleJet_VBF;   //!
   TBranch        *b_HLT_QuadJet45_TripleBTagCSV_p087;   //!
   TBranch        *b_HLT_QuadJet45_DoubleBTagCSV_p087;   //!
   TBranch        *b_HLT_DoubleJet90_Double30_TripleBTagCSV_p087;   //!
   TBranch        *b_HLT_DoubleJet90_Double30_DoubleBTagCSV_p087;   //!
   TBranch        *b_HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160;   //!
   TBranch        *b_HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6;   //!
   TBranch        *b_HLT_DoubleJetsC112_DoubleBTagCSV_p026_DoublePFJetsC172;   //!
   TBranch        *b_HLT_DoubleJetsC112_DoubleBTagCSV_p014_DoublePFJetsC112MaxDeta1p6;   //!
   TBranch        *b_HLT_DoubleJetsC100_SingleBTagCSV_p026;   //!
   TBranch        *b_HLT_DoubleJetsC100_SingleBTagCSV_p014;   //!
   TBranch        *b_HLT_DoubleJetsC100_SingleBTagCSV_p026_SinglePFJetC350;   //!
   TBranch        *b_HLT_DoubleJetsC100_SingleBTagCSV_p014_SinglePFJetC350;   //!
   TBranch        *b_HLT_Photon135_PFMET100;   //!
   TBranch        *b_HLT_Photon20_CaloIdVL_IsoL;   //!
   TBranch        *b_HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40;   //!
   TBranch        *b_HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_VBF;   //!
   TBranch        *b_HLT_Photon250_NoHE;   //!
   TBranch        *b_HLT_Photon300_NoHE;   //!
   TBranch        *b_HLT_Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60;   //!
   TBranch        *b_HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15;   //!
   TBranch        *b_HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40;   //!
   TBranch        *b_HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_VBF;   //!
   TBranch        *b_HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40;   //!
   TBranch        *b_HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_VBF;   //!
   TBranch        *b_HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40;   //!
   TBranch        *b_HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_VBF;   //!
   TBranch        *b_HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40;   //!
   TBranch        *b_HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_VBF;   //!
   TBranch        *b_HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_PFMET40;   //!
   TBranch        *b_HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_VBF;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL;   //!
   TBranch        *b_HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30;   //!
   TBranch        *b_HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30;   //!
   TBranch        *b_HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30;   //!
   TBranch        *b_HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30;   //!
   TBranch        *b_HLT_BTagMu_DiJet20_Mu5;   //!
   TBranch        *b_HLT_BTagMu_DiJet40_Mu5;   //!
   TBranch        *b_HLT_BTagMu_DiJet70_Mu5;   //!
   TBranch        *b_HLT_BTagMu_DiJet110_Mu5;   //!
   TBranch        *b_HLT_BTagMu_DiJet170_Mu5;   //!
   TBranch        *b_HLT_BTagMu_Jet300_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK8Jet300_Mu5;   //!
   TBranch        *b_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_L1JetTauSeeded;   //!
   TBranch        *b_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Mu30_Ele30_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_Mu33_Ele33_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_Mu37_Ele27_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_Mu27_Ele37_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_Mu8_DiEle12_CaloIdL_TrackIdL;   //!
   TBranch        *b_HLT_Mu12_Photon25_CaloIdL;   //!
   TBranch        *b_HLT_Mu12_Photon25_CaloIdL_L1ISO;   //!
   TBranch        *b_HLT_Mu12_Photon25_CaloIdL_L1OR;   //!
   TBranch        *b_HLT_Mu17_Photon22_CaloIdL_L1ISO;   //!
   TBranch        *b_HLT_Mu17_Photon30_CaloIdL_L1ISO;   //!
   TBranch        *b_HLT_Mu17_Photon35_CaloIdL_L1ISO;   //!
   TBranch        *b_HLT_DiMu9_Ele9_CaloIdL_TrackIdL;   //!
   TBranch        *b_HLT_TripleMu_5_3_3;   //!
   TBranch        *b_HLT_TripleMu_12_10_5;   //!
   TBranch        *b_HLT_Mu3er_PFHT140_PFMET125;   //!
   TBranch        *b_HLT_Mu6_PFHT200_PFMET80_BTagCSV_p067;   //!
   TBranch        *b_HLT_Mu6_PFHT200_PFMET100;   //!
   TBranch        *b_HLT_Mu14er_PFMET100;   //!
   TBranch        *b_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Ele12_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Ele17_CaloIdL_GsfTrkIdVL;   //!
   TBranch        *b_HLT_Ele17_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Ele23_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_PFHT650_WideJetMJJ900DEtaJJ1p5;   //!
   TBranch        *b_HLT_PFHT650_WideJetMJJ950DEtaJJ1p5;   //!
   TBranch        *b_HLT_Photon22;   //!
   TBranch        *b_HLT_Photon30;   //!
   TBranch        *b_HLT_Photon36;   //!
   TBranch        *b_HLT_Photon50;   //!
   TBranch        *b_HLT_Photon75;   //!
   TBranch        *b_HLT_Photon90;   //!
   TBranch        *b_HLT_Photon120;   //!
   TBranch        *b_HLT_Photon175;   //!
   TBranch        *b_HLT_Photon165_HE10;   //!
   TBranch        *b_HLT_Photon22_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon30_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon36_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon50_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon75_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon90_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon120_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon165_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90;   //!
   TBranch        *b_HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70;   //!
   TBranch        *b_HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55;   //!
   TBranch        *b_HLT_Diphoton30_18_Solid_R9Id_AND_IsoCaloId_AND_HE_R9Id_Mass55;   //!
   TBranch        *b_HLT_Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi_Muon;   //!
   TBranch        *b_HLT_Dimuon0_Upsilon_Muon;   //!
   TBranch        *b_HLT_QuadMuon0_Dimuon0_Jpsi;   //!
   TBranch        *b_HLT_QuadMuon0_Dimuon0_Upsilon;   //!
   TBranch        *b_HLT_Rsq0p25_Calo;   //!
   TBranch        *b_HLT_RsqMR240_Rsq0p09_MR200_4jet_Calo;   //!
   TBranch        *b_HLT_RsqMR240_Rsq0p09_MR200_Calo;   //!
   TBranch        *b_HLT_Rsq0p25;   //!
   TBranch        *b_HLT_Rsq0p30;   //!
   TBranch        *b_HLT_RsqMR240_Rsq0p09_MR200;   //!
   TBranch        *b_HLT_RsqMR240_Rsq0p09_MR200_4jet;   //!
   TBranch        *b_HLT_RsqMR270_Rsq0p09_MR200;   //!
   TBranch        *b_HLT_RsqMR270_Rsq0p09_MR200_4jet;   //!
   TBranch        *b_HLT_Rsq0p02_MR300_TriPFJet80_60_40_BTagCSV_p063_p20_Mbb60_200;   //!
   TBranch        *b_HLT_Rsq0p02_MR400_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;   //!
   TBranch        *b_HLT_Rsq0p02_MR450_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;   //!
   TBranch        *b_HLT_Rsq0p02_MR500_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;   //!
   TBranch        *b_HLT_Rsq0p02_MR550_TriPFJet80_60_40_DoubleBTagCSV_p063_Mbb60_200;   //!
   TBranch        *b_HLT_HT200_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT250_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT350_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT350_DisplacedDijet80_DisplacedTrack;   //!
   TBranch        *b_HLT_HT350_DisplacedDijet80_Tight_DisplacedTrack;   //!
   TBranch        *b_HLT_HT350_DisplacedDijet40_Inclusive;   //!
   TBranch        *b_HLT_HT400_DisplacedDijet40_Inclusive;   //!
   TBranch        *b_HLT_HT500_DisplacedDijet40_Inclusive;   //!
   TBranch        *b_HLT_HT550_DisplacedDijet40_Inclusive;   //!
   TBranch        *b_HLT_HT550_DisplacedDijet80_Inclusive;   //!
   TBranch        *b_HLT_HT650_DisplacedDijet80_Inclusive;   //!
   TBranch        *b_HLT_HT750_DisplacedDijet80_Inclusive;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_DisplacedTrack;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_DisplacedTrack_2TrackIP2DSig5;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_TightID_DisplacedTrack;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_Hadronic;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_Hadronic_2PromptTrack;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_TightID_Hadronic;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_VTightID_Hadronic;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_VVTightID_Hadronic;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_VTightID_DisplacedTrack;   //!
   TBranch        *b_HLT_VBF_DisplacedJet40_VVTightID_DisplacedTrack;   //!
   TBranch        *b_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight;   //!
   TBranch        *b_HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90_IDTight;   //!
   TBranch        *b_HLT_MonoCentralPFJet80_PFMETNoMu100_PFMHTNoMu100_IDTight;   //!
   TBranch        *b_HLT_MonoCentralPFJet80_PFMETNoMu110_PFMHTNoMu110_IDTight;   //!
   TBranch        *b_HLT_MonoCentralPFJet80_PFMETNoMu120_PFMHTNoMu120_IDTight;   //!
   TBranch        *b_HLT_Ele27_eta2p1_WPLoose_Gsf_HT200;   //!
   TBranch        *b_HLT_Photon90_CaloIdL_PFHT500;   //!
   TBranch        *b_HLT_DoubleMu8_Mass8_PFHT250;   //!
   TBranch        *b_HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT250;   //!
   TBranch        *b_HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT250;   //!
   TBranch        *b_HLT_DoubleMu8_Mass8_PFHT300;   //!
   TBranch        *b_HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT300;   //!
   TBranch        *b_HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT300;   //!
   TBranch        *b_HLT_Mu10_CentralPFJet30_BTagCSV_p13;   //!
   TBranch        *b_HLT_DoubleMu3_PFMET50;   //!
   TBranch        *b_HLT_Ele10_CaloIdM_TrackIdM_CentralPFJet30_BTagCSV_p13;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_BTagCSV_p067_PFHT400;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT350_PFMET50;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT600;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT350;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT400_PFMET50;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT400;   //!
   TBranch        *b_HLT_Ele50_IsoVVVL_PFHT400;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60;   //!
   TBranch        *b_HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_BTagCSV_p067_PFHT400;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT350_PFMET50;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT600;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT350;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT400_PFMET50;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT400;   //!
   TBranch        *b_HLT_Mu50_IsoVVVL_PFHT400;   //!
   TBranch        *b_HLT_Dimuon16_Jpsi;   //!
   TBranch        *b_HLT_Dimuon10_Jpsi_Barrel;   //!
   TBranch        *b_HLT_Dimuon8_PsiPrime_Barrel;   //!
   TBranch        *b_HLT_Dimuon8_Upsilon_Barrel;   //!
   TBranch        *b_HLT_Dimuon0_Phi_Barrel;   //!
   TBranch        *b_HLT_Mu16_TkMu0_dEta18_Onia;   //!
   TBranch        *b_HLT_Mu16_TkMu0_dEta18_Phi;   //!
   TBranch        *b_HLT_TrkMu15_DoubleTrkMu5NoFiltersNoVtx;   //!
   TBranch        *b_HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx;   //!
   TBranch        *b_HLT_Mu8;   //!
   TBranch        *b_HLT_Mu17;   //!
   TBranch        *b_HLT_Mu3_PFJet40;   //!
   TBranch        *b_HLT_Ele8_CaloIdM_TrackIdM_PFJet30;   //!
   TBranch        *b_HLT_Ele12_CaloIdM_TrackIdM_PFJet30;   //!
   TBranch        *b_HLT_Ele17_CaloIdM_TrackIdM_PFJet30;   //!
   TBranch        *b_HLT_Ele23_CaloIdM_TrackIdM_PFJet30;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet140;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;   //!
   TBranch        *b_HLT_PFHT400_SixJet30_DoubleBTagCSV_p056;   //!
   TBranch        *b_HLT_PFHT450_SixJet40_BTagCSV_p056;   //!
   TBranch        *b_HLT_PFHT400_SixJet30;   //!
   TBranch        *b_HLT_PFHT450_SixJet40;   //!
   TBranch        *b_HLT_Ele115_CaloIdVT_GsfTrkIdT;   //!
   TBranch        *b_HLT_Mu55;   //!
   TBranch        *b_HLT_Photon42_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon25_AND_HE10_R9Id65_Eta2_Mass15;   //!
   TBranch        *b_HLT_Photon90_CaloIdL_PFHT600;   //!
   TBranch        *b_HLT_PixelTracks_Multiplicity60ForEndOfFill;   //!
   TBranch        *b_HLT_PixelTracks_Multiplicity85ForEndOfFill;   //!
   TBranch        *b_HLT_PixelTracks_Multiplicity110ForEndOfFill;   //!
   TBranch        *b_HLT_PixelTracks_Multiplicity135ForEndOfFill;   //!
   TBranch        *b_HLT_PixelTracks_Multiplicity160ForEndOfFill;   //!
   TBranch        *b_HLT_FullTracks_Multiplicity80;   //!
   TBranch        *b_HLT_FullTracks_Multiplicity100;   //!
   TBranch        *b_HLT_FullTracks_Multiplicity130;   //!
   TBranch        *b_HLT_FullTracks_Multiplicity150;   //!
   TBranch        *b_HLT_ECALHT800;   //!
   TBranch        *b_HLT_DiSC30_18_EIso_AND_HE_Mass70;   //!
   TBranch        *b_HLT_Photon125;   //!
   TBranch        *b_HLT_MET100;   //!
   TBranch        *b_HLT_MET150;   //!
   TBranch        *b_HLT_MET200;   //!
   TBranch        *b_HLT_Ele27_HighEta_Ele20_Mass55;   //!
   TBranch        *b_HLT_L1FatEvents;   //!
   TBranch        *b_HLT_Physics;   //!
   TBranch        *b_HLT_L1FatEvents_part0;   //!
   TBranch        *b_HLT_L1FatEvents_part1;   //!
   TBranch        *b_HLT_L1FatEvents_part2;   //!
   TBranch        *b_HLT_L1FatEvents_part3;   //!
   TBranch        *b_HLT_Random;   //!
   TBranch        *b_HLT_ZeroBias;   //!
   TBranch        *b_HLT_AK4CaloJet30;   //!
   TBranch        *b_HLT_AK4CaloJet40;   //!
   TBranch        *b_HLT_AK4CaloJet50;   //!
   TBranch        *b_HLT_AK4CaloJet80;   //!
   TBranch        *b_HLT_AK4CaloJet100;   //!
   TBranch        *b_HLT_AK4PFJet30;   //!
   TBranch        *b_HLT_AK4PFJet50;   //!
   TBranch        *b_HLT_AK4PFJet80;   //!
   TBranch        *b_HLT_AK4PFJet100;   //!
   TBranch        *b_HLT_HISinglePhoton10;   //!
   TBranch        *b_HLT_HISinglePhoton15;   //!
   TBranch        *b_HLT_HISinglePhoton20;   //!
   TBranch        *b_HLT_HISinglePhoton40;   //!
   TBranch        *b_HLT_HISinglePhoton60;   //!
   TBranch        *b_HLT_EcalCalibration;   //!
   TBranch        *b_HLT_HcalCalibration;   //!
   TBranch        *b_HLT_GlobalRunHPDNoise;   //!
   TBranch        *b_HLT_L1BptxMinus;   //!
   TBranch        *b_HLT_L1BptxPlus;   //!
   TBranch        *b_HLT_L1NotBptxOR;   //!
   TBranch        *b_HLT_L1BeamGasMinus;   //!
   TBranch        *b_HLT_L1BeamGasPlus;   //!
   TBranch        *b_HLT_L1BptxXOR;   //!
   TBranch        *b_HLT_L1MinimumBiasHF_OR;   //!
   TBranch        *b_HLT_L1MinimumBiasHF_AND;   //!
   TBranch        *b_HLT_HcalNZS;   //!
   TBranch        *b_HLT_HcalPhiSym;   //!
   TBranch        *b_HLT_HcalIsolatedbunch;   //!
   TBranch        *b_HLT_ZeroBias_FirstCollisionAfterAbortGap;   //!
   TBranch        *b_HLT_ZeroBias_FirstCollisionAfterAbortGap_copy;   //!
   TBranch        *b_HLT_ZeroBias_FirstCollisionAfterAbortGap_TCDS;   //!
   TBranch        *b_HLT_ZeroBias_IsolatedBunches;   //!
   TBranch        *b_HLT_ZeroBias_FirstCollisionInTrain;   //!
   TBranch        *b_HLT_ZeroBias_FirstBXAfterTrain;   //!
   TBranch        *b_HLT_Photon500;   //!
   TBranch        *b_HLT_Photon600;   //!
   TBranch        *b_HLT_Mu300;   //!
   TBranch        *b_HLT_Mu350;   //!
   TBranch        *b_HLT_MET250;   //!
   TBranch        *b_HLT_MET300;   //!
   TBranch        *b_HLT_MET600;   //!
   TBranch        *b_HLT_MET700;   //!
   TBranch        *b_HLT_PFMET300;   //!
   TBranch        *b_HLT_PFMET400;   //!
   TBranch        *b_HLT_PFMET500;   //!
   TBranch        *b_HLT_PFMET600;   //!
   TBranch        *b_HLT_Ele250_CaloIdVT_GsfTrkIdT;   //!
   TBranch        *b_HLT_Ele300_CaloIdVT_GsfTrkIdT;   //!
   TBranch        *b_HLT_HT2000;   //!
   TBranch        *b_HLT_HT2500;   //!
   TBranch        *b_HLT_IsoTrackHE;   //!
   TBranch        *b_HLT_IsoTrackHB;   //!
   TBranch        *b_HLTriggerFinalPath;   //!
   TBranch        *b_L1simulation_step;   //!
*/

   nanov8(TChain *tree=0);
   virtual ~nanov8();
   //virtual Int_t    Cut(Long64_t entry);
   //virtual Int_t    GetEntry(Long64_t entry);
   //virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *tree);
   //virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

