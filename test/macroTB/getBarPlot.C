#include "TH1.h"
#include "TH1D.h"
#include "THStack.h"
#include "TFile.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TPad.h"

using namespace std;

void getBarPlot() {

  gStyle->SetOptStat(0);
  //  gROOT->LoadMacro("utils.C++");
  gROOT->LoadMacro("utils.C");

  bool do2l=true;

  TString histoName="";
  TString macroRegion="";
  TString mc="";
  TString legend="";
  TString fileName="MAY4_bin1/1l_3000_workspace_SYST.root";
  if(do2l)  fileName="MAY4_bin1/2L_3000_workspace_SYST.root";

  TString fileName2="MAY4_bin1/1l_500_workspace_SYST.root";
  if(do2l)  fileName2="MAY4_bin1/2L_500_workspace_SYST.root";

  int color=1;
  char *labels1l[9] = {"4j,1b","4j,2b","4j,>2b",  "5j,1b","5j,2b","5j,>2b",  ">=6j,1b",">=6j,2b",">=6j,>2b"};
  char *labels2l[9] = {"2j,1b","2j,2b","2j,>2b",  "3j,1b","3j,2b","3j,>2b",  ">=4j,1b",">=4j,2b",">=4j,>2b"};

  TH1D *hYIELD_sig3000 = new TH1D("h1", "histogram title", 9, 0., 9.);
  TH1D *hYIELD_sig500 = new TH1D("h1", "histogram title", 9, 0., 9.);
  TH1D *hYIELD_data = new TH1D("h1", "histogram title", 9, 0., 9.);
  THStack *hs = new THStack("hs","");
  THStack *hsUp = new THStack("hsUp","");
  THStack *hsDown = new THStack("hsDown","");

  //  std::vector<std::string> systlabel{ "_CMS_pileup","_CMS_res_J","_CMS_topreweight","_CMS_scale_J","_CMS_eff_b_CFerr2","_CMS_eff_b_CFerr1","_CMS_eff_b_HF","_CMS_eff_b_LF","_CMS_eff_b_LFstat2","_CMS_eff_b_LFstat1","_CMS_eff_b_HFstat2","_CMS_eff_b_HFstat1"}; 
  
  std::vector<std::string> systlabel;
  systlabel.push_back("_lumi_13TeV");
  systlabel.push_back("_CMS_scale_j");
  systlabel.push_back("_CMS_pileup");
  systlabel.push_back("_CMS_res_j");
  systlabel.push_back("_CMS_scale_uncluster");
  //###
  systlabel.push_back("_CMS_topreweight");
  systlabel.push_back("_muRF_ttbar");
  systlabel.push_back("_CMS_HPTB_ISR");
  systlabel.push_back("_CMS_HPTB_FSR");
  systlabel.push_back("_CMS_HPTB_HDAMP");
  //### EWK only
  //  CMS_HPTB_mcreweight_ewk
  //  muRF_ewk
  //###
  systlabel.push_back("_CMS_btag_CFerr2");
  systlabel.push_back("_CMS_btag_CFerr1");
  systlabel.push_back("_CMS_btag_HF");
  systlabel.push_back("_CMS_btag_LF");
  systlabel.push_back("_CMS_btag_LFstat2");
  systlabel.push_back("_CMS_btag_LFstat1");
  systlabel.push_back("_CMS_btag_HFstat2");
  systlabel.push_back("_CMS_btag_HFstat1");

  //////////
  //////////
  //////////
  
  TLegend* this_leg = new TLegend(0.4,0.6,0.55,0.89);
  this_leg->SetFillColor(0);
  this_leg->SetBorderSize(0);
  this_leg->SetTextSize(0.04);
  TLegend* this_leg1 = new TLegend(0.55,0.7,0.85,0.89);
  this_leg1->SetFillColor(0);
  this_leg1->SetBorderSize(0);
  this_leg1->SetTextSize(0.05);

  int nLepCat=2;
  //  if(do2l) nLepCat=3;
  if(do2l) nLepCat=1;

  for(int sampleI=0; sampleI<11; sampleI++) {
  //  for(int sampleI=3; sampleI<8; sampleI++) {
    //  for(int sampleI=0; sampleI<11; sampleI++) {
    
    if(sampleI==4) continue;

    if(sampleI==0 && do2l) continue;

    for(int lepFlavor=0; lepFlavor<nLepCat+1; lepFlavor++) {

      if(lepFlavor==0 && !do2l) continue;
      if(lepFlavor==1 && do2l) continue;
      
      for(int region=1; region<13; region++) {
      //      for(int region=4; region<5; region++) {
	//      for(int region=4; region<5; region++) {
	if(region<4) continue;
	if(region==6 && do2l) continue;
	
	//	cout << "region=" << region << " MC flavor=" << sampleI << " lepFlavor=" << lepFlavor << endl;
	if(region==4 || region==5) macroRegion="topCRR";
	if(region==7 || region==10) macroRegion="extraRadCRR";
	if(region==8) macroRegion="Baseline_SR1";
	if(region==9) macroRegion="Baseline_SR2";
	if(region==11) macroRegion="Baseline_SR3";
	if(region==12) macroRegion="Baseline_SR4";
	if(region==6) macroRegion="charmCR";
	
	//https://twiki.cern.ch/twiki/bin/viewauth/CMS/ChargedHiggsTBleptonic#Plots_color_scheme
	
	if(sampleI==0) {  mc="qcd" ; color=kGray; }
	if(sampleI==1) {  mc="top" ; color=kBlue; }
	if(sampleI==2) {  mc="ewk" ; color=kGreen-8; }
	/*
	  if(sampleI==4) {  mc="ttbb" ; color=kRed-3; }
	  if(sampleI==5) {  mc="tt2b" ; color=kRed-3; }
	  if(sampleI==3) {  mc="ttb" ; color=kRed-3; }
	  if(sampleI==6) {  mc="ttc" ; color=kRed-5; }
	  if(sampleI==7) {  mc="ttlf" ; color=kRed-7; }
	*/
	if(sampleI==0) {  mc="_qcd" ; color=kGray; legend="QCD";}
	if(sampleI==1) {  mc="_top" ; color=kBlue; legend="t,tW,tt+X w/ X=Z,W,h,#gamma";}
	if(sampleI==2) {  mc="_ewk" ; color=kGreen-8; legend="V+jets, VV, Vh, VVV";}
	//#
	if(sampleI==3) {  mc="_ttbb" ; color=kRed+3; legend="t#bar{t} + b#bar{b}";}
       	if(sampleI==4) {  mc="_tt2b" ; color=kRed; legend="t#bar{t} + 2B";}
	if(sampleI==5) {  mc="_ttb" ; color=kRed-5; legend="t#bar{t} + 1b";}
	if(sampleI==6) {  mc="_ttcc" ; color=kRed-3; legend="t#bar{t} + c(#bar{c})";}
	if(sampleI==7) {  mc="_ttlf" ; color=kRed-7; legend="t#bar{t} + lf";}
	//if(sampleI==3) {  mc="_ttbb" ; color=kRed+3; legend="t#bar{t} + b#bar{b}";}
	//if(sampleI==4) {  mc="_tt2b" ; color=kRed; legend="t#bar{t} + 2B";}
	//if(sampleI==5) {  mc="_ttb" ; color=kRed-3; legend="t#bar{t} + 1b";}
	//if(sampleI==6) {  mc="_ttcc" ; color=kRed-5; legend="t#bar{t} + c(#bar{c})";}
	//if(sampleI==7) {  mc="_ttlf" ; color=kRed-7; legend="t#bar{t} + lf";}
	//#
	if(sampleI==8) {  mc="_Hptb" ; color=kBlack; }
	if(sampleI==9) {  mc="_Hptb" ; color=kBlack; }
	
	TString pdf="pdf";
	if(sampleI==10) { pdf="data_obs"; mc=""; }

	if(region==4 || region==5 || region==7 || region==10) {
	  if(!do2l && lepFlavor==1) histoName=Form("%s%s_%s%d_1Mu",pdf.Data(),mc.Data(),macroRegion.Data(),region);
	  if(!do2l && lepFlavor==2) histoName=Form("%s%s_%s%d_1Ele",pdf.Data(),mc.Data(),macroRegion.Data(),region);

	  if(do2l && lepFlavor==0) histoName=Form("%s%s_%s%d_2L",pdf.Data(),mc.Data(),macroRegion.Data(),region);
	  //	  if(do2l && lepFlavor==1) histoName=Form("%s%s_%s%d_2Mu",pdf.Data(),mc.Data(),macroRegion.Data(),region);
	  //	  if(do2l && lepFlavor==2) histoName=Form("%s%s_%s%d_2Ele",pdf.Data(),mc.Data(),macroRegion.Data(),region);
	  //	  if(do2l && lepFlavor==3) histoName=Form("%s%s_%s%d_1MuEle",pdf.Data(),mc.Data(),macroRegion.Data(),region);
	} else {
	  if(!do2l && lepFlavor==1) histoName=Form("%s%s_%s_1Mu",pdf.Data(),mc.Data(),macroRegion.Data());
	  if(!do2l && lepFlavor==2) histoName=Form("%s%s_%s_1Ele",pdf.Data(),mc.Data(),macroRegion.Data());

	  if(do2l && lepFlavor==0) histoName=Form("%s%s_%s_2L",pdf.Data(),mc.Data(),macroRegion.Data());
	  //	  if(do2l && lepFlavor==1) histoName=Form("%s%s_%s_2Mu",pdf.Data(),mc.Data(),macroRegion.Data());
	  //	  if(do2l && lepFlavor==2) histoName=Form("%s%s_%s_2Ele",pdf.Data(),mc.Data(),macroRegion.Data());
	  //	  if(do2l && lepFlavor==3) histoName=Form("%s%s_%s_1Mu1Ele",pdf.Data(),mc.Data(),macroRegion.Data());
	}
	

	///// NOMINAL MC

	TH1 * hyield = getHisto(fileName, "0", histoName, color, 1, 1);
	//	if(hyield) cout << " Filling Bin=" << region-3 << " integral = " << hyield->Integral() << " histoName=" << histoName.Data() << endl;
	TH1D *h = new TH1D(Form("h_region%d_sampleMC%d_lepCat%d",region,sampleI,lepFlavor), "histogram title", 9, 0., 9.);
	if(hyield && region==4) cout << "region==4 hyield->Integral()=" << hyield->Integral() << " histoName=" << histoName.Data() << endl;
	//	if(hyield) h->SetBinContent(region-3,hyield->Integral());
	if(hyield) h->SetBinContent(region-3,hyield->Integral());
	if(h && sampleI<8) h->SetFillColor(color);
	if(h && sampleI<8) h->SetLineColor(color);
	if(h && sampleI<8) hs->Add(h);
	if(h && region==4 && !do2l && lepFlavor==1 && sampleI<8 && sampleI>2) this_leg->AddEntry(h, legend,"f");
	if(h && region==4 && !do2l && lepFlavor==1 && sampleI<3) this_leg1->AddEntry(h, legend,"f");
	if(h && region==4 && do2l && lepFlavor==0 && sampleI<8 && sampleI>2) this_leg->AddEntry(h, legend,"f");
	if(h && region==4 && do2l && lepFlavor==0 && sampleI<3) this_leg1->AddEntry(h, legend,"f");

	///// SYST

	for(unsigned int syst=0; syst < systlabel.size();  syst++) {

	  TString histoNameSystU = histoName + systlabel[syst].c_str() + "Up";
	  TString histoNameSystD = histoName + systlabel[syst].c_str() + "Down";

	  TH1 * hyieldSystU = getHisto(fileName, "0", histoNameSystU, color, 1, 1);
	  TH1 * hyieldSystD = getHisto(fileName, "0", histoNameSystD, color, 1, 1);
	  TH1D *hU = new TH1D(Form("h_region%d_sampleMC%d_lepCat%d%s",region,sampleI,lepFlavor,histoNameSystU.Data()), "histogram title", 9, 0., 9.);
	  TH1D *hD = new TH1D(Form("h_region%d_sampleMC%d_lepCat%d%s",region,sampleI,lepFlavor,histoNameSystD.Data()), "histogram title", 9, 0., 9.);

	  if(hyieldSystU) { double deltaUp=(hyieldSystU->Integral() - hyield->Integral());    hU->SetBinContent(region-3, deltaUp*deltaUp); }
	  if(hyieldSystD) { double deltaDown=(hyieldSystD->Integral() - hyield->Integral());   hD->SetBinContent(region-3, deltaDown*deltaDown); }

	  if(hU && (sampleI<3 || sampleI>7) && histoNameSystU.Contains("CMS_HPTB_FSR")) continue;
	  if(hU && (sampleI<3 || sampleI>7) && histoNameSystU.Contains("CMS_HPTB_ISR")) continue;
	  if(hU && (sampleI<3 || sampleI>7) && histoNameSystU.Contains("CMS_HPTB_HDAMP")) continue;
	  if(hU && (sampleI<3 || sampleI>7) && histoNameSystU.Contains("QCDscaleTTbar")) continue;

	  if(hU && sampleI<8) { hU->Scale(3/2); hsUp->Add(hU); }
	  if(hD && sampleI<8) { hD->Scale(3/2); hsDown->Add(hD);}

	  if(region==4 && sampleI<8 ) {
	    cout << "     region==4 histoName=" << histoNameSystU.Data(); // << endl;
	    cout << " hyieldSystU->Integral()=" << hU->Integral();// << endl; 
	    cout << " hyieldSystD->Integral()=" << hD->Integral();// << endl;
	    cout << " hsUp=" << hsUp << " hsDown=" << hsDown << endl;
	  }

	  /*
	  delete hyieldSystU;
	  delete hU;
	  delete hyieldSystD;
	  delete hD;
	  */

	}

	///// DATA

	if(sampleI==8 && hyield) { 
	  TH1D *hYIELD_sig3000_ = new TH1D("h1", "histogram title", 9, 0., 9.);
	  if(hyield) hYIELD_sig3000_->SetBinContent(region-3,hyield->Integral()); 
	  if(hyield) hYIELD_sig3000->Add(hYIELD_sig3000_);
	}

	if(sampleI==9) {
	  TH1D *hYIELD_sig500_ = new TH1D("h1", "histogram title", 9, 0., 9.);
	  TH1 * hyield2 = getHisto(fileName2, "0", histoName, color, 1, 1);
	  if(hyield2) hYIELD_sig500_->SetBinContent(region-3,hyield2->Integral());
	  if(hyield2) hYIELD_sig500->Add(hYIELD_sig500_);
	}
	if(sampleI==10) {

	  cout << "DOING DATA sampleI=" << sampleI << " histoName=" << histoName << " region=" << region << endl;
	  cout << "DOING DATA sampleI=" << sampleI << " histoName=" << histoName << " region=" << region <<endl;
	  cout << "DOING DATA sampleI=" << sampleI << " histoName=" << histoName << " region=" << region <<endl;

	  TH1D *hYIELD_data_ = new TH1D("h1", "histogram title", 9, 0., 9.);
	  TH1 * hyield3 = getHisto(fileName, "0", histoName, color, 1, 1);
	  
	  // blind for now
	  //	  if(region==4 || region==5 || region==6 || region==7 || region==10) {
	  if(region==4 || region==5 || region==7 || region==10) {
	    
	    if(hyield3) hYIELD_data_->SetBinContent(region-3,hyield3->Integral());
	    if(hyield3) hYIELD_data_->Sumw2();
	    if(hyield3) hYIELD_data->Add(hYIELD_data_);
	  }
	}
      }
    }

  }

  /////
  /////
  ///// histogram builded, now only drawing
  /////
  /////
  
  TCanvas *c = new TCanvas("","",0,0,800,800);
  c->cd();
  const double ydiv = 0.25;
  TPad *pad1 = new TPad("pad1", "",0.05,ydiv,1,1);
  //  pad1->SetLogy();

  pad1->SetTickx(1);
  pad1->SetTicky(1);
  pad1->SetBottomMargin(0.05);
  pad1->SetTopMargin(0.075);
  pad1->Draw();

  TPad *pad2 = new TPad("pad2", "",0.05,0,1,ydiv);
  pad2->SetTickx(1);
  pad2->SetTicky(1);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.35);
  pad2->Draw();

  /////
  ///// 
  pad1->cd();

  hs->SetMaximum(1.4*hs->GetMaximum());
  hs->Draw();
  //  hs->GetYaxis()->SetNdivisions(505);
  //  hs->SetMaxDigits(3);
  if(do2l) TGaxis::SetMaxDigits(3);
  hs->GetXaxis()->SetLabelOffset(999);
  hs->GetXaxis()->SetLabelSize(0);
  hs->GetYaxis()->SetTitle("Events");
  hs->GetYaxis()->SetTitleSize(0.06);
  hs->GetYaxis()->SetTitleOffset(1.2);
  hs->GetYaxis()->SetLabelSize(0.06);

  pad1->RedrawAxis();
  c->Modified();
  hYIELD_sig3000->SetLineColor(kBlack);
  hYIELD_sig3000->SetLineWidth(3);
  hYIELD_sig3000->Scale(100);
  hYIELD_sig3000->Draw("hist same");
  hYIELD_sig500->SetLineColor(kBlack);
  hYIELD_sig500->SetLineStyle(kDashed);
  hYIELD_sig500->SetLineWidth(3);
  hYIELD_sig500->Scale(100);
  hYIELD_sig500->Draw("hist same");
  hYIELD_data->SetMarkerStyle(20);
  hYIELD_data->Draw("p e same");
  this_leg1->SetTextSize(0.03);
  this_leg1->AddEntry(hYIELD_sig3000, "H^{#pm}(3TeV)=1pb^{-1}x100","l");
  this_leg1->AddEntry(hYIELD_sig500, "H^{#pm}(500GeV)=1pb^{-1}x100","l");

  this_leg->Draw("");
  this_leg1->Draw("");

  TLatex *latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.04);
  latex->SetTextColor(1);
  latex->SetTextFont(42);
  latex->SetTextAlign(33);
  latex->DrawLatex(0.9, 0.98, "35.9 fb^{-1}  (13 TeV)");
  latex->SetTextFont(62);
  latex->DrawLatex(0.22, 0.88, "CMS");
  latex->SetTextSize(0.04);
  latex->SetTextFont(52);
  latex->DrawLatex(0.30, 0.83, "Preliminary");

  latex->SetTextFont(72);
  latex->SetTextSize(0.035);
  if(!do2l) latex->DrawLatex(0.30, 0.78, "single-lepton");
  if(do2l) latex->DrawLatex(0.30, 0.78, "di-lepton");
  //  latex.SetTextAlign(22)
  //  latex.DrawLatex(0.5, 0.85, text)

  pad2->cd();

  TH1 *mcTOT = ((TH1 *)(hs->GetStack()->Last())); // the "SUM"                                                                                                              
  TH1F *hdataRatio = (TH1F*) hYIELD_data->Clone();
  hdataRatio->Divide(mcTOT);
  //  hdataRatio->GetXaxis()->SetNdivisions(414);
  hdataRatio->GetYaxis()->SetLabelSize(0.085*1.5);
  hdataRatio->GetYaxis()->SetTitleSize(0.085*1.7);
  hdataRatio->GetYaxis()->SetRangeUser(0.8,1.2);
  hdataRatio->GetYaxis()->SetNdivisions(-502);
  hdataRatio->GetYaxis()->SetTitle("data/MC");
  for(int region=4; region<13; region++)   if(!do2l) hdataRatio->GetXaxis()->SetBinLabel(region-3,labels1l[region-3-1]);
  for(int region=4; region<13; region++)   if(do2l) hdataRatio->GetXaxis()->SetBinLabel(region-3,labels2l[region-3-1]);
  hdataRatio->GetXaxis()->SetLabelSize(0.085*1.7);
  hdataRatio->GetYaxis()->SetTitleOffset(0.45);
  hdataRatio->SetTitle("");
  hdataRatio->Draw("p e");

  if(hsUp && hsDown) {
    TH1 *mcD = ((TH1 *)(hsDown->GetStack()->Last())); // the "SUM" in quadrature of the delta errors (xUp - xCentral)^2 
    TH1 *mcU = ((TH1 *)(hsUp->GetStack()->Last())); // the "SUM" in quadrature of the delta errors (xUp - xCentral)^2    
    for( unsigned int bin=1; bin<mcTOT->GetNbinsX(); bin++) {
      if(mcTOT->GetBinContent(bin)!=0) mcU->SetBinContent(bin, ( (mcTOT->GetBinContent(bin) + sqrt(0.1*0.1 + mcU->GetBinContent(bin)))/mcTOT->GetBinContent(bin)));
      if(mcTOT->GetBinContent(bin)!=0) mcD->SetBinContent(bin, ( (mcTOT->GetBinContent(bin) - sqrt(0.1*0.1 + mcD->GetBinContent(bin)))/mcTOT->GetBinContent(bin)));
    }
    
    if(mcU) cout << " mcU->Integral()=" << mcU->Integral() << " mcD->Integral()=" << mcD->Integral() << " central=" << mcTOT->Integral() << endl;
    //  if(mcU) mcU->SetLineColor(kRed);
    //  if(mcD) mcD->SetLineColor(kBlue);
    //  if(mcU) mcU->Divide(mcTOT);
    //  if(mcD) mcD->Divide(mcTOT);
    if(mcU) mcU->SetFillColor(kGray);
    //    if(mcU) mcU->SetFillStyle(3010);
    if(mcD) mcD->SetFillColor(10);
    //    if(mcD) mcD->SetFillStyle(1001);
    
    if(mcU) mcU->Draw("hist sames");
    if(mcD) mcD->Draw("hist sames");
    hdataRatio->Draw("p e sames");
    
  }

  pad2->RedrawAxis();

  //  getFillColor(kGray+3);
  //  toterree->SetLineColor(kGray+3);
  //  toterree->SetFillStyle(3010);
  //  toterree->Draw("2 same");
  //  toterree->Draw("p");

  TLine *lineZero = new TLine(mcTOT->GetXaxis()->GetXmin(), 1.,  mcTOT->GetXaxis()->GetXmax(), 1.);
  lineZero->SetLineColor(11);
  lineZero->Draw("same");

  if(!do2l) c->SaveAs("Counting_1l.png");
  if(do2l) c->SaveAs("Counting_2l.png");


}
