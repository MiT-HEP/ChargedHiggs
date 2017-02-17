//From Michalis Bachtis
//https://github.com/bachtis/Analysis/blob/KaMuCa_V3/Calibration/src/KalmanMuonCalibrator.cc

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


  int getN();
  void vary(int,int);
  void varyClosure(int);
  void reset();
  void randomize();


  ~KalmanMuonCalibrator();



 private:
  double closure(double,double);
  TRandom * random_;
  void resetHisto(TH1*,const TH1* );
  int varyClosure_;

  bool isData_;
  TFile *file_;


  //magnetic map correction
  TH2F *magnetic; 


  //precorrection from Z
  //  TH3F *scale_P1; 
  //  TH3F *scale_P2; 


  TH3F *scale_A; 
  TH3F *scale_K; 
  TH3F *scale_L; 
  TH3F *scale_A1; 
  TH3F *scale_A2; 
  TH3F *scale_A3; 
  TH3F *scale_A4; 
  TH3F *scale_e;
  TH3F *scale_B0;
  TH3F *scale_B1;
  TH3F *scale_B2;
  TH3F *scale_B3;
  TH3F *scale_B4;

  TH3F *shifted_A; 
  TH3F *shifted_K; 
  TH3F *shifted_L; 
  TH3F *shifted_A1; 
  TH3F *shifted_A2; 
  TH3F *shifted_A3; 
  TH3F *shifted_A4; 
  TH3F *shifted_e;
  TH3F *shifted_B0;
  TH3F *shifted_B1;
  TH3F *shifted_B2;
  TH3F *shifted_B3;
  TH3F *shifted_B4;


  TH1D* aRES_;
  TH1D* bRES_;
  TH1D* cRES_;
  TH1D* dRES_;
  TH1D* aEBE_;
  TH1D* bEBE_;
  TH1D* cEBE_;
  TH1D* dEBE_;




  TH3F *closure_;
  TMatrixD *eigenvectors_;
  TVectorD *eigenvalues_;
  TH1I *covHistoMap_;
  TH1I *covBinMap_;
};

#endif
