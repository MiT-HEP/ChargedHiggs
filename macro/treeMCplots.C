#include "TROOT.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"

TString fileHAD = "HAD.root"; 
//TString fileHAD = "HADanti.root";
TString fileMET = "MET.root"; 
//TString fileMET = "MET_norm0resolved.root";
TString fileBHAD = "BHAD.root"; 

bool doOnlySignal=false;

void draw2D(int myCategory, int nbin, float min, float max, float minY, float maxY) {

  gStyle->SetCanvasPreferGL(kTRUE);
  gStyle->SetOptStat(0);
  //  gROOT->LoadMacro("utils.C++");

  TString filename=fileMET;
  if(myCategory==1 || myCategory==2) filename=fileHAD;
  if(myCategory==5 || myCategory==6) filename=fileBHAD;
  //  filename="HAD_signal.root";
  filename="HAD.root";
  cout << "reading " << filename.Data() << endl;
  TFile *f = new TFile(filename.Data());
  TTree *t1 = (TTree*)f->Get("tree_vbs");
  Double_t weight;
  Int_t mc,ana_category;

  Float_t varMjj,varDetajj,varDphijj,varJet2Eta;
  Float_t varMVV,varPTV1,varPTV2,varDetaVV,varzepVB;
  Float_t variable;
  Float_t dauRatioV1,dauRatioV2;
  Float_t BDTnoBnoMET;

  t1->SetBranchAddress("ana_category",&ana_category);
  t1->SetBranchAddress("mc",&mc);
  t1->SetBranchAddress("weight",&weight);
  t1->SetBranchAddress("varMVV",&varMVV);
  t1->SetBranchAddress("varMjj",&varMjj);
  t1->SetBranchAddress("BDTnoBnoMET",&BDTnoBnoMET);

  t1->SetBranchAddress("dauRatioV1",&dauRatioV1);
  t1->SetBranchAddress("dauRatioV2",&dauRatioV2);

  TH2F *h_qcd   = new TH2F("h_qcd","hqcd",nbin, min, max, nbin, minY, maxY);
  TH2F *h_ewk   = new TH2F("h_ewk","hewk",nbin, min, max, nbin, minY, maxY);
  TH2F *h_Zinv   = new TH2F("h_zinv","hzinv",nbin, min, max, nbin, minY, maxY);
  TH2F *h_TT   = new TH2F("h_TT","hTT",nbin, min, max, nbin, minY, maxY);
  TH2F *h_sig  = new TH2F("h_sig","h_sig",nbin, min, max, nbin, minY, maxY);

  TH2F *h_sig8  = new TH2F("h_sig8","h_sig8",nbin, min, max, nbin, minY, maxY);
  TH2F *h_sig9  = new TH2F("h_sig9","h_sig9",nbin, min, max, nbin, minY, maxY);
  TH2F *h_sig10  = new TH2F("h_sig10","h_sig10",nbin, min, max, nbin, minY, maxY);

  TH1F *h_sig8_1d  = new TH1F("h_sig8_1d","h_sig8_1d",nbin, min, max);
  TH1F *h_sig9_1d  = new TH1F("h_sig9_1d","h_sig9_1d",nbin, min, max);
  TH1F *h_sig10_1d  = new TH1F("h_sig10_1d","h_sig10_1d",nbin, min, max);

  // all entries and fill the histograms
  Int_t nentries = (Int_t) t1->GetEntries();
  for (Int_t i=0;i<nentries;i++) {
 
    t1->GetEntry(i);

    //    cout << "HELLO " << "myCategory" << myCategory << " ana_category" << ana_category  << endl;
    if(myCategory!=ana_category) continue;

    /*
    if(mc==1) h_sig->Fill(varMVV,varMjj,weight);
    if(mc==500) h_qcd->Fill(varMVV,varMjj,weight);
    if(mc==200 || mc==210) h_TT->Fill(varMVV,varMjj,weight);
    if(mc==110 || mc==120) h_ewk->Fill(varMVV,varMjj,weight); 
    */

    if(mc==1) h_sig->Fill(BDTnoBnoMET,varMVV,weight);
    if(mc==500) h_qcd->Fill(BDTnoBnoMET,varMVV,weight);
    if(mc==200 || mc==210) h_TT->Fill(BDTnoBnoMET,varMVV,weight);
    if(mc==110 || mc==120) h_ewk->Fill(BDTnoBnoMET,varMVV,weight);

    if(mc==8) h_sig8->Fill(dauRatioV1,dauRatioV2,weight); h_sig8_1d->Fill(dauRatioV1,weight); h_sig8_1d->Fill(dauRatioV2,weight);
    if(mc==9) h_sig9->Fill(dauRatioV1,dauRatioV2,weight); h_sig9_1d->Fill(dauRatioV1,weight); h_sig9_1d->Fill(dauRatioV2,weight);
    if(mc==10) h_sig10->Fill(dauRatioV1,dauRatioV2,weight); h_sig10_1d->Fill(dauRatioV1,weight); h_sig10_1d->Fill(dauRatioV2,weight);

  }

  if(h_sig) h_sig->GetXaxis()->SetTitle("BDTnoBnoMET");
  if(h_sig) h_sig->GetYaxis()->SetTitle("MVV");
  //  if(h_sig) h_sig->GetYaxis()->SetTitle("Mjj");
  if(h_sig) h_sig->GetYaxis()->SetTitleOffset(1.);
  if(h_sig) h_sig->SetFillColor(kBlack);

  if(h_sig) h_sig->DrawNormalized("box");
  h_qcd->SetFillColor(kRed);
  h_ewk->SetFillColor(kGreen+1);
  h_TT->SetFillColor(kBlue);
  //  h_TT->DrawNormalized("box same");
  h_qcd->DrawNormalized("box same");
  //  h_ewk->DrawNormalized("box same");
  //  if(h_sig) h_sig->DrawNormalized("box same");

  /*
  if(h_sig8) h_sig8->SetFillColor(kRed);
  if(h_sig9) h_sig9->SetFillColor(kGreen+1);
  if(h_sig10) h_sig10->SetFillColor(kBlue);
  if(h_sig8) h_sig8->DrawNormalized("box");
  if(h_sig9) h_sig9->DrawNormalized("box sames");
  if(h_sig10) h_sig10->DrawNormalized("box sames");
  */

  /*
  if(h_sig8_1d) h_sig8_1d->SetLineColor(kRed);
  if(h_sig9_1d) h_sig9_1d->SetLineColor(kGreen+1);
  if(h_sig10_1d) h_sig10_1d->SetLineColor(kBlue);
  if(h_sig8_1d) h_sig8_1d->DrawNormalized("hist");
  if(h_sig9_1d) h_sig9_1d->DrawNormalized("hist sames");
  if(h_sig10_1d) h_sig10_1d->DrawNormalized("hist sames");
*/

}


TGraph* rocMC( TString var , int myCategory, int nbin, float min, float max, TString Xtitle, int color , int style, bool doInvert=false) {

  gStyle->SetCanvasPreferGL(kTRUE);
  gStyle->SetOptStat(0);
  //  gROOT->LoadMacro("utils.C++");

  TString filename="HAD.root";
  if(myCategory==3) filename="MET.root";
  if(myCategory==5) filename="BHAD.root";

  cout << "reading " << filename.Data() << endl;
  TFile *f = new TFile(filename.Data());
  TTree *t1 = (TTree*)f->Get("tree_vbs");
  Double_t weight;
  Int_t mc,ana_category;
  Float_t variable;

  t1->SetBranchAddress("ana_category",&ana_category);
  t1->SetBranchAddress("mc",&mc);
  t1->SetBranchAddress("weight",&weight);
  t1->SetBranchAddress(var.Data(),&variable);

  TH1F *h_sig  = new TH1F("h_sig",var.Data(),nbin, min, max);
  TH1F *h_qcd  = new TH1F("h_qcd","hqcd",nbin, min, max);
  TH1F *h_ewk  = new TH1F("h_ewk","hewk",nbin, min, max);
  TH1F *h_Zinv = new TH1F("h_zinv","hzinv",nbin, min, max);
  TH1F *h_TT   = new TH1F("h_TT","hTT",nbin, min, max);

  // all entries and fill the histograms
  Int_t nentries = (Int_t) t1->GetEntries();
  for (Int_t i=0;i<nentries;i++) {

    t1->GetEntry(i);

    //    if(varMjjCut < 3000) continue;

    //    cout << "HELLO " << "myCategory" << myCategory << " ana_category" << ana_category  << endl;
    if(myCategory!=ana_category) continue;
    variable = fabs(variable);

    if(myCategory==1 && mc==1) h_sig->Fill(variable,weight);
    if(myCategory==3 && mc==3) h_sig->Fill(variable,weight);
    if(myCategory==5 && mc==2) h_sig->Fill(variable,weight);
    if(mc==500) h_qcd->Fill(variable,weight);
    if(mc==200 || mc==210) h_TT->Fill(variable,weight);
    if(mc==300 || mc==310) h_Zinv->Fill(variable,weight);
    if(mc==110 || mc==120) h_ewk->Fill(variable,weight);

  }

  h_sig->Scale(1./h_sig->Integral());
  /*
  h_qcd->Scale(1./h_qcd->Integral());
  h_TT->Scale(1./h_TT->Integral());
  h_Zinv->Scale(1./h_Zinv->Integral());
  h_ewk->Scale(1./h_ewk->Integral());
  */

  TH1F * h_SigClone= (TH1F*) h_sig->Clone();
  TH1F * h_BkgClone= (TH1F*) h_qcd->Clone();
  //  TH1F * h_BkgClone= (TH1F*) h_TT->Clone();
  h_BkgClone->Add(h_TT);
  h_BkgClone->Add(h_ewk);
  h_BkgClone->Add(h_Zinv);
  h_BkgClone->Scale(1./h_BkgClone->Integral());
  //  TH2D * h2d_ = new TH2D("2d","2d", h_SigClone->GetNbinsX(), minX, maxX, h_BkgClone->GetNbinsX() , minY, maxY);

  int maxSteps = h_SigClone->GetNbinsX();
  Double_t x[maxSteps], y[maxSteps];

  for(int step=0; step< h_SigClone->GetNbinsX(); step++) {

    //    double effSig=h_SigClone->Integral(step, h_SigClone->GetNbinsX() );
    //    double effBKG=1.-(h_BkgClone->Integral(step, h_BkgClone->GetNbinsX()) );
    if(doInvert) {
      x[step] = h_SigClone->Integral(0., step );
      y[step] = 1.-(h_BkgClone->Integral(0., step) );
    } else {
      x[step] = h_SigClone->Integral(step, h_SigClone->GetNbinsX() );
      y[step] = 1.-(h_BkgClone->Integral(step, h_BkgClone->GetNbinsX()) );      
    }
    
    //    h2d_->Fill(effSig,effBKG);
  
  }

  TGraph* gr = new TGraph(maxSteps,x,y);
  gr->Draw("AC*");
  //    cout << "Bin[GeV]" << h_Sig->GetBinCenter(step) << "GeV  -- step " << step << " -- eff " << effSig << " rate " << effBKG << endl;
  //  h2d_->Draw(pe);
  gr->SetTitle(var.Data());
  gr->SetLineColor(color);
  gr->SetMarkerColor(color);
  gr->SetMarkerStyle(style);

  return gr;

}



void treeMC( TString var , int myCategory, int nbin, float min, float max, TString Xtitle ) {

  bool plotPol=false;

  gStyle->SetCanvasPreferGL(kTRUE);
  gStyle->SetOptStat(0);
  //  gROOT->LoadMacro("utils.C++");

  TString filename=fileMET;
  if(myCategory==1 || myCategory==2) filename=fileHAD;
  if(myCategory==5 || myCategory==6) filename=fileBHAD;
  cout << "reading " << filename.Data() << endl;
  TFile *f = new TFile(filename.Data());
  TTree *t1 = (TTree*)f->Get("tree_vbs");
  Double_t weight;
  Int_t mc,ana_category;

  Float_t varMjj,varDetajj,varDphijj,varJet2Eta;
  Float_t varMVV,varPTV1,varPTV2,varDetaVV,varzepVB;
  Float_t variable;
  Float_t varMjjCut;

  t1->SetBranchAddress("ana_category",&ana_category);
  t1->SetBranchAddress("mc",&mc);
  t1->SetBranchAddress("weight",&weight);
  //
  /*
  t1->SetBranchAddress("varMjj",&varMjj);
  t1->SetBranchAddress("varDetajj",&varDetajj);
  t1->SetBranchAddress("varDphijj",&varDphijj);
  t1->SetBranchAddress("varJet2Eta",&varJet2Eta);
  ////////
  t1->SetBranchAddress("varMVV",&varMVV);
  t1->SetBranchAddress("varPTV1",&varPTV1);
  t1->SetBranchAddress("varPTV2",&varPTV2);
  t1->SetBranchAddress("varDetaVV",&varDetaVV);
  t1->SetBranchAddress("varzepVB",&varzepVB);
  //
  */
  t1->SetBranchAddress(var.Data(),&variable);
  t1->SetBranchAddress("varMjj",&varMjjCut);

  // SR and CR yield
  // SR and CR yield
  TH1F *h_qcd   = new TH1F("h_qcd","hqcd",nbin, min, max);
  TH1F *h_ewk   = new TH1F("h_ewk","hewk",nbin, min, max);
  TH1F *h_Zinv   = new TH1F("h_zinv","hzinv",nbin, min, max);
  TH1F *h_TT   = new TH1F("h_TT","hTT",nbin, min, max);
  TH1F *h_sig  = new TH1F("h_sig",var.Data(),nbin, min, max);
  TH1F *h_sig2  = new TH1F("h_sig",var.Data(),nbin, min, max);

  TH1F *h_sig8  = new TH1F("h_sig8",var.Data(),nbin, min, max);
  TH1F *h_sig9  = new TH1F("h_sig9",var.Data(),nbin, min, max);
  TH1F *h_sig10  = new TH1F("h_sig10",var.Data(),nbin, min, max);

  // all entries and fill the histograms
  Int_t nentries = (Int_t) t1->GetEntries();
  for (Int_t i=0;i<nentries;i++) {
 
    t1->GetEntry(i);

    //    if(varMjjCut < 3000) continue;

    //    cout << "HELLO " << "myCategory" << myCategory << " ana_category" << ana_category  << " var " << var.Data() << endl;
    if(myCategory!=ana_category) continue;

    //    if(var.Contains("Mjj")) cout << varMjjCut << " " << variable << endl;
    //    if(var.Contains("Mjj")) cout << variable << endl;

    /*
    int mc=0;
    if(label.find("WPhadWPhadJJ") !=string::npos ) mc = 1 ;
    if(label.find("ZbbZhadJJ") !=string::npos ) mc = 2 ;
    if(label.find("ZnnZhadJJ") !=string::npos ) mc = 3 ;
    */
    /*
    if(myCategory==1 && mc==1) h_sig->Fill(variable,weight);
    if(myCategory==2 && mc==1) h_sig->Fill(variable,weight);
    if(myCategory==3 && mc==3) h_sig->Fill(variable,weight);
    if(mc==500) h_qcd->Fill(variable,weight);
    if(mc==200) h_Zinv->Fill(variable,weight);
    if(mc==100) h_ewk->Fill(variable,weight);
    */

    if(!var.Contains("Peta")) variable = fabs(variable);
    if(var.Contains("Mjj")) variable = varMjjCut;

    if(myCategory==1 && mc==1) h_sig->Fill(variable,weight);
    if(myCategory==2 && mc==1) h_sig->Fill(variable,weight);
    if(myCategory==3 && mc==3) h_sig->Fill(variable,weight);
    if(myCategory==4 && mc==3) h_sig->Fill(variable,weight);
    if(myCategory==5 && mc==2) h_sig->Fill(variable,weight);
    if(myCategory==6 && mc==2) h_sig->Fill(variable,weight);
    if(myCategory==1 && mc==11) h_sig2->Fill(variable,weight);
    if(myCategory==1 && mc==8 && plotPol) h_sig8->Fill(variable,weight);
    if(myCategory==1 && mc==9 && plotPol) h_sig9->Fill(variable,weight);
    if(myCategory==1 && mc==10 && plotPol) h_sig10->Fill(variable,weight);
    if(mc==500) h_qcd->Fill(variable,weight);
    if(mc==200 || mc==210) h_TT->Fill(variable,weight);
    if(mc==300 || mc==310) h_Zinv->Fill(variable,weight);
    if(mc==110 || mc==120) h_ewk->Fill(variable,weight);

  }

  TCanvas * c1 = new TCanvas("", "",800,800);
  c1->cd();
  //  gPad->SetLogy(1);
  
  TString label="";
  if(myCategory==1) label="BB";
  if(myCategory==2) label="RB";
  if(myCategory==3) label="BMET";
  if(myCategory==4) label="RMET";
  if(myCategory==5) label="BBtag";
  if(myCategory==6) label="RBtag";

  if(h_sig) h_sig->SetMaximum(2*h_sig->GetMaximum());
  if(h_sig && var.Contains("Mjj")) h_sig->SetMaximum(2.5*h_sig->GetMaximum());
  if(h_sig && (myCategory==1 || myCategory==2)) h_sig->SetLineColor(kRed);
  if(h_sig && (myCategory==3 || myCategory==4)) h_sig->SetLineColor(kOrange+1);
  if(h_sig && (myCategory==5 || myCategory==6)) h_sig->SetLineColor(kYellow-6);
  if(h_sig2 && myCategory==1) h_sig2->SetLineColor(kGray+1);
  if(h_sig8 && myCategory==1 && plotPol) h_sig8->SetLineColor(kBlue);
  if(h_sig9 && myCategory==1 && plotPol) h_sig9->SetLineColor(kGreen+1);
  if(h_sig10 && myCategory==1 && plotPol) h_sig10->SetLineColor(kViolet+1);

  h_qcd->SetLineColor(kGreen+1);
  h_ewk->SetLineColor(4);
  h_Zinv->SetLineColor(kViolet+1);
  h_TT->SetLineColor(kMagenta);

  //
  if(h_sig) h_sig->SetFillColor(kOrange-4);
  if(myCategory==1 || myCategory==2) h_sig->SetFillColor(kRed-10);
  if(myCategory==5 || myCategory==6) h_sig->SetFillColor(kYellow-10);
  if(h_sig) h_sig->SetLineWidth(3);
  if(h_sig2) h_sig2->SetLineWidth(3);
  if(h_sig8 && plotPol) h_sig8->SetLineWidth(3);
  if(h_sig9 && plotPol) h_sig9->SetLineWidth(3);
  if(h_sig10 && plotPol) h_sig10->SetLineWidth(3);
  h_qcd->SetLineWidth(3);
  h_ewk->SetLineWidth(3);
  h_Zinv->SetLineWidth(3);
  h_TT->SetLineWidth(3);

  if(h_sig) h_sig->SetTitle("");
  if(h_sig) h_sig->GetYaxis()->SetTitleOffset(1.5);
  if(h_sig) h_sig->GetYaxis()->SetTitle("Normalized Events");
  if(h_sig) h_sig->GetXaxis()->SetTitle(Xtitle.Data());
  //  h_sig->SetFillStyle(4010);
  //  h_sig->SetFillColorAlpha(kRed, 0.5);
  if(h_sig) h_sig->DrawNormalized("hist");
  if(h_sig2) h_sig2->DrawNormalized("hist same");
  if(h_sig8 && plotPol) h_sig8->DrawNormalized("hist same");
  if(h_sig9 && plotPol) h_sig9->DrawNormalized("hist same");
  if(h_sig10 && plotPol) h_sig10->DrawNormalized("hist same");

  if(!doOnlySignal) {
    h_qcd->DrawNormalized("hist same");
    h_ewk->DrawNormalized("hist same");
    h_Zinv->DrawNormalized("hist same");
    h_TT->DrawNormalized("hist same");
  }

  auto legend = new TLegend(0.58-0.05,0.68,0.88-0.1,0.88);
  //  legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header

  /*
  if(myCategory==1) label="BB";
  if(myCategory==2) label="RB";
  if(myCategory==3) label="BMET";
  if(myCategory==4) label="RMET";
  if(myCategory==5) label="BBtag";
  if(myCategory==6) label="RBtag";
  */

  if(h_sig && (myCategory==1 || myCategory==2)) legend->AddEntry(h_sig,"SIG WWss ","f");
  if(h_sig && (myCategory==3 || myCategory==4)) legend->AddEntry(h_sig,"SIG Z #nu #nu","f");
  if(h_sig && (myCategory==5 || myCategory==6)) legend->AddEntry(h_sig,"SIG Z bb","f");
  if(h_sig2 && myCategory==1) legend->AddEntry(h_sig2,"H^{++} WWss ","l");
  if(h_sig8 && myCategory==1 && plotPol) legend->AddEntry(h_sig8,"LL ","l");
  if(h_sig9 && myCategory==1 && plotPol) legend->AddEntry(h_sig9,"LT ","l");
  if(h_sig10 && myCategory==1 && plotPol) legend->AddEntry(h_sig10,"TT ","l");

  if(!doOnlySignal) {
    legend->AddEntry(h_qcd,"QCD","l");
    legend->AddEntry(h_ewk,"MULTIBOSON","l");
    legend->AddEntry(h_Zinv,"Zinv + Wj","l");
    legend->AddEntry(h_TT,"t,tt,ttV","l");
  }
  legend->SetBorderSize(0);
  legend->SetFillColor(0);
  legend->Draw();

  TLatex latexLabel;
  latexLabel.SetNDC();
  latexLabel.SetTextSize(0.04);
  latexLabel.SetTextColor(1);
  latexLabel.SetTextFont(42);
  latexLabel.SetTextAlign(33);
  if(myCategory==1) latexLabel.DrawLatex(0.8, 0.6, "BB");
  if(myCategory==2) latexLabel.DrawLatex(0.8, 0.6, "RB");
  if(myCategory==3) latexLabel.DrawLatex(0.8, 0.6, "BMET");
  if(myCategory==4) latexLabel.DrawLatex(0.8, 0.6, "RMET");
  if(myCategory==5) latexLabel.DrawLatex(0.8, 0.6, "BBtag");
  if(myCategory==6) latexLabel.DrawLatex(0.8, 0.6, "RBtag");
  latexLabel.SetTextFont(62);

  c1->SaveAs(Form("PLOTS/shape_%s_%s.png",var.Data(),label.Data()));

  return;

}

void makeROCS(int myCategory){

  //  kWhite  = 0,   kBlack  = 1,   kGray    = 920,  kRed    = 632,  kGreen  = 416,
  //  kBlue   = 600, kYellow = 400, kMagenta = 616,  kCyan   = 432,  kOrange = 800,
  //  kSpring = 820, kTeal   = 840, kAzure   =  860, kViolet = 880,  kPink   = 900

  TGraph* grMjj = rocMC("varMjj" , myCategory, 100 , 0., 4000.,"M(j,j)",632,20);
  TGraph* grDetaJJ = rocMC("varDetajj" , myCategory, 50 , 0., 10.,"#Delta#eta(j,j)",800+1,21);
  TGraph* grDphiJJ = rocMC("varDphijj" , myCategory, 50 , 0., 3.14,"#Delta#eta(j,j)",800+2,22);

  TGraph* grJet2Eta = rocMC("varJet2Eta" , myCategory, 50 , 0., 10.,"#eta j_{2}^{tag}",800-7,23);
  TGraph* grJet2Pt = rocMC("varJet2Pt" , myCategory, 100 , 0., 500.,"p_{T} j_{2}^{tag}",800-3,24);

  //##

  TGraph* grMVV = rocMC("varMVV" , myCategory, 100 , 0., 4000.,"M(V,V)",600,20);
  TGraph* grDetaVV = rocMC("varDetaVV" , myCategory, 50 , 0., 10.,"#Delta#eta(V,V)",600-7,21);
  TGraph* grPTVV = rocMC("varPTVV" , myCategory, 100 , 0., 1000.,"p_T(V,V)",432,22);
  TGraph* grPTV1 = rocMC("varPTV1" , myCategory, 100 , 0., 1000.,"p_T(V1)",432+1,23);
  TGraph* grPTV2 = rocMC("varPTV2" , myCategory, 100 , 0., 1000.,"p_T(V2)",432+2,24);


  TGraph* grZepVB = rocMC("varzepVB" , myCategory, 50 , 0., 2.,"z(V_{1})",416+1,20,true);
  TGraph* grZepVV = rocMC("varzepVV" , myCategory, 50 , 0., 2.,"z(V+V)",416-7,21,true);
  TGraph* varDRVj = rocMC("varDRVj" , myCategory, 15 , 0., 6.,"#DeltaR(V,j)",416-3,22,true);
  TGraph* varnormPTVVjj = rocMC("varnormPTVVjj" , myCategory, 50 , 0., 1.,"|#vec{p_{T}^{tot}}|/ #Sigma p_{T}",416+3,23,true);
  TGraph* varcenPTVVjj = rocMC("varcenPTVVjj" , myCategory, 50 , 0., 1.,"|#vec{p_{T}^{VV}} - #vec{p_{T}^{jj}}|/ #Delta#eta(j,j)",416-1,24);
  TGraph* grCen = rocMC("varCen" , myCategory, 50 , 0., 10.,"min[min(\eta^{V_{1}},\eta^{V_{2}}) - min(\eta^{j_{1}},\eta^{j_{2}}), max(\eta^{j_{1}},\eta^{j_{2}}) - max(\eta^{V_{1}},\eta^{V_{2}})]",416-9,25); 

  //##

  TGraph* grBDT = rocMC("BDTnoBnoMET" , myCategory, 100 , -1., 1.,"BDT",1,21);

  auto c3 = new TCanvas("c3","c3",800, 600);

  TMultiGraph *mg = new TMultiGraph();
  mg->Add(grMjj,"lp");
  mg->Add(grDetaJJ,"lp");
  mg->Add(grDphiJJ,"lp");
  mg->Add(grJet2Eta,"lp");
  mg->Add(grJet2Pt,"lp");

  mg->Add(grMVV,"lp");
  mg->Add(grPTVV,"lp");
  mg->Add(grPTV1,"lp");
  mg->Add(grPTV2,"lp");

  mg->Add(grDetaVV,"lp");
  mg->Add(grCen,"lp");
  mg->Add(grZepVB,"lp");
  mg->Add(grZepVV,"lp");
  mg->Add(varDRVj,"lp");
  mg->Add(varnormPTVVjj,"lp");
  mg->Add(varcenPTVVjj,"lp");

  if(myCategory==1) mg->Add(grBDT,"lp");

  if(myCategory==5) mg->SetTitle("BBtag; signal efficiency; background rejection");
  if(myCategory==3) mg->SetTitle("BMET; signal efficiency; background rejection");
  if(myCategory==1) mg->SetTitle("BB; signal efficiency; background rejection");
  //  mg->SetTitle("; signal efficiency; QCD background rejection");

  mg->Draw("a");

  gStyle->SetLegendBorderSize(0);

  c3->BuildLegend();
  c3->SaveAs("roc.png");

  return;

}

void treeMCplots(){

  treeMC("bosV2chi2" , 2, 22 , -2., 20.,"V2chi2 (Resolved)");
  treeMC("bosV2chi2" , 4, 22 , -2., 20.,"V2chi2 (Resolved)");
  treeMC("bosV2chi2" , 6, 22 , -2., 20.,"V2chi2 (Resolved)");

  treeMC("bosV2mass" , 2, 15 , 50., 125.,"V2mass (Resolved)");
  treeMC("bosV2mass" , 4, 15 , 50., 125.,"V2mass (Resolved)");
  treeMC("bosV2mass" , 6, 15 , 50., 125.,"V2mass (Resolved)");

  //
  treeMC("bosV1mass" , 1, 15 , 50., 125.,"V1mass(Boosted)");
  treeMC("bosV2mass" , 1, 15 , 50., 125.,"V2mass(Boosted)");
  treeMC("bosV1discr" , 1, 10 , 0., 1.,"V1discr(Boosted)");
  treeMC("bosV2discr" , 1, 10 , 0., 1.,"V2discr(Boosted)");

  //  treeMC("bosV2mass" , 2, 15 , 50., 125.,"V2mass (Resolved)");
  //  treeMC("bosV2chi2" , 2, 22 , -2., 20.,"V2chi2 (Resolved)");

  //MET only infos on v2
  //  treeMC("bosV1mass" , 3, 10 , 0., 1000.,"V1mass");
  //  treeMC("bosV1discr" , 3, 10 , 0., 1.,"V1discr");
  treeMC("bosV2mass" , 3, 15 , 50., 125.,"V2mass (Boosted)");
  treeMC("bosV2discr" , 3, 10 , 0., 1.,"V2discr (Boosted)");
  treeMC("bosV2tdiscr" , 3, 10 , 0., 1.,"V2discr top (Boosted)");
  //  treeMC("bosV1mass" , 4, 10 , 0., 1000.,"V1mass");
  //  treeMC("bosV1discr" , 4, 10 , 0., 1.,"V1discr");
  //  treeMC("bosV2mass" , 4, 15 , 50., 125.,"V2mass (Resolved)");
  //  treeMC("bosV2discr" , 4, 10 , 0., 1.,"V2discr (Resolved)");
  //  treeMC("bosV2chi2" , 4, 22 , -2., 20.,"V2chi2 (Resolved)");

  //tag
  treeMC("bosV1mass" , 5, 15 , 50., 125.,"V1mass (doubleB)");
  treeMC("bosV1discr" , 5, 10 , 0., 1.,"V1discr (doubleB)");
  treeMC("bosV2mass" , 5, 15 , 50., 125.,"V2mass (Boosted)");
  treeMC("bosV2discr" , 5, 10 , 0., 1.,"V2discr (Boosted)");
  treeMC("bosV2discr" , 5, 10 , 0., 1.,"V2discr top (Boosted)");

  treeMC("bosV1mass" , 6, 15 , 50., 125.,"V1mass (doubleB)");
  treeMC("bosV1discr" , 6, 10 , 0., 1.,"V1discr (doubleB)");
  treeMC("bosV1tdiscr" , 6, 10 , 0., 1.,"V1discr top (doubleB)");
  //  treeMC("bosV2mass" , 6, 15 , 50., 125.,"V2mass (Resolved)");
  //  treeMC("bosV2discr" , 6, 10 , 0., 1.,"V2discr (Resolved)");
  //  treeMC("bosV2chi2" , 6, 22 , -2., 20.,"V2chi2 (Resolved)");

  //  return;

  //  draw2D(1, 20 , 0., 1.);
  //  return;
  // for the mass
  //  draw2D(1, 20 , 0., 4000., 0., 4000.);
  // for the BDT
  //  draw2D(1, 20 , -1., 1., 0., 3000.);
  //  return;

  // BB

  treeMC("varMjj" , 1, 10 , 0., 4000.,"M(j,j)");
  treeMC("varDetajj" , 1, 10 , 0., 10.,"#Delta#eta(j,j)");
  treeMC("varDphijj" , 1, 8 , 0., 3.2,"#Delta#Phi(j,j)");
  treeMC("varJet2Eta" , 1, 10 , 0., 10.,"#eta j_{2}^{tag}");
  treeMC("varJet2Pt" , 1, 10 , 0., 500.,"p_{T} j_{2}^{tag}");

  treeMC("varMVV" , 1, 10 , 0., 2500.,"M(V,V)");
  treeMC("varPTVV" , 1, 20 , 0., 1000.,"p_T(V,V)");
  treeMC("varPTV1" , 1, 20 , 0., 1000.,"p_T V_{1} ");
  treeMC("varPTV2" , 1, 20 , 0., 1000.,"p_T V_{2} ");
  treeMC("varDetaVV" , 1, 20 , 0., 10.,"#Delta#eta(V,V)");

  treeMC("varzepVB" , 1, 20 , 0., 2.,"z(V_{1})");
  treeMC("varzepVV" , 1, 20 , 0., 2.,"z(V+V)");
  treeMC("varDRVj" , 1, 15 , 0., 6.,"#DeltaR(V,j)");
  treeMC("varnormPTVVjj" , 1, 20 , 0., 1.,"|#vec{p_{T}^{tot}}|/ #Sigma p_{T}");
  treeMC("varcenPTVVjj" , 1, 20 , 0., 20.,"|#vec{p_{T}^{VV}} - #vec{p_{T}^{jj}}|/ #Delta#eta(j,j)");

  treeMC("varCen" , 1, 20 , 0., 10.,"min[min(\eta^{V_{1}},\eta^{V_{2}}) - min(\eta^{j_{1}},\eta^{j_{2}}), max(\eta^{j_{1}},\eta^{j_{2}}) - max(\eta^{V_{1}},\eta^{V_{2}})]");
  treeMC("varEtaMinV" , 1, 20 , 0., 10.,"min(#etaV_{1},#etaV_{2})");
  treeMC("varEtaMaxV" , 1, 20 , 0., 10.,"max(#etaV_{1},#etaV_{2})");
  treeMC("varFW2j" , 1, 10 , 0., 1.,"FW2");

  treeMC("varPetaVV" , 1, 20 , -10, 10.,"#etaV_{1} x #etaV_{2}");

  treeMC("BDTnoBnoMET" , 1, 20 , -1., 1.,"BDT");


  // RB
  treeMC("varMjj" , 2, 10 , 0., 4000.,"M(j,j)");
  treeMC("varDetajj" , 2, 10 , 0., 10.,"#Delta#eta(j,j)");
  treeMC("varDphijj" , 2, 8 , 0., 3.2,"#Delta#Phi(j,j)");
  treeMC("varJet2Eta" , 2, 10 , 0., 10.,"#eta j_{2}^{tag}");
  treeMC("varJet2Pt" , 2, 10 , 0., 500.,"p_{T} j_{2}^{tag}");

  treeMC("varMVV" , 2, 10 , 0., 2500.,"M(V,V)");
  treeMC("varPTVV" , 2, 20 , 0., 1000.,"p_T(V,V)");
  treeMC("varPTV1" , 2, 20 , 0., 1000.,"p_T V_{1}");
  treeMC("varPTV2" , 2, 20 , 0., 1000.,"p_T V_{2}");
  treeMC("varDetaVV" , 2, 20 , 0., 10.,"#Delta#Eta(V,V)");

  treeMC("varCen" , 2, 20 , 0., 10.,"min[min(\eta^{V_{1}},\eta^{V_{2}}) - min(\eta^{j_{1}},\eta^{j_{2}}), max(\eta^{j_{1}},\eta^{j_{2}}) - max(\eta^{V_{1}},\eta^{V_{2}})]");
  treeMC("varEtaMinV" , 2, 20 , 0., 10.,"min(#etaV_{1},#etaV_{2})");
  treeMC("varEtaMaxV" , 2, 20 , 0., 10.,"max(#etaV_{1},#etaV_{2})");

  treeMC("varzepVB" , 2, 20 , 0., 2.,"z(V_{1})");
  treeMC("varzepVV" , 2, 20 , 0., 2.,"z(V+V)");
  treeMC("varDRVj" , 2, 15 , 0., 6.,"#DeltaR(V,j)");
  treeMC("varnormPTVVjj" , 2, 20 , 0., 1.,"|#vec{p_{T}^{tot}}|/ #Sigma p_{T}");
  treeMC("varcenPTVVjj" , 2, 20 , 0., 20.,"|#vec{p_{T}^{VV}} - #vec{p_{T}^{jj}}|/ #Delta#eta(j,j)");
  treeMC("varFW2j" , 2, 10 , 0., 1.,"FW2");

  treeMC("varPetaVV" , 2, 20 , -10, 10.,"#etaV_{1} x #etaV_{2}");

  // BMET
  treeMC("varMjj" , 3, 10 , 0., 4000.,"M(j,j)");
  treeMC("varDetajj" , 3, 10 , 0., 10.,"#Delta#eta(j,j)");
  treeMC("varDphijj" , 3, 8 , 0., 3.2,"#Delta#Phi(j,j)");
  treeMC("varJet2Eta" , 3, 10 , 0., 10.,"#eta j_{2}^{tag}");

  treeMC("varMVV" , 3, 10 , 0., 2500.,"M(V,V)");
  treeMC("varPTVV" , 3, 20 , 0., 1000.,"p_T(V,V)");
  treeMC("varPTV1" , 3, 20 , 0., 1000.,"p_T(V_{1})");
  treeMC("varPTV2" , 3, 20 , 0., 1000.,"p_T(V_{2})");
  treeMC("varJet2Pt" , 3, 10 , 0., 500.,"p_{T} j_{2}^{tag}");

  //  treeMC("varDetaVV" , 3, 50 , 0., 10.);
  treeMC("varzepVB" , 3, 20 , 0., 2.,"z(V_{1})");
  treeMC("varDRVj" , 3, 15 , 0., 6.,"#DeltaR(V,j)");
  treeMC("varnormPTVVjj" , 3, 20 , 0., 1.,"|#vec{p_{T}^{tot}}|/ #Sigma p_{T}");
  treeMC("varcenPTVVjj" , 3, 20 , 0., 20.,"|#vec{p_{T}^{VV}} - #vec{p_{T}^{jj}}|/ #Delta#eta(j,j)");
  treeMC("met_pt" , 3, 20 , 0., 1000.,"p_{T}^{miss}");
  treeMC("varFW2j" , 3, 10 , 0., 1.,"FW2");


  // RMET
  treeMC("varMjj" , 4, 10 , 0., 4000.,"M(j,j)");
  treeMC("varDetajj" , 4, 10 , 0., 10.,"#Delta#eta(j,j)");
  treeMC("varDphijj" , 4, 8 , 0., 3.2,"#Delta#Phi(j,j)");
  treeMC("varJet2Eta" , 4, 10 , 0., 10.,"#eta j_{2}^{tag}");

  treeMC("varMVV" , 4, 10 , 0., 2500.,"M(V,V)");
  treeMC("varPTVV" , 4, 20 , 0., 1000.,"p_T(V,V)");
  treeMC("varPTV1" , 4, 20 , 0., 1000.,"p_T(V_{1})");
  treeMC("varPTV2" , 4, 20 , 0., 1000.,"p_T(V_{2})");
  treeMC("varJet2Pt" , 4, 10 , 0., 500.,"p_{T} j_{2}^{tag}");

  //  treeMC("varDetaVV" , 3, 50 , 0., 10.);
  treeMC("varzepVB" , 4, 20 , 0., 2.,"z(V_{1})");
  treeMC("varDRVj" , 4, 15 , 0., 6.,"#DeltaR(V,j)");
  treeMC("varnormPTVVjj" , 4, 20 , 0., 1.,"|#vec{p_{T}^{tot}}|/ #Sigma p_{T}");
  treeMC("varcenPTVVjj" , 4, 20 , 0., 20.,"|#vec{p_{T}^{VV}} - #vec{p_{T}^{jj}}|/ #Delta#eta(j,j)");
  treeMC("met_pt" , 4, 20 , 0., 1000.,"p_{T}^{miss}");
  treeMC("varFW2j" , 4, 10 , 0., 1.,"FW2");


  // BBtag
  treeMC("varMjj" , 5, 10 , 0., 4000.,"M(j,j)");
  treeMC("varDetajj" , 5, 10 , 0., 10.,"#Delta#eta(j,j)");
  treeMC("varDphijj" , 5, 8 , 0., 3.2,"#Delta#Phi(j,j)");
  treeMC("varJet2Eta" , 5, 10 , 0., 10.,"#eta j_{2}^{tag}");
  treeMC("varJet2Pt" , 5, 10 , 0., 500.,"p_{T} j_{2}^{tag}");

  treeMC("varMVV" , 5, 10 , 0., 2500.,"M(V,V)");
  treeMC("varPTVV" , 5, 20 , 0., 1000.,"p_T(V,V)");
  treeMC("varPTV1" , 5, 20 , 0., 1000.,"p_T V_{1} ");
  treeMC("varPTV2" , 5, 20 , 0., 1000.,"p_T V_{2} ");
  treeMC("varDetaVV" , 5, 20 , 0., 10.,"#Delta#eta(V,V)");

  treeMC("varzepVB" , 5, 20 , 0., 2.,"z(V_{1})");
  treeMC("varzepVV" , 5, 20 , 0., 2.,"z(V+V)");
  treeMC("varDRVj" , 5, 15 , 0., 6.,"#DeltaR(V,j)");
  treeMC("varnormPTVVjj" , 5, 20 , 0., 1.,"|#vec{p_{T}^{tot}}|/ #Sigma p_{T}");
  treeMC("varcenPTVVjj" , 5, 20 , 0., 20.,"|#vec{p_{T}^{VV}} - #vec{p_{T}^{jj}}|/ #Delta#eta(j,j)");

  treeMC("varCen" , 5, 20 , 0., 10.,"min[min(\eta^{V_{1}},\eta^{V_{2}}) - min(\eta^{j_{1}},\eta^{j_{2}}), max(\eta^{j_{1}},\eta^{j_{2}}) - max(\eta^{V_{1}},\eta^{V_{2}})]");
  treeMC("varEtaMinV" , 5, 20 , 0., 10.,"min(#etaV_{1},#etaV_{2})");
  treeMC("varEtaMaxV" , 5, 20 , 0., 10.,"max(#etaV_{1},#etaV_{2})");
  treeMC("varFW2j" , 5, 10 , 0., 1.,"FW2");

  treeMC("varPetaVV" , 5, 20 , -10, 10.,"#etaV_{1} x #etaV_{2}");

  // RBtag
  treeMC("varMjj" , 6, 10 , 0., 4000.,"M(j,j)");
  treeMC("varDetajj" , 6, 10 , 0., 10.,"#Delta#eta(j,j)");
  treeMC("varDphijj" , 6, 8 , 0., 3.2,"#Delta#Phi(j,j)");
  treeMC("varJet2Eta" , 6, 10 , 0., 10.,"#eta j_{2}^{tag}");
  treeMC("varJet2Pt" , 6, 10 , 0., 500.,"p_{T} j_{2}^{tag}");

  treeMC("varMVV" , 6, 10 , 0., 2500.,"M(V,V)");
  treeMC("varPTVV" , 6, 20 , 0., 1000.,"p_T(V,V)");
  treeMC("varPTV1" , 6, 20 , 0., 1000.,"p_T V_{1} ");
  treeMC("varPTV2" , 6, 20 , 0., 1000.,"p_T V_{2} ");
  treeMC("varDetaVV" , 6, 20 , 0., 10.,"#Delta#eta(V,V)");

  treeMC("varzepVB" , 6, 20 , 0., 2.,"z(V_{1})");
  treeMC("varzepVV" , 6, 20 , 0., 2.,"z(V+V)");
  treeMC("varDRVj" , 6, 15 , 0., 6.,"#DeltaR(V,j)");
  treeMC("varnormPTVVjj" , 6, 20 , 0., 1.,"|#vec{p_{T}^{tot}}|/ #Sigma p_{T}");
  treeMC("varcenPTVVjj" , 6, 20 , 0., 20.,"|#vec{p_{T}^{VV}} - #vec{p_{T}^{jj}}|/ #Delta#eta(j,j)");

  treeMC("varCen" , 6, 20 , 0., 10.,"min[min(\eta^{V_{1}},\eta^{V_{2}}) - min(\eta^{j_{1}},\eta^{j_{2}}), max(\eta^{j_{1}},\eta^{j_{2}}) - max(\eta^{V_{1}},\eta^{V_{2}})]");
  treeMC("varEtaMinV" , 6, 20 , 0., 10.,"min(#etaV_{1},#etaV_{2})");
  treeMC("varEtaMaxV" , 6, 20 , 0., 10.,"max(#etaV_{1},#etaV_{2})");
  treeMC("varFW2j" , 6, 10 , 0., 1.,"FW2");

  treeMC("varPetaVV" , 6, 20 , -10, 10.,"#etaV_{1} x #etaV_{2}");

}
