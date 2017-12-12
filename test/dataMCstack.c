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

//double lumi=36.2;
double lumi=35.87;
//double lumi=12.9;
//double lumi=36.2;
double eff=1.;
//12878700468.905

bool doSyst=true;
bool doFinal=true;
bool doHF=true;

//bool doSyst=true;
//bool doFinal=true;


#include <iostream>
using namespace std;

TH1* RebinHT(TH1* h1, bool do2l) {

  TH1 *clone = (TH1 *) h1->Clone(Form("%s_rebin",h1->GetName()));

  //  clone->Rebin(8);

  Double_t newBins1L[34] = {0, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 
			    400, 420, 440, 460, 480, 500, 550,
			    600, 700, 800, 1000, 1250, 1500, 
			    2000, 3000, 4000, 5000, 8000 };
  
  if(!do2l) clone = h1->Rebin(33,Form("%s_rebin",h1->GetName()),newBins1L);

  Double_t newBins2L[40] = {0, 150, 160, 170, 180, 190, 200, 210, 220, 230, //10
			    240, 250, 260, 270, 280, 290, 300, 310, 320, 330, //10
			    340, 350, 360, 370, 380, 390, 400, 420, 440, 460, //10
			    480, 500, 550, 600, 700, 800, 1000, //8
			    2000, 4000, 8000 }; //3
    
  if(do2l) clone = h1->Rebin(39,Form("%s_rebin",h1->GetName()),newBins2L);

  for(int ibin=0; ibin<clone->GetNbinsX()+1 ; ibin++) {
    if(clone->GetBinContent(ibin)<0) cout << "negative yield: " << h1->GetName() << " " << clone->GetBinContent(ibin) << endl;
    if(clone->GetBinContent(ibin)<0) clone->SetBinContent(ibin, 0);
  }

  return clone;
  
}

TH1* RebinBDT1(TH1* h1) {

  TH1 *clone = (TH1 *) h1->Clone(Form("%s_rebin",h1->GetName()));

  Double_t newBins[69] = {-1, //1
			  -0.98,-0.95,-0.92, //3
			  -0.89,-0.86,-0.83,-0.8, //4
			  -0.75,-0.7, //2 
			  -0.65, -0.60, -0.55, -0.50, -0.45, -0.40, //6 
			  -0.35, -0.30, -0.25, -0.20, -0.15, -0.10, //6 
			  -0.05, 0.0, 0.05, 0.10, 0.15, 0.20, //6 
			  0.25,0.30,0.35,0.40, //4
			  0.45,0.50,0.55,0.60,0.65,0.68,0.70, //7 
			  /*
			    -0.68,-0.66,-0.64,-0.62,-0.60, //5 
			    -0.58,-0.56,-0.54,-0.52,-0.50, //5
			    -0.48,-0.46,-0.44,-0.42,-0.40, //5
			    -0.36,-0.32,-0.28,-0.24,-0.2, //5
			    -0.16,-0.12,-0.08,-0.04,0.0, 0.04,0.08,0.12,0.16,// 9
			    0.2, 0.24,0.28,0.32,0.36, //5
			    0.40,0.42,0.44,0.46,0.48,0.5, //6
			    0.50,0.52,0.54,0.56,0.58,0.6, //6
			    0.60,0.62,0.64,0.66,0.68,0.7, //6
			  */
			  0.71,0.72,0.73,0.74,0.75,0.76,0.77,0.78,0.79,0.8,  //10 // all discrimination is here
			  0.81,0.82,0.83,0.84,0.85,0.86,0.87,0.88,0.89,0.9,  //10  
			  0.91,0.92,0.93,0.94,0.95,0.96,0.97,0.98,0.99,  //9 
			  1.}; 
  
  clone =h1->Rebin(68,Form("%s_rebin",h1->GetName()),newBins);
  //  h1=h1.Rebin(112,"rebon",newBins);

  for(int ibin=0; ibin<clone->GetNbinsX()+1 ; ibin++) {
    if(clone->GetBinContent(ibin)<0) cout << "negative yield: " << h1->GetName() << " " << clone->GetBinContent(ibin) << endl;
    if(clone->GetBinContent(ibin)<0) clone->SetBinContent(ibin, 0);
  }

  return clone;

}

TH1* RebinBDT2(TH1* h1) {

  TH1 *clone = (TH1 *) h1->Clone(Form("%s_rebin",h1->GetName()));

  Double_t newBins[50] = {-1, //1
			  -0.90,-0.8,-0.75,-0.7, //4
			  -0.65,-0.6,-0.55,-0.50,-0.45,-0.40, //6
			  -0.36,-0.32,-0.29,-0.26,-0.23,-0.20,-0.17,-0.14, //8
			  -0.11,-0.08,-0.05,-0.02,0.01,0.04,0.07,0.10, //8
			  0.13,0.16,0.19,0.22,0.25,0.28,0.31,0.34,0.37,0.40, //10
			  0.43,0.46,0.49,0.52,0.55,0.58,0.61,0.64,0.67,0.70, //10
			  0.8,0.9, //2
			  1.}; //1
  
  clone =h1->Rebin(50-1,Form("%s_rebin",h1->GetName()),newBins);
  //  h1=h1.Rebin(113,"rebon",newBins);

  for(int ibin=0; ibin<clone->GetNbinsX()+1 ; ibin++) {
    if(clone->GetBinContent(ibin)<0) cout << "negative yield: " << h1->GetName() << " " << clone->GetBinContent(ibin) << endl;
    if(clone->GetBinContent(ibin)<0) clone->SetBinContent(ibin, 0);
  }

  return clone;

}


TH1* RebinBDT3(TH1* h1) {

  TH1 *clone = (TH1 *) h1->Clone(Form("%s_rebin",h1->GetName()));

  Double_t newBins[63] = {-1, //1
			  -0.90,-0.80, //2
			  -0.70,-0.60,-0.50,-0.45,-0.40,-0.35,-0.30, //7
			  -0.28,-0.26,-0.24,-0.22,-0.20,//5
			  -0.18,-0.16,-0.14,-0.12,-0.10, //5
			  -0.08,-0.06,-0.04,-0.02,0., //5
                           0.02,0.04,0.06,0.08,0.1, //5
                           0.12,0.14,0.16,0.18,0.2, //5
                           0.22,0.24,0.26,0.28,0.3, //5
                           0.32,0.34,0.36,0.38,0.4, //5
                           0.42,0.44,0.46,0.48,0.5, //5
                           0.52,0.54,0.56,0.58,0.6, //5
                           0.65,0.70,0.75,0.80,0.85, //5
			   0.9,0.95, //2
			   1.}; //1
  
  clone =h1->Rebin(63-1,Form("%s_rebin",h1->GetName()),newBins);
  //  h1=h1.Rebin(113,"rebon",newBins);

  for(int ibin=0; ibin<clone->GetNbinsX()+1 ; ibin++) {
    if(clone->GetBinContent(ibin)<0) cout << "negative yield: " << h1->GetName() << " " << clone->GetBinContent(ibin) << endl;
    if(clone->GetBinContent(ibin)<0) clone->SetBinContent(ibin, 0);
  }

  return clone;

}

TH1* RebinBDT4(TH1* h1) {

  TH1 *clone = (TH1 *) h1->Clone(Form("%s_rebin",h1->GetName()));

  Double_t newBins[39] = {-1, //1
			  -0.98,-0.95,-0.92,//3
			  -0.89,-0.86,-0.83,-0.8,//4
			  -0.75,-0.7, //2
			  -0.65, -0.60, -0.55, -0.50, -0.45, -0.40, //6
			  -0.35, -0.30, -0.25, -0.20, -0.15, -0.10, //6
			  -0.05, 0.0, 0.05, 0.10, 0.15, 0.20, //6
			  0.25,0.30, //2
			  0.32,0.34,0.36,0.38,0.40,0.45, //6
			  0.60,0.80, //2
			  1.}; //1
  
  clone =h1->Rebin(39-1,Form("%s_rebin",h1->GetName()),newBins);
  //  h1=h1.Rebin(113,"rebon",newBins);

  for(int ibin=0; ibin<clone->GetNbinsX()+1 ; ibin++) {
    if(clone->GetBinContent(ibin)<0) cout << "negative yield: " << h1->GetName() << " " << clone->GetBinContent(ibin) << endl;
    if(clone->GetBinContent(ibin)<0) clone->SetBinContent(ibin, 0);
  }

  return clone;

}

TH1* RebinBDT6(TH1* h1) {

  TH1 *clone = (TH1 *) h1->Clone(Form("%s_rebin",h1->GetName()));

  Double_t newBins[27] = {-1, //1
			  -0.7,-0.6,-0.5,-0.4, //4
			  -0.35,-0.3,-0.25,-0.20,-0.15,-0.10, //6
			  -0.05, 0.0, 0.05, 0.10, 0.15, 0.20, //6
			  0.25,0.30, //2
			  0.35,0.40, //2
			  0.50,0.60,0.70,0.80,0.90,//5
			  //			  0.55,0.80, //2
			  1.}; //1
  
  clone =h1->Rebin(27-1,Form("%s_rebin",h1->GetName()),newBins);
  //  h1=h1.Rebin(113,"rebon",newBins);

  for(int ibin=0; ibin<clone->GetNbinsX()+1 ; ibin++) {
    if(clone->GetBinContent(ibin)<0) cout << "negative yield: " << h1->GetName() << " " << clone->GetBinContent(ibin) << endl;
    if(clone->GetBinContent(ibin)<0) clone->SetBinContent(ibin, 0);
  }

  return clone;

}

TH1* getSQRT(TH1* h1B, TH1* h1S) {

  //  cout << "SQRT " << endl;
  TH1 *clone = (TH1 *) h1S->Clone("ciao");

  for(int ibin=0; ibin<clone->GetNbinsX()+1 ; ibin++) {

    if(h1B->GetBinContent(ibin)!=0) clone->SetBinContent(ibin,lumi*1000*h1S->GetBinContent(ibin)/sqrt(lumi*1000*h1B->GetBinContent(ibin)));
    //    if(h1B->GetBinContent(ibin)==0) clone->SetBinContent(ibin,0);
    //    clone->SetBinContent(ibin,lumi*1000*h1S->GetBinContent(ibin));

    //    clone->SetBinContent(ibin,lumi*1000*h1S->GetBinContent(ibin));

  }

  return clone;

}

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
			    (hmc->GetBinContent(km)-mcTOT_BTAGLdown->GetBinContent(km))*(hmc->GetBinContent(km)-mcTOT_BTAGLdown->GetBinContent(km)) +
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
  //  den1->Scale(-1.);
  //  den2->Scale(-1.);
  //  ratiop->Add(den1);
  //  ratiom->Add(den2);
  //  ratiop->Divide (den1);
  //  ratiom->Divide (den2);

  for (int km = 0; km < nvar; km++){

    if(ratio->GetBinContent(km+1)==0) {
      y1[km] = 1.;
      eyl1[km] = 0.;
      eyh1[km] = 0.;
    } else {
      y1[km] = 1;
      //      y1[km] =ratio->GetBinContent(km+1);
      eyl1[km] = fabs(ratiop->GetBinContent(km+1) - ratio->GetBinContent(km+1));
      eyh1[km] = fabs(ratiom->GetBinContent(km+1) - ratio->GetBinContent(km+1));
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

TCanvas * makeHFplots(TString HFplot="", TString subDirName="" , bool do2l = true) {

  //  bool do2l = true;
  bool doSigPlot = true;

  //  gStyle->SetOptStat(0);  
  //  gROOT->LoadMacro("utils.C++");
  //  gROOT->LoadMacro("utils.C");

  //  TString fileName="FEB15/1l.root";
  //  TString fileName2l="FEB15/2l.root";

  //  TString fileName="FEB26/1l.root";
  //  if(do2l) fileName="FEB26/2l.root";

  //  TString fileName="MAR1/1l.root";
  //  if(do2l) fileName="MAR1/2l.root";

  //  TString fileName="APR29/APR29_Final_1l.root";
  //  if(do2l) fileName="APR29/APR29_Final_2l.root";

  //  TString fileName="MAY9/MAY9_MCfinal_1l.root";
  TString fileName="MAY9/1l_MAY9_Final_2D.root";
  if(do2l) fileName="MAY10/2l_MAY10_Final_2D.root";
  //  if(do2l) fileName="MAY9/1l_MAY9_Final_2D.root";

  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="FEB9topMC/2l.root";

  TString HFlabel="";
  TString dirName=Form("%s_1Mu",subDirName.Data());
  if(do2l) dirName=Form("%s_2Mu",subDirName.Data());
  //  TString dirName=Form("%s_2Mu",subDirName.Data());
  //  TString dirName1Ele=Form("%s_1Ele",subDirName.Data());

  TCanvas *c = new TCanvas(HFplot,dirName+"_"+HFplot,10,10,700,700);
  
  TLegend* this_leg = new TLegend(0.6,0.6,0.8,0.8);
  this_leg->SetFillColor(0);
  //  this_leg->SetFillStyle(0);
  this_leg->SetBorderSize(0);
  
  /*
    if(LepCat==0) { dirName=Form("ChargedHiggsTopBottom/%s_1Mu",subDirName); histoName=Form("NBjets_%s",HFlabel);}
    if(LepCat==1) { dirName=Form("ChargedHiggsTopBottom/%s_2Mu",subDirName); histoName=Form("NBjets_%s",HFlabel);}
    if(LepCat==2) { dirName=Form("ChargedHiggsTopBottom/%s_1Mu1Ele",subDirName); histoName=Form("NBjets_%s",HFlabel);}
    if(LepCat==3) { dirName=Form("ChargedHiggsTopBottom/%s_1Ele",subDirName); histoName=Form("NBjets_%s",HFlabel);}
    if(LepCat==4) { dirName=Form("ChargedHiggsTopBottom/%s_2Ele",subDirName); histoName=Form("NBjets_%s",HFlabel);}
  */


  TString histoName3000 = Form("%s_ChargedHiggs_HplusTB_HplusToTB_M-3000_13TeV_amcatnlo_pythia8",HFplot.Data());
  TString histoName500 = Form("%s_ChargedHiggs_HplusTB_HplusToTB_M-500_13TeV_amcatnlo_pythia8",HFplot.Data());
  TString histoName300 = Form("%s_ChargedHiggs_HplusTB_HplusToTB_M-300_13TeV_amcatnlo_pythia8",HFplot.Data());

  cout << "fileName=" << fileName << " HFplot=" << HFplot.Data() ;
  cout << " subDirName=" << subDirName.Data() << " dirName=" << dirName << endl;

  int rebin=1;
  if(do2l) rebin=1;

  TH1 * h3000 = getHisto(fileName, dirName , histoName3000, kOrange, 1, rebin);
  TH1 * h500 = getHisto(fileName, dirName, histoName500, kRed, 1, rebin);
  TH1 * h300 = getHisto(fileName, dirName, histoName300, kMagenta, 1, rebin);
  dirName=Form("%s_1Ele",subDirName.Data());
  if(do2l) dirName=Form("%s_2Ele",subDirName.Data());
  TH1 * h3000_b = getHisto(fileName, dirName , histoName3000, kOrange, 1, rebin);
  TH1 * h500_b = getHisto(fileName, dirName, histoName500, kRed, 1, rebin);
  TH1 * h300_b = getHisto(fileName, dirName, histoName300, kMagenta, 1, rebin);
  h3000->Add(h3000_b);
  h500->Add(h500_b);
  h300->Add(h300_b);
  if(do2l) dirName=Form("%s_1Mu1Ele",subDirName.Data());
  TH1 * h3000_c = getHisto(fileName, dirName , histoName3000, kOrange, 1, rebin);
  TH1 * h500_c = getHisto(fileName, dirName, histoName500, kRed, 1, rebin);
  TH1 * h300_c = getHisto(fileName, dirName, histoName300, kMagenta, 1, rebin);
  if(h3000_c) h3000->Add(h3000_c);
  if(h500_c) h500->Add(h500_c);
  if(h300_c) h300->Add(h300_c);

  //  std::cout << "before Nbins = " << h3000->GetNbinsX() << std::endl;

  if(histoName3000.Contains("HT")) h3000=RebinHT(h3000,do2l);
  if(histoName500.Contains("HT")) h500=RebinHT(h500,do2l);
  if(histoName300.Contains("HT")) h300=RebinHT(h300,do2l);

  if(histoName3000.Contains("bdt1")) h3000=RebinBDT1(h3000);
  if(histoName500.Contains("bdt1")) h500=RebinBDT1(h500);
  if(histoName300.Contains("bdt1")) h300=RebinBDT1(h300);

  if(histoName3000.Contains("bdt4")) h3000=RebinBDT4(h3000);
  if(histoName500.Contains("bdt4")) h500=RebinBDT4(h500);
  if(histoName300.Contains("bdt4")) h300=RebinBDT4(h300);

  if(histoName3000.Contains("bdt2") || histoName3000.Contains("bdt5")) h3000=RebinBDT2(h3000);
  if(histoName500.Contains("bdt2") || histoName500.Contains("bdt5")) h500=RebinBDT2(h500);
  if(histoName300.Contains("bdt2") || histoName300.Contains("bdt5")) h300=RebinBDT2(h300);

  if(histoName3000.Contains("bdt3")) h3000=RebinBDT3(h3000);
  if(histoName500.Contains("bdt3")) h500=RebinBDT3(h500);
  if(histoName300.Contains("bdt3")) h300=RebinBDT3(h300);

  if(histoName3000.Contains("bdt6")) h3000=RebinBDT6(h3000);
  if(histoName500.Contains("bdt6")) h500=RebinBDT6(h500);
  if(histoName300.Contains("bdt6")) h300=RebinBDT6(h300);

  //  if(histoName3000.Contains("bdt6")) h3000->Rebin(2);
  //  if(histoName500.Contains("bdt6")) h500->Rebin(2);
  //  if(histoName300.Contains("bdt6")) h300->Rebin(2);

  //  if(do2l) h3000->Rebin(2);
  //  if(do2l) h500->Rebin(2);
  //  if(do2l) h300->Rebin(2);
  //  std::cout << "after Nbins = " << h3000->GetNbinsX() << std::endl;

  int color=1;

  double totLF=0;
  double totTT1b=0;
  double totTT2b=0;
  double totTT2bMer=0;
  double totTTc=0;

  double totLF_cut=0;
  double totTT1b_cut=0;
  double totTT2b_cut=0;
  double totTT2bMer_cut=0;
  double totTTc_cut=0;

  //  TH1* getSQRT(TH1* h1B, TH1* h1S) {

  for( int i=0; i<5; i++ ) {
    if(i==0) { HFlabel="ttlight"; color=1; }
    if(i==1) { HFlabel="ttc"; color=4;}
    if(i==2) { HFlabel="tt1b"; color=kGreen+1; }
    if(i==3) { HFlabel="tt2b"; color=kGreen+8; }
    if(i==4) { HFlabel="tt2bMerged"; color=kGreen-9; }

    //    TString histoName=Form("%s_%s_TTTo2L2Nu",HFplot.Data(),HFlabel.Data());
    //    TString histoName2l=Form("%s_%s_TTTo2L2Nu",HFplot.Data(),HFlabel.Data());
    //    TString histoName1l=Form("%s_%s_TTToSemilepton",HFplot.Data(),HFlabel.Data());

    TString histoName=Form("%s_%s_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8",HFplot.Data(),HFlabel.Data());
    //    TString histoName=Form("%s_%s_TTTo2L2Nu",HFplot.Data(),HFlabel.Data());

    dirName=Form("%s_1Mu",subDirName.Data());
    if(do2l) dirName=Form("%s_2Mu",subDirName.Data());
    TH1 * h = getHisto(fileName, dirName,  histoName, color , 1, rebin);

    dirName=Form("%s_1Ele",subDirName.Data());
    if(do2l) dirName=Form("%s_2Ele",subDirName.Data());
    TH1 * h_b = getHisto(fileName, dirName,  histoName, color , 1, rebin);
    h->Add(h_b);

    if(do2l) dirName=Form("%s_1Mu1Ele",subDirName.Data());
    TH1 * h_c = getHisto(fileName, dirName,  histoName, color , 1, rebin);
    if(do2l) h->Add(h_c);

    //    cout << "subDirName=" << subDirName.Data() << " HFlabel=" << HFlabel.Data() << " histoName=" << histoName << " events=" <<  h->Integral()  << endl;
    
    if(i==0) totLF=h->Integral();
    if(i==1) totTTc=h->Integral();
    if(i==2) totTT1b=h->Integral();
    if(i==3) totTT2b=h->Integral();
    if(i==4) totTT2bMer=h->Integral();

    if(i==0) totLF_cut=h->Integral(h->FindBin(0), h->FindBin(1));
    if(i==1) totTTc_cut=h->Integral(h->FindBin(0), h->FindBin(1));
    if(i==2) totTT1b_cut=h->Integral(h->FindBin(0), h->FindBin(1));
    if(i==3) totTT2b_cut=h->Integral(h->FindBin(0), h->FindBin(1));
    if(i==4) totTT2bMer_cut=h->Integral(h->FindBin(0), h->FindBin(1));

    //    cout << "integral=" << h->Integral(h->FindBin(0), h->FindBin(1)) << endl;

    if(histoName.Contains("bdt1")) h=RebinBDT1(h);
    if(histoName.Contains("bdt4")) h=RebinBDT4(h);
    if(histoName.Contains("bdt2") || histoName.Contains("bdt5")) h=RebinBDT2(h);
    if(histoName.Contains("bdt3")) h=RebinBDT3(h);
    if(histoName.Contains("bdt6")) h=RebinBDT6(h);

    if(histoName.Contains("HT")) h=RebinHT(h,do2l);
    //    if(do2l) h->Rebin(2);

    TH1 *allTT;
    if(i==0) allTT = (TH1 *) h->Clone();
    if(i>0) allTT->Add(h);

    TH1 *allHF;
    if(i==2) allHF = (TH1 *) h->Clone();
    if(i>2) allHF->Add(h);

    c->cd();
    if(i==0) h->SetTitle("");
    if(i==0) h->GetYaxis()->SetTitle("normalized entries");
    if(i==0) h->GetYaxis()->SetTitleOffset(1.5);
    if(i==0) h->GetYaxis()->SetLabelSize(0.035);
    if(i==0) h->GetXaxis()->SetLabelSize(0.035);
    if(i==0) h->GetYaxis()->SetTitleSize(0.035);
    if(i==0) h->GetXaxis()->SetTitleSize(0.035);
    if(i==0) h->DrawNormalized("hist");
    if(i!=0) h->DrawNormalized("hist sames");

    if(i==0) { HFlabel="ttlight"; color=1; }
    if(i==1) { HFlabel="ttc"; color=4;}
    if(i==1) { HFlabel="tt1b"; color=2; }
    if(i==2) { HFlabel="tt2b"; color=kGreen+1; }
    if(i==3) { HFlabel="tt2bMerged"; color=kGreen+1; }


    if(i==0 && !doSigPlot) this_leg->AddEntry(h, "t#bar{t} + LF" , "l");
    if(i==1 && !doSigPlot) this_leg->AddEntry(h, "t#bar{t} + c(#bar{c})" , "l");
    if(i==2 && !doSigPlot) this_leg->AddEntry(h, "t#bar{t} + 1b" , "l");
    if(i==3 && !doSigPlot) this_leg->AddEntry(h, "t#bar{t} + b#bar{b}" , "l");
    if(i==4 && !doSigPlot) this_leg->AddEntry(h, "t#bar{t} + 2B" , "l");

    if(allTT && doSigPlot) allTT->SetLineColor(1);
    if(allTT && doSigPlot) allTT->DrawNormalized("hist same");

  }

  if(h3000 && !doSigPlot) this_leg->AddEntry(h3000, "m(H^{+})=3000" , "l");
  if(h500 && !doSigPlot) this_leg->AddEntry(h500, "m(H^{+})=500" , "l");
  if(h300 && !doSigPlot) this_leg->AddEntry(h300, "m(H^{+})=300" , "l");

  if(h3000) h3000->DrawNormalized("hist same");
  if(h500) h500->DrawNormalized("hist same");
  if(h300) h300->DrawNormalized("hist same");

  //  if(h3000) this_leg->AddEntry(h3000, "m(H^{+})=3000 GeV" , "f");
  //  if(h500) this_leg->AddEntry(h500, "m(H^{+})=500 GeV" , "f");

  cout << "-------------------------" << endl;

  if(doSigPlot) {

    TH1* sign=getSQRT(allTT, h3000);
    sign->GetYaxis()->SetTitle("S/sqrt(B)");
    sign->SetTitle("");
    sign->DrawNormalized("hist");
    
    TH1* signM=getSQRT(allTT, h500);
    //  signM->SetLineStyle(2);
    signM->SetLineColor(kRed);
    signM->DrawNormalized("hist sames");
    
    TH1* signL=getSQRT(allTT, h300);
    //  signL->SetLineStyle(2);
    signL->SetLineColor(kBlue);
    signL->DrawNormalized("hist sames");
    
    TH1* signHF=getSQRT(allTT, allHF);
    signHF->SetLineColor(kGreen+1);
    signHF->DrawNormalized("hist sames");
    
    if(sign && doSigPlot) this_leg->AddEntry(sign, "m(H^{+})=3000 vs TT" , "l");
    if(signM && doSigPlot) this_leg->AddEntry(signM, "m(H^{+})=500 vs TT " , "l");
    if(signL && doSigPlot) this_leg->AddEntry(signL, "m(H^{+})=300 vs TT" , "l");
    if(signHF && doSigPlot) this_leg->AddEntry(signHF, " tt+HF vs tt" , "l");
    
    //  TH1* signHF=getSQRT(allTT, allHF);
    //  signHF->SetLineStyle(2);
    //  signHF->DrawNormalized("hist sames");
  }

  //////////

  double TOT=totLF + totTT1b + totTT2b + totTTc;
  double TOT_cut=totLF_cut + totTT1b_cut + totTT2b_cut + totTTc_cut;

  cout << "light"<< endl;
  cout << "TTc"<< endl;
  cout << "TT1b"<< endl;
  cout << "TT2b"<< endl;
  cout << "TT2bMer"<< endl;

  cout << "w/o BDT cut"<< endl;

  cout <<  Form("%.2f",totLF/TOT) << endl;
  cout <<  Form("%.2f",totTTc/TOT) << endl;
  cout <<  Form("%.2f",totTT1b/TOT) << endl;
  cout <<  Form("%.2f",totTT2b/TOT) << endl;
  cout <<  Form("%.2f",totTT2bMer/TOT) << endl;

  cout << "w/ BDT cut"<< endl;

  cout <<  Form("%.2f",totLF_cut/TOT_cut) << endl;
  cout <<  Form("%.2f",totTTc_cut/TOT_cut) << endl;
  cout <<  Form("%.2f",totTT1b_cut/TOT_cut) << endl;
  cout <<  Form("%.2f",totTT2b_cut/TOT_cut) << endl;
  cout <<  Form("%.2f",totTT2bMer_cut/TOT_cut) << endl;

  this_leg->Draw("");

  return c;

}

void drawHF() {

  TCanvas *c1 = new TCanvas("","",10,10,700,700);
  c1->Divide(2,2);

  gStyle->SetOptStat(0);
  gROOT->LoadMacro("utils.C");

  /*
  c1 = makeHFplots("NBjets","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("minDRbb","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("minDRbb_mass","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("thirdBDiscr","ChargedHiggsTopBottom/Baseline",false);

  return;
  //  TString HFplot="Ndiscr";
  //Ndiscr, minDRbb, minDRbb_mass, NBjets
  //  TString subDirName="topCR";
  c1 = makeHFplots("HT_SR1","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("HT_SR2","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("HT_SR3","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("HT_SR4","ChargedHiggsTopBottom/Baseline",false);

  return;

  c1 = makeHFplots("HT","ChargedHiggsTopBottom/topCR",false);
  c1 = makeHFplots("HT","ChargedHiggsTopBottom/charmCR",false);
  c1 = makeHFplots("HT","ChargedHiggsTopBottom/extraRadCR",false);

  return;
  */

  c1 = makeHFplots("bdt1","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("bdt2","ChargedHiggsTopBottom/Baseline",false);
  c1 = makeHFplots("bdt3","ChargedHiggsTopBottom/Baseline",false);
  
  c1 = makeHFplots("bdt4","ChargedHiggsTopBottom/Baseline",true);
  c1 = makeHFplots("bdt5","ChargedHiggsTopBottom/Baseline",true);
  c1 = makeHFplots("bdt6","ChargedHiggsTopBottom/Baseline",true);
 
  return;

  //  c1 = makeHFplots("HT","ChargedHiggsTopBottom/Baseline");
  //  return;

  c1 = makeHFplots("HT_SR1","ChargedHiggsTopBottom/Baseline");
  c1 = makeHFplots("HT_SR2","ChargedHiggsTopBottom/Baseline");
  c1 = makeHFplots("HT_SR3","ChargedHiggsTopBottom/Baseline");
  c1 = makeHFplots("HT_SR4","ChargedHiggsTopBottom/Baseline");

  return;

  /*

  c1 = makeHFplots("HT","ChargedHiggsTopBottom/topCR");
  c1 = makeHFplots("HT","ChargedHiggsTopBottom/charmCR");
  c1 = makeHFplots("HT","ChargedHiggsTopBottom/extraRadCR");
  */  

  /*
  cout << "-----------------------------------------" << endl;
  c1 = makeHFplots("NBjets","ChargedHiggsTopBottom/topCR");
  c1 = makeHFplots("NBjets","ChargedHiggsTopBottom/extraRadCR");
  c1 = makeHFplots("NBjets","ChargedHiggsTopBottom/charmCR");
  cout << "-----------------------------------------" << endl;

  c1 = makeHFplots("thirdBDiscr","ChargedHiggsTopBottom/Baseline");
  */

}


void makeTable( int LepCat=0 , TString MClabel="TTTo2L2Nu") {

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

  /*
  //CutFlowNoWei

  if(LepCat==0) { dirName="ChargedHiggsTopBottom/CutFlow_1Mu"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==1) { dirName="ChargedHiggsTopBottom/CutFlow_2Mu"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==2) { dirName="ChargedHiggsTopBottom/CutFlow_1Mu1Ele"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==3) { dirName="ChargedHiggsTopBottom/CutFlow_1Ele"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==4) { dirName="ChargedHiggsTopBottom/CutFlow_2Ele"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  */

  //  fileName="JAN25/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="JAN25/2l.root";

  //  fileName="FEB1/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="FEB1/2l.root";

  fileName="APR21/1l.root";
  //  fileName="FEB20/FEB21_TTbar_VetoEle.root";
  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="APR21/2l.root";

  if(doFinal) fileName="APR29/APR29_Final_1l.root";
  if(doFinal && (LepCat==1 || LepCat==2 || LepCat==4)) fileName="APR29/APR29_Final_2l.root";

  std::cout << " ====== " << dirName.Data() << "======" << histoName.Data() << "======" << fileName.Data() << std::endl;

  int color =1;
  int rebin=1;

  TH1 * h=getHisto (fileName, dirName, histoName, color, 1, rebin);
  //  if(h) h->Scale(lumi*1000*eff);

  //  cout << sampleName.Data() << "          &  " << int(h->GetBinContent(1)) << " & " << int(h->GetBinContent(2)) << " & " << int(h->GetBinContent(3)) << endl;

  std::cout << "total       & " << int(h->GetBinContent(1)) << std::endl;
  std::cout << "N(lep)>=1   & " << int(h->GetBinContent(2)) << std::endl;
  std::cout << "N(lep)==1   & " << int(h->GetBinContent(3)) << std::endl;
  std::cout << "N(tau)==0   & " << int(h->GetBinContent(4)) << std::endl;
  std::cout << "MET==30     & " << int(h->GetBinContent(5)) << std::endl;
  std::cout << "Njets>=5    & " << int(h->GetBinContent(7)) << std::endl;
  std::cout << "NB>=2       & " << int(h->GetBinContent(8)) << std::endl;

}

void makeBaselineCount( int LepCat=0 ) {

  gStyle->SetOptStat(0);  
  //  gROOT->LoadMacro("utils.C++");
  gROOT->LoadMacro("utils.C");

  TString histoName="HT_SR4_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8";
  TString dirName="";
  TString fileName="";


  if(LepCat==0) { dirName="ChargedHiggsTopBottom/Baseline_1Mu"; }
  if(LepCat==1) { dirName="ChargedHiggsTopBottom/Baseline_2Mu"; }
  if(LepCat==2) { dirName="ChargedHiggsTopBottom/Baseline_1Mu1Ele"; }
  if(LepCat==3) { dirName="ChargedHiggsTopBottom/Baseline_1Ele"; }
  if(LepCat==4) { dirName="ChargedHiggsTopBottom/Baseline_2Ele"; }

  /*
  //CutFlowNoWei

  if(LepCat==0) { dirName="ChargedHiggsTopBottom/CutFlow_1Mu"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==1) { dirName="ChargedHiggsTopBottom/CutFlow_2Mu"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==2) { dirName="ChargedHiggsTopBottom/CutFlow_1Mu1Ele"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==3) { dirName="ChargedHiggsTopBottom/CutFlow_1Ele"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  if(LepCat==4) { dirName="ChargedHiggsTopBottom/CutFlow_2Ele"; histoName=Form("CutFlow_%s",MClabel.Data()); }
  */

  //  fileName="JAN25/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="JAN25/2l.root";

  //  fileName="FEB1/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="FEB1/2l.root";

  //  fileName="MAY9/MAY9_final_1l.root";
  fileName="MAY9/1l_MAY9_Final_2D.root";
  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="MAY10/2l_MAY10_Final_2D.root";

  //  if(doFinal) fileName="FEB17/final_1l.root";
  //  if(doFinal && (LepCat==1 || LepCat==2 || LepCat==4)) fileName="FEB17/final_2l.root";

  std::cout << " ====== " << dirName.Data() << "======" << histoName.Data() << "======" << fileName.Data() << std::endl;

  int color =1;
  int rebin =1;

  TH1 * h=getHisto (fileName, dirName, histoName, color, 1, rebin);
  if(h) h->Scale(lumi*1000*eff);

  std::cout << "total       & " << h->Integral() << std::endl; 
  //  if(h) h->Scale(lumi*1000*eff);

  //  cout << sampleName.Data() << "          &  " << int(h->GetBinContent(1)) << " & " << int(h->GetBinContent(2)) << " & " << int(h->GetBinContent(3)) << endl;

  //  std::cout << "total       & " << int(h->GetBinContent(1)) << std::endl;
  //  std::cout << "N(lep)>=1   & " << int(h->GetBinContent(2)) << std::endl;
  //  std::cout << "N(lep)==1   & " << int(h->GetBinContent(3)) << std::endl;
  //  std::cout << "N(tau)==0   & " << int(h->GetBinContent(4)) << std::endl;
  //  std::cout << "MET==30     & " << int(h->GetBinContent(5)) << std::endl;
  //  std::cout << "Njets>=5    & " << int(h->GetBinContent(7)) << std::endl;
  //  std::cout << "NB>=2       & " << int(h->GetBinContent(8)) << std::endl;

}

TH1 *getDATA( TString fileName="", TString histoName="", TString dirName="", bool doLog=false, int LepCat=0) {
  TString sampleName="";
  TString sampleLabel="";
  int color =1;

  //  sampleName="Data"; sampleLabel="Data";  color=kBlack; 
  // MARIA TMP
  if(LepCat==0 || LepCat==1 || LepCat==2) sampleName="SingleMuon"; sampleLabel="SingleMuon";  color=kBlack;
  if(LepCat==3 || LepCat==4) sampleName="SingleElectron"; sampleLabel="SingleElectron";  color=kBlack;

  cout << "fileName=" << fileName << "histoName=" << histoName << "dirName=" << dirName << endl;
  TH1 * hdata=getHisto (fileName, dirName, histoName+sampleName, color, 1,1);
  
  if(histoName.Contains("bdt1_")) hdata=RebinBDT1(hdata);
  if(histoName.Contains("bdt4_")) hdata=RebinBDT4(hdata);
  if(histoName.Contains("bdt2_") || histoName.Contains("bdt5")) hdata=RebinBDT2(hdata);
  if(histoName.Contains("bdt3_")) hdata=RebinBDT3(hdata);
  if(histoName.Contains("bdt6_")) hdata=RebinBDT6(hdata);

  bool do2l=(LepCat==1 || LepCat==2 || LepCat==4);
  if(histoName.Contains("HT_") && (!histoName.Contains("HT_zoom"))) hdata=RebinHT(hdata,do2l);

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

  for(int sampleI=0; sampleI<25; sampleI++) {

    //    if(sampleI==0) { sampleName="TTJets_DiLept_PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1"; sampleLabel="TTJets_DiLept";  color=kRed;}   
    //    if(sampleI==1) { sampleName="TTJets_SingleLeptFromT"; sampleLabel="TTJets_SingleLeptFromT"; color=kBlue;}

    //    cout << "sampleI=" << sampleI << endl;
    if(sampleI==0) continue;
    //    if(sampleI==0) { sampleName="TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept";  color=kRed;}
    if(sampleI==1) { sampleName="TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"; sampleLabel="TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"; color=kBlue;}

    //    if(sampleI==0) { sampleName="TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept";  color=kRed;}
    //    if(sampleI==1) { sampleName="TTToSemilepton_TuneCUETP8M2"; sampleLabel="TTJets_SingleLeptFromT"; color=kBlue;}

    //    if(sampleI==2) { sampleName="ST"; sampleLabel="ST"; color=kMagenta;}
    if(sampleI==2) { sampleName="ST"; sampleLabel="ST"; color=kRed;}
    if(sampleI==3) { sampleName="WWTo"; sampleLabel="WW"; color=kGreen-8;}
    if(sampleI==4) { sampleName="WZTo"; sampleLabel="WZ"; color=kGreen-8;}
    if(sampleI==5) { sampleName="ZZTo"; sampleLabel="ZZ"; color=kGreen-8;}
    if(sampleI==6) { sampleName="VHToNonbb_M125"; sampleLabel="VHToNonbb_M125"; color=kGreen-8;}
    if(sampleI==7) { sampleName="WH_HToBB_WToLNu_M125"; sampleLabel="WH_HToBB_WToLNu_M125"; color=kGreen-8;}

    if(sampleI==8) { sampleName="DYJetsToLL_M-50_HT"; sampleLabel="DYJetsToLL_M-50_HT"; color=kGreen-10;}
    if(sampleI==9) { sampleName="WJetsToLNu_HT"; sampleLabel="WJetsToLNu_HT"; color=kGreen-9;}
    if(sampleI==10) { sampleName="TTTT"; sampleLabel="TTTT"; color=kOrange;}
    if(sampleI==11) { sampleName="TTW"; sampleLabel="TTW"; color=kOrange;}
    if(sampleI==12) { sampleName="TTZ"; sampleLabel="TTZ"; color=kOrange;}
    if(sampleI==13) { sampleName="TTG"; sampleLabel="TTG"; color=kOrange;}
    if(sampleI==14) { sampleName="tZq"; sampleLabel="tZq"; color=kOrange;}
    if(sampleI==15) { sampleName="ttH"; sampleLabel="ttH"; color=kOrange;}
    if(sampleI==16) { sampleName="WWW"; sampleLabel="WWW"; color=kGreen-8;}
    if(sampleI==17) { sampleName="WWZ"; sampleLabel="WWZ"; color=kGreen-8;}
    if(sampleI==18) { sampleName="WZZ"; sampleLabel="WZZ"; color=kGreen-8;}
    if(sampleI==19) { sampleName="ZZZ"; sampleLabel="ZZZ"; color=kGreen-8;}

    if(!doHF && sampleI>19) continue;
    //    if(sampleI==16) continue;
    //    if(sampleI==22) continue;

    //    if(sampleI==16) { sampleName="other_TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept_other";  color=kRed+1;}
    if(sampleI==20) { sampleName="tt1b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"; sampleLabel="TTJets_DiLept_tt1b";  color=kBlue-4;}
    if(sampleI==21) { sampleName="tt2b_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"; sampleLabel="TTJets_DiLept_tt2b";  color=kBlue-4;}
    if(sampleI==22) { sampleName="tt2bMerged_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"; sampleLabel="TTJets_DiLept_tt2b";  color=kBlue-4;}
    if(sampleI==23) { sampleName="ttc_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"; sampleLabel="TTJets_DiLept_ttc";  color=kBlue-9;}
    if(sampleI==24) { sampleName="ttlight_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"; sampleLabel="TTJets_DiLept_light";  color=kBlue-10;}

    //    if(sampleI==17) { sampleName="tt1b_TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept_tt1b";  color=kRed-4;}
    //    if(sampleI==18) { sampleName="tt2b_TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept_tt2b";  color=kRed-4;}
    //    if(sampleI==19) { sampleName="tt2bMerged_TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept_tt2b";  color=kRed-4;}
    //    if(sampleI==20) { sampleName="ttc_TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept_ttc";  color=kRed-9;}
    //    if(sampleI==21) { sampleName="ttlight_TTTo2L2Nu_TuneCUETP8M2"; sampleLabel="TTJets_DiLept_light";  color=kRed-10;}

    //    if(sampleI==22) { sampleName="other_TTToSemilepton_TuneCUETP8M2"; sampleLabel="TTJets_SingleLept_other";  color=kBlue+1;}
    //    if(sampleI==23) { sampleName="tt1b_TTToSemilepton_TuneCUETP8M2"; sampleLabel="TTJets_SingleLept_tt1b";  color=kBlue-4;}
    //    if(sampleI==24) { sampleName="tt2b_TTToSemilepton_TuneCUETP8M2"; sampleLabel="TTJets_SingleLept_tt2b";  color=kBlue-4;}
    //    if(sampleI==25) { sampleName="tt2bMerged_TTToSemilepton_TuneCUETP8M2"; sampleLabel="TTJets_SingleLept_tt2bMerged";  color=kBlue-4;}
    //    if(sampleI==26) { sampleName="ttc_TTToSemilepton_TuneCUETP8M2"; sampleLabel="TTJets_SingleLept_ttc";  color=kBlue-9;}
    //    if(sampleI==27) { sampleName="ttlight_TTToSemilepton_TuneCUETP8M2"; sampleLabel="TTJets_SingleLept_light";  color=kBlue-10;}

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

    /*
    // MARIA: this only for ICHEP ntuples
    if(LepCat==0) eff=(1-0.15); //1mu
    if(LepCat==1) eff=1-(0.15*0.15); // dimu
    if(LepCat==2) eff=(1-0.15); //mue

    if(LepCat==3) eff=(1-0.15); //1e
    if(LepCat==4) eff=1-(0.15*0.15); // diele
    */

    //    std::cout << "fileName=" << fileName.Data() << " histoName=" << histoName.Data() << " dirName=" << dirName.Data() << " " << histoName+sampleName+variation << std::endl;  

    TH1 * h=getHisto (fileName, dirName, histoName+sampleName+variation, color, 1,1);
    //    if(h) h->SetFillStyle(3354);
    if(h) {
      if(histoName.Contains("bdt1_")) h=RebinBDT1(h);
      if(histoName.Contains("bdt4_")) h=RebinBDT4(h);
      if(histoName.Contains("bdt2_") || histoName.Contains("bdt5")) h=RebinBDT2(h);
      if(histoName.Contains("bdt3_")) h=RebinBDT3(h);
      if(histoName.Contains("bdt6_")) h=RebinBDT6(h);
      bool do2l=(LepCat==1 || LepCat==2 || LepCat==4);
      //      if(histoName.Contains("HT_")) hdata=RebinHT(h,do2l);
      if(histoName.Contains("HT_") && (!histoName.Contains("HT_zoom"))) h=RebinHT(h,do2l);
    }

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

      if(doHF && ( histoName.Contains("AvCSVPt") || histoName.Contains("NBjets_") || histoName.Contains("minDRbb_") || histoName.Contains("minDRbb_mass_") || histoName.Contains("thirdBDiscr_") || histoName.Contains("bdt") || histoName.Contains("HT") || histoName.Contains("ST")) ) {

	if(doHF && h && sampleI==20) this_leg->AddEntry(h, "(t#bar{t} + 1b,2b,2B)" , "f");
	//	if(doHF && h && sampleI==17) this_leg->AddEntry(h, "(t#bar{t} + 1b,2b,2B), #rightarrow 2l" , "f");
	//	if(doHF && h && sampleI==17) this_leg->AddEntry(h, "(TT+1b) #rightarrow 2l" , "f");
	//	if(doHF && h && sampleI==18) this_leg->AddEntry(h, "(TT+2b) #rightarrow 2l" , "f");
	//	if(doHF && h && sampleI==19) this_leg->AddEntry(h, "(TT+2bMerged) #rightarrow 2l" , "f");
	if(doHF && h && sampleI==23) this_leg->AddEntry(h, "(t#bar{t} + c)" , "f");
	//	if(doHF && h && sampleI==20) this_leg->AddEntry(h, "(t#bar{t} + +c) #rightarrow 2l" , "f");
	if(doHF && h && sampleI==24) this_leg->AddEntry(h, "(t#bar{t} + LF)" , "f");
	//	if(doHF && h && sampleI==21) this_leg->AddEntry(h, "(t#bar{t} + LF) #rightarrow 2l" , "f");
	//	if(doHF && h && sampleI==16) this_leg->AddEntry(h, "(TT ??) + #rightarrow 2l" , "f");
	
	//	if(doHF && h && sampleI==23) this_leg->AddEntry(h, "(t#bar{t} + 1b,2b,2B)) #rightarrow 1l" , "f");
	//	if(doHF && h && sampleI==23) this_leg->AddEntry(h, "(TT+1b) #rightarrow 1l" , "f");
	//	if(doHF && h && sampleI==24) this_leg->AddEntry(h, "(TT+2b) #rightarrow 1l" , "f");
	//	if(doHF && h && sampleI==25) this_leg->AddEntry(h, "(TT+2bMerged) #rightarrow 2l" , "f");
	//	if(doHF && h && sampleI==26) this_leg->AddEntry(h, "(t#bar{t} + c) #rightarrow 1l" , "f");
	//	if(doHF && h && sampleI==27) this_leg->AddEntry(h, "(t#bar{t} + LF) #rightarrow 1l" , "f");
	//	if(doHF && h && sampleI==22) this_leg->AddEntry(h, "(tt??) #rightarrow 1l" , "f");

      } else {

	//      if(h && doHF && sampleI==0) this_leg->AddEntry(h, "TTJets #rightarrow 2l " , "f");
	//	if(h && sampleI==0) this_leg->AddEntry(h, "TTJets #rightarrow 2l " , "f");
	//	if(h && sampleI==1) this_leg->AddEntry(h, "TTJets #rightarrow 1l" , "f");
	if(h && sampleI==1) this_leg->AddEntry(h, "TTJets" , "f");

      }	

      if(h && sampleI==2) this_leg->AddEntry(h, "Single Top" , "f");
      //    if(h && sampleI==7) this_leg->AddEntry(h, "Diboson" , "f");
      if(h && sampleI==11) this_leg->AddEntry(h, "rares" , "f");
      //    if(h && sampleI==16) this_leg->AddEntry(h, "DY+jets" , "f");
      //    if(h && sampleI==17) this_leg->AddEntry(h, "W+jets" , "f");
      if(h && sampleI==3) this_leg->AddEntry(h, "EWK" , "f");
      
    }

    if(doHF && (histoName.Contains("NBjets_") || histoName.Contains("minDRbb_") || histoName.Contains("minDRbb_mass_") || histoName.Contains("thirdBDiscr_") || histoName.Contains("bdt") || histoName.Contains("HT") || histoName.Contains("ST")) ) {
      if(h && sampleI!=0 && sampleI!=1) hs->Add(h);
    } else {
      if(sampleI==16 || sampleI==22 ) cout << "Integral Other" << h->Integral() << endl;
      if(h) hs->Add(h);
    }

    //    if(h) this_leg->AddEntry(h, sampleLabel, "f");
    //    if(h) hs->GetHistogram()->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());

  }  

  return hs;

}


TCanvas *dataMCstackPlot( TString histoName="", TString dirName="", bool doLog=false, int LepCat=0, TString dirPlot="") {

  //  bool do1l=true;
  //  TString histoName="";
  //  TString dirName="";

  TLegend* this_leg;
  this_leg = new TLegend(0.6,0.6,0.4+0.5,0.4+0.5);
  if(histoName.Contains("Mva")) this_leg = new TLegend(0.15,0.6,0.15+0.5,0.35+0.5);
  this_leg->SetFillColor(0);
  //  this_leg->SetFillStyle(0);
  this_leg->SetBorderSize(0);

  //  TString fileName="FEB7/Final_1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="FEB7/Final_2l.root";

  //  TString fileName="FEB7plots/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="FEB7plots//2l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="FEB6moriond_2l.root";

  THStack *hs_JESUp;
  THStack *hs_JESDown;
  THStack *hs_BTAGBUp;
  THStack *hs_BTAGBDown;
  THStack *hs_BTAGLUp;
  THStack *hs_BTAGLDown;
  THStack *hs_PUUp;
  THStack *hs_PUDown;
  THStack *hs_UNCLUSTERUp;
  THStack *UNCLUSTERDown;

  //  TString fileName="APR1/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="APR1/2l.root";

  TString fileName="APR21/1l.root";
  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="APR21/2l.root";

  //  if(doFinal)  fileName="MAY9/MAY9_final_1l.root";
  if(doFinal)  fileName="MAY9/1l_MAY9_Final_2D.root";
  if(doFinal && (LepCat==1 || LepCat==2 || LepCat==4)) fileName="MAY10/2l_MAY10_Final_2D.root";


  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="APR12/apr21_2l_final.root";

  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="March13_2l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="March13_final_2l.root";

  //  TString fileName="MAR1/1l_final.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="MAR1/2l_final.root";
  //  TString fileName="MAR1/1l.root";
  //  if(LepCat==1 || LepCat==2 || LepCat==4) fileName="MAR1/2l.root";

  //  if(doFinal) fileName="FEB17/final_1l.root";
  //  if(doFinal && (LepCat==1 || LepCat==2 || LepCat==4)) fileName="FEB17/final_2l.root";

  THStack *hs = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"");

  if(doSyst) {

    hs_JESUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_JESUp");
    hs_JESDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_JESDown");
    
    /*
    hs_BTAGBUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGBUp");
    hs_BTAGBDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGBDown");
    hs_BTAGLUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGLUp");
    hs_BTAGLDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_BTAGLDown");
    
    hs_PUUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_PUUp");
    hs_PUDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_PUDown");
    hs_UNCLUSTERUp = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_UNCLUSTERUp");
    hs_UNCLUSTERDown = getMCStack(fileName,histoName,dirName,doLog,LepCat,this_leg,"_UNCLUSTERDown");
    */    
  }

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
  if(doLog) gPad->SetLogy(1);
  TH1* mcTOT=hs->GetHistogram(); // get tot MC from stack
  if(hdata) hdata->SetTitle("");
  
  if(!dirName.Contains("Baseline")) {
    // MARIA TMP
    if(hdata) hdata->Draw("p e");
    if(hs) hs->Draw("hist sames");
    //    if(hdata) hdata->Draw("p e");
    //    if(hs) hs->Draw("hist");
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
  latexLabel.DrawLatex(0.5, 0.9+0.04, Form("        L=%.1f /fb (13TeV)",lumi));
  if(LepCat==0) latexLabel.DrawLatex(0.13, 0.9+0.04, "1#mu");
  if(LepCat==1) latexLabel.DrawLatex(0.13, 0.9+0.04, "2#mu");
  if(LepCat==2) latexLabel.DrawLatex(0.13, 0.9+0.04, "e#mu");
  if(LepCat==3) latexLabel.DrawLatex(0.13, 0.9+0.04, "1e");
  if(LepCat==4) latexLabel.DrawLatex(0.13, 0.9+0.04, "ee");
  if((LepCat==0 || LepCat==3) && dirPlot.Contains("topCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+1,2b + 4jets");
  if((LepCat==1 || LepCat==2 || LepCat==4) && dirPlot.Contains("topCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+>=1,2 + 2jets");
  if((LepCat==0 || LepCat==3) && dirPlot.Contains("extraRadCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 1b + >=5jets");
  if((LepCat==1 || LepCat==2 || LepCat==4) && dirPlot.Contains("extraRadCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ 1b + >=3jets");
  if((LepCat==0 || LepCat==3) && dirPlot.Contains("charmCR")) latexLabel.DrawLatex(0.18, 0.9+0.04, "+ >=3b + 4jets");

  //  pad1->RedrawAxis();

  if(!dirName.Contains("Baseline")) {

    pad2->cd();
    //  TH1* mcTOT=hs->GetHistogram(); // get tot MC from stack
    TH1 *mcTOT = ((TH1 *)(hs->GetStack()->Last())); // the "SUM"

    TH1F *hdataRatio = (TH1F*) hdata->Clone();
    //    TH1F *hmcClone = (TH1F*) mcTOT->Clone();
    //    hmcClone->Scale(-1);
    //    hdataRatio->Add(hmcClone);
    hdataRatio->Divide(mcTOT);
    //  hdataRatio->GetYaxis()->SetRangeUser(0.,2.);
    //    hdataRatio->GetYaxis()->SetRangeUser(-0.5,0.5);
    hdataRatio->GetYaxis()->SetRangeUser(0.5,1.5);
    if(histoName.Contains("Mva")) hdataRatio->GetYaxis()->SetRangeUser(-1.,1.);
    hdataRatio->GetYaxis()->SetTitle("data/MC");
    hdataRatio->SetTitle("");
    hdataRatio->Draw("p e");


    TGraphAsymmErrors * toterree;

    if(doSyst) {
      
      TH1 *mcTOT_JESUp = ((TH1 *)(hs_JESUp->GetStack()->Last())); // the "SUM"
      TH1 *mcTOT_JESDown = ((TH1 *)(hs_JESDown->GetStack()->Last())); // the "SUM"
      
      /*
      TH1 *mcTOT_BTAGBUp = ((TH1 *)(hs_BTAGBUp->GetStack()->Last())); // the "SUM"
      TH1 *mcTOT_BTAGBDown = ((TH1 *)(hs_BTAGBDown->GetStack()->Last())); // the "SUM"
      
      TH1 *mcTOT_BTALBUp = ((TH1 *)(hs_BTAGBUp->GetStack()->Last())); // the "SUM"
      TH1 *mcTOT_BTAGLDown = ((TH1 *)(hs_BTAGBDown->GetStack()->Last())); // the "SUM"
      
      TH1 *mcTOT_PUUp = ((TH1 *)(hs_PUUp->GetStack()->Last())); // the "SUM"
      TH1 *mcTOT_PUDown = ((TH1 *)(hs_PUDown->GetStack()->Last())); // the "SUM"
      
      TH1 *mcTOT_UNCLUSTERUp = ((TH1 *)(hs_UNCLUSTERUp->GetStack()->Last())); // the "SUM"
      TH1 *mcTOT_UNCLUSTERDown = ((TH1 *)(hs_UNCLUSTERDown->GetStack()->Last())); // the "SUM"

      toterree= TOTerror(mcTOT,mcTOT_JESUp,mcTOT_JESDown,
			 mcTOT_BTAGBUp,mcTOT_BTAGBDown,mcTOT_BTALBUp,mcTOT_BTAGLDown,
			 mcTOT_PUUp,mcTOT_PUDown,mcTOT_UNCLUSTERUp,mcTOT_UNCLUSTERDown,
			 hdataRatio);
      */

      toterree= JESerror(mcTOT,mcTOT_JESUp,mcTOT_JESDown,
			 hdataRatio);

      TAxis *toterree_yaxis = toterree->GetYaxis ();
      TAxis *toterree_xaxis = toterree->GetXaxis ();
      //      toterree_yaxis->SetRangeUser(-0.5,0.5);
      toterree_yaxis->SetRangeUser(0.5,1.5);
      toterree_xaxis->SetRangeUser(0.,10.);
      if(histoName.Contains("Mva")) toterree_xaxis->SetRangeUser(-1.,1.);
      toterree->SetTitle(0);
      //      toterree_yaxis->SetTitle("data-MC/MC");
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
      
    }


    hdataRatio->Draw("p e sames");

    TLine *lineZero = new TLine(mcTOT->GetXaxis()->GetXmin(), 0.,  mcTOT->GetXaxis()->GetXmax(), 0.);
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


  TCanvas *c;


  //// doing 1l
  if(lepCat==0) {

    if(!doFinal) dirName="ChargedHiggsTopBottom/Baseline_1Mu";
    if(!doFinal) c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"Baseline");
    if(!doFinal) c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"Baseline");
    if(!doFinal) c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"Baseline");

    if(!doFinal) c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_1Mu",true,lepCat);
    
    //  dirName="ChargedHiggsTopBottom/PreselectionN1_1Mu";
    //  c = dataMCstackPlot("EtMiss_",dirName,true,lepCat);
    //  c = dataMCstackPlot("NBjets_",dirName,false,lepCat);
    //  c = dataMCstackPlot("Njets_",dirName,false,lepCat);
    //  c = dataMCstackPlot("Mt_",dirName,false,lepCat);
    
    dirName="ChargedHiggsTopBottom/topCR_1Mu";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Njets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MaxCSV_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRlb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRlb_mass_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    //  c = dataMCstackPlot("HT_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Mt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Met_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MT2bb1l_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("HemiOut_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Centrality_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MJJJmaxPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("AvDRJJJmaxPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"topCR");

    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_1ll_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_1lm_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_1lh_",dirName,false,lepCat,"topCR");

    //    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Vertices_",dirName,false,lepCat,"topCR");
    
    dirName="ChargedHiggsTopBottom/extraRadCR_1Mu";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("MaxCSV_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Mt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("MJJJmaxPT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("AvDRJJJmaxPT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_1ll_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_1lm_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_1lh_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Vertices_",dirName,false,lepCat,"extraRadCR");    

    dirName="ChargedHiggsTopBottom/charmCR_1Mu";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"charmCR");
    //  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"charmCRCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("MaxCSV_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("MJJJmaxPT_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("AvDRJJJmaxPT_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"charmCR");
    //  c = dataMCstackPlot("Mt_",dirName,false,lepCat,"charmCRCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"charmCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"charmCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2D_1ll_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2D_1lm_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2D_1lh_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"charmCR");

  }

  /////////////////
  ///// doing 1ele

  if(lepCat==3) {

    dirName="ChargedHiggsTopBottom/topCR_1Ele";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Njets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MaxCSV_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRlb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRlb_mass_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Mt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Met_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMva_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MT2bb1l_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("HemiOut_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Centrality_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MJJJmaxPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("AvDRJJJmaxPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_1ll_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_1lm_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_1lh_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Vertices_",dirName,false,lepCat,"topCR");    
    
    dirName="ChargedHiggsTopBottom/extraRadCR_1Ele";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("MaxCSV_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("MJJJmaxPT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("AvDRJJJmaxPT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Mt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMva_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_1ll_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_1lm_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_1lh_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");
    
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
    */
    
    dirName="ChargedHiggsTopBottom/charmCR_1Ele";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"charmCR");
    //  c = dataMCstackPlot("Njets_",dirName,false,lepCat,"charmCRCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("MaxCSV_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("MJJJmaxPT_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("AvDRJJJmaxPT_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"charmCR");
    //  c = dataMCstackPlot("Mt_",dirName,false,lepCat,"charmCRCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"charmCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"charmCR");
    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2D_1ll_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2D_1lm_",dirName,false,lepCat,"charmCR");
    c = dataMCstackPlot("bdt2D_1lh_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMva_",dirName,true,lepCat,"charmCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"charmCR");

  }

  //// doing 2l

  if(lepCat==1) {

    if(!doFinal) c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_2Mu",true,lepCat);
    
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
    if(!doFinal) c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMiniIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MT2bb1l_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("HemiOut_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Centrality_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRlb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRlb_mass_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minMT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("totMT_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("MJJJmaxPT_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("AvDRJJJmaxPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2ll_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2lm_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2lh_",dirName,false,lepCat,"topCR");
    
    dirName="ChargedHiggsTopBottom/extraRadCR_2Mu";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    //    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("minMT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("totMT_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2ll_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2lm_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2lh_",dirName,false,lepCat,"extraRadCR");

  }
  ///////
  
  if(lepCat==4) {

    if(!doFinal) c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_2Ele",true,lepCat);
    
    dirName="ChargedHiggsTopBottom/topCR_2Ele";
    if(!doFinal) c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMva_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMva_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMiniIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("MT2bb1l_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("HemiOut_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Centrality_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRlb_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRlb_mass_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minMT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("totMT_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2ll_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2lm_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2lh_",dirName,false,lepCat,"topCR");
    
    dirName="ChargedHiggsTopBottom/extraRadCR_2Ele";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    //    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMva_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMva_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMiniIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("minMT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("totMT_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2ll_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2lm_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2lh_",dirName,false,lepCat,"extraRadCR");

  //  return;
  }

  if(lepCat==2) {

    //// doing 2l
    
    if(!doFinal) c = dataMCstackPlot("CutFlow_","ChargedHiggsTopBottom/CutFlow_1Mu1Ele",true,lepCat);
    
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
    if(!doFinal) c = dataMCstackPlot("NBjets_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"topCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMva_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMva_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMiniIso_",dirName,true,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRbb_mass_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("minDRlb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minDRlb_mass_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"topCR");
    //    if(!doFinal) c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("MT2bb1l_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("HemiOut_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("Centrality_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("minMT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"topCR");
    if(!doFinal) c = dataMCstackPlot("totMT_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"topCR");
    //    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2ll_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2lm_",dirName,false,lepCat,"topCR");
    c = dataMCstackPlot("bdt2D_2lh_",dirName,false,lepCat,"topCR");

    dirName="ChargedHiggsTopBottom/extraRadCR_1Mu1Ele";
    if(!doFinal) c = dataMCstackPlot("Ncentraljets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Nforwardjets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Njets_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeadingBPt_",dirName,true,lepCat,"extraRadCR");
    //    if(!doFinal) c = dataMCstackPlot("thirdBDiscr_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("AvCSVPt_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("HT_zoom_",dirName,true,lepCat,"extraRadCR");
    c = dataMCstackPlot("ST_zoom_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("Met_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMva_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonMiniIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailEta_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailPt_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMva_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("LeptonTrailMiniIso_",dirName,true,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonM_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("DiLeptonPT_",dirName,false,lepCat,"extraRadCR");
    //  c = dataMCstackPlot("minDRbb_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("dRlb_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("MT2ll_",dirName,false,lepCat,"extraRadCR");
    //  c = dataMCstackPlot("MT2bb_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("minMT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("maxMT_",dirName,false,lepCat,"extraRadCR");
    if(!doFinal) c = dataMCstackPlot("totMT_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt1_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt2_",dirName,false,lepCat,"extraRadCR");
    //    c = dataMCstackPlot("bdt3_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt4_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt5_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt6_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2ll_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2lm_",dirName,false,lepCat,"extraRadCR");
    c = dataMCstackPlot("bdt2D_2lh_",dirName,false,lepCat,"extraRadCR");

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

  /*
  makeTable(0,"ChargedHiggs_HplusTB_HplusToTB_M-500_13TeV_amcatnlo_pythia8"); //1Mu
  makeTable(3,"ChargedHiggs_HplusTB_HplusToTB_M-500_13TeV_amcatnlo_pythia8"); //1Ele
  makeTable(2,"ChargedHiggs_HplusTB_HplusToTB_M-500_13TeV_amcatnlo_pythia8"); //1Mu1Ele
  makeTable(4,"ChargedHiggs_HplusTB_HplusToTB_M-500_13TeV_amcatnlo_pythia8"); //2Ele
  makeTable(1,"ChargedHiggs_HplusTB_HplusToTB_M-500_13TeV_amcatnlo_pythia8"); // 2Mu
  //////
  */

  makeTable(0,"TTTo2L2Nu"); //1Mu
  makeTable(3,"TTTo2L2Nu"); //1Ele

  makeTable(0,"TTToSemilepton"); //1Mu
  makeTable(3,"TTToSemilepton"); //1Ele

  /*
  makeTable(2,"TTTo2L2Nu"); //1Mu1Ele
  makeTable(4,"TTTo2L2Nu"); //2Ele
  makeTable(1,"TTTo2L2Nu"); // 2Mu

  ///////

  makeTable(0,"TTToSemilepton"); //1Mu
  makeTable(3,"TTToSemilepton"); //1Ele

  makeTable(2,"TTToSemilepton"); //1Mu1Ele
  makeTable(4,"TTToSemilepton"); //2Ele
  makeTable(1,"TTToSemilepton"); // 2Mu
  */

}

