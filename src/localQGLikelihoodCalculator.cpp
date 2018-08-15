#include <vector>
#include <map>
#include <stdexcept>
#include <cmath>

#include <TFile.h>
#include <TKey.h>
#include <TList.h>
#include <TIterator.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <TVector.h>
#include <TString.h>

#include "interface/localQGLikelihoodCalculator.hpp"


/// Constructor
QGLikelihoodCalculator::QGLikelihoodCalculator(const TString filename){
  if(filename == "" || !init(filename)) throw std::runtime_error(("Initialization failed: " + filename + " not found or corrupt!").Data());
}


/// Initialisation of the QGLikelihoodCalculator
bool QGLikelihoodCalculator::init(const TString& fileName){
  f = new TFile(fileName);
  if(f->IsZombie()) 				return false;
  if(!getBinsFromFile(etaBins, "etaBins"))	return false;
  if(!getBinsFromFile(ptBins,  "ptBins"))	return false;
  if(!getBinsFromFile(rhoBins, "rhoBins"))	return false;

  TList *keys = f->GetListOfKeys();
  if(!keys) return false;

  TIter nextdir(keys);
  TKey *keydir;
  while((keydir = (TKey*) nextdir())){
    if(!keydir->IsFolder()) continue;
    TDirectory *dir = (TDirectory*) keydir->ReadObj();
    TIter nexthist(dir->GetListOfKeys());
    TKey *keyhist;
    while((keyhist = (TKey*) nexthist())) pdfs[keyhist->GetName()] = (TH1F*) keyhist->ReadObj();
  }
  return true;
}


/// Compute the QGLikelihood, given the pT, eta, rho and likelihood variables vector
float QGLikelihoodCalculator::computeQGLikelihood(float pt, float eta, float rho, std::vector<float> vars){
  if(!isValidRange(pt, rho, eta)) return -1;

  TString binName;
  if(!getBinName(binName, eta, pt, rho)) return -1;
  return computeQGLikelihood(binName, vars);
}

float QGLikelihoodCalculator::computeQGLikelihood(TString binName, std::vector<float> vars){
  float Q=1., G=1.;
  for(unsigned int varIndex = 0; varIndex < vars.size(); ++varIndex){
    if(vars[varIndex] < -0.5) continue; //use to inspect variables separately (i.e. skip if feeding -1)

    auto quarkEntry = findEntry(binName, 0, varIndex);
    auto gluonEntry = findEntry(binName, 1, varIndex);
    if(!quarkEntry || !gluonEntry) return -2;

    int binQ = quarkEntry->FindBin(vars[varIndex]);
    int binG = gluonEntry->FindBin(vars[varIndex]);

    Q *= quarkEntry->GetBinContent(binQ);
    G *= gluonEntry->GetBinContent(binG);
  }

  if(Q==0) return 0;
  return Q/(Q+G);
}



/// Find matching entry for a given binName, qgIndex and varIndex
TH1F* QGLikelihoodCalculator::findEntry(TString& binName, int qgIndex, int varIndex){
  TString histName = (varIndex == 2 ? "axis2" : (varIndex? "ptD" : "mult")) + TString("_") + (qgIndex ? "gluon":"quark")  + TString("_") +  binName;
  return pdfs[histName];
}


// Construct the binName using eta, pt, rho
bool QGLikelihoodCalculator::getBinName(TString& binName, float eta, float pt, float rho){
  int etaBin, ptBin, rhoBin;
  if(!getBinNumber(etaBins, fabs(eta), etaBin)) return false;
  if(!getBinNumber(ptBins, pt, ptBin)) 		return false;
  if(!getBinNumber(rhoBins, rho, rhoBin)) 	return false;
  binName = TString::Format("eta%d_pt%d_rho%d", etaBin, ptBin, rhoBin);
  return true;
}


/// Check the valid range of this qg tagger, using the bin vectors
bool QGLikelihoodCalculator::isValidRange(float pt, float rho, float eta){
  if(pt < ptBins.front()) 		return false;
  if(pt > ptBins.back()) 		return false;
  if(rho < rhoBins.front()) 		return false;
  if(rho > rhoBins.back()) 		return false;
  if(fabs(eta) < etaBins.front()) 	return false;
  if(fabs(eta) > etaBins.back()) 	return false;
  return true;
}


/// Translates the TVector with the bins to std::vector
bool QGLikelihoodCalculator::getBinsFromFile(std::vector<float>& bins, const TString& name){
  TVectorT<float> *tbins = nullptr;
  f->GetObject(name, tbins);
  if(!tbins) return false;
  for(int i = 0; i < tbins->GetNoElements(); ++i) bins.push_back((*tbins)[i]);
  return true;
}


/// Find the bin number for a value, given the bin vector
bool QGLikelihoodCalculator::getBinNumber(std::vector<float>& bins, float value, int& bin){
  if(value < bins.front() || value > bins.back()) return false;
  auto binUp = bins.begin() + 1;
  while(value > *binUp) ++binUp;
  bin = binUp - bins.begin() - 1;
  return true;
}


/// Destroy the QGLikelihoodCalculator
QGLikelihoodCalculator::~QGLikelihoodCalculator(){
  for(auto& pdf : pdfs) delete pdf.second;
  delete f;
}
