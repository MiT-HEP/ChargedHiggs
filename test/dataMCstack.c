#include "TROOT.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "THStack.h"
#include "TString.h"
#include "TStyle.h"
#include "TLine.h"
#include "TH1.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TLatex.h"

double lumi=12.9;
//double lumi=36.2;
double eff=1.;
//12878700468.905

#include <iostream>
using namespace std;

TGraphAsymmErrors * TOTerror(TH1 *hmc, TH1 *hmcJECup, TH1 *hmcJECdown,
			     TH1 *mcTOT_BTAGBup,TH1 *mcTOT_BTAGBdown,TH1 *mcTOT_BTAGLup,TH1 *mcTOT_BTAGLdown,
			     TH1 *mcTOT_PUup,TH1 *mcTOT_PUdown,TH1 *mcTOT_UNCLUSTERup,TH1 *mcTOT_UNCLUSTERdown,
			     TH1 *ratio) {

  TH1 *den1 = (TH1 *) hmc->Clone ("bkgden1");
  TH1 *den2 = (TH1 *) hmc->Clone ("bkgden2");

  Int_t nvar = hmc->GetNbinsX();

  Double_t *x = new Double_t[nvar];
  Double_t *y = new Double_t[nvar];
  Double_t *exl = new Double_t[nvar];
  Double_t *eyl = new Double_t[nvar];
  Double_t *exh = new Double_t[nvar];
  Double_t *eyh = new Double_t[nvar];
  Double_t *x1 = new Double_t[nvar];
  Double_t *y1 = new Double_t[nvar];
  Double_t *exl1 = new Double_t[nvar];
  Double_t *eyl1 = new Double_t[nvar];
  Double_t *exh1 = new Double_t[nvar];
  Double_t *eyh1 = new Double_t[nvar];


  for (int km = 1; km <= nvar; km++){
    //  for (int km = 1; km <= 8; km++){
    double deltaUp = sqrt(hmc->GetBinError(km)*hmc->GetBinError(km) + 
			  (hmc->GetBinContent(km)-hmcJECup->GetBinContent(km))*(hmc->GetBinContent(km)-hmcJECup->GetBinContent(km)) +
			  (hmc->GetBinContent(km)-mcTOT_BTAGBup->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_BTAGBup->GetBinContent(km)) +
			  (hmc->GetBinContent(km)-mcTOT_BTAGLup->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_BTAGLup->GetBinContent(km)) +
			  (hmc->GetBinContent(km)-mcTOT_PUup->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_PUup->GetBinContent(km)) +
			  (hmc->GetBinContent(km)-mcTOT_UNCLUSTERup->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_UNCLUSTERup->GetBinContent(km))
			  ); 
    double deltaDown = sqrt(hmc->GetBinError(km)*hmc->GetBinError(km) + 
			    (hmc->GetBinContent(km)-hmcJECdown->GetBinContent(km))*(hmc->GetBinContent(km)-hmcJECdown->GetBinContent(km)) +
			    (hmc->GetBinContent(km)-mcTOT_BTAGBdown->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_BTAGBdown->GetBinContent(km)) +
			    (hmc->GetBinContent(km)-mcTOT_PUdown->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_PUdown->GetBinContent(km)) +
			    (hmc->GetBinContent(km)-mcTOT_UNCLUSTERdown->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_UNCLUSTERdown->GetBinContent(km))
			    );
    
    den1->SetBinContent (km,hmc->GetBinContent (km) + deltaUp);
    den2->SetBinContent (km,hmc->GetBinContent (km) - deltaDown);

  }

  // ratio from variation and nominal
  TH1F *ratiop = (TH1F *) hmc->Clone ("backgroundratiop");
  TH1F *ratiom = (TH1F *) hmc->Clone ("backgroundratiom");
  ratiop->Divide (den1);
  ratiom->Divide (den2);

  for (int km = 0; km < nvar; km++){

    if(ratio->GetBinContent(km+1)==0) {
      y1[km] = 1.;
      eyl1[km] = 0.;
      eyh1[km] = 0.;
    } else {
      y1[km] = 1;
      //      y1[km] =ratio->GetBinContent(km+1);
      eyl1[km] = fabs(ratiop->GetBinContent(km) - ratio->GetBinContent(km+1));
      eyh1[km] = fabs(ratiom->GetBinContent(km) - ratio->GetBinContent(km+1));
      //      eyl1[km] = 1-fabs(ratiop->GetBinContent(km) - ratio->GetBinContent(km+1));
      //      eyh1[km] = 1+fabs(ratiom->GetBinContent(km) - ratio->GetBinContent(km+1));
    }
    x1[km] = ratio->GetBinCenter(km+1);
    exl1[km] = ratio->GetBinWidth(km)/2;
    exh1[km] = ratio->GetBinWidth(km)/2;
    //    eyl1[km] = ratiop->GetBinContent(km) - ratio->GetBinContent(km);
    //    eyh1[km] = ratiom->GetBinContent(km) - ratio->GetBinContent(km);

    cout << "x=" << x1[km] << " y="<< y1[km] << " eyl="<< eyl1[km] << " eyh="<< eyh1[km] << endl;

  }

  TGraphAsymmErrors *err = new TGraphAsymmErrors (nvar, x1, y1, exl1, exh1, eyl1, eyh1);

  return err;



}



TGraphAsymmErrors * JESerror(TH1 *hmc, TH1 *hmcJECup, TH1 *hmcJECdown,
			     TH1 *ratio) {

  TH1 *den1 = (TH1 *) hmc->Clone ("bkgden1");
  TH1 *den2 = (TH1 *) hmc->Clone ("bkgden2");

  Int_t nvar = hmc->GetNbinsX();

  Double_t *x = new Double_t[nvar];
  Double_t *y = new Double_t[nvar];
  Double_t *exl = new Double_t[nvar];
  Double_t *eyl = new Double_t[nvar];
  Double_t *exh = new Double_t[nvar];
  Double_t *eyh = new Double_t[nvar];
  Double_t *x1 = new Double_t[nvar];
  Double_t *y1 = new Double_t[nvar];
  Double_t *exl1 = new Double_t[nvar];
  Double_t *eyl1 = new Double_t[nvar];
  Double_t *exh1 = new Double_t[nvar];
  Double_t *eyh1 = new Double_t[nvar];


  for (int km = 1; km <= nvar; km++){
    //  for (int km = 1; km <= 8; km++){
    double deltaUp = sqrt(hmc->GetBinError(km)*hmc->GetBinError(km) + 
			  (hmc->GetBinContent(km)-hmcJECup->GetBinContent(km))*(hmc->GetBinContent(km)-hmcJECup->GetBinContent(km)) 
			  ); 
    double deltaDown = sqrt(hmc->GetBinError(km)*hmc->GetBinError(km) + 
			    (hmc->GetBinContent(km)-hmcJECdown->GetBinContent(km))*(hmc->GetBinContent(km)-hmcJECdown->GetBinContent(km))
			    );
    
    den1->SetBinContent (km,hmc->GetBinContent (km) + deltaUp);
    den2->SetBinContent (km,hmc->GetBinContent (km) - deltaDown);

  }

  // ratio from variation and nominal
  TH1F *ratiop = (TH1F *) hmc->Clone ("backgroundratiop");
  TH1F *ratiom = (TH1F *) hmc->Clone ("backgroundratiom");
  ratiop->Divide (den1);
  ratiom->Divide (den2);

  for (int km = 0; km < nvar; km++){

    if(ratio->GetBinContent(km+1)==0) {
      y1[km] = 1.;
      eyl1[km] = 0.;
      eyh1[km] = 0.;
    } else {
      y1[km] = 1;
      //      y1[km] =ratio->GetBinContent(km+1);
      eyl1[km] = fabs(ratiop->GetBinContent(km) - ratio->GetBinContent(km+1));
      eyh1[km] = fabs(ratiom->GetBinContent(km) - ratio->GetBinContent(km+1));
      //      eyl1[km] = 1-fabs(ratiop->GetBinContent(km) - ratio->GetBinContent(km+1));
      //      eyh1[km] = 1+fabs(ratiom->GetBinContent(km) - ratio->GetBinContent(km+1));
    }
    x1[km] = ratio->GetBinCenter(km+1);
    exl1[km] = ratio->GetBinWidth(km)/2;
    exh1[km] = ratio->GetBinWidth(km)/2;
    //    eyl1[km] = ratiop->GetBinContent(km) - ratio->GetBinContent(km);
    //    eyh1[km] = ratiom->GetBinContent(km) - ratio->GetBinContent(km);

    cout << "x=" << x1[km] << " y="<< y1[km] << " eyl="<< eyl1[km] << " eyh="<< eyh1[km] << endl;

  }

  TGraphAsymmErrors *err = new TGraphAsymmErrors (nvar, x1, y1, exl1, exh1, eyl1, eyh1);

  return err;



}


void makeTable( int LepCat=0 ) {

  gStyle->SetOptStat(0);  
  //  gROOT->LoadMacro("utils.C++");
  gROOT->LoadMacro("utils.C");

  TString histoName="";
  TString dirName="";
  TString fileName="";

  if(LepCat==0) { dirName="ChargedHiggsTopBottom/CutFlow_1Mu"; histoName="CutFlow_SingleMuon"; }
  if(LepCat==1) { dirName="ChargedHiggsTopBottom/CutFlow_2Mu"; histoName="CutFlow_SingleMuon"; }
  if(LepCat==2) { dirName="ChargedHiggsTopBottom/CutFlow_1Mu1Ele"; histoName="CutFlow_SingleMuon"; }
  if(LepCat==3) { dirName="ChargedHiggsTopBottom/CutFlow_1Ele"; histoName="CutFlow_SingleElectron"; }
  if(LepCat==4) { dirName="ChargedHiggsTopBottom/CutFlow_2Ele"; histoName="CutFlow_SingleElectron"; }

  fileName="JAN18/1l.root";
  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="JAN18/2l.root";

  std::cout << " ====== " << dirName.Data() << "======" << histoName.Data() << "======" << fileName.Data() << std::endl;

  int color =1;

  TH1 * h=getHisto (fileName, dirName, histoName, color, 1, 1);

  //  cout << sampleName.Data() << "          &  " << int(h->GetBinContent(1)) << " & " << int(h->GetBinContent(2)) << " & " << int(h->GetBinContent(3)) << endl;
 

  std::cout << "total       & " << int(h->GetBinContent(1)) << std::endl;
  std::cout << "N(lep)>=1   & " << int(h->GetBinContent(2)) << std::endl;
  std::cout << "N(lep)==1   & " << int(h->GetBinContent(3)) << std::endl;
  std::cout << "N(tau)==0   & " << int(h->GetBinContent(4)) << std::endl;
  std::cout << "MET==30     & " << int(h->GetBinContent(5)) << std::endl;
  std::cout << "Njets>=5    & " << int(h->GetBinContent(7)) << std::endl;
  std::cout << "NB>=2       & " << int(h->GetBinContent(8)) << std::endl;

}

TH1 *getDATA( TString fileName="", TString histoName="", TString dirName="", bool doLog=false, int LepCat=0) {
  TString sampleName="";
  TString sampleLabel="";
  int color =1;

  sampleName="Data"; sampleLabel="Data";  color=kBlack; 
  if(LepCat==0 || LepCat==1 || LepCat==2) sampleName="SingleMuon"; sampleLabel="SingleMuon";  color=kBlack;
  if(LepCat==3 || LepCat==4) sampleName="SingleElectron"; sampleLabel="SingleElectron";  color=kBlack;
  TH1 * hdata=getHisto (fileName, dirName, histoName+sampleName, color, 1,1);

  if(histoName.Contains("CutFlow")) hdata->GetYaxis()->SetRangeUser(1000,1000000000);
  if(!histoName.Contains("CutFlow") && !doLog) hdata->GetYaxis()->SetRangeUser(0.,hdata->GetMaximum()*1.5);
  if(!histoName.Contains("CutFlow") && doLog) hdata->GetYaxis()->SetRangeUser(0.1,hdata->GetMaximum()*10);
  hdata->SetMarkerStyle(20);
  if(!dirName.Contains("Baseline")) hdata->Draw("p e");
  hdata->SetDirectory(0);

  return hdata;

}

THStack *getMCStack( TString fileName="", TString histoName="",TString dirName="", bool doLog=false, int LepCat=0, TLegend * this_leg, TString variation="") {

  TString sampleName="";
  TString sampleLabel="";
  int color =1;

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

  //  TLegend* this_leg = new TLegend(0.75,0.6,0.5+0.5,0.5+0.5);
  //  TLegend* this_leg = new TLegend(0.7,0.5,0.5+0.5,0.4+0.5);
  //  TLegend* this_leg = new TLegend(0.6,0.6,0.4+0.5,0.4+0.5);
  this_leg->SetFillColor(0);
  //  this_leg->SetFillStyle(0);
  this_leg->SetBorderSize(0);


  THStack *hs = new THStack("hs",Form("",histoName.Data()));

  for(int sampleI=0; sampleI<16; sampleI++) {

    if(sampleI==0) { sampleName="TTJets_DiLept_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"; sampleLabel="TTJets_DiLept";  color=kRed;}   
    if(sampleI==1) { sampleName="TTJets_SingleLeptFromT_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"; sampleLabel="TTJets_SingleLeptFromT"; color=kBlue;} 
    if(sampleI==2) { sampleName="TTJets_SingleLeptFromTbar_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"; sampleLabel="TTJets_SingleLeptFromTbar"; color=kBlue;}  
    if(sampleI==3) { sampleName="ST"; sampleLabel="ST"; color=kMagenta;}
    if(sampleI==4) { sampleName="WW"; sampleLabel="WW"; color=kGreen-8;}
    if(sampleI==5) { sampleName="WZ"; sampleLabel="WZ"; color=kGreen-8;}
    if(sampleI==6) { sampleName="ZZ"; sampleLabel="ZZ"; color=kGreen-8;}
    if(sampleI==7) { sampleName="VHToNonbb_M125"; sampleLabel="VHToNonbb_M125"; color=kGreen-8;}
    if(sampleI==8) { sampleName="WH_HToBB_WToLNu_M125"; sampleLabel="WH_HToBB_WToLNu_M125"; color=kGreen-8;}

    if(sampleI==9) { sampleName="DYJets-madgraph"; sampleLabel="DYJetsToLL"; color=kGreen-10;}
    if(sampleI==10) { sampleName="WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"; sampleLabel="WJetsToLNu"; color=kGreen-9;}
    if(sampleI==11) { sampleName="TTTT"; sampleLabel="TTTT"; color=kOrange;}
    if(sampleI==12) { sampleName="TTW"; sampleLabel="TTW"; color=kOrange;}
    if(sampleI==13) { sampleName="TTZ"; sampleLabel="TTZ"; color=kOrange;}
    if(sampleI==14) { sampleName="tZq"; sampleLabel="tZq"; color=kOrange;}
    if(sampleI==15) { sampleName="ttH"; sampleLabel="ttH"; color=kOrange;}

    /*
      // all splitted
    if(sampleI==3) { sampleName="ST_t-channel_antitop"; sampleLabel="ST_t-channel_antitop"; color=kMagenta+1;}
    if(sampleI==4) { sampleName="ST_t-channel_top"; sampleLabel="ST_t-channel_top"; color=kMagenta;}
    if(sampleI==5) { sampleName="ST_tW_antitop"; sampleLabel="ST_tW_antitop"; color=kMagenta-9;}
    if(sampleI==6) { sampleName="ST_tW_top"; sampleLabel="ST_tW_top"; color=kMagenta-10;}
    if(sampleI==7) { sampleName="ST_s-channel"; sampleLabel="ST_s-channel"; color=kMagenta+1;}

    if(sampleI==8) { sampleName="WWTo2L2Nu"; sampleLabel="WWTo2L2Nu"; color=kGreen-8;}
    if(sampleI==9) { sampleName="WWToLNuQQ"; sampleLabel="WWToLNuQQ"; color=kGreen-8;}
    if(sampleI==10) { sampleName="WZTo1L1Nu2Q"; sampleLabel="WZTo1L1Nu2Q"; color=kGreen-8;}
    if(sampleI==11) { sampleName="WZTo1L3Nu"; sampleLabel="WZTo1L3Nu"; color=kGreen-8;}
    if(sampleI==12) { sampleName="WZTo2L2Q"; sampleLabel="WZTo2L2Q"; color=kGreen-8;}
    if(sampleI==13) { sampleName="WZTo3LNu"; sampleLabel="WZTo3LNu"; color=kGreen-8;}
    if(sampleI==14) { sampleName="ZZTo2L2Nu"; sampleLabel="ZZTo2L2Nu"; color=kGreen-8;}
    if(sampleI==15) { sampleName="ZZTo2L2Q"; sampleLabel="ZZTo2L2Q"; color=kGreen-8;}
    if(sampleI==16) { sampleName="ZZTo4L"; sampleLabel="ZZTo4L"; color=kGreen-8;}
    if(sampleI==17) { sampleName="VHToNonbb_M125"; sampleLabel="VHToNonbb_M125"; color=kGreen-8;}
    if(sampleI==18) { sampleName="WH_HToBB_WToLNu_M125"; sampleLabel="WH_HToBB_WToLNu_M125"; color=kGreen-8;}

    if(sampleI==19) { sampleName="DYJets-madgraph"; sampleLabel="DYJetsToLL"; color=kGreen-10;}
    if(sampleI==20) { sampleName="WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"; sampleLabel="WJetsToLNu"; color=kGreen-9;}

    if(sampleI==21) { sampleName="TTTT"; sampleLabel="TTTT"; color=kOrange;}
    if(sampleI==22) { sampleName="TTWJetsToLNu"; sampleLabel="TTWJetsToLNu"; color=kOrange;}
    if(sampleI==23) { sampleName="TTWJetsToQQ"; sampleLabel="TTWJetsToQQ"; color=kOrange;}
    if(sampleI==24) { sampleName="TTZToLLNuNu"; sampleLabel="TTZToLLNuNu"; color=kOrange;}
    if(sampleI==25) { sampleName="TTZToQQ"; sampleLabel="TTZToQQ"; color=kOrange;}
    if(sampleI==26) { sampleName="tZq_ll_4f;"; sampleLabel="tZq_ll_4f"; color=kOrange;}
    if(sampleI==27) { sampleName="tZq_nunu_4f;"; sampleLabel="tZq_nunu_4f"; color=kOrange;}
    if(sampleI==28) { sampleName="ttHJetTobb;"; sampleLabel="ttHJetTobb"; color=kOrange;}
    if(sampleI==29) { sampleName="ttHJetToNonbb;"; sampleLabel="ttHJetToNonbb"; color=kOrange;}
    */

    if(LepCat==0) eff=(1-0.15); //1mu
    if(LepCat==1) eff=1-(0.15*0.15); // dimu
    if(LepCat==2) eff=(1-0.15); //mue

    if(LepCat==3) eff=(1-0.15); //1e
    if(LepCat==4) eff=1-(0.15*0.15); // diele


    TH1 * h=getHisto (fileName, dirName, histoName+sampleName+variation, color, 1,1);
    //    if(h) h->SetFillStyle(3354);
    if(h) h->Scale(lumi*1000*eff);
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

    if(!(variation.Contains("Up") || variation.Contains("Down"))) {
      if(h && sampleI==0) this_leg->AddEntry(h, "TTJets #rightarrow 2l " , "f");
      if(h && sampleI==1) this_leg->AddEntry(h, "TTJets #rightarrow 1l" , "f");
      if(h && sampleI==3) this_leg->AddEntry(h, "Single Top" , "f");
      //    if(h && sampleI==7) this_leg->AddEntry(h, "Diboson" , "f");
      if(h && sampleI==12) this_leg->AddEntry(h, "rares" , "f");
      //    if(h && sampleI==16) this_leg->AddEntry(h, "DY+jets" , "f");
      //    if(h && sampleI==17) this_leg->AddEntry(h, "W+jets" , "f");
      if(h && sampleI==4) this_leg->AddEntry(h, "EWK" , "f");
    }
    if(h) hs->Add(h);
    //    if(h) this_leg->AddEntry(h, sampleLabel, "f");
    //    if(h) hs->GetHistogram()->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());

  }

  return hs;

}


TCanvas *dataMCstackPlot( TString histoName="", TString dirName="", bool doLog=false, int LepCat=0, TString dirPlot="") {

  //  bool do1l=true;
  //  TString histoName="";
  //  TString dirName="";

  TLegend* this_leg = new TLegend(0.6,0.6,0.4+0.5,0.4+0.5);
  this_leg->SetFillColor(0);
  //  this_leg->SetFillStyle(0);
  this_leg->SetBorderSize(0);

  //  TString fileName="JAN2/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="JAN2/2l.root";
  //  TString fileName="JAN3syst/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="JAN3syst/2l.root";
  TString fileName="JAN18/1l.root";
  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="JAN18/2l.root";

  THStack *hs = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"");
  THStack *hs_JESUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_JESUp");
  THStack *hs_JESDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_JESDown");

  THStack *hs_BTAGBUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGBUp");
  THStack *hs_BTAGBDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGBDown");
  THStack *hs_BTAGLUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGLUp");
  THStack *hs_BTAGLDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGLDown");

  THStack *hs_PUUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_PUUp");
  THStack *hs_PUDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_PUDown");
  THStack *hs_UNCLUSTERUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_UNCLUSTERUp");
  THStack *hs_UNCLUSTERDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_UNCLUSTERDown");


  TH1 *hdata = getDATA(fileName,histoName,dirName,doLog,LepCat);

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

  pad1->cd();

  if(hdata) hdata->Draw("p e");

  if(!dirName.Contains("Baseline")) {
    if(hs) hs->Draw("hist same");
    if(hdata) hdata->Draw("p e same");
  } else {
    if(hs) hs->Draw("hist");
  }


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
  /*
  if(!(dirPlot.Contains("topCR") || dirPlot.Contains("extraRadCR"))) {
    if(h500) h500->Draw("hist sames");
    if(h500) this_leg->AddEntry(h500, "M500 x 10 000 000", "l");
    if(h1000) h1000->Draw("hist sames");
    if(h1000) this_leg->AddEntry(h1000, "M1000 x 100 000 000", "l");
  }
  */
  this_leg->Draw("sames");

  TLatex latexLabel;
  latexLabel.SetTextSize(0.06);
  latexLabel.SetNDC();
  latexLabel.DrawLatex(0.5, 0.9+0.04, Form("L=%.1f /fb",lumi));
  if(LepCat==0) latexLabel.DrawLatex(0.13, 0.9+0.04, "1#mu");
  if(LepCat==1) latexLabel.DrawLatex(0.13, 0.9+0.04, "2#mu");
  if(LepCat==2) latexLabel.DrawLatex(0.13, 0.9+0.04, "e#mu");
  if(LepCat==3) latexLabel.DrawLatex(0.13, 0.9+0.04, "1e");
  if(LepCat==4) latexLabel.DrawLatex(0.13, 0.9+0.04, "ee");
  if((LepCat==0 || LepCat==3) && dirPlot.Contains("topCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+>=1b + 4jets");
  if((LepCat==1 || LepCat==2 || LepCat==4) && dirPlot.Contains("topCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+>=1b + 2jets");
  if((LepCat==0 || LepCat==3) && dirPlot.Contains("extraRadCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 1b + >=5jets");
  if((LepCat==1 || LepCat==2 || LepCat==4) && dirPlot.Contains("extraRadCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 1b + >=3jets");
  if((LepCat==0 || LepCat==3) && dirPlot.Contains("charmCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ >=3b + 4jets");

  if(!dirName.Contains("Baseline")) {

    pad2->cd();
    //  TH1* mcTOT=hs->GetHistogram(); // get tot MC from stack
    TH1 *mcTOT = ((TH1 *)(hs->GetStack()->Last())); // the "SUM"
    TH1 *mcTOT_JESUp = ((TH1 *)(hs_JESUp->GetStack()->Last())); // the "SUM"
    TH1 *mcTOT_JESDown = ((TH1 *)(hs_JESDown->GetStack()->Last())); // the "SUM"

    TH1 *mcTOT_BTAGBUp = ((TH1 *)(hs_BTAGBUp->GetStack()->Last())); // the "SUM"
    TH1 *mcTOT_BTAGBDown = ((TH1 *)(hs_BTAGBDown->GetStack()->Last())); // the "SUM"

    TH1 *mcTOT_BTALBUp = ((TH1 *)(hs_BTAGBUp->GetStack()->Last())); // the "SUM"
    TH1 *mcTOT_BTAGLDown = ((TH1 *)(hs_BTAGBDown->GetStack()->Last())); // the "SUM"

    TH1 *mcTOT_PUUp = ((TH1 *)(hs_PUUp->GetStack()->Last())); // the "SUM"
    TH1 *mcTOT_PUDown = ((TH1 *)(hs_PUDown->GetStack()->Last())); // the "SUM"

    TH1 *mcTOT_UNCLUSTERUp = ((TH1 *)(hs_UNCLUSTERUp->GetStack()->Last())); // the "SUM"
    TH1 *mcTOT_UNCLUSTERDown = ((TH1 *)(hs_UNCLUSTERDown->GetStack()->Last())); // the "SUM"

    TH1F *hdataRatio = (TH1F*) hdata->Clone();
    hdataRatio->Divide(mcTOT);
    //  hdataRatio->GetYaxis()->SetRangeUser(0.,2.);
    hdataRatio->GetYaxis()->SetRangeUser(0.5,1.5);
    hdataRatio->GetYaxis()->SetTitle("data/MC");
    hdataRatio->SetTitle("");
    hdataRatio->Draw("p e");

    TGraphAsymmErrors * toterree= TOTerror(mcTOT,mcTOT_JESUp,mcTOT_JESDown,
					   mcTOT_BTAGBUp,mcTOT_BTAGBDown,mcTOT_BTALBUp,mcTOT_BTAGLDown,
					   mcTOT_PUUp,mcTOT_PUDown,mcTOT_UNCLUSTERUp,mcTOT_UNCLUSTERDown,
					   hdataRatio);

    /*
    TGraphAsymmErrors * toterree= JESerror(mcTOT,mcTOT_JESUp,mcTOT_JESDown,
    					   hdataRatio);
    */

    TAxis *toterree_yaxis = toterree->GetYaxis ();
    TAxis *toterree_xaxis = toterree->GetXaxis ();
    toterree_yaxis->SetRangeUser(0.5,1.5);
    toterree_xaxis->SetRangeUser(0.,10.);
    toterree->SetTitle(0);
    toterree_yaxis->SetTitle("data/MC");
    toterree_xaxis->SetTitle(hdata->GetXaxis()->GetTitle());
    toterree_xaxis->SetTitleOffset(1.);
    toterree_xaxis->SetLabelSize(0.05*0.75/0.25);
    toterree_yaxis->SetTitleSize(0.05*0.75/0.25);
    toterree_yaxis->SetLabelSize(0.05*0.75/0.25);
    toterree_yaxis->SetTitleOffset(0.5);
    //    hdataRatio->GetYaxis()->SetTitleSize(0.07*0.75/0.25);
    toterree_yaxis->SetTitleSize(0.05*0.75/0.25);
    toterree_yaxis->SetNdivisions(502);

    toterree->SetFillColor(kGray+3);
    toterree->SetLineColor(kGray+3);
    toterree->SetFillStyle(3010);                                   
    toterree->Draw("2 same");
    toterree->Draw("p");
    hdataRatio->Draw("p e sames");

    TLine *lineZero = new TLine(mcTOT->GetXaxis()->GetXmin(), 1.,  mcTOT->GetXaxis()->GetXmax(), 1.);
    lineZero->SetLineColor(11);
    lineZero->Draw("same");
  
    hdataRatio->GetXaxis()->SetTitle(hdata->GetXaxis()->GetTitle());
    hdataRatio->GetXaxis()->SetTitleOffset(1.);
    hdataRatio->GetXaxis()->SetLabelSize(0.05*0.75/0.25);
    hdataRatio->GetXaxis()->SetTitleSize(0.05*0.75/0.25);
    hdataRatio->GetYaxis()->SetLabelSize(0.05*0.75/0.25);
    hdataRatio->GetYaxis()->SetTitleOffset(0.5);
    //    hdataRatio->GetYaxis()->SetTitleSize(0.07*0.75/0.25);
    hdataRatio->GetYaxis()->SetTitleSize(0.05*0.75/0.25);
    hdataRatio->GetYaxis()->SetNdivisions(502);

    //    pad2->RedrawAxis();
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
      if(LepCat==0) cst->SaveAs(Form("topCR/%s1Mu.png",histoName.Data())); 
      if(LepCat==1) cst->SaveAs(Form("topCR/%s2Mu.png",histoName.Data())); 
      if(LepCat==2) cst->SaveAs(Form("topCR/%s2l_emu.png",histoName.Data())); 
      if(LepCat==3) cst->SaveAs(Form("topCR/%s1Ele.png",histoName.Data())); 
      if(LepCat==4) cst->SaveAs(Form("topCR/%s2Ele.png",histoName.Data())); 
    } else if(dirPlot.Contains("charmCR"))   {
      if(LepCat==0) cst->SaveAs(Form("charmCR/%s1Mu.png",histoName.Data())); 
      if(LepCat==1) cst->SaveAs(Form("charmCR/%s2Mu.png",histoName.Data())); 
      if(LepCat==2) cst->SaveAs(Form("charmCR/%s2l_emu.png",histoName.Data())); 
      if(LepCat==3) cst->SaveAs(Form("charmCR/%s1Ele.png",histoName.Data())); 
      if(LepCat==4) cst->SaveAs(Form("charmCR/%s2Ele.png",histoName.Data())); 
    } else if(dirPlot.Contains("extraRadCR"))   {
      if(LepCat==0) cst->SaveAs(Form("extraRadCR/%s1Mu.png",histoName.Data())); 
      if(LepCat==1) cst->SaveAs(Form("extraRadCR/%s2Mu.png",histoName.Data())); 
      if(LepCat==2) cst->SaveAs(Form("extraRadCR/%s2l_emu.png",histoName.Data())); 
      if(LepCat==3) cst->SaveAs(Form("extraRadCR/%s1Ele.png",histoName.Data())); 
      if(LepCat==4) cst->SaveAs(Form("extraRadCR/%s2Ele.png",histoName.Data())); 
    } else if(dirPlot.Contains("lowMTCR"))   {
      if(LepCat==0) cst->SaveAs(Form("lowMTCR/%s1Mu.png",histoName.Data())); 
      if(LepCat==1) cst->SaveAs(Form("lowMTCR/%s2Mu.png",histoName.Data())); 
      if(LepCat==2) cst->SaveAs(Form("lowMTCR/%s2l_emu.png",histoName.Data())); 
      if(LepCat==3) cst->SaveAs(Form("lowMTCR/%s1Ele.png",histoName.Data())); 
      if(LepCat==4) cst->SaveAs(Form("lowMTCR/%s2Ele.png",histoName.Data())); 
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

void dataMCstack(int lepCat) {

  gStyle->SetOptStat(0);  
  gROOT->LoadMacro("utils.C++");

  TString histoName="";
  TString dirName="";

  //  histoName="HT_"; dirName="ChargedHiggsTopBottom/Baseline"; 
  //  histoName="CutFlow_"; dirName="ChargedHiggsTopBottom/CutFlow"; 

  TCanvas *c;


  //// doing 1l
  if(lepCat==0) {

    c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_1Mu",true,lepCat);
    
    //  dirName="ChargedHiggsTopBottom/PreselectionN1_1Mu";
    //  c = dataMCstackPlot("EtMiss_",dirName,true,lepCat);
    //  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
    //  c = dataMCstackPlot("Njets_",dirName,false,lepCat);
    //  c = dataMCstackPlot("Mt_",dirName,false,lepCat);
    
    dirName="ChargedHiggsTopBottom/topCR_1Mu";
    c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("Njets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    //  c = dataMCstackPlot("HT_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("Mt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("Met_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    
    dirName="ChargedHiggsTopBottom/extraRadCR_1Mu";
    c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("Mt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");
    
    /*
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
      c = dataMCstackPlot("thirdBDiscr_",dirName,true,lepCat);
      c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat);
      c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat);
      c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
      c = dataMCstackPlot("minDRbb_",dirName,false,lepCat);
      c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat);
      c = dataMCstackPlot("dRlb_",dirName,false,lepCat);
      c = dataMCstackPlot("bdt1_",dirName,false,lepCat);
      c = dataMCstackPlot("bdt2_",dirName,false,lepCat);
      c = dataMCstackPlot("bdt3_",dirName,false,lepCat);
      c = dataMCstackPlot("bdt4_",dirName,false,lepCat);
      //  c = dataMCstackPlot("ptWb_",dirName,false,lepCat);
      c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
      
      dirName="ChargedHiggsTopBottom/lowMTCR_1Mu";
      c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"lowMTCR");
      c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"lowMTCR");
      //  c = dataMCstackPlot("HT_",dirName,true,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"lowMTCR");
      
      dirName="ChargedHiggsTopBottom/charmCR_1Mu";
      c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"charmCR");
      //  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"charmCRCR");
      c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"charmCR");
      c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"charmCR");
      //  c = dataMCstackPlot("Mt_",dirName,false,lepCat,"charmCRCR");
      c = dataMCstackPlot("HT_",dirName,true,lepCat,"charmCRCR");
      c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"charmCR");
      c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"charmCR");
    */
    
  }

  /////////////////
  ///// doing 1ele

  if(lepCat==3) {

    dirName="ChargedHiggsTopBottom/topCR_1Ele";
    c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("Njets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("Mt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("Met_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    
    dirName="ChargedHiggsTopBottom/extraRadCR_1Ele";
    c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("Mt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");
    
    /*
      dirName="ChargedHiggsTopBottom/lowMTCR_1Ele";
      c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"lowMTCR");
      c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"lowMTCR");
      c = dataMCstackPlot("Met_",dirName,true,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"lowMTCR");
      c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"lowMTCR");
      
      dirName="ChargedHiggsTopBottom/charmCR_1Ele";
      c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"charmCR");
      //  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"charmCRCR");
      c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"charmCR");
      c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"charmCR");
      //  c = dataMCstackPlot("Mt_",dirName,false,lepCat,"charmCRCR");
      c = dataMCstackPlot("HT_",dirName,true,lepCat,"charmCRCR");
      c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"charmCR");
      c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"charmCR");
    */
  }

  //// doing 2l

  if(lepCat==1) {

    c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_2Mu",true,lepCat);
    
    //  dirName="ChargedHiggsTopBottom/PreselectionN1_2Mu";
    //  c = dataMCstackPlot("EtMiss_",dirName,true,lepCat);
    //  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
    //  c = dataMCstackPlot("Njets_",dirName,false,lepCat);
    /*
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
      c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat);
      c = dataMCstackPlot("dRlb_",dirName,false,lepCat);
      c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat);
      c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat);
      c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
      c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
      c = dataMCstackPlot("MT2bb_",dirName,false,lepCat);
      c = dataMCstackPlot("minMT_",dirName,false,lepCat);
      c = dataMCstackPlot("maxMT_",dirName,false,lepCat);
      c = dataMCstackPlot("totMT_",dirName,false,lepCat);
    */
    

    dirName="ChargedHiggsTopBottom/topCR_2Mu";
    c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
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
    c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("totMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    
    dirName="ChargedHiggsTopBottom/extraRadCR_2Mu";
    c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("minMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("totMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");

  }
  ///////
  
  if(lepCat==4) {

    c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_2Ele",true,lepCat);
    
    dirName="ChargedHiggsTopBottom/topCR_2Ele";
    c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("totMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    
    dirName="ChargedHiggsTopBottom/extraRadCR_2Ele";
    c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("minMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("totMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");

  //  return;
  }

  if(lepCat==2) {

    //// doing 2l
    
    c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_1Mu1Ele",true,lepCat);
    
    //  dirName="ChargedHiggsTopBottom/PreselectionN1_1Mu1Ele";
    //  c = dataMCstackPlot("EtMiss_",dirName,true,lepCat);
    //  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
    //  c = dataMCstackPlot("Njets_",dirName,false,lepCat);
    /*
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
      c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat);
      c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat);
      c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat);
      c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
      c = dataMCstackPlot("dRlb_",dirName,false,lepCat);
      c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
      c = dataMCstackPlot("minMT_",dirName,false,lepCat);
      c = dataMCstackPlot("maxMT_",dirName,false,lepCat);
      c = dataMCstackPlot("totMT_",dirName,false,lepCat);
    */
    
    dirName="ChargedHiggsTopBottom/topCR_1Mu1Ele";
    c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("minMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("totMT_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");


    dirName="ChargedHiggsTopBottom/extraRadCR_1Mu1Ele";
    c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"properties");
    c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"properties");
    //  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"properties");
    //  c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("minMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("totMT_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");

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

  }

}

void makeAllPlots(){

  dataMCstack(0); //1Mu  
  dataMCstack(3); //1Ele  
  dataMCstack(1); //2Mu
  dataMCstack(4); //2Ele  
  dataMCstack(2); //1Mu1Ele  
  
}

void makeAllTable(){

  makeTable(0); //1Mu
  makeTable(3); //1Ele

  makeTable(2); //1Mu1Ele
  makeTable(4); //2Ele
  makeTable(1); // 2Mu

}

