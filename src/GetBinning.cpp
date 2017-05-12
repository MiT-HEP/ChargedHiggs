#include <iostream>
//#include "TH2.h"
#include "TROOT.h"
//#include "TFile.h"
#include "interface/GetBinning.hpp"
using namespace std;

// Expected input
// vsHF-->X
// vsLF-->Y

GetBinning::GetBinning()
{
}

void GetBinning::SetBinning(TString fileName)
{
  //  TString fileName = "aux/binning_1l_highmass.root";
  file = (TFile*) gROOT->GetListOfFiles()->FindObject(fileName);
  if (!file || !file->IsOpen()) file = TFile::Open(fileName);
  hBinning = (TH2F*) file->Get("hTargetBinning");
  return;
}


Int_t GetBinning::GetCluster( Double_t x, Double_t y)
{
  return hBinning->GetBinContent( hBinning->FindBin(x,y) );
}

GetBinning ClusteringSuite;

Int_t FinalBinning(Double_t x, Double_t y)
{
  return ClusteringSuite.GetCluster(x,y)+1;

}


