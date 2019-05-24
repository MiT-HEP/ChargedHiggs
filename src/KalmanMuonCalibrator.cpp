#include "interface/KalmanMuonCalibrator.hpp"
//#include "FWCore/ParameterSet/interface/FileInPath.h"
#include <math.h>
KalmanMuonCalibrator::KalmanMuonCalibrator() {
}

KalmanMuonCalibrator::KalmanMuonCalibrator(const std::string& filename) {

  random_ = new TRandom3(10101982);

  //edm::FileInPath path("KaMuCa/Calibration/data/scale_"+filename+".root");
  std::string path="aux/KaMuCa/"+filename+".root";
  //scaleFile_ = new TFile(path.fullPath().c_str());
  scaleFile_ = new TFile(path.c_str());
  isData_ = (filename.find("DATA")!=std::string::npos);

  //edm::FileInPath rpath("KaMuCa/Calibration/data/resolution_"+filename+".root");
  std::string rpath("aux/KaMuCa/Calibration/data/resolution_"+filename+".root");
  //resolutionFile_ = new TFile(rpath.fullPath().c_str());
  //resolutionFile_ = new TFile(rpath.c_str());
  //aEbE_ = (TH1D*)resolutionFile_->Get("aEbE");
  //bEbE_ = (TH1D*)resolutionFile_->Get("bEbE");
  //cEbE_ = (TH1D*)resolutionFile_->Get("cEbE");
  //dEbE_ = (TH1D*)resolutionFile_->Get("dEbE");
  //aRes_ = (TH1D*)resolutionFile_->Get("aRes");
  //bRes_ = (TH1D*)resolutionFile_->Get("bRes");
  //cRes_ = (TH1D*)resolutionFile_->Get("cRes");
  //dRes_ = (TH1D*)resolutionFile_->Get("dRes");


  calib_ = (TH1D*)scaleFile_->Get("data");
  shifted_calib_ = (TH1D*) calib_->Clone();
  shifted_calib_->SetName("shifted_calib");
  eigenvalues_ = (TVectorD*)scaleFile_->Get("eigenvalues");
  eigenvectors_ = (TMatrixD*)scaleFile_->Get("eigenvectors");


  TH1D * etaH = (TH1D*)scaleFile_->Get("B");
  nEtaBins_ = etaH->GetNbinsX();
  etaAxis_ = etaH->GetXaxis(); 
  nPhiBins_ = etaH->GetNbinsY();
  phiAxis_ = etaH->GetYaxis(); 

  TH1D* etaMH =(TH1D*)scaleFile_->Get("e");   
  nEtaMaterialBins_ = etaMH->GetNbinsX();
  etaMaterialAxis_ = etaMH->GetXaxis(); 

  //edm::FileInPath path1("KaMuCa/Calibration/data/bFieldMap.root");
  std::string path1("aux/KaMuCa/bFieldMap.root"); // hard-coded
  //magneticFile_=new TFile(path1.fullPath().c_str());
  magneticFile_=new TFile(path1.c_str());
  magnetic_ = (TH2F*)magneticFile_->Get("bfieldMap");


  random_ = new TRandom3(10101982);

  varyClosure_=0;
 }


void KalmanMuonCalibrator::reset() {
  varyClosure_=0;
  for (int i=0;i<calib_->GetNbinsX()+1;++i)
    shifted_calib_->SetBinContent(i,calib_->GetBinContent(i));
}



KalmanMuonCalibrator::~KalmanMuonCalibrator() {
  scaleFile_->Close();
  resolutionFile_->Close();
  magneticFile_->Close();
}


unsigned int KalmanMuonCalibrator::getBin(Measurement histo,float eta,float phi) {
  switch(histo) {
  case A:
    return 0;
  case K:
    return 1;
  case L:
    return 2+etaAxis_->FindBin(eta)-1;
  case e:
    return 2+nEtaBins_+etaMaterialAxis_->FindBin(eta)-1;
  case B:
    return 2+nEtaBins_+nEtaMaterialBins_+etaAxis_->FindBin(eta)+nEtaBins_*(phiAxis_->FindBin(phi)-1)-1;
  }
  return -1;
}




// unsigned int KalmanMuonCalibrator::getBin(Measurement histo,float eta) {
//   switch(histo) {
//   case A:
//     return etaAxis_->FindBin(eta);
//   case A11:
//     return nEtaBins_+etaAxis_->FindBin(eta);
//   case A12:
//     return 2*nEtaBins_+etaAxis_->FindBin(eta);
//   case A21:
//     return 3*nEtaBins_+etaAxis_->FindBin(eta);
//   case A22:
//     return 4*nEtaBins_+etaAxis_->FindBin(eta);
//   case A31:
//     return 5*nEtaBins_+etaAxis_->FindBin(eta);
//   case A32:
//     return 6*nEtaBins_+etaAxis_->FindBin(eta);
//   case B0:
//     return 7*nEtaBins_+etaAxis_->FindBin(eta);
//   case B11:
//     return 8*nEtaBins_+etaAxis_->FindBin(eta);
//   case B12:
//     return 9*nEtaBins_+etaAxis_->FindBin(eta);
//   case B21:
//     return 10*nEtaBins_+etaAxis_->FindBin(eta);
//   case B22:
//     return 11*nEtaBins_+etaAxis_->FindBin(eta);
//   case B31:
//     return 12*nEtaBins_+etaAxis_->FindBin(eta);
//   case B32:
//     return 13*nEtaBins_+etaAxis_->FindBin(eta);
//   case e:
//     return 14*nEtaBins_+etaMaterialAxis_->FindBin(eta);
//   case e2:
//     return 14*nEtaBins_+nMaterialEtaBins_+etaMaterialAxis_->FindBin(eta);
//   default:
//     return -1;
//   }
//   return -1;
// }



double KalmanMuonCalibrator::getData(Measurement histo,float eta,float phi) {
  unsigned int bin = getBin(histo,eta,phi);
  return shifted_calib_->GetBinContent(bin+1);
}



double KalmanMuonCalibrator::closure(double pt,double eta) {
  return 1.0;
}


double KalmanMuonCalibrator::getCorrectedPt(double pt,double eta1,double phi1,int charge) {
  double curvature = 1.0/getCorrectedPtMag(pt,eta1,phi1);

  double a_1 = getData(A,eta1,phi1);
  double k_1 = getData(K,eta1,phi1);
  double l_1 = getData(L,eta1,phi1);
  double b0_1 = getData(B,eta1,phi1);
  double e_1 = getData(e,eta1,phi1);
  double st1 =sin(2*atan(exp(-eta1))); 
  //  double tt1 =tan(2*atan(exp(-eta1))); 


  double magnetic = a_1+k_1*eta1*eta1;
  double material= -e_1*st1*curvature;

  double alignment=charge*b0_1+l_1;
  curvature = (magnetic+material)*curvature+alignment;
  return (1.0/curvature)*(1.0+varyClosure_*closure(pt,eta1));
}







double KalmanMuonCalibrator::getCorrectedPtMag(double pt,double eta,double phi) {


  double magneticFactor=1.0;
  if (isData_)
    magneticFactor = magneticFactor*magnetic_->GetBinContent(magnetic_->GetBin(
							     magnetic_->GetXaxis()->FindBin(phi),
							     magnetic_->GetYaxis()->FindBin(eta)));
  double curvature = (magneticFactor)/pt;
  return 1.0/curvature;
}


double KalmanMuonCalibrator::getResolution(double pt,double eta) {
  int bin = aRes_->GetXaxis()->FindBin(eta);
  float a  = aRes_->GetBinContent(bin);
  float b  = bRes_->GetBinContent(bin);
  float c  = cRes_->GetBinContent(bin);
  float d  = dRes_->GetBinContent(bin);
  return sqrt(a+b*pt*pt+c/(1+d/(pt*pt)));
}




double KalmanMuonCalibrator::getCorrectedError(double pt,double eta,double error) {
  int bin = aEbE_->GetXaxis()->FindBin(eta);
  float a  = aEbE_->GetBinContent(bin);
  float b  = bEbE_->GetBinContent(bin);
  float c  = cEbE_->GetBinContent(bin);
  float d  = dEbE_->GetBinContent(bin);
  float newError2 =error*error+a+b*pt*pt+c/(1+d/(pt*pt));
  if (newError2>=0.0)
    return sqrt(newError2);
  else {
    printf("Corrected EBE uncertainty negative... not correcting\n"); 
    return error;
  }

}

int KalmanMuonCalibrator::getN() {
  return eigenvalues_->GetNoElements();
}


void KalmanMuonCalibrator::varyClosure(int sigmas) {
    varyClosure_ = sigmas;

}

void KalmanMuonCalibrator::vary(int ii,int sigmas) {
  reset();
  int N = getN();

  TVectorD * v = new TVectorD(N);
  v->Zero();

  if (ii>N) {
    printf("Hey you are trying to vary outside the number of elements,which is pretty stupid \n");
    printf("You ask for element %d out of %d\n",ii,N);   
  }

  (*v)[ii] = sqrt((*eigenvalues_)[ii])*sigmas;


  TVectorD correlated = (*eigenvectors_)*(*v);

  for (int i=0;i<N;++i) {
    shifted_calib_->SetBinContent(i+1,calib_->GetBinContent(i+1)+correlated[i]);
  }

  delete v;

}

double KalmanMuonCalibrator::smear(double pt,double eta) {
  return pt;
}


double KalmanMuonCalibrator::smearForSync(double pt,double eta) {
  return pt;
}

