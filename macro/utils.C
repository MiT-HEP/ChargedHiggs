#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraphAsymmErrors.h"
#include "TString.h"
#include "TStyle.h"

//using namespace std;

TH1 * getHisto (TString fileName, TString fileName2, TString dirName, TString histoName, int color, int style, int rebin, double is2d=false) {

  //   cout << "fileName=" << fileName.Data() << " histoName=" << histoName.Data() << " dirName=" << dirName.Data() << endl;
  //  std::cout << "fileName=" << fileName.Data() << " histoName=" << histoName.Data() << std::endl;

  TH1 * h_=0;
  TH1 * h2_=0;

  TFile *file_ = TFile::Open(fileName);
  TFile *file2_ = TFile::Open(fileName2);

  if(!file_) return h_;
  if(!file2_) return h_;

  TDirectory * dir;
  TDirectory * dir2;

  if(dirName.Contains("0")) {
    //    file_->ls();

    if(!is2d) h_ = (TH1F*) file_->Get(histoName);
    if(is2d) h_ = (TH2F*) file_->Get(histoName);

  } else {
    file_->ls();

    dir = (TDirectory*) file_->Get(dirName);
    dir2 = (TDirectory*) file2_->Get(dirName);
    //    dir->ls();

    if(!is2d) h_ = (TH1F*) dir->Get(histoName);
    if(is2d) h_ = (TH2F*) dir->Get(histoName);

    if(!is2d) h2_ = (TH1F*) dir2->Get(histoName);
    if(is2d) h2_ = (TH2F*) dir2->Get(histoName);

    if(h_ && h2_) h_->Add(h2_);

  }

  if(is2d && h_) return h_;

  if(h_) h_->SetMarkerColor(color);
  if(h_) h_->SetLineColor(color);
  if(h_) h_->SetLineWidth(3);
  //  if(h_) h_->SetFillColor(color);
  //  if(h_) h_->SetFillStyle(style);

  if(fileName.Contains("data")) {

    if(h_) h_->SetMarkerColor(color);
    if(h_) h_->SetLineColor(color);
    if(h_) h_->SetLineWidth(3);

  } else {

    if(h_) h_->SetLineColor(color);
    if(h_) h_->SetLineStyle(style);
    if(h_) h_->SetLineWidth(3);

    if(h_) h_->SetMarkerColor(color);
    if(h_) h_->SetMarkerStyle(1);

    //    if(h_) h_->SetFillStyle(style);
    //    if(color==2)h_->SetLineWidth(3);

  }

  if(h_) {

    Int_t nBinX= h_->GetXaxis()->GetNbins();

    // overFlow
    h_->SetBinContent(nBinX,h_->GetBinContent(nBinX)+h_->GetBinContent(nBinX+1));
    h_->SetBinContent(nBinX+1,0);
    // underFlow
    h_->SetBinContent(1,h_->GetBinContent(0)+h_->GetBinContent(1));
    h_->SetBinContent(0,0);

  }

  //  if(h_) h_->Scale(1./h_->Integral());                                                                                                                                                                         

  if(h_) h_->Rebin(rebin);

  if(h_) h_->SetDirectory(0);
  file_->Close();
  file2_->Close();

  //  if(h_) cout << "found " << h_ << endl;                                                                                                                                                                       

  if(h_) return h_;

  return 0;

}

TH1 * getHisto (TString fileName, TString dirName, TString histoName, int color, int style, int rebin, double is2d=false) {

  //  cout << "fileName=" << fileName.Data() << " histoName=" << histoName.Data() << " dirName=" << dirName.Data() << endl;
  //  std::cout << "fileName=" << fileName.Data() << " histoName=" << histoName.Data() << std::endl;

  TH1 * h_=0;

  TFile *file_ = TFile::Open(fileName);

  if(!file_) return h_;

  TDirectory * dir;

  if(dirName.Contains("0")) {
    //    file_->ls();

    if(!is2d) h_ = (TH1F*) file_->Get(histoName);
    if(is2d) h_ = (TH2F*) file_->Get(histoName);
  } else {
    //    file_->ls();

    dir = (TDirectory*) file_->Get(dirName);
    //    dir->ls();

    if(!is2d) h_ = (TH1F*) dir->Get(histoName);
    if(is2d) h_ = (TH2F*) dir->Get(histoName);
  }

  if(is2d && h_) return h_;

  if(h_) h_->SetMarkerColor(color);
  if(h_) h_->SetLineColor(color);
  if(h_) h_->SetLineWidth(3);
  //  if(h_) h_->SetFillColor(color);
  //  if(h_) h_->SetFillStyle(style);

  if(fileName.Contains("data")) {

    if(h_) h_->SetMarkerColor(color);
    if(h_) h_->SetLineColor(color);
    if(h_) h_->SetLineWidth(3);

  } else {

    if(h_) h_->SetLineColor(color);
    if(h_) h_->SetLineStyle(style);
    if(h_) h_->SetLineWidth(3);

    if(h_) h_->SetMarkerColor(color);
    if(h_) h_->SetMarkerStyle(1);

    //    if(h_) h_->SetFillStyle(style);
    //    if(color==2)h_->SetLineWidth(3);

  }

  if(h_) {

    Int_t nBinX= h_->GetXaxis()->GetNbins();

    // overFlow
    h_->SetBinContent(nBinX,h_->GetBinContent(nBinX)+h_->GetBinContent(nBinX+1));
    h_->SetBinContent(nBinX+1,0);
    // underFlow
    h_->SetBinContent(1,h_->GetBinContent(0)+h_->GetBinContent(1));
    h_->SetBinContent(0,0);

  }

  //  if(h_) h_->Scale(1./h_->Integral());                                                                                                                                                                         

  if(h_) h_->Rebin(rebin);

  if(h_) h_->SetDirectory(0);
  file_->Close();

  //  if(h_) cout << "found " << h_ << endl;                                                                                                                                                                       

  if(h_) return h_;

  return 0;

}

void plotLeg(TString tagger, TH1* h1,float posX, float posY, TString drawOption){

  TString leg = "";
  leg += tagger.Data();

  TLegend* this_leg = new TLegend(posX,posY,posX+0.1,posY+0.05);
  this_leg->SetFillColor(0);
  this_leg->SetBorderSize(0);
  this_leg->SetTextSize(0.04);
  this_leg->AddEntry(h1, leg, drawOption);
  this_leg->Draw();

}

void plotPreliminary(TString text, float posX ,float posY){

  //  TString leg = "L = ";
  //  leg += text.data();

  TLatex latexLabel;

  latexLabel.SetTextSize(0.04);
  latexLabel.SetNDC();
  latexLabel.DrawLatex(posX, posY+0.04, "CMS Preliminary");
  latexLabel.DrawLatex(posX, posY, "#sqrt{s} = 8 TeV");
  latexLabel.DrawLatex(posX, posY-0.04, text.Data());

}
