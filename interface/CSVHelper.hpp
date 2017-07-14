#ifndef CSVHelper_h__
#define CSVHelper_h__

#include <string>
#include <vector>

#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "interface/Systematics.hpp"

class CSVHelper
{
public:
  // nHFptBins specifies how many of these pt bins are used:
  // (jetPt >= 19.99 && jetPt < 30), (jetPt >= 30 && jetPt < 40), (jetPt >= 40 && jetPt < 60), 
  // (jetPt >= 60 && jetPt < 100), (jetPt >= 100 && jetPt < 160), (jetPt >= 160 && jetPt < 10000).
  // If nHFptBins < 6, the last on is inclusive (eg jetPt >=100 && jetPt < 10000 for nHFptBins=5).
  // The SFs from data have 5 bins, the pseudo data scale factors 6 bins.

  // standard constructor
  CSVHelper();
  // another constructor
  CSVHelper(const std::string& hf, const std::string& lf, const int& nHFptBins=5,const int& nLFptBins=4,const int& nLFetaBins=3,const std::vector<Systematics::Type>& jecsysts = std::vector<Systematics::Type>(1,Systematics::NA));
  // destructor
  ~CSVHelper();
  // function to set up the needed stuff
  void init(const std::string& hf, const std::string& lf, const int& nHFptBins,const int& nLFptBins,const int& nLFetaBins,const std::vector<Systematics::Type>& jecsysts);
  // function to get the csv weight
  double getCSVWeight(const std::vector<double>& jetPts,
		      const std::vector<double>& jetEtas,
		      const std::vector<double>& jetCSVs,
		      const std::vector<int>& jetFlavors,
		      const Systematics::Type syst,
		      double &csvWgtHF,
		      double &csvWgtLF,
		      double &csvWgtCF) const;

  // If there is no SF for a jet because it is out of acceptance
  // of SF, an SF of 1 is used for this jet. Intended when running
  // on MC with a more inclusive selection.
  // USE WITH CARE!
  void allowJetsOutOfBinning(const bool allow) { allowJetsOutOfBinning_ = allow; }


private:
  bool isInit_;
  int nHFptBins_;//number of pt bins in hf(including c flavour) histograms
  int nLFptBins_;//number of pt bins in lf histograms
  int nLFetaBins_;//number of eta bins in lf histograms
  bool allowJetsOutOfBinning_;

  std::vector< std::vector<TH1*> > h_csv_wgt_hf;//vector to store pointers to the needed hf histograms
  std::vector< std::vector<TH1*> > c_csv_wgt_hf;//vector to store pointers to the needed c flavour histograms
  std::vector< std::vector< std::vector<TH1*> > > h_csv_wgt_lf;//vector to store pointers to the needed lf histograms
  // vector for the csv systematics
  std::vector<Systematics::Type> csvsysts = {
                                                Systematics::CSVLFup,
                                                Systematics::CSVLFdown,
                                                Systematics::CSVHFup,
                                                Systematics::CSVHFdown,
                                                Systematics::CSVHFStats1up,
                                                Systematics::CSVHFStats1down,
                                                Systematics::CSVLFStats1up,
                                                Systematics::CSVLFStats1down,
                                                Systematics::CSVHFStats2up,
                                                Systematics::CSVHFStats2down,
                                                Systematics::CSVLFStats2up,
                                                Systematics::CSVLFStats2down,
                                                Systematics::CSVCErr1up,
                                                Systematics::CSVCErr1down,
                                                Systematics::CSVCErr2up,
                                                Systematics::CSVCErr2down
                                            };
  // vector for all desired systematics -> csv+jec systematics
  std::vector<Systematics::Type> systs;
  // function to get the histograms from the provided root files
  void fillCSVHistos(TFile *fileHF, TFile *fileLF, const std::vector<Systematics::Type>& systs);
  // function which reads the desired histogram from the provided root file
  TH1* readHistogram(TFile* file, const TString& name) const;
};

#endif
