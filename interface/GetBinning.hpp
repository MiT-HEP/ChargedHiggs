#include "TH2F.h"
#include "TFile.h"

class GetBinning
{
public:
  GetBinning();
  Int_t GetCluster(Double_t, Double_t);
protected:
  TFile* file;
  TH2F* hBinning;
};
