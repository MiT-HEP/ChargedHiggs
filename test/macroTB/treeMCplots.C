#include "TROOT.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"


//double lumi=12.9;
double lumi=36.2;                                                                                                                                                            
double eff=1.;
//12878700468.905                                                                                                                                                              

#include <iostream>
using namespace std;

void fillPlot(int mc, int sig, int binContent, TH1F* & h_tt1l, TH1F* & h_tt2l, TH1F* & h_Other,TH1F* & h_htb500, TH1F* & h_htb2000, TH1F* & h_htauNu500, double weight) {

  if(mc==101) { h_tt2l->Fill(binContent,weight);}
  if(mc==102 || mc==103) { h_tt1l->Fill(binContent,weight); }
  if(sig==1 && (mc==3)) { h_htb500->Fill(binContent,weight); }
  if(sig==1 && mc==6) { h_htb2000->Fill(binContent,weight); }
  if(sig==1 && mc==53) { h_htauNu500->Fill(binContent,weight); }
  if(sig==0 && (mc>110 && mc<350)) { h_Other->Fill(binContent,weight); }

}

void restylePlot(TH1F* & h_tt1l, TH1F* & h_tt2l, TH1F* & h_Other, TH1F* & h_htb500, TH1F* & h_htb2000, TH1F* & h_htauNu500) {

  //Tan beta = 30
  //  BR top-bottom: 75% (flat)  BR tau-nu: 15% (flat) 
  //  Production x-secs from https://twiki.cern.ch/twiki/bin/view/LHCPhysics/LHCHXSWGMSSMCharged
  //  MH       xSec prod.  xSec TBxSec TauNu
  //    300   0.343796    0.257847     0.0515694
  //    400   0.158142    0.1186065    0.0237213
  //    500   0.0785572   0.0589179    0.01178358
  //    800   0.0130645   0.009798375  0.001959675
  //    1000  0.00474564  0.00355923   0.000711846
  //    2000  8.71E-005   6.53187E-005 1.306374E-005
  //    3000  1.491E-006  1.118E-006   2.2359E-007

  h_tt1l->Scale( lumi*1000*eff );
  h_tt2l->Scale( lumi*1000*eff );
  h_Other->Scale( lumi*1000*eff );
  h_htb500->Scale( lumi*1000*eff*0.0589179);
  h_htb2000->Scale( lumi*1000*eff*6.53187E-005 );
  h_htauNu500->Scale( lumi*1000*eff*0.01178358);

  h_tt1l->SetLineWidth(3);
  h_tt2l->SetLineWidth(3);
  h_htb500->SetLineWidth(3);
  h_htb2000->SetLineWidth(3);
  h_htauNu500->SetLineWidth(3);

  h_tt1l->SetLineColor(kBlue);
  h_tt2l->SetLineColor(2);
  h_htb500->SetLineColor(kGray+1);
  h_htb2000->SetLineColor(kBlack);
  h_htauNu500->SetLineColor(kGreen+1);
}

void treeMCplots( ) {

  gStyle->SetOptStat(0);
  gROOT->LoadMacro("utils.C++");

  TFile *f = new TFile("DEC17/1l.root");
  TTree *t1 = (TTree*)f->Get("tree_tb");
  Float_t mt,mt2bb,mtMin,DRl1b1;
  Double_t weight;
  Int_t mc,lep_category,sig;
  Int_t NB,NJ,NcJ;
  Double_t *jet_eta=0;
  t1->SetBranchAddress("mt",&mt);
  t1->SetBranchAddress("mtMin",&mtMin);
  t1->SetBranchAddress("DRl1b1",&DRl1b1);
  t1->SetBranchAddress("mt2bb",&mt2bb);
  t1->SetBranchAddress("NBJets",&NB);
  t1->SetBranchAddress("NJets",&NJ);
  t1->SetBranchAddress("NcentralJets",&NcJ);
  t1->SetBranchAddress("mc",&mc);
  t1->SetBranchAddress("sig",&sig);
  t1->SetBranchAddress("lep_category",&lep_category);
  t1->SetBranchAddress("weight",&weight);
  //  t1->SetBranchAddress("jet_eta",&jet_eta);

  // SR and CR yield
  TH1F *h_tt1l = new TH1F("h_tt1l","TTJets #rightarrow 1l",10,0,10);
  TH1F *h_tt2l = new TH1F("h_tt2l","TTJets #rightarrow 2l",10,0,10);
  TH1F *h_Other = new TH1F("h_Other ","Other",10,0,10);
  TH1F *h_htb500    = new TH1F("h_htb500","h^{+} #rightarrow tb (M300,M400,M500,M800)",5,0,5);
  TH1F *h_htb2000   = new TH1F("h_htb2000","h^{+} #rightarrow tb (M300,M400,M500,M800)",5,0,5);
  TH1F *h_htauNu500 = new TH1F("h_htauNu500","h^{+} #rightarrow #tau #nu (M300,M500,M800)",5,0,5);

  // two histograms                                                                                                                                                
  TH1F *hmt_tt1l      = new TH1F("hmt_tt1l","TTJets #rightarrow 1l",50,0,500);
  TH1F *hmt_tt2l      = new TH1F("hmt_tt2l","TTJets #rightarrow 2l",50,0,500);
  TH1F *hmt_htb500    = new TH1F("hmt_htb500","h^{+} #rightarrow tb (M300,M400,M500,M800)",50,0,500);
  TH1F *hmt_htb2000   = new TH1F("hmt_htb2000","h^{+} #rightarrow tb (M1000,M2000,M3000)",50,0,500);
  TH1F *hmt_htauNu500 = new TH1F("hmt_htauNu500","h^{+} #rightarrow #tau #nu (M300,M500,M800)",50,0,500);

  TH1F *hdr_tt1l      = new TH1F("hdr_tt1l","TTJets #rightarrow 1l",100,0,6.28);
  TH1F *hdr_tt2l      = new TH1F("hdr_tt2l","TTJets #rightarrow 2l",100,0,6.28);
  TH1F *hdr_htb500    = new TH1F("hdr_htb500","h^{+} #rightarrow tb (M300,M400,M500,M800)",100,0,6.28);
  TH1F *hdr_htb2000   = new TH1F("hdr_htb2000","h^{+} #rightarrow tb (M1000,M2000,M3000)",100,0,6.28);
  TH1F *hdr_htauNu500 = new TH1F("hdr_htauNu500","h^{+} #rightarrow #tau #nu (M300,M500,M800)",100,0,6.28);

  // two histograms                                                                                                                                                
  TH1F *hmt2bb_tt1l      = new TH1F("hmt2bb_tt1l","TTJets #rightarrow 1l",50,0,1000);
  TH1F *hmt2bb_tt2l      = new TH1F("hmt2bb_tt2l","TTJets #rightarrow 2l",50,0,1000);
  TH1F *hmt2bb_htb500    = new TH1F("hmt2bb_htb500","h^{+} #rightarrow tb (M300,M400,M500,M800)",50,0,1000);
  TH1F *hmt2bb_htb2000   = new TH1F("hmt2bb_htb2000","h^{+} #rightarrow tb (M1000,M2000,M3000)",50,0,1000);
  TH1F *hmt2bb_htauNu500 = new TH1F("hmt2bb_htauNu500","h^{+} #rightarrow #tau #nu (M300,M500,M800)",50,0,1000);

  TH1F *hNB_tt1l      = new TH1F("hNB_tt1l","TTJets #rightarrow 1l",5,0,5);
  TH1F *hNB_tt2l      = new TH1F("hNB_tt2l","TTJets #rightarrow 2l",5,0,5);
  TH1F *hNB_htb500    = new TH1F("hNB_htb500","h^{+} #rightarrow tb (M300,M400,M500,M800)",5,0,5);
  TH1F *hNB_htb2000   = new TH1F("hNB_htb2000","h^{+} #rightarrow tb (M1000,M2000,M3000)",5,0,5);
  TH1F *hNB_htauNu500 = new TH1F("hNB_htauNu500","h^{+} #rightarrow #tau #nu (M300,M500,M800)",5,0,5);

  // all entries and fill the histograms
  Int_t nentries = (Int_t) t1->GetEntries();
  for (Int_t i=0;i<nentries;i++) {
 
    t1->GetEntry(i);

    bool do1l = (lep_category==2);
    //    bool do1l = (lep_category==1 || lep_category==2);
    bool do2l = (lep_category==3 || lep_category==4 || lep_category==5);
    //    bool do1l = (lep_category==1 || lep_category==2)?True:False;
    bool baseline = ((do1l && NcJ>=5 && NB>=2) || (do2l && NcJ>=3 && NB>=2));

    if(do1l) {
      if(mc==101) { hmt_tt2l->Fill(mt,weight); hNB_tt2l->Fill(NB,weight); hmt2bb_tt2l->Fill(mt2bb,weight); hdr_tt2l->Fill(DRl1b1,weight);}
      if(mc==102 || mc==103) { hmt_tt1l->Fill(mt,weight); hNB_tt1l->Fill(NB,weight); hmt2bb_tt1l->Fill(mt2bb,weight); hdr_tt1l->Fill(DRl1b1,weight);}
      if(mc==3 || mc==2 || mc==1 || mc==4) { hmt_htb500->Fill(mt,weight); hNB_htb500->Fill(NB,weight); hmt2bb_htb500->Fill(mt2bb,weight); hdr_htb500->Fill(DRl1b1,weight);}
      if(mc==6 || mc==5 || mc==7) { hmt_htb2000->Fill(mt,weight); hNB_htb2000->Fill(NB,weight); hmt2bb_htb2000->Fill(mt2bb,weight); hdr_htb2000->Fill(DRl1b1,weight);}
      if(mc==53 || mc==51 || mc==54) { hmt_htauNu500->Fill(mt,weight); hNB_htauNu500->Fill(NB,weight); hmt2bb_htauNu500->Fill(mt2bb,weight); hdr_htauNu500->Fill(DRl1b1,weight);}
    }

    if(!baseline) continue;

    /*
    int NcJ=0;
    for(int j=0;j!=NJ;++j) {
      if(fabs(jet_eta[j])<2.4) NcJ++;
    }
    */


    if(!do1l) continue;
    if(NB==2 && mt<120) fillPlot(mc, sig, 0, h_tt1l, h_tt2l, h_Other, h_htb500, h_htb2000, h_htauNu500,weight);
    if(NB==2 && mt>120) fillPlot(mc, sig, 1, h_tt1l, h_tt2l, h_Other, h_htb500, h_htb2000, h_htauNu500,weight);
    if(NB>=3 && mt<120) fillPlot(mc, sig, 2, h_tt1l, h_tt2l, h_Other, h_htb500, h_htb2000, h_htauNu500,weight);
    if(NB>=3 && mt>120) fillPlot(mc, sig, 3, h_tt1l, h_tt2l, h_Other, h_htb500, h_htb2000, h_htauNu500,weight);

    /*
    if(!do2l) continue;
    if(NB==2) fillPlot(mc, sig, 0, h_tt1l, h_tt2l, h_Other, h_htb500, h_htb2000, h_htauNu500,weight);
    if(NB>=3) fillPlot(mc, sig, 2, h_tt1l, h_tt2l, h_Other, h_htb500, h_htb2000, h_htauNu500,weight);
    */
  }

  restylePlot(h_tt1l,h_tt2l,h_Other, h_htb500,h_htb2000,h_htauNu500);

  std::cout << "name  & " << "       tt1l                " << " & " << "       tt2l                " << "       Other                ";
  std::cout << "       h_htb500              " << "       h_htb2000              " << "       h_htauNu500              "<< std::endl;
  std::cout << "SR1   & " << h_tt1l->GetBinContent(1) << " & " << h_tt2l->GetBinContent(1) << " & " << h_Other->GetBinContent(1)<< " & " ;
  std::cout << h_htb500->GetBinContent(1) << " & "<< h_htb2000->GetBinContent(1) << " & " << h_htauNu500->GetBinContent(1) << std::endl;

  std::cout << "SR2   & " << h_tt1l->GetBinContent(2) << " & " << h_tt2l->GetBinContent(2) << " & " << h_Other->GetBinContent(2)<< " & " ;
  std::cout << h_htb500->GetBinContent(2) << " & " << h_htb2000->GetBinContent(2) << " & " << h_htauNu500->GetBinContent(2) << std::endl;

  std::cout << "SR3   & " << h_tt1l->GetBinContent(3) << " & " << h_tt2l->GetBinContent(3) << " & " << h_Other->GetBinContent(3)<< " & " ;
  std::cout << h_htb500->GetBinContent(3) << " & " << h_htb2000->GetBinContent(3) << " & " << h_htauNu500->GetBinContent(3) << std::endl;

  std::cout << "SR4   & " << h_tt1l->GetBinContent(4) << " & " << h_tt2l->GetBinContent(4) << " & " << h_Other->GetBinContent(4) << " & "; 
  std::cout << h_htb500->GetBinContent(4) << " & " << h_htb2000->GetBinContent(4) << " & " << h_htauNu500->GetBinContent(4) << std::endl;

  //  return;

  restylePlot(hmt_tt1l,hmt_tt2l,hmt_tt1l,hmt_htb500,hmt_htb2000,hmt_htauNu500);
  restylePlot(hNB_tt1l,hNB_tt2l,hNB_tt1l,hNB_htb500,hNB_htb2000,hNB_htauNu500);
  restylePlot(hmt2bb_tt1l,hmt2bb_tt2l,hmt2bb_tt1l,hmt2bb_htb500,hmt2bb_htb2000,hmt2bb_htauNu500);
  restylePlot(hdr_tt1l,hdr_tt2l,hdr_tt1l,hdr_htb500,hdr_htb2000,hdr_htauNu500);

  hdr_tt1l->DrawNormalized("hist");
  hdr_tt2l->DrawNormalized("hist sames");
  hdr_htb500->DrawNormalized("hist sames");
  hdr_htb2000->DrawNormalized("hist sames");
  hdr_htauNu500->DrawNormalized("hist sames");

  return;

  hmt2bb_tt1l->DrawNormalized("hist");
  hmt2bb_tt2l->DrawNormalized("hist sames");
  hmt2bb_htb500->DrawNormalized("hist sames");
  hmt2bb_htb2000->DrawNormalized("hist sames");
  hmt2bb_htauNu500->DrawNormalized("hist sames");

  hmt_tt1l->DrawNormalized("hist");
  hmt_tt2l->DrawNormalized("hist sames");
  hmt_htb500->DrawNormalized("hist sames");
  hmt_htb2000->DrawNormalized("hist sames");
  hmt_htauNu500->DrawNormalized("hist sames");

  hNB_tt1l->DrawNormalized("hist");
  hNB_tt2l->DrawNormalized("hist sames");
  hNB_htb500->DrawNormalized("hist sames");
  hNB_htb2000->DrawNormalized("hist sames");
  hNB_htauNu500->DrawNormalized("hist sames");

}
