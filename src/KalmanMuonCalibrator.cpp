#include "interface/KalmanMuonCalibrator.hpp"
#include <math.h>
KalmanMuonCalibrator::KalmanMuonCalibrator() {
}

KalmanMuonCalibrator::KalmanMuonCalibrator(const std::string& filename) {

  random_ = new TRandom3(10101982);


  //edm::FileInPath path("KaMuCa/Calibration/data/"+filename+".root");
  //file_ = new TFile(path.fullPath().c_str());
  std::string path="aux/KaMuCa/"+filename+".root";
  file_ = new TFile(path.c_str());
  isData_ = (filename.find("DATA")!=std::string::npos);


  //inputs


  //magnetic map correction from Solenoid mapping 
  magnetic = (TH2F*)file_->Get("magnetic");


  //magnetic map precorrection from Z to break the correlations
  //  scale_P1 =(TH3F*)file_->Get("P1"); 
  //  scale_P2 =(TH3F*)file_->Get("P2"); 
  

  //Magnetic correction :Up vs downstairs
  scale_A =(TH3F*)file_->Get("A"); 
  scale_K =(TH3F*)file_->Get("K"); 
  scale_L =(TH3F*)file_->Get("L"); 
  scale_A1 =(TH3F*)file_->Get("A1"); 
  scale_A2 =(TH3F*)file_->Get("A2"); 
  scale_A3 =(TH3F*)file_->Get("A3"); 
  scale_A4 =(TH3F*)file_->Get("A4"); 


  //De/Dx
  scale_e = (TH3F*)file_->Get("e") ; 


  //Misalignment fourier terms
  scale_B0 = (TH3F*)file_->Get("B0") ; 
  scale_B1 = (TH3F*)file_->Get("B1") ; 
  scale_B2 = (TH3F*)file_->Get("B2") ; 
  scale_B3 = (TH3F*)file_->Get("B3") ; 
  scale_B4 = (TH3F*)file_->Get("B4") ; 


  //Shifted versions for systematic errors
  shifted_A =(TH3F*)scale_A->Clone();
  shifted_A->SetName("shifted_A");
  shifted_K =(TH3F*)scale_K->Clone();
  shifted_K->SetName("shifted_K");
  shifted_L =(TH3F*)scale_L->Clone();
  shifted_L->SetName("shifted_L");
  shifted_A1 =(TH3F*)scale_A1->Clone();
  shifted_A1->SetName("shifted_A1");
  shifted_A2 =(TH3F*)scale_A2->Clone();
  shifted_A2->SetName("shifted_A2");
  shifted_A3 =(TH3F*)scale_A3->Clone();
  shifted_A3->SetName("shifted_A3");
  shifted_A4 =(TH3F*)scale_A4->Clone();
  shifted_A4->SetName("shifted_A4");

  shifted_e = (TH3F*)scale_e->Clone();
  shifted_e->SetName("shifted_e");

  shifted_B0 = (TH3F*)scale_B0->Clone();
  shifted_B0->SetName("shifted_B0"); 
  shifted_B1 = (TH3F*)scale_B1->Clone();
  shifted_B1->SetName("shifted_B1"); 
  shifted_B2 = (TH3F*)scale_B2->Clone();
  shifted_B2->SetName("shifted_B2"); 
  shifted_B3 = (TH3F*)scale_B3->Clone();
  shifted_B3->SetName("shifted_B3"); 
  shifted_B4 = (TH3F*)scale_B4->Clone();
  shifted_B4->SetName("shifted_B4"); 


  //Resolution Histograms
  aRES_ = (TH1D*)file_->Get("aRES");
  bRES_ = (TH1D*)file_->Get("bRES");
  cRES_ = (TH1D*)file_->Get("cRES");
  dRES_ = (TH1D*)file_->Get("dRES");

  aEBE_ = (TH1D*)file_->Get("aEBE");
  bEBE_ = (TH1D*)file_->Get("bEBE");
  cEBE_ = (TH1D*)file_->Get("cEBE");
  dEBE_ = (TH1D*)file_->Get("dEBE");



  //Closure plot
  closure_ = (TH3F*)file_->Get("closure"); 

  //Mapping between covariance Matrix and correction factors
  covHistoMap_ = (TH1I*)file_->Get("covHistoMap");
  covBinMap_ = (TH1I*)file_->Get("covBinMap");

  //Eigen vectors and eigen values
  eigenvalues_ = (TVectorD*)file_->Get("eigenvalues");
  eigenvectors_ = (TMatrixD*)file_->Get("eigenvectors");

  varyClosure_=0;
 }


void KalmanMuonCalibrator::reset() {
  varyClosure_=0;
  resetHisto(shifted_A,scale_A);
  resetHisto(shifted_K,scale_K);
  resetHisto(shifted_L,scale_L);
  resetHisto(shifted_A1,scale_A1);
  resetHisto(shifted_A2,scale_A2);
  resetHisto(shifted_A3,scale_A3);
  resetHisto(shifted_A4,scale_A4);
  resetHisto(shifted_e,scale_e);
  resetHisto(shifted_B0,scale_B0);
  resetHisto(shifted_B1,scale_B1);
  resetHisto(shifted_B2,scale_B2);
  resetHisto(shifted_B3,scale_B3);
  resetHisto(shifted_B4,scale_B4);
}


void KalmanMuonCalibrator::resetHisto(TH1* histo,const TH1* ref) {
  for (int i=0;i<histo->GetNbinsX()+1;++i)
    for (int j=0;j<histo->GetNbinsY()+1;++j)
      for (int k=0;k<histo->GetNbinsZ()+1;++k) {
	int bin = histo->GetBin(i,j,k);
	histo->SetBinContent(bin,ref->GetBinContent(bin));
      }
}


KalmanMuonCalibrator::~KalmanMuonCalibrator() {
  file_->Close();
}


double KalmanMuonCalibrator::closure(double pt,double eta) {
  Int_t binx = closure_->GetXaxis()->FindBin(pt);
  Int_t biny = closure_->GetYaxis()->FindBin(fabs(eta));
  Int_t bin = closure_->GetBin(binx, biny, 1);

  // If we're outside the valid pt range, use the first/last bin
  if(binx > closure_->GetNbinsX()) // overflow
    {
      binx -= 1;
      bin = closure_->GetBin(binx, biny, 1);
    }
  if(!binx) // underflow
    {
      binx = 1;
      bin = closure_->GetBin(binx, biny, 1);
    }

  return closure_->GetBinContent(bin)-1.0;
}


double KalmanMuonCalibrator::getCorrectedPt(double pt,double eta,double phi,int charge) {
    double curvature = 1.0/getCorrectedPtMag(pt,eta,phi);
    double e = shifted_e->GetBinContent(scale_e->GetBin(1,scale_e->GetYaxis()->FindBin(eta),1));
    double sinTheta  = sin(2*atan(exp(-eta))); 


    double A1 = shifted_A1->GetBinContent(scale_A1->GetBin(1,scale_A1->GetYaxis()->FindBin(eta),scale_A1->GetZaxis()->FindBin(phi)));
    double A2 = shifted_A2->GetBinContent(scale_A2->GetBin(1,scale_A2->GetYaxis()->FindBin(eta),scale_A2->GetZaxis()->FindBin(phi)));
    double A3 = shifted_A3->GetBinContent(scale_A3->GetBin(1,scale_A3->GetYaxis()->FindBin(eta),scale_A3->GetZaxis()->FindBin(phi)));
    double A4 = shifted_A4->GetBinContent(scale_A4->GetBin(1,scale_A4->GetYaxis()->FindBin(eta),scale_A4->GetZaxis()->FindBin(phi)));

    double B0 = shifted_B0->GetBinContent(scale_B0->GetBin(1,scale_B0->GetYaxis()->FindBin(eta),scale_B0->GetZaxis()->FindBin(phi)));
    double B1 = shifted_B1->GetBinContent(scale_B1->GetBin(1,scale_B1->GetYaxis()->FindBin(eta),scale_B1->GetZaxis()->FindBin(phi)));
    double B2 = shifted_B2->GetBinContent(scale_B2->GetBin(1,scale_B2->GetYaxis()->FindBin(eta),scale_B2->GetZaxis()->FindBin(phi)));
    double B3 = shifted_B3->GetBinContent(scale_B3->GetBin(1,scale_B3->GetYaxis()->FindBin(eta),scale_B3->GetZaxis()->FindBin(phi)));
    double B4 = shifted_B4->GetBinContent(scale_B4->GetBin(1,scale_B4->GetYaxis()->FindBin(eta),scale_B4->GetZaxis()->FindBin(phi)));


    double A = shifted_A->GetBinContent(13);
    double K = shifted_K->GetBinContent(13);
    double L = shifted_L->GetBinContent(13);

    //    double tanTheta = 2.0/(exp(eta)-exp(-eta)); 
    //    double mag = A+K/(tanTheta*tanTheta)+A1*sin(phi)+A2*sin(2*phi)+A3*cos(phi)+A4*cos(2*phi);
    double mag = A+K*eta*eta+L*eta*eta*eta+A1*sin(phi)+A2*sin(2*phi)+A3*cos(phi)+A4*cos(2*phi);
    double B = B0+B1*sin(phi)+B2*sin(2*phi)+B3*cos(phi)+B4*cos(2*phi);
    curvature = mag*curvature -e*sinTheta*curvature*curvature+charge*B;
    return (1.0/curvature)*(1.0+varyClosure_*closure(pt,eta));
}
double KalmanMuonCalibrator::getCorrectedPtMag(double pt,double eta,double phi) {


  double magneticFactor=1.0;
  if (isData_)
    magneticFactor = magneticFactor*magnetic->GetBinContent(magnetic->GetBin(
									     magnetic->GetXaxis()->FindBin(phi),
									     magnetic->GetYaxis()->FindBin(eta)));
  
  double curvature = (magneticFactor)/pt;

  //Now precorrection from Z
  //  double P1 = scale_P1->GetBinContent(13);
  //  double P2 = scale_P2->GetBinContent(13);

  //  curvature = curvature*(P1+P2*eta*eta);
  return 1.0/curvature;
}


double KalmanMuonCalibrator::getCorrectedError(double pt,double eta,double error) {
  Int_t bin = aEBE_->GetXaxis()->FindBin(eta); 
  double a2 = aEBE_->GetBinContent(bin);

  bin = bEBE_->GetXaxis()->FindBin(eta); 
  double f = bEBE_->GetBinContent(bin);
  bin = cEBE_->GetXaxis()->FindBin(eta); 
  double c2 = cEBE_->GetBinContent(bin);

  bin = dEBE_->GetXaxis()->FindBin(eta); 
  double d2 = dEBE_->GetBinContent(bin);

  double pt2=pt*pt;


  //  printf("%f %f %f %f %f\n",a2,b2,c2,d2,pt2); 
  
  //new ebe^2 = ebe^2 + sigma^2-ebe_avg^2

  double error2=error*error + a2+a2*f/(1+d2/pt2)+c2*pt2;

    if (error2<0) {
    //    printf("Got Negative EbE !! Will ignore and not correct the ebe of this muon pt=%f , eta=%f ,error=%f ,residual2=%f\n",pt,eta,error,error2-error*error); 
    return error;
  }

  return sqrt(error2);
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
    int histo = covHistoMap_->GetBinContent(i+1);
    int bin = covBinMap_->GetBinContent(i+1);
    float value = correlated[i];
    switch (histo) {
    case 0:
      shifted_A->SetBinContent(bin,scale_A->GetBinContent(bin)+value);
      break;
    case 1:
      shifted_A1->SetBinContent(bin,scale_A1->GetBinContent(bin)+value);
      break;
    case 2:
      shifted_A2->SetBinContent(bin,scale_A2->GetBinContent(bin)+value);
      break;
    case 3:
      shifted_A3->SetBinContent(bin,scale_A3->GetBinContent(bin)+value);
      break;
    case 4:
      shifted_A4->SetBinContent(bin,scale_A4->GetBinContent(bin)+value);
      break;
    case 5:
      shifted_e->SetBinContent(bin,scale_e->GetBinContent(bin)+value);
      break;

    case 6:
      shifted_B0->SetBinContent(bin,scale_B0->GetBinContent(bin)+value);
      break;
    case 7:
      shifted_B1->SetBinContent(bin,scale_B1->GetBinContent(bin)+value);
      break;
    case 8:
      shifted_B2->SetBinContent(bin,scale_B2->GetBinContent(bin)+value);
      break;
    case 9:
      shifted_B3->SetBinContent(bin,scale_B3->GetBinContent(bin)+value);
      break;
    case 10:
      shifted_B4->SetBinContent(bin,scale_B4->GetBinContent(bin)+value);
      break;
    case 11:
      shifted_K->SetBinContent(bin,scale_K->GetBinContent(bin)+value);
      break;
    case 12:
      shifted_L->SetBinContent(bin,scale_L->GetBinContent(bin)+value);
      break;

    default:
      printf("UNKNOWN HISTO-> problem while varying (That is important\n");
    }

  }

  delete v;

}

double KalmanMuonCalibrator::smear(double pt,double eta) {
  Int_t bin = aRES_->GetXaxis()->FindBin(eta);
  double aRES = aRES_->GetBinContent(bin);

  bin = bRES_->GetXaxis()->FindBin(eta);
  double bRES = bRES_->GetBinContent(bin);
  bin = cRES_->GetXaxis()->FindBin(eta);
  double cRES = cRES_->GetBinContent(bin);
  bin = dRES_->GetXaxis()->FindBin(eta);
  double dRES = dRES_->GetBinContent(bin);

  double factor = aRES+bRES/(1+dRES/(pt*pt))+cRES*pt*pt;

  //If it is positive and data smear.
  //If it is negative and MC smear
  //or lese do not smear

  if ((isData_ && factor<0) ||((!isData_)&&factor>0) ) {
    factor = sqrt(fabs(factor));
    return random_->Gaus(1.0,factor)*pt;
  }
  else {
    return pt;
  }
}


double KalmanMuonCalibrator::smearForSync(double pt,double eta) {
  Int_t bin = aRES_->GetXaxis()->FindBin(eta);

  double aRES = aRES_->GetBinContent(bin);
  double bRES = bRES_->GetBinContent(bin);
  double cRES = cRES_->GetBinContent(bin);
  double dRES = dRES_->GetBinContent(bin);

  double factor = aRES+bRES/(1+dRES/(pt*pt))+cRES*pt*pt;

  //If it is positive and data smear.
  //If it is negative and MC smear
  //or lese do not smear

  if ((isData_ && factor<0) ||((!isData_)&&factor>0) ) {
    factor = sqrt(fabs(factor));
    return pt*(1.0+factor);
  }
  else {
    return pt;
  }

}

