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

const double aaano = 35.87;
const int bkgtype = 7;

TString bkgname[bkgtype] = {"VV_QCD","EWKV","Zinv","Winv","ttbar","QCD","VV_AQGC_EWK"};
TString bkgshort[bkgtype] = {"VVQCD","EWKV","Zinv","W+jets","t#bar{t}","QCD","VVEWK"};
//const int colorbkg[bkgtype] = {kOrange+9,kBlue-7,kOrange-9,kMagenta-2,kGreen+2,kAzure-4,kOrange};
const int colorbkg[bkgtype] = {kGreen+2, kYellow-9, kOrange+1, kOrange-9,kAzure-4,kOrange-2,kOrange+9};




void postfitaQGC(TString cat, TString ana, TString pro){

        setTDRStyle();


	//TFile* f1 = TFile::Open(Form("/afs/cern.ch/user/h/hum/work/private/CMSSW_10_2_13/src/ChargedHiggs/fitDiagnosticsNanoVsystModBin_%s_%s_VV.root",ana.Data(), cat.Data())  );
	//TFile* f2 = TFile::Open(Form("/afs/cern.ch/user/h/hum/work/private/CMSSW_10_2_13/src/ChargedHiggs/Datacards/NanoV2NewBinning/cms_vbshad_%s_%s.inputs.root", ana.Data(), cat.Data()));

	TFile* f1 = TFile::Open(Form("/afs/cern.ch/user/h/hum/work/private/CMSSW_10_2_13/src/ChargedHiggs/aQGC/fitDiagnosticsMAR24_2021_MVV_%s_SR_VVEWK.root",cat.Data() ));
	TFile* f2 = TFile::Open(Form("/afs/cern.ch/user/h/hum/work/private/CMSSW_10_2_13/src/ChargedHiggs/aQGC/Datacards/MAR24/cms_vbshad_2021_5_aqgc_ft5_MVV_%s_SR.inputs.root", cat.Data()));
        TFile* f3 = TFile::Open(Form("/afs/cern.ch/user/h/hum/work/private/CMSSW_10_2_13/src/ChargedHiggs/aQGC/Datacards/MAR24/cms_vbshad_2021_5_aqgc_fs1_MVV_%s_SR.inputs.root", cat.Data()));



        vector<TDirectoryFile*> catep;
        vector<TDirectoryFile*> cater;
	vector<TH1D*> cateorg;

	cater.push_back( (TDirectoryFile*)f1->Get(Form("shapes_fit_s/%s" ,cat.Data())) );

        catep.push_back( (TDirectoryFile*)f1->Get(Form("shapes_fit_s/%s" ,cat.Data())) );

        //cater.push_back( (TDirectoryFile*)f1->Get(Form("shapes_prefit/%s" ,cat.Data())) );

	//catep.push_back( (TDirectoryFile*)f1->Get(Form("shapes_prefit/%s" ,cat.Data())) );

	//cater.push_back( (TDirectoryFile*)f1->Get("shapes_prefit/ch1"));
	//catep.push_back( (TDirectoryFile*)f1->Get("shapes_prefit/ch1"));

	//cateorg.push_back( (TH1D*)f2->Get(Form("%s_data_obs",cat.Data())) );
	cateorg.push_back( (TH1D*)f2->Get(Form("%s_ttbar",cat.Data())) );

	THStack *cateyields = new THStack("cateyield","");

	TH1D* ht[bkgtype];
        TH1D* htbin[bkgtype];
	TH1D* tot_bkg;
	TH1D* tot_sig;
	TH1D* tot_sigbin;
	TH1D* tot_sig2;
        TH1D* tot_sigbin2;
	TH1D* tot_sig3;
        TH1D* tot_sigbin3;

                
        TGraphAsymmErrors* fit_data;

        TGraphErrors* bkgall = new TGraphErrors();
        TGraphErrors* HhRatio = new TGraphErrors();


        double x[100] = {0.};
        double y[100] = {0.};
        double xrl[100] = {0.};
        double xrh[100] = {0.};
        double yrl[100] = {0.};
        double yrh[100] = {0.};


        double xd[100] = {0.};
        double yd[100] = {0.};
        double xdrl[100] = {0.};
        double xdrh[100] = {0.};
        double ydrl[100] = {0.};
        double ydrh[100] = {0.};


	double upperlim = 0;
        double lowerlim = 0;
	int flaglim = 0;
	int NBins = 0;
	double xend = -1.;
        double xbegin = -1.;

        for(Int_t i=0; i<1; i++) {

                if(catep[i]==NULL) continue;


		tot_sig  = (TH1D*)f2->Get(Form("%s_VV_AQGC_EWK_5p20",cat.Data()));
		tot_sig2 = (TH1D*)f2->Get(Form("%s_VV_AQGC_EWK_5p20",cat.Data()));
		tot_sig3 = (TH1D*)f3->Get(Form("%s_VV_AQGC_EWK_24p00",cat.Data()));
		
		tot_bkg = (TH1D*)catep[i]->Get("total");
	        //tot_bkg = (TH1D*)catep[i]->Get("total_background");
                fit_data = (TGraphAsymmErrors*)catep[i]->Get("data");

                if(fit_data == NULL or tot_bkg == NULL) continue;

                NBins = cateorg[i]->GetNbinsX();
		xbegin = cateorg[i]->GetXaxis()->GetXmin();
		xend = cateorg[i]->GetXaxis()->GetXmax();
		// signal *************************//
                tot_sigbin = (TH1D*)cateorg[i]->Clone("sig");
		tot_sigbin2 = (TH1D*)cateorg[i]->Clone("sig2");
		tot_sigbin3 = (TH1D*)cateorg[i]->Clone("sig3");
                for(int k=1; k<=NBins; k++){
                        double xwidth = cateorg[i]->GetBinWidth(k);
                        if (NBins == 1) xwidth = 5000;
                        //else if(k == 1) xwidth = cateorg[i]->GetBinWidth(k) - 500;
                        //else if(k == NBins) xwidth = cateorg[i]->GetBinWidth(k) - 5500;
                        tot_sigbin->SetBinContent(k,tot_sig->GetBinContent(k)/xwidth);
			tot_sigbin2->SetBinContent(k,tot_sig2->GetBinContent(k)/xwidth);
			tot_sigbin3->SetBinContent(k,tot_sig3->GetBinContent(k)/xwidth);
                }

                tot_sigbin->SetLineWidth(2);
                tot_sigbin->SetLineColor(kBlue+2);

		tot_sigbin2->SetLineWidth(2);
                tot_sigbin2->SetLineColor(kRed+1);

		tot_sigbin3->SetLineWidth(2);
                tot_sigbin3->SetLineColor(kViolet);

		////////////////////////////////////

		//tot_bkg
                for(int k=1; k<=NBins; k++){
                                
			if(tot_bkg->GetBinContent(k)/cateorg[i]->GetBinWidth(k) > upperlim) upperlim = tot_bkg->GetBinContent(k)/cateorg[i]->GetBinWidth(k);
                        if (NBins == 1 && tot_bkg->GetBinContent(k)/5000 > upperlim) upperlim = tot_bkg->GetBinContent(k)/5000;
                        double xbincenter = cateorg[i]->GetXaxis()->GetBinCenter(k);
                        double xbinerr = (cateorg[i]->GetBinWidth(k))/2;
                        double xwidth = cateorg[i]->GetBinWidth(k);

                        if (NBins == 1){cout << "1 la" << endl;
                        	xwidth = 5000;
                                bkgall->SetPoint(k-1,2500,tot_bkg->GetBinContent(k)/xwidth);
                                bkgall->SetPointError(k,2500,tot_bkg->GetBinError(k)/xwidth);
                                HhRatio->SetPoint(k-1,2500,1.0);
                                HhRatio->SetPointError(k-1,2500,tot_bkg->GetBinError(k)/tot_bkg->GetBinContent(k));
                        //}else if(k == 1){
                        //        xwidth-=500;
                        //        bkgall->SetPoint(k,xbincenter+250,m1000tass->GetBinContent(k+1)/xwidth);
                        //        bkgall->SetPointError(k,xbinerr-250,m1000tass->GetBinError(k+1)/xwidth);
                        //        HhRatio->SetPoint(k,xbincenter+250,1.0);
                        //        HhRatio->SetPointError(k,xbinerr-250,m1000tass->GetBinError(k+1)/m1000tass->GetBinContent(k+1));
			//}else if(k == NBins){
                        //        xwidth = 2500 - (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2);
                        //        bkgall->SetPoint(k,(2500 + (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2, m1000tass->GetBinContent(k+1)/xwidth);
                        //        bkgall->SetPointError(k,(2500 - (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2,m1000tass->GetBinError(k+1)/xwidth);
                        //        HhRatio->SetPoint(k,(2500 + (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2,1.0);
                        //        HhRatio->SetPointError(k,(2500 - (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2,m1000tass->GetBinError(k+1)/m1000tass->GetBinContent(k+1));
                        }else{
                                bkgall->SetPoint(k-1,xbincenter,tot_bkg->GetBinContent(k)/xwidth);
                                bkgall->SetPointError(k-1,xbinerr,tot_bkg->GetBinError(k)/xwidth);
                                HhRatio->SetPoint(k-1,xbincenter,1.0);
                                HhRatio->SetPointError(k-1,xbinerr,tot_bkg->GetBinError(k)/tot_bkg->GetBinContent(k));
                        }
                }



		bkgall->SetFillColor(kBlack);
		bkgall->SetFillStyle(3004);


                for(int j=0; j<bkgtype; j++){

                        char copyname[10];
                        sprintf(copyname, "htbin_%i", j);

                        ht[j] = (TH1D*)catep[i]->Get(bkgname[j]);
                        htbin[j] = (TH1D*)cateorg[i]->Clone(copyname);
                        if (NBins == 1) htbin[j] = new TH1D(copyname,copyname,1,0,5000);
                        if(ht[j] != NULL && htbin[j]!= NULL){
                                for(int k=0; k<NBins; k++){
                                        double xwidth = cateorg[i]->GetBinWidth(k+1);
                                        if (NBins == 1){xwidth = 5000;}
                                        //else if(k==0) xwidth = cateorg[i]->GetBinWidth(k+1) - 500;
                                        //else if(k+1 == NBins) xwidth = cateorg[i]->GetBinWidth(k+1) - 5500;
                                        htbin[j]->SetBinContent(k+1,ht[j]->GetBinContent(k+1)/xwidth);
                               		if (!flaglim && lowerlim < ht[j]->GetBinContent(k+1)/xwidth) lowerlim = ht[j]->GetBinContent(k+1)/xwidth;

				}
                                if(lowerlim<1000) flaglim = 1; 
                                htbin[j]->SetFillColor(colorbkg[j]);
                                htbin[j]->SetLineColor(colorbkg[j]);
                                cateyields->Add(htbin[j]);

				cout << bkgname[j] << " " <<  250*htbin[j]->Integral() << endl;


                        }

                }




		//data**********************************************************
		for(int k=0; k<NBins; k++){

                	double xbincenter = cateorg[i]->GetXaxis()->GetBinCenter(k+1);
                        double xbinerr = (cateorg[i]->GetBinWidth(k+1))/2;
                        double xwidth = cateorg[i]->GetBinWidth(k+1);
                        if (NBins == 1){xwidth = 5000;xbincenter=2500; xbinerr=2500;}
                        //else if(k==0) {xwidth = cateorg[i]->GetBinWidth(k+1) - 500; xbincenter+=250; xbinerr-=250;}
                        //else if(k+1 == NBins) xwidth = cateorg[i]->GetBinWidth(k+1) - 5500;

                        fit_data->GetPoint(k,x[k],y[k]);
                        x[k] = xbincenter;
                        xd[k] = xbincenter;
                        yd[k] = y[k]/xwidth;
                        if (NBins == 1) cout << "data: << " << yd[k] << endl;

                        if(tot_bkg->GetBinContent(k+1)!=0){
                        	y[k] /= tot_bkg->GetBinContent(k+1);
                                xrl[k] = xbinerr;
                                xrh[k] = xbinerr;
                                xdrl[k] = xbinerr;
                                xdrh[k] = xbinerr;
                                yrl[k] = fit_data->GetErrorYlow(k)/tot_bkg->GetBinContent(k+1);
                                yrh[k] = fit_data->GetErrorYhigh(k)/tot_bkg->GetBinContent(k+1);
                                ydrl[k] = fit_data->GetErrorYlow(k)/xwidth;
                                ydrh[k] = fit_data->GetErrorYhigh(k)/xwidth;
                        }

			/*
                        if(k+1 == NBins && NBins > 1){
                        	x[k] = (2500 + (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2;
                                xd[k] = (2500 + (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2;

                                xrl[k] = (2500 - (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2;
                                xrh[k] = (2500 - (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2;

                                xdrl[k] = (2500 - (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2;
                                xdrh[k] = (2500 - (cateorg[i]->GetXaxis()->GetBinCenter(k) + (cateorg[i]->GetBinWidth(k))/2))/2;
                        }
			*/
                }



	}


	TGraphAsymmErrors* ddbin = new TGraphAsymmErrors(NBins,xd,yd,xdrl,xdrh,ydrl,ydrh);
	ddbin->SetMarkerStyle(20);
        ddbin->SetMarkerColor(kBlack);
        ddbin->SetMarkerSize(1.5);


	tot_bkg->SetLineColor(kWhite);
	tot_bkg->GetXaxis()->SetRangeUser(500,xend);


	cout << "debug" << endl;

        TCanvas *c1a = new TCanvas("c1a","c1a",900,1200);

	//c1a->cd();
	//c1a->SetLogy();
	
        TPad *pad_up = new TPad("pad_up", "pad_up", 0., 0.25, 1., 1.);
        TPad *pad_dn = new TPad("pad_dn", "pad_dn", 0., 0., 1., 0.25);

        pad_up->SetBottomMargin(0);
        pad_up->SetLeftMargin(0.13);
        pad_up->SetRightMargin(0.07);
        pad_up->SetTickx(1);
        pad_up->SetTicky(1);
        pad_up->SetLogy();

        pad_dn->SetTopMargin(0);
        pad_dn->SetBottomMargin(0.50);
        pad_dn->SetLeftMargin(0.13);
        pad_dn->SetRightMargin(0.07);
        pad_dn->SetTickx(1);
        pad_dn->SetTicky(1);

        pad_up->Draw();
        pad_up->cd();
	

	cout << "zenmel000" << endl;



        cateyields->Draw("hist");
        bkgall->Draw("e2 same");
        tot_sigbin->Draw("hist same");
	//tot_sigbin2->Draw("hist same");
	tot_sigbin3->Draw("hist same");
	//ddbin->Draw("P e same");


	cout << "zenmel0" << endl;

        const char *lumien = "#scale[1.2]{#bf{137 fb^{-1} (13TeV)}}";
        //const char *lumien = "#scale[1.2]{#bf{35.9 fb^{-1}(13TeV 2016)}}";
        TLatex latex;
        latex.SetNDC();
        latex.SetTextSize(0.03);
        latex.SetTextAlign(11);
        //latex.DrawLatex(0.58, 0.91, lumien);
        latex.DrawLatex(0.69, 0.93, lumien);
        latex.SetTextSize(0.05);
        latex.DrawLatex(0.15, 0.84, "CMS");

	cateyields->GetXaxis()->SetLimits(500, xend);	
	cateyields->SetMaximum(1000*upperlim);
	cateyields->SetMinimum(std::max(lowerlim/100000,0.00001));
//        cateyields->SetMaximum(1000*upperlim);
//        cateyields->SetMinimum(100000000*lowerlim);
        //cateyields->GetYaxis()->SetLimits(1.,16000.0);
        cateyields->GetYaxis()->SetTitle("Events/GeV");
        cateyields->GetYaxis()->SetTitleFont(43);
        cateyields->GetYaxis()->SetTitleOffset(1.8);
        cateyields->GetYaxis()->SetTitleSize(39);
        cateyields->GetXaxis()->SetTitle("m_{jj}");
/*
	for(int ibin = 1; ibin<=10; ibin++){
		char binnum[50];
		sprintf(binnum,"%i",ibin*20*25);
		cateyields->GetXaxis()->SetBinLabel(ibin*20,binnum);
	}
*/

        TLegend *legend0 = new TLegend(0.29,0.64,0.59,0.89);
        legend0->SetFillStyle(0);
        legend0->SetBorderSize(0);
        legend0->AddEntry(bkgall,"Bkg. unc.","f");

	legend0->AddEntry(tot_sigbin,"f_{T7}/#Lambda^{4} = 5.2 TeV^{-4}","l");
	//legend0->AddEntry(tot_sigbin,"EWK W#pmW#pm","l");
	//legend0->AddEntry(tot_sigbin2,"EWK W#pmW#mp","l");
	//legend0->AddEntry(tot_sigbin3,"EWK ZZ","l");

        legend0->AddEntry(tot_sigbin3,"f_{S1}/#Lambda^{4} = 24 TeV^{-4}","l");

	//legend0->AddEntry(ddbin,"Data","LP");
        //legend0->AddEntry(bkgall,"f_{S0}/#Lambda^{4} = 20.0 TeV^{-4}","l");
        legend0->Draw("same");

	cout << "zenmel1" << endl;


        TLegend *legend1 = new TLegend(0.6,0.6,0.90,0.88);
        legend1->SetFillStyle(0);
        legend1->SetBorderSize(0);
        for(int j=bkgtype-1; j>=0; j--){
                        if((j>0 && colorbkg[j]==colorbkg[j-1] && ht[j-1] != NULL) || (ht[j] == NULL))continue;
                        legend1->AddEntry(htbin[j],bkgshort[j],"f");
        }
        legend1->Draw("same");

	c1a->cd();
        pad_dn->Draw();
        pad_dn->cd();


	cout << "zenmel" << endl;

        HhRatio->SetFillColor(kBlack);
        HhRatio->SetFillStyle(3004);

	TH1D* hRatio = NULL;
        //if (ana == "Mjj") hRatio = new TH1D("hRatio","",NBins,0,5000);
	//else 

	hRatio = new TH1D("hRatio","",NBins,xbegin, xend);

        for (int kai=0; kai < NBins; kai++) hRatio->SetBinContent(kai+1, 1.0);


        hRatio->SetLineStyle(7);
        hRatio->SetLineWidth(1);
        hRatio->SetLineColor(kBlack);

        hRatio->GetXaxis()->SetTitle(ana);
        hRatio->GetXaxis()->SetRangeUser(500,xend);
        hRatio->GetYaxis()->SetTitle("Data/BKG");
        hRatio->GetYaxis()->SetLabelFont(43);
        hRatio->GetYaxis()->SetRangeUser(0.15,1.75);
        hRatio->GetYaxis()->SetTitleOffset(1.8);
        hRatio->GetXaxis()->SetTitleOffset(4.5);
        hRatio->GetXaxis()->SetLabelFont(43);
        hRatio->GetYaxis()->SetTitleFont(43);
        hRatio->GetXaxis()->SetTitleFont(43);
        hRatio->GetYaxis()->SetLabelSize(27);
        hRatio->GetXaxis()->SetLabelSize(39);
        hRatio->GetYaxis()->SetTitleSize(39);
        hRatio->GetXaxis()->SetTitleSize(48);
        hRatio->GetXaxis()->SetNdivisions(8, kTRUE);
        hRatio->GetYaxis()->SetNdivisions(5, kTRUE);
        hRatio->Draw("same");
        HhRatio->Draw("E2 same");

	double debugx, debugy;
	HhRatio->GetPoint(3,debugx, debugy);

	//cout << debugx << "," << debugy << "shenmea" << endl;
	for(int a = 0; a< NBins; a++){
	//if (HhRatio->GetErrorY(a) > 0.8)
	cout << a << "," << HhRatio->GetErrorY(a) << "shenmea" << endl;
	}
                TGraphAsymmErrors* gr = new TGraphAsymmErrors(NBins,x,y,xrl,xrh,yrl,yrh);
                gr->SetMarkerStyle(20);
                gr->SetMarkerColor(kBlack);
                gr->SetMarkerSize(1.5);
                //gr->Draw("P e ");
                char savepath[150];
                char Ssavepath2[150];
                char Ssavepath3[150];
		sprintf(savepath,"/afs/cern.ch/user/h/hum/work/public/CMSSW_10_2_13/src/ChargedHiggs/Analysis/plots/2021Apr/Apr02_2021_unlog_%s_%s_%s.pdf",ana.Data(),cat.Data(),pro.Data());
		sprintf(Ssavepath2,"/afs/cern.ch/user/h/hum/work/public/CMSSW_10_2_13/src/ChargedHiggs/Analysis/plots/2021Apr/Apr02_2021_unlog_%s_%s_%s.png",ana.Data(),cat.Data(),pro.Data());
		sprintf(Ssavepath3,"/afs/cern.ch/user/h/hum/work/public/CMSSW_10_2_13/src/ChargedHiggs/Analysis/plots/2021Apr/Apr02_2021_unlog_%s_%s_%s.C",ana.Data(),cat.Data(),pro.Data());

                c1a->SaveAs(savepath);
                c1a->SaveAs(Ssavepath2);
                c1a->SaveAs(Ssavepath3);



}
