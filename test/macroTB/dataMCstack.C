#include "TROOT.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TString.h"

double lumi=12.9;
//double lumi=36.2;
double eff=1.;
//12878700468.905

TCanvas *dataMCstackPlot( TString histoName="",TString dirName="", bool doLog=false, int LepCat=0, TString dirPlot="") {

  //  bool do1l=true;
  TString sampleName="";
  TString sampleLabel="";
  //  TString histoName="";
  //  TString dirName="";

  TString fileName="NOV28/1l.root";
  if(LepCat!=0) fileName="NOV28/2l.root";
  int color =1;

  TCanvas *cst = new TCanvas(histoName,dirName+"_"+histoName,10,10,700,700);

  //////
  //////
  //////
  //////
  ///// get Ratio PAD

  const double ydiv = 0.25;
  TPad *pad1 = new TPad("pad1", "",0.05,ydiv,1,1);
  //  pad1->SetLogx(logx);
  //  pad1->SetLogy(logy);
  //  pad1->SetLogz(logz);

  pad1->SetTickx(1);
  pad1->SetTicky(1);
  pad1->SetBottomMargin(0);
  pad1->SetTopMargin(0.075);
  pad1->Draw();

  TPad *pad2 = new TPad("pad2", "",0.05,0,1,ydiv);
  //  pad2->SetLogx(logx);                                                                                                                                              
  pad2->SetTickx(1);
  pad2->SetTicky(1);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(0.35);
  pad2->Draw();

  //////
  //////
  //////
  //////

  pad1->cd();
  if(doLog) gPad->SetLogy(1);

  sampleName="Data"; sampleLabel="Data";  color=kBlack;  
  TH1 * hdata=getHisto (fileName, dirName, histoName+sampleName, color, 1,1);
  //  if(do1l) hdata->SetTitle("1l selection");
  //  if(!do1l) hdata->SetTitle("2l selection");

  if(histoName.Contains("CutFlow")) hdata->GetYaxis()->SetRangeUser(1000,1000000000);
  if(!histoName.Contains("CutFlow") && !doLog) hdata->GetYaxis()->SetRangeUser(0.,hdata->GetMaximum()*2);
  if(!histoName.Contains("CutFlow") && doLog) hdata->GetYaxis()->SetRangeUser(0.1,hdata->GetMaximum()*10);
  hdata->SetMarkerStyle(20);
  if(!dirName.Contains("Baseline")) hdata->Draw("p e");
  hdata->SetDirectory(0);


  sampleName="ChargedHiggs_HplusTB_HplusToTB_M-500_13TeV_amcatnlo_pythia8";  sampleLabel="ChargedHiggs_HplusTB_M-500"; color=kGray+2;
  TH1 * h500=getHisto (fileName, dirName, histoName+sampleName, color, 1,1);
  if(h500) h500->Scale(10000000);
  if(h500)h500->SetDirectory(0);
  sampleName="ChargedHiggs_HplusTB_HplusToTB_M-1000_13TeV_amcatnlo_pythia8";  sampleLabel="ChargedHiggs_HplusTB_M-1000"; color=kGreen+1;
  TH1 * h1000=getHisto (fileName, dirName, histoName+sampleName, color, 1,1);
  if(h1000) h1000->Scale(100000000);
  if(h1000) h1000->SetDirectory(0);


  /////
  /////
  /////

  THStack *hs = new THStack("hs",Form("",histoName.Data()));

  TLegend* this_leg = new TLegend(0.75,0.6,0.5+0.5,0.5+0.5);
  this_leg->SetFillColor(0);
  //  this_leg->SetFillStyle(0);
  this_leg->SetBorderSize(0);

  for(int sampleI=0; sampleI<24; sampleI++) {

    if(sampleI==0) { sampleName="TTJets_DiLept_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"; sampleLabel="TTJets_DiLept";  color=kRed;}   
    if(sampleI==1) { sampleName="TTJets_SingleLeptFromT_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"; sampleLabel="TTJets_SingleLeptFromT"; color=kBlue;} 
    if(sampleI==2) { sampleName="TTJets_SingleLeptFromTbar_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"; sampleLabel="TTJets_SingleLeptFromTbar"; color=kBlue;}  
    if(sampleI==3) { sampleName="ST_t-channel_antitop"; sampleLabel="ST_t-channel_antitop"; color=kMagenta+1;}
    if(sampleI==4) { sampleName="ST_t-channel_top"; sampleLabel="ST_t-channel_top"; color=kMagenta;}
    if(sampleI==5) { sampleName="ST_tW_antitop"; sampleLabel="ST_tW_antitop"; color=kMagenta-9;}
    if(sampleI==6) { sampleName="ST_tW_top"; sampleLabel="ST_tW_top"; color=kMagenta-10;}

    if(sampleI==7) { sampleName="WWTo2L2Nu"; sampleLabel="WWTo2L2Nu"; color=kOrange;}
    if(sampleI==8) { sampleName="WWToLNuQQ"; sampleLabel="WWToLNuQQ"; color=kOrange;}
    if(sampleI==9) { sampleName="WZTo1L1Nu2Q"; sampleLabel="WZTo1L1Nu2Q"; color=kOrange;}
    if(sampleI==10) { sampleName="WZTo1L3Nu"; sampleLabel="WZTo1L3Nu"; color=kOrange;}
    if(sampleI==11) { sampleName="WZTo2L2Q"; sampleLabel="WZTo2L2Q"; color=kOrange;}
    if(sampleI==12) { sampleName="WZTo3LNu"; sampleLabel="WZTo3LNu"; color=kOrange;}
    if(sampleI==13) { sampleName="ZZTo2L2Nu"; sampleLabel="ZZTo2L2Nu"; color=kOrange;}
    if(sampleI==14) { sampleName="ZZTo2L2Q"; sampleLabel="ZZTo2L2Q"; color=kOrange;}
    if(sampleI==15) { sampleName="ZZTo4L"; sampleLabel="ZZTo4L"; color=kOrange;}

    if(sampleI==16) { sampleName="DYJets-madgraph"; sampleLabel="DYJetsToLL"; color=kYellow-9;}
    if(sampleI==17) { sampleName="WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"; sampleLabel="WJetsToLNu"; color=kGreen-9;}

    if(sampleI==18) { sampleName="TTTT"; sampleLabel="TTTT"; color=kOrange;}
    if(sampleI==19) { sampleName="TTWJetsToLNu"; sampleLabel="TTWJetsToLNu"; color=kOrange;}
    if(sampleI==20) { sampleName="TTWJetsToQQ"; sampleLabel="TTWJetsToQQ"; color=kOrange;}
    if(sampleI==21) { sampleName="TTZToLLNuNu"; sampleLabel="TTZToLLNuNu"; color=kOrange;}
    if(sampleI==22) { sampleName="TTZToQQ"; sampleLabel="TTZToQQ"; color=kOrange;}
    if(sampleI==23) { sampleName="tZq_ll_4f;"; sampleLabel="tZq_ll_4f"; color=kOrange;}

    if(LepCat==0) eff=(0.85); //1mu
    if(LepCat==1) eff==1-(0.15*0.15); // dimu
    if(LepCat==2) eff==0.85; //mue

    TH1 * h=getHisto (fileName, dirName, histoName+sampleName, color, 1,1);
    //    if(h) h->SetFillStyle(3354);
    h->Scale(lumi*1000*eff);
    //    h->Scale(lumi*1000);
    //    if(sampleI==0 || sampleI==1 || sampleI==2) {
    //      h->Scale(lumi*1000/1.5);
    //      h->Scale(lumi*1000);
    //      h->Scale(lumi*1000/1.5);
    //    } else {
    //      h->Scale(lumi*1000/1.5);
    //      h->Scale(lumi*1000);
    //      h->Scale(lumi);
    //    }
    if(h) h->SetFillColor(color);
    if(sampleI==0 && h) cout << "TTJets_DiLept" << endl;
    if(sampleI==1 && h ) cout << "TTJets_SingleLeptFromT" << endl;
    if(sampleI==2 && h ) cout << "TTJets_SingleLeptFromTbar" << endl;
    if(sampleI==3 && h ) cout << "ST" << endl;
    if(sampleI==4 && h ) cout << "ST" << endl;
    if(sampleI==5 && h ) cout << "ST" << endl;
    if(sampleI==6 && h ) cout << "ST" << endl;
    if(sampleI==7 && h ) cout << "Diboson" << endl;
    if(sampleI==8 && h ) cout << "Diboson" << endl;
    if(sampleI==9 && h ) cout << "Diboson" << endl;
    if(sampleI==10 && h ) cout << "Diboson" << endl;
    if(sampleI==11 && h ) cout << "Diboson" << endl;
    if(sampleI==12 && h ) cout << "Diboson" << endl;
    if(sampleI==13 && h ) cout << "Diboson" << endl;
    if(sampleI==14 && h ) cout << "Diboson" << endl;
    if(sampleI==15 && h ) cout << "Diboson" << endl;

    if(sampleI==16 && h ) cout << "DY" << endl;
    if(sampleI==17 && h ) cout << "W" << endl;

    if(h) hs->Add(h);
    if(h) this_leg->AddEntry(h, sampleLabel, "f");
    //    if(h) hs->GetHistogram()->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());

  }

  //  cout << "SO FAR SO GOOD ?? " << endl;

  //  cst->cd();

  pad1->cd();

  if(!dirName.Contains("Baseline")) {
    if(hs) hs->Draw("hist same");
    if(hdata) hdata->Draw("p e same");
  } else {
    if(hs) hs->Draw("hist");
  }

  //  return cst;

  pad1->cd();
  if(doLog) gPad->SetLogy(1);
  TH1* mcTOT=hs->GetHistogram(); // get tot MC from stack
  if(hdata) hdata->SetTitle("");

  if(!dirName.Contains("Baseline")) {
  if(hdata) hdata->Draw("p e");
  if(hs) hs->Draw("hist sames");
  if(hdata) hdata->Draw("p e sames");
  } else {
    if(hs) hs->Draw("hist");
  }
  if(!(dirPlot.Contains("topCR") || dirPlot.Contains("extraRadCR"))) {
    if(h500) h500->Draw("hist sames");
    if(h500) this_leg->AddEntry(h500, "M500 x 10 000 000", "l");
    if(h1000) h1000->Draw("hist sames");
    if(h1000) this_leg->AddEntry(h1000, "M1000 x 100 000 000", "l");
  }
  this_leg->Draw("sames");

  TLatex latexLabel;
  latexLabel.SetTextSize(0.06);
  latexLabel.SetNDC();
  latexLabel.DrawLatex(0.5, 0.9+0.04, Form("L=%.1f /fb",lumi));
  if(LepCat==0) latexLabel.DrawLatex(0.1, 0.9+0.04, "1#mu");
  if(LepCat==1) latexLabel.DrawLatex(0.1, 0.9+0.04, "2#mu");
  if(LepCat==2) latexLabel.DrawLatex(0.1, 0.9+0.04, "e#mu");
  if(LepCat==0 && dirPlot.Contains("topCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 4jets");
  if((LepCat==1 || LepCat==2) && dirPlot.Contains("topCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 2jets");
  if(LepCat==0 && dirPlot.Contains("extraRadCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 1b + >=5jets");
  if((LepCat==1 || LepCat==2) && dirPlot.Contains("extraRadCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 1b + >=3jets");
  if(LepCat==0 && dirPlot.Contains("charmCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ >=3b + 4jets");

  if(!dirName.Contains("Baseline")) {

    pad2->cd();
    //  TH1* mcTOT=hs->GetHistogram(); // get tot MC from stack
    TH1 *mcTOT = ((TH1 *)(hs->GetStack()->Last())); // the "SUM"
    TH1F *hdataRatio = (TH1F*) hdata->Clone();
    hdataRatio->Divide(mcTOT);
    //  hdataRatio->GetYaxis()->SetRangeUser(0.,2.);
    hdataRatio->GetYaxis()->SetRangeUser(0.5,1.5);
    hdataRatio->SetTitle("");
    hdataRatio->Draw("p e");

    TLine *lineZero = new TLine(mcTOT->GetXaxis()->GetXmin(), 1.,  mcTOT->GetXaxis()->GetXmax(), 1.);
    lineZero->SetLineColor(11);
    lineZero->Draw("same");

    hdataRatio->GetXaxis()->SetTitle(hdata->GetXaxis()->GetTitle());
    hdataRatio->GetXaxis()->SetTitleOffset(1.);
    hdataRatio->GetXaxis()->SetLabelSize(0.05*0.75/0.25);
    hdataRatio->GetXaxis()->SetTitleSize(0.05*0.75/0.25);
    hdataRatio->GetYaxis()->SetLabelSize(0.05*0.75/0.25);
    hdataRatio->GetYaxis()->SetTitleOffset(0.5);
    hdataRatio->GetYaxis()->SetTitleSize(0.05*0.75/0.25);
    hdataRatio->GetYaxis()->SetNdivisions(502);
    pad2->RedrawAxis();
  }

  if(dirName.Contains("PreselectionN1"))   {
    if(LepCat==0) cst->SaveAs(Form("plots/N-1/N-1_%s1l.png",histoName.Data()));
    if(LepCat==1) cst->SaveAs(Form("plots/N-1/N-1_%s2l.png",histoName.Data())); 
    if(LepCat==2) cst->SaveAs(Form("plots/N-1/N-1_%s2l_emu.png",histoName.Data())); 
  } else if(dirName.Contains("Baseline"))   {
      if(LepCat==0) cst->SaveAs(Form("Baseline/%s1l.png",histoName.Data()));
      if(LepCat==1) cst->SaveAs(Form("Baseline/%s2l.png",histoName.Data()));
      if(LepCat==2) cst->SaveAs(Form("Baseline/%s2l_emu.png",histoName.Data()));
  } else {
    if(dirPlot.Contains("topCR"))   {
      if(LepCat==0) cst->SaveAs(Form("topCR/%s1l.png",histoName.Data()));
      if(LepCat==1) cst->SaveAs(Form("topCR/%s2l.png",histoName.Data()));
      if(LepCat==2) cst->SaveAs(Form("topCR/%s2l_emu.png",histoName.Data()));
    } else if(dirPlot.Contains("charmCR"))   {
      if(LepCat==0) cst->SaveAs(Form("charmCR/%s1l.png",histoName.Data()));
      if(LepCat==1) cst->SaveAs(Form("charmCR/%s2l.png",histoName.Data()));
      if(LepCat==2) cst->SaveAs(Form("charmCR/%s2l_emu.png",histoName.Data()));
    } else if(dirPlot.Contains("extraRadCR"))   {
      if(LepCat==0) cst->SaveAs(Form("extraRadCR/%s1l.png",histoName.Data()));
      if(LepCat==1) cst->SaveAs(Form("extraRadCR/%s2l.png",histoName.Data()));
      if(LepCat==2) cst->SaveAs(Form("extraRadCR/%s2l_emu.png",histoName.Data()));
    } else if(dirPlot.Contains("highMTCR"))   {
      if(LepCat==0) cst->SaveAs(Form("highMTCR/%s1l.png",histoName.Data()));
      if(LepCat==1) cst->SaveAs(Form("highMTCR/%s2l.png",histoName.Data()));
      if(LepCat==2) cst->SaveAs(Form("highMTCR/%s2l_emu.png",histoName.Data()));
    } else if(dirPlot.Contains("properties"))   {
      if(LepCat==0) cst->SaveAs(Form("plots/properties/%s_%s1l.png",dirName.Data(),histoName.Data()));
      if(LepCat==1) cst->SaveAs(Form("plots/properties/%s_%s2l.png",dirName.Data(),histoName.Data()));
      if(LepCat==2) cst->SaveAs(Form("plots/properties/%s_%s2l_emu.png",dirName.Data(),histoName.Data()));
    } else {
      if(LepCat==0) cst->SaveAs(Form("plots/%s1l.png",histoName.Data()));
      if(LepCat==1) cst->SaveAs(Form("plots/%s2l.png",histoName.Data()));
      if(LepCat==2) cst->SaveAs(Form("plots/%s2l_emu.png",histoName.Data()));
    }
    }

  return cst;

};

void dataMCstack() {

  gStyle->SetOptStat(0);  
  gROOT->LoadMacro("utils.C++");

  TString histoName="";
  TString dirName="";

  //  histoName="HT_"; dirName="ChargedHiggsTopBottom/Baseline"; 
  //  histoName="CutFlow_"; dirName="ChargedHiggsTopBottom/CutFlow"; 

  TCanvas *c;

  //// doing 1l

  int lepCat=0;
  c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_1Mu",true,lepCat);

  dirName="ChargedHiggsTopBottom/PreselectionN1_1Mu";
  c = dataMCstackPlot("EtMiss_",dirName,true,lepCat);
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
  c = dataMCstackPlot("Njets_",dirName,false,lepCat);
  c = dataMCstackPlot("Mt_",dirName,false,lepCat);
  dirName="ChargedHiggsTopBottom/Baseline_1Mu";
  c = dataMCstackPlot("Vertices_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat);
  c = dataMCstackPlot("ptW_",dirName,true,lepCat);
  c = dataMCstackPlot("HT_",dirName,true,lepCat);
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat);
  c = dataMCstackPlot("LeadingBDiscr_",dirName,true,lepCat);
  c = dataMCstackPlot("NextLeadingBPt_",dirName,true,lepCat);
  c = dataMCstackPlot("NextLeadingBDiscr_",dirName,true,lepCat);
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat);
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat);
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat);
  c = dataMCstackPlot("dRlb_",dirName,false,lepCat);
  //  c = dataMCstackPlot("ptWb_",dirName,false,lepCat);

  dirName="ChargedHiggsTopBottom/highMTCR_1Mu";
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"highMTCR");
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"highMTCR");

  dirName="ChargedHiggsTopBottom/topCR_1Mu";
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("Mt_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");

  dirName="ChargedHiggsTopBottom/extraRadCR_1Mu";
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("Mt_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");

  dirName="ChargedHiggsTopBottom/charmCR_1Mu";
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"charmCR");
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"charmCR");
  //  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"charmCRCR");
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"charmCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"charmCR");
  //  c = dataMCstackPlot("Mt_",dirName,false,lepCat,"charmCRCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"charmCRCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"charmCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"charmCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"charmCR");


  //// doing 2l

  lepCat=1;

  c = dataMCstackPlot("CutFlowDilep_","ChargedHiggsTopBottom/CutFlow_2Mu",true,lepCat);

  dirName="ChargedHiggsTopBottom/PreselectionN1_2Mu";
  c = dataMCstackPlot("EtMiss_",dirName,true,lepCat);
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
  c = dataMCstackPlot("Njets_",dirName,false,lepCat);
  dirName="ChargedHiggsTopBottom/Baseline_2Mu";
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat);
  c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat);
  c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonDeltaR_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("HT_",dirName,true,lepCat);
  c = dataMCstackPlot("Vertices_",dirName,false,lepCat);
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat);
  c = dataMCstackPlot("dRlb_",dirName,false,lepCat);
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat);
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat);
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
  c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("MT2bb_",dirName,false,lepCat);
  c = dataMCstackPlot("minMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("totMT_",dirName,false,lepCat,"properties");

  dirName="ChargedHiggsTopBottom/topCR_2Mu";
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("minMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("totMT_",dirName,false,lepCat,"properties");

  dirName="ChargedHiggsTopBottom/extraRadCR_2Mu";
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("minMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("totMT_",dirName,false,lepCat,"properties");

  lepCat=2;

  //// doing 2l

  c = dataMCstackPlot("CutFlowDilep_","ChargedHiggsTopBottom/CutFlow_1Mu1Ele",true,lepCat);

  dirName="ChargedHiggsTopBottom/PreselectionN1_1Mu1Ele";
  c = dataMCstackPlot("EtMiss_",dirName,true,lepCat);
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
  c = dataMCstackPlot("Njets_",dirName,false,lepCat);
  dirName="ChargedHiggsTopBottom/Baseline_1Mu1Ele";
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat);
  c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat);
  c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat);
  c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonDeltaR_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("HT_",dirName,true,lepCat);
  c = dataMCstackPlot("Vertices_",dirName,false,lepCat);
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat);
  c = dataMCstackPlot("LeadingBDiscr_",dirName,true,lepCat);
  c = dataMCstackPlot("NextLeadingBPt_",dirName,true,lepCat);
  c = dataMCstackPlot("NextLeadingBDiscr_",dirName,true,lepCat);
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat);
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat);
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
  c = dataMCstackPlot("dRlb_",dirName,false,lepCat);
  c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("minMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("totMT_",dirName,false,lepCat,"properties");

  dirName="ChargedHiggsTopBottom/topCR_1Mu1Ele";
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"topCR");
  c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
  c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("minMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("totMT_",dirName,false,lepCat,"properties");

  /*
  dirName="ChargedHiggsTopBottom/highMTCR_1Mu1Ele";
  c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"highMTCR");
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"highMTCR");
  c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"highMTCR");
  c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"highMTCR");
  */

  dirName="ChargedHiggsTopBottom/extraRadCR_1Mu1Ele";
  c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("HT_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
  c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"extraRadCR");
  c = dataMCstackPlot("minMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"properties");
  c = dataMCstackPlot("totMT_",dirName,false,lepCat,"properties");

}

