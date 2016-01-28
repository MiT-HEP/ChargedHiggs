#include "TH1D.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"

// this macro is used to draw the bdt
void drawBdt(int idx=0)
{
	_file0->cd("ChargedHiggsTmva/Vars");

	TCanvas *c=new TCanvas("c","c",800,800);

	TH1D*h;
	h= (TH1D*)gDirectory->Get(Form("Bdt%d_HplusToTauNu_M-200_13TeV_amcatnlo",idx))->Clone();
	h->SetLineColor(kBlack);
	h->SetLineWidth(2);
	h->GetXaxis()->SetTitle(Form("BDT%d",idx));
	h->GetXaxis()->SetRangeUser(-1,1);
	h->DrawNormalized("HIST");

	h= (TH1D*)gDirectory->Get(Form("Bdt%d_HplusToTauNu_M-400_13TeV_amcatnlo",idx))->Clone();
	h->SetLineColor(kBlue);
	h->SetLineWidth(2);
	h->DrawNormalized("HIST SAME");

	h=(TH1D*)gDirectory->Get( Form("Bdt%d_HplusToTauNu-M200",idx)) ->Clone();
	h->SetMarkerColor(kBlack);
	h->SetLineColor(kBlack);
	h->SetMarkerStyle(20);
	h->SetMarkerSize(0.7);
	h->DrawNormalized("P SAME");

	h=(TH1D*)gDirectory->Get( Form("Bdt%d_HplusToTauNu-M900",idx))->Clone();
	h->SetLineColor(kBlue);
	h->SetLineStyle(7);
	h->DrawNormalized("HIST SAME");

	// bkg
	h=(TH1D*)gDirectory->Get(Form("Bdt%d_QCD",idx))->Clone();
	h->SetLineColor(kRed);
	h->SetLineWidth(2);
	h->DrawNormalized("HIST SAME");	

	h=(TH1D*)gDirectory->Get( Form("Bdt%d_DY",idx))->Clone();
	h->SetLineColor(kCyan);	
	h->SetLineWidth(2);	
	h->SetLineStyle(7);	
	h->DrawNormalized("HIST SAME");	

	h=(TH1D*)gDirectory->Get(Form("Bdt%d_TTJets",idx))->Clone();
	h->SetLineColor(kMagenta+2);	
	h->SetLineWidth(2);	
	h->DrawNormalized("HIST SAME");	

	c->Update();
}
