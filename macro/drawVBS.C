#include "TROOT.h"
//#include "utils.C"

TString fileHAD = "HAD.root";
TString fileBHAD = "BHAD.root";
TString fileMET = "MET.root";

void compareSRCR(TString histoName="", TString dirName="") {

  gROOT->LoadMacro("utils.C+");
  gStyle->SetOptStat(0);

  TCanvas * c1 = new TCanvas(histoName, histoName,800,800);

  int rebin=1;

  TString fileName1 = "HAD.root";
  TString fileName2 = "HADanti.root";
  TString categoryName = "_BB";
  TString sampleName = "_QCD_HT";

  cout << "dirName = " << dirName;
  cout << " histoName = " << histoName;
  cout << " categoryName = " << categoryName;
  cout << " sampleName = " << sampleName << endl;

  TH1 * h1=getHisto(fileName1, dirName, histoName+categoryName+sampleName, kBlack, 1,rebin);
  TH1 * h2=getHisto(fileName2, dirName, histoName+categoryName+sampleName, kRed, 1,rebin);

  //  auto ha = new TH1F ("h1","Histogram empty",100, 50., 150.);
  //  if(h1) h1->GetYaxis()->SetRangeUser(0.,0.3);
  if(h1) h1->SetTitle("");
  //  if(h1) h1->GetXaxis()->SetTitle("| cos #Theta^{*} | ");

  if(h1) h1->DrawNormalized("hist");
  if(h2) h2->DrawNormalized("hist sames");

  TLegend* this_leg = new TLegend(0.55,0.5,0.8,0.7);
  this_leg->SetFillColor(0);
  this_leg->SetBorderSize(0);
  this_leg->SetTextSize(0.035);

  cout << " h1 " << h1->Integral() << " h2 " << h2->Integral() << endl;

  this_leg->AddEntry(h1,Form("SR %.2f",h1->Integral()),"l"); 
  this_leg->AddEntry(h2,Form("CR %.2f",h2->Integral()),"l"); 
  this_leg->Draw("");

  c1->SaveAs(Form("COMPARE/compare_%s_%s_%s.png",histoName.Data(),categoryName.Data(),sampleName.Data()));

}


void comparePlot(TString histoName="", TString dirName="") {

  gROOT->LoadMacro("utils.C+");
  gStyle->SetOptStat(0);

  TCanvas * c1 = new TCanvas(histoName, histoName,800,800);

  cout << " dirName = " << dirName << " histoName " << histoName << endl;

  int rebin=5;
  if(histoName.Contains("CosThetaStar")) rebin=10;
  if(histoName.Contains("SDMass")) rebin=1;

  TH1 * hLL=getHisto ("HAD.root", dirName, histoName+"WWjj_SS_ll", kBlack, 1,rebin);
  TH1 * hLT=getHisto ("HAD.root", dirName, histoName+"WWjj_SS_lt", kRed, 1,rebin);
  TH1 * hTT=getHisto ("HAD.root", dirName, histoName+"WWjj_SS_tt", kBlue, 1,rebin);

  if(histoName.Contains("CosThetaStar")) hTT->GetXaxis()->SetRangeUser(0.,1.);

  //  auto h1 = new TH1F ("h1","Histogram empty",100, 0., 1.);
  auto h1 = new TH1F ("h1","Histogram empty",100, 50., 150.);

  h1->GetYaxis()->SetRangeUser(0.,0.3);
  if(histoName.Contains("BosonDecayRatio_")) h1->SetMaximum(0.05);
  if(histoName.Contains("CosThetaStar")) h1->SetMaximum(0.15);
  if(histoName.Contains("SDMass")) h1->SetMaximum(0.1);

  //  if(h) h->Rebin(2);
  //    h->SetTitle(Form("%s %s",sampleLabel->Data(),dirName->Data()));
  //  if(h1) h1->SetTitle("decay products of polarized boson - gen Level");
  if(h1) h1->SetTitle("");
  if(h1) h1->GetYaxis()->SetTitleOffset(1.4);
  if(h1) {
    if(histoName.Contains("BosonDecayRatio_")) {
      h1->GetXaxis()->SetTitle("PT(trailing Jet)/PT(leading Jet)");
    } else if(histoName.Contains("WvsQCD_FatJet")) {
      h1->GetXaxis()->SetTitle("discr(WvsQCD) MD");
    } else if(histoName.Contains("CosThetaStar")) {
      h1->GetXaxis()->SetTitle("| cos #Theta^{*} | ");
    }
  }
  if(h1) h1->Draw("hist");
  if(hTT) hTT->DrawNormalized("hist sames");
  if(hLT) hLT->DrawNormalized("hist sames");
  if(hLL) hLL->DrawNormalized("hist sames");

  TLegend* this_leg = new TLegend(0.55,0.65-0.5,0.8,0.8-0.5);
  this_leg->SetFillColor(0);
  this_leg->SetBorderSize(0);
  this_leg->SetTextSize(0.035);

  this_leg->AddEntry(hTT,"TT","l"); 
  this_leg->AddEntry(hLL,"LL","l"); 
  this_leg->AddEntry(hLT,"LT","l"); 

  this_leg->Draw("");

  c1->SaveAs(Form("polarization_%s.png",histoName.Data()));

}

void drawOnePlot(TString histoName="", TString dirName="", TString sampleName="") {

  gROOT->LoadMacro("utils.C+");
  gStyle->SetOptStat(0);

  TCanvas * c1 = new TCanvas(histoName, histoName,800,800);

  cout << " dirName = " << dirName << " histoName " << histoName << " sampleName " << sampleName<< endl;

  TH1 * h=getHisto ("HAD_signal.root", dirName, histoName+sampleName, 1, 1,1);

  if(h) h->Rebin(2);
  //    h->SetTitle(Form("%s %s",sampleLabel->Data(),dirName->Data()));
  if(h) h->SetTitle("");
  if(h) h->GetYaxis()->SetTitleOffset(1.4);

  if(h) h->GetYaxis()->SetRangeUser(0.,0.0002);
  if(h) h->Draw("hist");

  TLegend* this_leg = new TLegend(0.55,0.65,0.8,0.8);
  this_leg->SetFillColor(0);
  this_leg->SetBorderSize(0);
  this_leg->SetTextSize(0.035);

  this_leg->AddEntry(h,"W^{+/-} W^{+/-} VBF","l"); 

  this_leg->Draw("");

  c1->SaveAs(Form("shape_%s_%s.png",histoName.Data(),sampleName.Data()));

}

void drawPlot(TString histoName="", TString dirName="", TString categoryName="", int signal=0) {

  double lumi=137;
  //  bool doNorm=true;

  TString sampleName=histoName;
  TCanvas * c1 = new TCanvas(histoName, histoName,800,800);

  //  TCanvas *cst = new TCanvas(histoName,dirName+"_"+histoName,800,800);

  const double ydiv = 0.25;
  TPad *pad1 = new TPad("pad1", "",0.05,ydiv,1,1);

  //  pad1->SetLogx(1);
  pad1->SetLogy(1);
  //  pad1->SetLogz(logz);

  pad1->SetTickx(1);
  pad1->SetTicky(1);
  pad1->SetBottomMargin(0.05);
  pad1->SetTopMargin(0.075);
  pad1->Draw();

  TPad *pad2 = new TPad("pad2", "",0.05,0,1,ydiv);
  //  pad2->SetLogx(logx);
  //  pad1->SetLogx(1);
  pad2->SetTickx(1);
  pad2->SetTicky(1);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.35);
  pad2->Draw();

  TString fileName=fileHAD;
  if(categoryName.Contains("BMET") || categoryName.Contains("RMET")) fileName=fileMET;
  if(categoryName.Contains("tag")) fileName=fileBHAD;

  if(histoName.Contains("pT_BJet") || histoName.Contains("WvsQCD") || histoName.Contains("ZHbbvsQCD")) fileName=fileBHAD;
  if(histoName.Contains("SDMass") && signal==1) fileName=fileHAD;
  if(histoName.Contains("SDMass") && signal==3) fileName=fileBHAD;

  if(histoName.Contains("WvsQCD") || histoName.Contains("ZHbbvsQCD")) pad1->SetLogy(0);

  sampleName="";

  TLegend* this_leg = new TLegend(0.65,0.65,0.9,0.9);
  this_leg->SetFillColor(0);
  this_leg->SetBorderSize(0);
  this_leg->SetTextSize(0.035);

  THStack *hs = new THStack("hs",Form("%s",histoName.Data()));

  int color =1;

  int rebin=5;
  if(histoName.Contains("SDMass")) { 
    rebin=2;
    pad1->SetLogy(0);
  }
  if(histoName.Contains("MVV")) { 
    rebin=10;
  }

  if(histoName.Contains("BDTnoBnoMET")) { 
    rebin=20;
    pad1->SetLogy(0);
  }

  TString sigName="_WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8";  
  if(categoryName.Contains("BMET") || categoryName.Contains("RMET")) sigName="_ZnnZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8";
  if(categoryName.Contains("tag") || signal==3) sigName="_ZbbZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8";

  cout << " fileName = " << fileName << " dirName = " << dirName << " histoName " << histoName << " categoryName " << categoryName << " sampleName " << sigName<< endl;

  TH1 * hSig=getHisto (fileName, dirName, histoName+categoryName+sigName, kBlack, 1,rebin);
  cout << "hSig->Integral()" <<  hSig->Integral() << endl;

  if(hSig) {
    hSig->Scale(lumi*1000);
    if(sigName.Contains("WPhadWPhadJJ")) hSig->Scale(2);
    //    if(!sigName.Contains("SDMass"))  hSig->Rebin(rebin);
  }

  TH1 * hSig2=getHisto (fileName, dirName, histoName+categoryName+"_DoublyChargedHiggsGMmodel_HWW_M1500_13TeV-madgraph", kBlack, 1,rebin);

  if(hSig2) {
    hSig2->Scale(lumi*1000);
    //    hSig2->Rebin(rebin);
  }

  TString sampleLabel="";

  for(int sampleI=1; sampleI<8+1; sampleI++) {

    if(sampleI==1) { sampleName="_TRIBOSON"; sampleLabel="VVV"; color=kGreen+1; } ;
    if(sampleI==2) { sampleName="_DIBOSON"; sampleLabel="VV"; color=kGreen-7; } ;
    if(sampleI==3) { sampleName="_ZJetsToNuNu"; sampleLabel="Zinv"; color=kOrange+1; } ;
    if(sampleI==4) { sampleName="_WJetsToLNu"; sampleLabel="W+j"; color=kOrange+7; } ;
    if(sampleI==5) { sampleName="_TT_TuneCUETP8M2T4"; sampleLabel="tt"; color=kBlue-3; } ;
    //    if(sampleI==4) { sampleName="_TT"; sampleLabel="tt"; color=kBlue-3; } ;
    if(sampleI==6) { sampleName="_TTX"; sampleLabel="ttV"; color=kCyan; } ;
    if(sampleI==7) { sampleName="_ST"; sampleLabel="t"; color=kBlue-10; } ;
    if(sampleI==8) { sampleName="_QCD_HT"; sampleLabel="QCD"; color=kRed; } ;

    cout << " fileName = " << fileName << " dirName = " << dirName << " histoName " << histoName << " categoryName " << categoryName << " sampleName " << sampleName<< endl;

    TH1 * h=getHisto (fileName, dirName, histoName+categoryName+sampleName, color, 1,rebin);
    //    if(h) h->GetXaxis()->SetRangeUser(0.,2000.);
    //    if(h) h->Rebin(rebin);
    if(h) h->Scale(lumi*1000);
    //    h->SetTitle(Form("%s %s",sampleLabel->Data(),dirName->Data()));
    if(h) h->SetTitle(sampleLabel);
    if(h) h->SetFillColor(color);

    if(h) cout << sampleLabel << " Integral = " << h->Integral() << endl;

    if(h) hs->Add(h);
    if(h) this_leg->AddEntry(h, sampleLabel , "f");

  }

  pad1->cd();

  if(hSig && histoName.Contains("BDT") ) hSig->Scale(100);
  if(hSig) hSig->GetYaxis()->SetTitleOffset(1.5);
  if(hSig && !histoName.Contains("BDT") ) hSig->SetMaximum(10000000);
  if(hSig && histoName.Contains("BDT") ) hSig->SetMaximum(1000);
  if(hSig) hSig->SetMinimum(0.1);
  if(hSig) hSig->SetLineWidth(3);
  if(hSig) hSig->Draw("hist");
  if(hs) hs->Draw("hist sames");
  if(hSig) hSig->Draw("hist same ");
  if(hSig2) hSig2->SetLineColor(kGray);
  if(hSig2) hSig2->SetLineWidth(6);
  //  if(hSig2) hSig2->Draw("hist same ");

  if(histoName.Contains("SDMass") || histoName.Contains("pT_FatJet") || histoName.Contains("pT_BJet") || histoName.Contains("WvsQCD") || histoName.Contains("ZHbbvsQCD")) {
    TH1F *mcTOT = new TH1F(*((TH1F *)(hs->GetStack()->Last())));
    mcTOT->Scale(1./mcTOT->Integral());
    mcTOT->SetFillColor(11);
    mcTOT->SetLineColor(11);
    mcTOT->SetMaximum(1.);
    if(histoName.Contains("SDMass")) mcTOT->SetMaximum(0.3);
    mcTOT->SetTitle("");
    mcTOT->GetYaxis()->SetTitleOffset(1.2);
    mcTOT->GetYaxis()->SetTitle("normalized events");
    mcTOT->Draw("hist");
    //    hs->Draw("hist");
    if(hSig) hSig->DrawNormalized("hist sames");
    //    if(hSig2) hSig2->DrawNormalized("hist sames");
  }

  if(hSig) {
    if(categoryName.Contains("BMET") || categoryName.Contains("RMET") ) { 
      this_leg->AddEntry(hSig,"Z#nu#nu Zqq VBF","l"); }
    else if (categoryName.Contains("tag") || signal==3) { this_leg->AddEntry(hSig,"Zbb Zqq VBF","l"); }
    else { 
      if(histoName.Contains("BDT"))  this_leg->AddEntry(hSig,"100 x W^{+/-} W^{+/-} VBF","l");
      if(!histoName.Contains("BDT"))  this_leg->AddEntry(hSig,"W^{+/-} W^{+/-} VBF","l");
    }
  }

  //  if(hSig2) this_leg->AddEntry(hSig2,"H^{++} #rightarrow W^{+}W^{+}","l");

  this_leg->Draw("");
  //  pad1->BuildLegend();

  if(histoName.Contains("SDMass")) {
    int expM=80;
    if(signal==3) expM=91;
    TLine *lineZero = new TLine(expM, 0., expM, 0.3);
    lineZero->SetLineColor(1);
    lineZero->SetLineStyle(3);
    lineZero->Draw("same");
  }

  TLatex latexLabel;
  latexLabel.SetNDC();
  latexLabel.SetTextSize(0.04);
  latexLabel.SetTextColor(1);
  latexLabel.SetTextFont(42);
  latexLabel.SetTextAlign(33);
  latexLabel.DrawLatex(0.9, 0.98, "137 fb^{-1}  (13 TeV)");
  latexLabel.SetTextFont(62);
  latexLabel.DrawLatex(0.22, 0.88, "CMS");
  latexLabel.SetTextSize(0.04);
  latexLabel.SetTextFont(52);
  latexLabel.DrawLatex(0.30, 0.83, "Preliminary");

  gPad->RedrawAxis();
  c1->Update();

  //  pad2->cd();
  /*
  TH1 *mcTOT = ((TH1 *)(hs->GetStack()->Last())); // the "SUM"   
  TH1F *ratio = (TH1F *) hdata->Clone("dataratio");   
  ratio->Divide(mcTOT);
  //  ratio->GetXaxis()->SetLog(1);
  ratio->GetYaxis()->SetNdivisions(-502);
  ratio->GetYaxis()->SetTitle("data/MC");
  ratio->GetYaxis()->SetRangeUser(0.5,1.5);
  //
  ratio->GetXaxis()->SetTitleSize(0.15);
  ratio->GetXaxis()->SetLabelSize(0.15);

  ratio->GetYaxis()->SetTitleOffset(0.3);
  ratio->GetYaxis()->SetTitleSize(0.15);
  ratio->GetYaxis()->SetLabelSize(0.1);
  //
  ratio->Draw("pe");

  TLine *lineZero = new TLine(mcTOT->GetXaxis()->GetXmin(), 1.,  mcTOT->GetXaxis()->GetXmax(), 1.);
  lineZero->SetLineColor(11);
  lineZero->Draw("same");
  */

  if(fileName.Contains("anti")) {
    c1->SaveAs(Form("SUMPLOT/shapeAnti_%s_%s.png",histoName.Data(),categoryName.Data()));
    c1->SaveAs(Form("SUMPLOT/shapeAnti_%s_%s.pdf",histoName.Data(),categoryName.Data()));
  } else { 
    c1->SaveAs(Form("SUMPLOT/shape_%s_%s.png",histoName.Data(),categoryName.Data()));
    c1->SaveAs(Form("SUMPLOT/shape_%s_%s.pdf",histoName.Data(),categoryName.Data()));
  }

  if(histoName.Contains("SDMass")) {
    if(signal==3) c1->SaveAs(Form("SUMPLOT/shape_%s_%s_ZZ.png",histoName.Data(),categoryName.Data()));
    if(signal==3) c1->SaveAs(Form("SUMPLOT/shape_%s_%s_ZZ.pdf",histoName.Data(),categoryName.Data()));
    if(signal==1) c1->SaveAs(Form("SUMPLOT/shape_%s_%s_WW.png",histoName.Data(),categoryName.Data()));
    if(signal==1) c1->SaveAs(Form("SUMPLOT/shape_%s_%s_WW.pdf",histoName.Data(),categoryName.Data()));
  } else {
    c1->SaveAs(Form("SUMPLOT/shape_%s_%s.png",histoName.Data(),categoryName.Data()));
    c1->SaveAs(Form("SUMPLOT/shape_%s_%s.pdf",histoName.Data(),categoryName.Data()));
  }

}



void compareRes() {

  gROOT->LoadMacro("utils.C+");
  gStyle->SetOptStat(0);

  TString fileName="HAD.root";
  TString fileName2="MET.root";
  TString fileName3="BHAD.root";
  TString dirName="VBShadAnalysis";

  TString histoName="MVVres";

  TString sigName="_WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8";  
  TString sigName2="_ZnnZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8";
  TString sigName3="_ZbbZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8";

  TH1 * hSigBB=getHisto (fileName, dirName, histoName+"_BB"+sigName, kBlack, 1,1);
  TH1 * hSigRB=getHisto (fileName, dirName, histoName+"_RB"+sigName, kRed, 1,1);
  TH1 * hSigBMET=getHisto (fileName2, dirName, histoName+"_BMET"+sigName2, kBlue, 1,1);
  TH1 * hSigRMET=getHisto (fileName2, dirName, histoName+"_RMET"+sigName2, kCyan, 1,1);
  TH1 * hSigBBtag=getHisto (fileName3, dirName, histoName+"_BBtag"+sigName3, kGreen+1, 1,1);

  TCanvas * c1 = new TCanvas(histoName, histoName,800,800);
  hSigBB->GetYaxis()->SetTitleOffset(1.5);
  hSigBB->GetXaxis()->SetRangeUser(-2,5.);

  hSigBB->SetTitle("");
  hSigBB->DrawNormalized("hist");
  //  hSigRB->DrawNormalized("hist sames");
  hSigBMET->DrawNormalized("hist sames");
  hSigRMET->DrawNormalized("hist sames");
  //  hSigBBtag->DrawNormalized("hist sames");

  TLegend* this_leg = new TLegend(0.7,0.65,0.9,0.9);
  this_leg->SetFillColor(0);
  this_leg->SetBorderSize(0);
  this_leg->SetTextSize(0.035);
  this_leg->AddEntry(hSigBB, "BB" , "l");
  this_leg->AddEntry(hSigRB, "RB" , "l");
  this_leg->AddEntry(hSigBMET, "BMET" , "l");
  this_leg->AddEntry(hSigRMET, "RMET" , "l");
  this_leg->AddEntry(hSigBBtag, "BBtag" , "l");
  //  this_leg->AddEntry(hSigRBtag, "RBtag" , "l");
  this_leg->Draw("");

  c1->SaveAs(Form("shape_%s.png",histoName.Data()));
  c1->SaveAs(Form("shape_%s.pdf",histoName.Data()));

}

void drawVBS() {

  gROOT->LoadMacro("utils.C");
  gStyle->SetOptStat(0);

  gStyle->SetOptStat(000000);
  gStyle->SetTextSize(0.5);
  gStyle->SetPaintTextFormat("4.0f");

  /*
  comparePlot("SDMass_FatJet_", "VBShadAnalysis/Baseline");
  //  comparePlot("WvsQCD_FatJet_", "VBShadAnalysis/Baseline");
  //  comparePlot("BosonDecayRatio_", "VBShadAnalysis/BOSON");
  //  comparePlot("CosThetaStar_", "VBShadAnalysis/BOSON");

  return;
  */

  //  compareSRCR("MVV","VBShadAnalysis");
  //  compareSRCR("Mjj","VBShadAnalysis/FWJETS");

  //  return;

  //  compareRes();
  //  return;

  /*
  //  drawPlot("SDMass_FatJet","VBShadAnalysis/Baseline","",1);
  drawPlot("SDMass_FatJet","VBShadAnalysis/Baseline","",3);
  drawPlot("ZHbbvsQCD_FatJet","VBShadAnalysis/Baseline","",3);
  drawPlot("ZHbbvsQCD_FatJetFake","VBShadAnalysis/Baseline","",3);

  //  return;

  drawPlot("pT_BJet","VBShadAnalysis/Baseline","",3);
  drawPlot("pT_FatJet","VBShadAnalysis/Baseline","",3);

  drawPlot("WvsQCD_FatJetFake","VBShadAnalysis/Baseline","",1);

  drawPlot("WvsQCD_FatJet","VBShadAnalysis/Baseline","",1);

  //  return;
  */

  drawPlot("Mjj","VBShadAnalysis/FWJETS","_BB");
  drawPlot("Mjj","VBShadAnalysis/FWJETS","_RB");
  drawPlot("Mjj","VBShadAnalysis/FWJETS","_BBtag");
  drawPlot("Mjj","VBShadAnalysis/FWJETS","_RBtag");
  drawPlot("Mjj","VBShadAnalysis/FWJETS","_RMET");
  drawPlot("Mjj","VBShadAnalysis/FWJETS","_BMET");

  drawPlot("MVV","VBShadAnalysis","_BB");
  drawPlot("MVV","VBShadAnalysis","_RB");
  drawPlot("MVV","VBShadAnalysis","_BBtag");
  drawPlot("MVV","VBShadAnalysis","_RBtag");
  drawPlot("MVV","VBShadAnalysis","_RMET");
  drawPlot("MVV","VBShadAnalysis","_BMET");

  //  drawPlot("Mjj","VBShadAnalysis/FWJETS","_BB");
  //  drawPlot("MVV","VBShadAnalysis","_BB");
  drawPlot("BDTnoBnoMET","VBShadAnalysis","_BB");

  /*
  drawPlot("MVV","VBShadAnalysis/IN1500","_BB");
  drawPlot("Mjj","VBShadAnalysis/IN1500","_BB");
  */

  /*
  drawOnePlot("SDMass_FatJet_", "VBShadAnalysis/Baseline","WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8");
  drawOnePlot("ResBosonMass_", "VBShadAnalysis/Baseline","WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8");
  */

}
