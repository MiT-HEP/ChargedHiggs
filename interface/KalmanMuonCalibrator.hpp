//from https://github.com/emanca/Analysis/blob/KalmanDevelopment/Calibration/src/KalmanMuonCalibrator.cc
//adapted by Andrea Carlo Marini
//Mon May 13 15:58:29 CEST 2019
//
#ifndef KALMAN_MUON_CALIBRATOR_H
#define KALMAN_MUON_CALIBRATOR_H

#include "TFile.h"
#include "TH3F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1I.h"
#include "TProfile2D.h"
#include "TRandom3.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"
class KalmanMuonCalibrator {
 public:
  KalmanMuonCalibrator();
  KalmanMuonCalibrator(const std::string&);
  double getCorrectedPt(double pt,double eta,double phi,int charge);
  double smear(double pt,double eta);
  double smearForSync(double pt,double eta);
  double getCorrectedPtMag(double,double,double);
  double getCorrectedError(double pt,double eta,double error);
  double getResolution(double pt,double eta);


  int getN();
  void vary(int,int);
  void varyClosure(int);
  void reset();
  void randomize();


  ~KalmanMuonCalibrator();



 private:
  enum  Measurement {A,K,e,B,L};
  unsigned int getBin(Measurement measurement,float eta,float phi);
  double getData(Measurement histo,float eta,float phi);

  double closure(double,double);
  TRandom * random_;
  void resetHisto();
  int varyClosure_;

  bool isData_;
  TFile *scaleFile_;
  TFile *precalibFile_;
  TFile *magneticFile_;
  TFile *resolutionFile_;



  //magnetic map correction
  TH2F *magnetic_; 

  //Scale Calibration
  TH1D* calib_;
  TH1D* shifted_calib_;


  //Resolution
  TH1D* aRes_,*bRes_,*cRes_,*dRes_;
  TH1D* aEbE_,*bEbE_,*cEbE_,*dEbE_;


  //For stat error
  TMatrixD *eigenvectors_;
  TVectorD *eigenvalues_;

  //Navigation
  unsigned int nEtaBins_;
  TAxis* etaAxis_;
  unsigned int nEtaMaterialBins_;
  TAxis* etaMaterialAxis_;

  unsigned int nPhiBins_;
  TAxis* phiAxis_;



};

#endif
