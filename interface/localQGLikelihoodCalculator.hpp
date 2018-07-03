#ifndef Local_QGLikelihoodCalculator_h
#define Local_QGLikelihoodCalculator_h
#include <vector>
#include <map>
#include <TFile.h>
#include <TH1F.h>
#include <TString.h>

/*
 * A modified version of /RecoJets/JetAlgorithms/interface/QGLikelihoodCalculator.h, working with ROOT files instead of database obect
 */

class QGLikelihoodCalculator{
  public:
    QGLikelihoodCalculator(const TString fileName);
    ~QGLikelihoodCalculator();
    float computeQGLikelihood(float pt, float eta, float rho, std::vector<float> vars_);
    float computeQGLikelihood(TString binName, std::vector<float> vars_);

  private:
    bool init(const TString& fileName);
    bool getBinsFromFile(std::vector<float>& bins, const TString& name);
    TH1F* findEntry(TString& binName, int qgIndex, int varIndex);
    bool isValidRange(float pt, float rho, float eta);
    bool getBinNumber(std::vector<float>& bins, float value, int& bin);
    bool getBinName(TString& binName, float eta, float pt, float rho);

    std::vector<float> etaBins, ptBins, rhoBins;
    std::map<TString, TH1F*> pdfs; 
    std::map<TString, std::vector<float>> weights;
    TFile* f;
};
#endif
