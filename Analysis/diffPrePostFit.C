#include "THStack.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TFile.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "RooGlobalFunc.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TRandom.h"
#include "TLatex.h"
#include <string>
#include <vector>
#include <math.h>
#include "tdrstyle.C"

const int bkgtype = 9;
TString bkgname[bkgtype] = {"VVOther","triBoson","EWKV","Zinv","Winv","ttbar","VQQ","QCD","VVEWK"};
const int colorbkg[bkgtype] = {kGreen+2,kGreen-9, kYellow-9, kOrange+1, kOrange-9,kAzure-4,kViolet-9,kOrange-2,kOrange+9};

TString fileName1= "$CMSSW_BASE/src/ChargedHiggs/Analysis/DATACARDMVA_bin0p05_stat0p5/AUG4/fitDiagnosticsfinal_2021_DNN_comb_all_DNN_comb_all_VVEWKunb_VVEWK.root";

TString stringToPlot;
TString regionToPlot;
//TString cat="BBtag";
TString ana;


// to run 
// root -l -q -b diffPrePostFit.C+'("BMET",2)'

void diffPrePostFit(TString cat,int cy) {

  gStyle->SetOptStat(0);

  //  int cy = 2;
  if(cy==1) { stringToPlot = "_sr"; regionToPlot = "SR"; }
  if(cy==2) { stringToPlot = "_side"; regionToPlot = "side"; }
  if(cy==3) { stringToPlot = "_anti"; regionToPlot = "anti"; }

  stringToPlot = regionToPlot;

  int cx = 1;
  if (cat == "BB") { cx = 1; ana = "BDTnoBnoMET";}
  else if (cat == "BMET") { cx = 2;  ana = "BDTwithMET";}
  else if (cat == "RMET") { cx = 3; ana = "BDTwithMET";}
  else if (cat == "BBtag") { cx = 4; ana = "BDTbtag";}

  TFile* f1 = TFile::Open(Form(fileName1.Data(),cat.Data()));
  //  TFile* f1 = TFile::Open(Form(fileName2.Data(), ana.Data(), cat.Data(), regionToPlot.Data()));

  vector<TDirectoryFile*> catePre;
  vector<TDirectoryFile*> catePost;
  vector<TH1D*> cateorg;

  catePre.push_back( (TDirectoryFile*)f1->Get(Form("shapes_prefit/ch%i_ch%i",cx,cy)) );
  catePost.push_back( (TDirectoryFile*)f1->Get(Form("shapes_fit_s/ch%i_ch%i",cx,cy)) );

  TH1D* h;
  TLegend *legend0 = new TLegend(0.8,0.5,0.95,0.9);
  legend0->SetFillStyle(0);
  legend0->SetBorderSize(0);
  legend0->SetTextSize(0.05);

  TCanvas *c1a = new TCanvas("c1a","c1a",900,1200);

  for(Int_t i=0; i<1; i++) {

    if(catePre[i]==NULL) continue;
    if(catePost[i]==NULL) continue;

    for(int j=0; j<bkgtype; j++){

      TH1D* pre = (TH1D*) catePre[i]->Get(bkgname[j]);
      TH1D* post = (TH1D*) catePost[i]->Get(bkgname[j]);
      
      if(pre==NULL) continue;

      h = (TH1D*) pre->Clone(); 
      h->Divide(post); h->SetLineColor(colorbkg[j]); h->SetName(bkgname[j]); h->SetTitle(bkgname[j]); h->SetLineWidth(3);
      if(j==0) h->SetMaximum(2*h->GetMaximum());
      if(j==0) h->Draw("hist");
      else h->Draw("hist sames");
      legend0->AddEntry(h,bkgname[j],"f");

    }
  }

  legend0->Draw("same");

  char savepath2[150];

  //  sprintf(savepath,"~/www/VBS/JAN20fits/Aug4unb_%s_%s_%s_%s.pdf",ana.Data(),cat.Data(),pro.Data(),stringToPlot.Data());
  sprintf(savepath2,"~/www/VBS/JAN20fits/Shapes/Aug4unb_%s_%s_%s.png",ana.Data(),cat.Data(),stringToPlot.Data());
  //  sprintf(Ssavepath3,"~/www/VBS/JAN20fits/Aug4unb_%s_%s_%s_%s.C",ana.Data(),cat.Data(),pro.Data(),stringToPlot.Data());

  //  c1a->SaveAs(savepath);
  c1a->SaveAs(savepath2);
  //  c1a->SaveAs(Ssavepath3);

}
