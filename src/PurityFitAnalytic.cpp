#include "interface/PurityFitAnalytic.hpp"
#include "TMath.h"
#include "TF1.h"
#include <fstream>

#define VERBOSE 1


void PurityFitAnalytic::init(){
    fIn_ = TFile::Open(inname.c_str() );
    if (fIn_ == NULL) Log(__FUNCTION__,"ERROR",string("No input file:") + inname );
}

void PurityFitAnalytic::fit(){
    // Uperp, EtMiss
    string what="EtMiss";
    string signame   ="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_IsoInv_Data";
    string bkgname   ="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_%s";
    string bkgnameInv="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_IsoInv_%s";
    string targetname="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_Data";
    string fullselInv   ="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_IsoInv_FullSelection_NoR_Data";
    string tauname="ChargedHiggsQCDPurity/Vars/TauPt_pt%.0f_%.0f_eta0.0_2.1_Data"; // use to compute the mean

    if (bkglabels.empty() )
    {
        Log(__FUNCTION__,"INFO","Using default bkg labels");
        bkglabels.push_back("WJets");
        bkglabels.push_back("TT");
        bkglabels.push_back("WW");
        bkglabels.push_back("WZ");
        bkglabels.push_back("ZZ");
        bkglabels.push_back("DY");
        bkglabels.push_back("ST");
    }

    // reset output file
    TFile *fOut= TFile::Open(outname.c_str(),"RECREATE");
    fOut->Close();

    ofstream  fw;
    fw.open(txtoutname.c_str());
    fw <<"# QCD R-factor computed by "<<name()<<endl;
    fw <<"# with fFull as last "<<name()<<endl;

    for (size_t iBin=0;iBin+1<PtBins.size() ;++iBin)
    {
        TCanvas *cEWK= new TCanvas(Form("EWK_control_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ));
        TCanvas *cQCD= new TCanvas(Form("QCD_control_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ));
       
        string histname = Form(targetname.c_str(),PtBins[iBin],PtBins[iBin+1]); 
        if (verbose_ > 0 ) Log(__FUNCTION__,"INFO","Getting histogram "+ histname);
        TH1D *h   = (TH1D*) fIn_ -> Get( histname.c_str()  ) -> Clone();  

        histname=Form(fullselInv.c_str(),PtBins[iBin],PtBins[iBin+1]);
        if (verbose_ > 0 ) Log(__FUNCTION__,"INFO","Getting histogram "+ histname);
        TH1D *hFullInv   = (TH1D*) fIn_ -> Get( histname.c_str()  ) -> Clone();  
        if (hFullInv == NULL) Log(__FUNCTION__,"ERROR","No Such file or dir");

        histname=Form(tauname.c_str(),PtBins[iBin],PtBins[iBin+1]);
        if (verbose_ > 0 ) Log(__FUNCTION__,"INFO","Getting histogram "+ histname);
        TH1D *hTau   = (TH1D*) fIn_ -> Get( histname.c_str()  ) -> Clone();  

        histname=Form(signame.c_str(),PtBins[iBin],PtBins[iBin+1]);
        if (verbose_ >0 ) Log(__FUNCTION__,"INFO","Getting histogram "+ histname); 
        TH1D *sig = (TH1D*) fIn_ -> Get( histname.c_str() ) -> Clone();// QCD
       
        if ( h != NULL and sig != NULL and h->Integral() >0 and sig->Integral() >0)  // control plots QCD
        {
            cQCD->cd();

            sig->DrawNormalized("P");

            histname=Form(bkgname.c_str(),PtBins[iBin],PtBins[iBin+1],"QCD");
            if (verbose_>0) Log(__FUNCTION__,"INFO","Getting histogram "+histname);
            TH1D * qcd = (TH1D*)  fIn_ ->Get( histname.c_str() );
            
            histname=Form(bkgnameInv.c_str(),PtBins[iBin],PtBins[iBin+1],"QCD");
            if (verbose_>0) Log(__FUNCTION__,"INFO","Getting histogram "+histname);
            TH1D * qcdInv = (TH1D*)  fIn_ ->Get(histname.c_str()) ;

            sig->SetMarkerStyle(20);
            qcd->SetLineColor(kRed+2);
            qcdInv->SetMarkerColor(kRed);
            qcdInv->SetLineColor(kRed);
            qcdInv->SetMarkerStyle(21);
            
            if (qcd->Integral() >0 ) qcd->DrawNormalized("HIST SAME");
            if (qcdInv->Integral() > 0 ) qcdInv->DrawNormalized("P SAME");

            TLegend *l = new TLegend(0.6,.6,.9,.9);
            l->SetFillStyle(0);
            l->SetBorderSize(0);
            l->AddEntry(sig,"Data InvIso");
            l->AddEntry(qcd,"QCD DirectIso");
            l->AddEntry(qcdInv,"QCD InvIso");
            l->Draw();
            cQCD->Update();

            // Open and close. Don't leave files open, otherwise root will write stuff inside
            fOut = TFile::Open(outname.c_str(),"UPDATE");
            fOut->cd();
            cQCD->Write();
            fOut->Close();

            delete cQCD;
        }
        //else { cout << "NO QCD Control plots"<<endl; }  //DEBUG

        cEWK->cd() ;
        TLegend *l =new TLegend(0.6,.6,.9,.9);
        l->SetFillStyle(0);
        l->SetBorderSize(0);

        TH1D *bkg= NULL;
        TH1D *bkgInv= NULL;

        for (string& s : bkglabels)
        {
            histname= Form( bkgname.c_str() , PtBins[iBin],PtBins[iBin+1],s.c_str());
            TH1D *bkg_tmp = (TH1D*)  fIn_ ->Get( histname.c_str() );
            if (bkg_tmp == NULL) Log(__FUNCTION__,"ERROR", string("histo: ") + histname + " is NULL");

            histname=Form( bkgnameInv.c_str() , PtBins[iBin],PtBins[iBin+1],s.c_str());
            TH1D *bkgInv_tmp = (TH1D*)  fIn_ ->Get( histname.c_str());
            if (bkgInv_tmp == NULL) Log(__FUNCTION__,"ERROR",string("histo: ") + histname + " is NULL");

            bool first = false;
            if (bkg==NULL) {
                bkg = (TH1D*) bkg_tmp->Clone( Form( bkgname.c_str(), PtBins[iBin],PtBins[iBin+1], "EWK") );
                first = true;
                }
            else bkg->Add(bkg_tmp);

            if (bkgInv==NULL) {
                bkgInv = (TH1D*) bkgInv_tmp->Clone( Form( bkgnameInv.c_str(), PtBins[iBin],PtBins[iBin+1], "EWK") );
                first = true;
                }
            else bkgInv->Add(bkgInv_tmp);
      
            // control plots EWK
            if (s == "DY") { bkg_tmp->SetLineColor(kCyan);}// bkg_binned->SetLineColor(kCyan); } 
            else if (s == "TT") bkg_tmp->SetLineColor(kMagenta+2);
            else if (s == "WJets" ) { bkg_tmp->SetLineColor(kGreen+2);}// bkg_binned -> SetLineColor(kGreen+2);}
            else if (s == "WW"  )  bkg_tmp->SetLineColor(kRed);
            else if (s == "WZ"  )  bkg_tmp->SetLineColor(kRed+2);
            else if (s == "ZZ"  )  bkg_tmp->SetLineColor(kRed-4);
            else                   bkg_tmp->SetLineColor(kGray);
            l->AddEntry(bkg_tmp,s.c_str());

            if (first) bkg_tmp->Draw("HIST");
            else bkg_tmp->Draw("HIST SAME");

        } // labels loop
        
        l->Draw();
        cEWK->Update();
        fOut = TFile::Open(outname.c_str(),"UPDATE"); // don't leave them open. Root will try to use them.
        fOut->cd();
        cEWK->Write();
        fOut->Close(); // close for the fit_specific

        delete cEWK;
    
        map<string,float> pars; 

        bkg->Scale(lumi);
        bkgInv->Scale(lumi);

        float hI= h->Integral();
        float sI= sig->Integral();
        float bI= bkg->Integral();
        float biI= bkgInv->Integral();

        float f = fit_specific( h, sig, bkg, bkgInv, hFullInv, Form("fit_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ), outname, &pars);

        // propagate the fraction to the yields
        float R = f * hI / (sI * pars["fInvIso"]);
        float Rhi = pars["fracErrorHigh"] * hI / (sI * pars["fInvIso"]);
        float Rlo = pars["fracErrorLow"] * hI / (sI * pars["fInvIso"]);

        //  INFO
        cout <<"[INFO] pt "<<PtBins[iBin]<<" "<<PtBins[iBin+1]
            << " frac "<<f <<" +"<<pars["fracErrorHigh"]<< " -"<< pars["fracErrorLow"]
            << " hI = "<< hI 
            << " sI="<< sI 
            << " bI="<< bI <<" [~(1-f)*hI] "
            << " biI="<< biI<<" [~(1-fI)*sI]"
            << " fInvIso="<<pars["fInvIso"]
            << " fFull= "<<pars["fFull"]
            << " R' ="<< (hI-bI)/sI 
            << " R "<< R <<" +"<<Rhi<<" -"<<Rlo
            <<endl;
        //  for SF DB
        fw <<"tauinviso pteta "<<PtBins[iBin]<<" "<<PtBins[iBin+1]<< " -2.1 2.1 "<<R<<" "<< (Rhi + Rlo)/2.0<<endl;
        fw <<"tauinvisospline spline "<<hTau->GetMean()<< " "<<R<<" "<< (Rhi + Rlo)/2.0<< " " << pars["fFull"] <<endl;

    } // bin loop

}


// ------------------------------ ROOFIT -----------------------------
using namespace RooFit;
// -------------------------------------------------------------------

float PurityFitAnalytic::fit_specific( const TH1* h_, const TH1* sig_, const TH1* bkg_, 
        TH1* bkgInv_,
        TH1* hFullInv,
        string name, // unique name of the result
        string outname , // output file name, where to save results
        map<string,float> *pars	 // to gather additional params
        )
{

    // 1) perform preliminary checks
    if ( h_ == NULL ) { Log(__FUNCTION__,"ERROR", "no target histogram"); return -1;}
    if ( sig_ == NULL ) { Log(__FUNCTION__,"ERROR","no sig histogram"); return -1;}
    if ( bkg_ == NULL ) { Log(__FUNCTION__,"ERROR","no bkg histogram"); return -1;}
    if ( bkgInv_ == NULL ) { Log(__FUNCTION__,"ERROR" ,"no bkgInv histogram"); return -1;}

    if (sig_ -> Integral() == 0 ) { Log(__FUNCTION__,"ERROR" ,"sig integral is NULL"); return -2;}
    if (bkg_ -> Integral() == 0 ) { Log(__FUNCTION__,"ERROR","bkg integral is NULL"); return -2;}

    if (verbose_ > 0) Log(__FUNCTION__,"INFO",string("fitting: ") + h_->GetName() + " " +   sig_->GetName() + " " + bkg_->GetName() ) ;

    // 1.5) Clone
    TH1 * sig = (TH1*)sig_ -> Clone(Form("%s_fitspecific_clone",sig_->GetName()));
    TH1 * bkg = (TH1*)bkg_ -> Clone(Form("%s_fitspecific_clone",bkg_->GetName()));
    TH1 * h = (TH1*)h_ -> Clone(Form("%s_fitspecific_clone",h_->GetName()));
    TH1 * bkgInv = (TH1*)bkgInv_ -> Clone(Form("%s_fitspecific_clone",bkgInv_->GetName()));

    // 1.6) check no negative entries otherwise 0
    for(int i=1;i<=sig->GetNbinsX();++i)
        if(sig->GetBinContent(i) <0) sig->SetBinContent(i,0);
    for(int i=1;i<=bkg->GetNbinsX();++i)
        if(bkg->GetBinContent(i) <0) bkg->SetBinContent(i,0);
    for(int i=1;i<=bkgInv->GetNbinsX();++i)
        if(bkgInv->GetBinContent(i) <0) bkgInv->SetBinContent(i,0);

    // 2) scale templates: template normalization is meaningless
    sig->Sumw2();	
    bkg->Sumw2();
    bkgInv->Sumw2();

    if (bkg->Integral() <=0)
    {
    for(int i=1;i<=bkg->GetNbinsX();++i)
        bkg->SetBinContent(i,1.0);
    }

    // 2.1) save integrals
    float bkgIntegral = bkg->Integral();
    float sigIntegral = sig->Integral();
    float hIntegral = h ->Integral();
    float bkgInvIntegral = bkgInv->Integral();

    // 2.5) fit background with exponential series.
    // TODO User RooFit
    int   nbins = h->GetNbinsX();
    float xmin = h->GetBinLowEdge(1);
    float xmax = h->GetBinLowEdge(nbins+1);

    double prevChi2=-1;

    vector<TCanvas*> cbkgs;
    vector<TF1> expos;

    // --- double cached_pars[50]; //cache
    // --- for(int i=0;i< sizeof(cached_pars)/sizeof(double); ++i) cached_pars[i]=0;

    // --- int poln=0;
    // --- for( poln=0;poln<5; ++poln)
    // --- {
    // ---     string formula = "TMath::Exp(-[0]*x) * ( ";
    // ---     for(int i=0;i<=poln;++i)
    // ---             {
    // ---                 if (i>0 ) formula += " + ";
    // ---                 formula += Form("[%d]",i+1);
    // ---                 if (i>0) formula += Form("*TMath::Power(x,%d)",i);
    // ---             }
    // ---     formula += ")";
    // ---     cout <<"Considering Formula"<<formula<<endl;
    // ---     TF1 expo(Form("expo%d",poln),formula.c_str(),xmin,xmax);
    // ---     expo.SetParameter(0,0.01);
    // ---     expo.SetParameter(1,0.05);

    // ---     expo.SetParLimits(0,TMath::Min(bkg->Integral()*.1,0.001), bkg->Integral()*10);
    // ---     expo.SetParLimits(1,1e-9, 1.);

    // ---     if (poln>0)for(int k=0;k < poln+1;++k) expo.SetParameter(k, cached_pars[k]);

    // ---     bkg->Fit( &expo ,"QN") ;
    // ---     bkg->Fit( &expo ,"QNM") ;
    // ---     expos.push_back(expo);

    // ---     //cache parameters
    // ---     for(int k=0;k < poln+1;++k) cached_pars[k]=expo.GetParameter(k);

    // ---     double chi2=expo.GetChisquare();
    // ---     double prob = 0 ; 
    // ---     if (poln > 0)
    // ---     {
    // ---         int n = 0;
    // ---         for(int i=1;i<=bkg->GetNbinsX();++i) if ( bkg->GetBinContent(i)> 0 ) ++n;
    // ---         int dof = n - (poln+2) + 1;
    // ---         double f = (prevChi2 - chi2) / ( chi2 /dof) ;
    // ---         prob= 1.- TMath::FDistI (f , 1, dof  ) ;
    // ---     }
    // ---     prevChi2 = chi2;

    // ---     cout<<"----------- BKG PARAMETERS ARE -------"<<endl;
    // ---     cout << "Prob = "<<prob<<endl;
    // ---     cout << "chi2 = "<<chi2<<endl;
    // ---     cout<<" 0 : "<< expo.GetParameter(0) <<endl;
    // ---     for(int i=0; i<=poln;++i)
    // ---         cout<<" "<< i+1 <<" : "<< expo.GetParameter(i+1) <<endl;
    // ---     cout<<"--------------------------------------"<<endl;
    // ---     TCanvas *cbkg=new TCanvas( (string(name)+"_bkgfit"+ Form("pol%d",poln)).c_str(),"Canvas"); cbkgs.push_back(cbkg);
    // ---     bkg->Clone("bkgForDraw")->Draw("P E"); // FIXME, memory leak
    // ---     expo.DrawClone("L SAME");

    // ---     if (prob >0.05 and poln> 0) break; // ---------------------- EXIT BEFORE UPDATING
    // --- }
    
    // -- poln -= 1;  // the last has a negligible improvement

    // -- for(int i=1;i<=bkg->GetNbinsX() ;++i)
    // --         bkg->SetBinContent(i,expos[poln].Integral( bkg->GetBinLowEdge(i),bkg->GetBinLowEdge(i+1) ) ) ;

    RooRealVar x("x","EtMiss",xmin,xmax);
    // ---------------------------------- EWK MODEL -----------------
    if (initvalues.find("sigmaR_EWK") == initvalues.end())initvalues["sigmaR_EWK"] = 100.;
    if (initvalues.find("exp_EWK") == initvalues.end())initvalues["exp_EWK"] = 0.003; // ~ 300
    if (initvalues.find("aEWK") == initvalues.end())initvalues["aEWK"] = 0.6;
    if (initvalues.find("bEWK") == initvalues.end())initvalues["bEWK"] = 0.3;
    if (initvalues.find("mEWK") == initvalues.end())initvalues["mEWK"] = 50.;
    if (initvalues.find("sEWK") == initvalues.end())initvalues["sEWK"] = 50.;


    RooRealVar sR_EWK("sigmaR_EWK","sigmaR_EWK",initvalues["sigmaR_EWK"],50.,200.);
    RooRealVar exp_EWK("exp_EWK","exp_EWK",initvalues["exp_EWK"],1./1000.,1./50.);
    RooGenericPdf rayleighEWK("rayleigh_EWK","@0/@1 * TMath::Exp(-@0*@0/(2*@1*@1))",RooArgList(x,sR_EWK) );
    RooGenericPdf expEWK("exp_mod_EWK","@0*TMath::Exp(-@0*@1)",RooArgList(x,exp_EWK) ) ;
    RooRealVar mEWK("mEWK","mEWK",initvalues["mEWK"],0,100);
    RooRealVar sEWK("sEWK","sEWK",initvalues["sEWK"],0,100);
    RooGaussian  gausEWK("gausEWK","gausEWK",x,mEWK,sEWK);
    RooRealVar aEWK("aEwk","aEWK",initvalues["aEWK"],0.,1.);
    RooRealVar bEWK("bEwk","bEWK",initvalues["bEWK"],0.,1.);
    RooAddPdf modelEwk("modelEwk","modelEwk",RooArgList(rayleighEWK,expEWK,gausEWK),RooArgList(aEWK,bEWK)); 

    RooDataHist HistEwk("ewk","hist ewk",x,bkg);
    modelEwk.fitTo(HistEwk,
            SumW2Error(kTRUE),
            Save(), 
            PrintEvalErrors(-1),
            PrintLevel(-1),
            //Minos(kTRUE),
            Warnings(0)
            );
    // follow values
    initvalues["sigmaR_EWK"]= sR_EWK.getVal();
    initvalues["exp_EWK"] = exp_EWK.getVal();
    initvalues["aEWK"] = aEWK.getVal();
    initvalues["bEWK"] = bEWK.getVal();
    initvalues["mEWK"] = mEWK.getVal();
    initvalues["sEWK"] = sEWK.getVal();
    // freeze
    sR_EWK.setConstant();
    exp_EWK.setConstant();
    aEWK.setConstant();
    bEWK.setConstant();
    mEWK.setConstant();
    sEWK.setConstant();

    RooPlot * frameEwk = x.frame();
    HistEwk.plotOn(frameEwk,SumW2Error(kTRUE));
    modelEwk.plotOn(frameEwk);

    TCanvas *cbkg=new TCanvas( (string(name) + "_bkgfit").c_str(),"Canvas");
    frameEwk->Draw();
    cbkgs.push_back(cbkg);
    //plot

    // remove bkgInv from Sig ?
    //sig->Add(bkgInv,-1);

    if (sig->Integral() >0)
        sig -> Scale( 1./sig->Integral() );
    if (bkg->Integral() >0)
        bkg -> Scale( 1./bkg->Integral() );


    // ---------------------------------- EWK MODEL -----------------
    // create bkg model for ewk
    // expos[poln]
    // --- string model= Form("TMath::Exp(-%e*x ) * (",expos[poln].GetParameter(0));
    // --- for(int i=0;i<=poln;++i)
    // --- {
    // ---                 if (i>0 ) model += " + ";
    // ---                 model += Form("%e",expos[poln].GetParameter(i+1));
    // ---                 if (i>0) model += Form("*TMath::Power(x,%d)",i);
    // --- }
    // --- model += ")";
    // --- RooGenericPdf modelEwk("modelEwk",model.c_str(),x);

    // ----------------------- FIT INV ISO TO EXTRACT QCD MODEL ---------------------------
    if (initvalues.find("sigmaR_QCD") == initvalues.end())initvalues["sigmaR_QCD"] = 20;
    if (initvalues.find("cE_QCD") == initvalues.end())initvalues["cE_QCD"] = -0.001;
    if (initvalues.find("sigmaG_QCD") == initvalues.end())initvalues["sigmaG_QCD"] = 5;
    if (initvalues.find("muG_QCD") == initvalues.end())initvalues["muG_QCD"] = 0;
    if (initvalues.find("f1QCD") == initvalues.end())initvalues["f1QCD"] = .95;
    if (initvalues.find("f2QCD") == initvalues.end())initvalues["f2QCD"] = (1.-initvalues["f1QCD"])/2.;
    if (initvalues.find("fInvIso") == initvalues.end())initvalues["fInvIso"] = .9;
    if (initvalues.find("f") == initvalues.end())initvalues["f"] = .9;
    //if (initvalues.find("XXX") == initvalues.end())initvalues["XXX"] = 40;
    // x) create QCD model
    RooRealVar sR("sigmaR_QCD","sigmaR",initvalues["sigmaR_QCD"]);
    RooGenericPdf rayleighQCD ("rayleigh_QCD","@0/(@1*@1)*TMath::Exp(-@0*@0/(2*@1*@1))",RooArgList(x,sR) );
    RooRealVar cE("cE_QCD","cE",initvalues["cE_QCD"],-1.,0.);
    RooExponential expoQCD("expo_QCD","expoQCD",x,cE);
    RooRealVar sG("sigmaG_QCD","sigmaG",initvalues["sigmaG_QCD"],0.00001,30);
    RooRealVar mG("muG_QCD","muG",initvalues["muG_QCD"],0,30);
    RooGaussian gaussQCD("gauss_QCD","gauss_QCD",x,mG,sG);

    RooRealVar f1QCD("f1QCD","f1QCD",initvalues["f1QCD"],.1,1.0);
    RooRealVar f2QCD("f2QCD","f2QCD",initvalues["f2QCD"]);
    //RooAddPdf modelQCD("modelQCD","modelQCD",RooArgList(rayleighQCD,expoQCD,gaussQCD) , RooArgList(f1QCD,f2QCD) );
    RooGenericPdf modelQCD("rayleigh_QCD","@0/(@1*@1)*TMath::Exp(-@0*@0/(2*@1*@1))",RooArgList(x,sR) );
    //RooAddPdf modelQCD("modelQCD","modelQCD",RooArgList(rayleighQCD,expoQCD) , RooArgList(f1QCD) );
    //RooFFTConvPdf modelQCD("rxg","rayleigh (X) gauss",x,rayleighQCD,gaussQCD);

    RooRealVar fInvIso("fInvIso","fInvIso",initvalues["fInvIso"],0.,1.);
    RooAddPdf modelInvIso("modelInvIso","modelInvIso",RooArgList(modelQCD,modelEwk) , RooArgList(fInvIso) );
    

    RooMsgService::instance().setSilentMode(true);
    RooDataHist HistSig("sig","hist sig",x,sig); // BKGINVISO SUB
    //MODELQCD
    modelInvIso.fitTo(HistSig,
            SumW2Error(kTRUE),
            Save(), 
            PrintEvalErrors(-1),
            PrintLevel(-1),
            //Minos(kTRUE),
            Warnings(0)
            );

    RooPlot * frameInvIso = x.frame();
    HistSig.plotOn(frameInvIso);
    modelInvIso.plotOn(frameInvIso);
    modelInvIso.plotOn(frameInvIso,Components(modelQCD),LineStyle(kDashed));
    modelInvIso.plotOn(frameInvIso,Components(modelEwk),LineStyle(kDashed),LineColor(kRed));
    // freeze model QCD
    sR.setConstant();
    cE.setConstant();
    sG.setConstant();
    mG.setConstant();
    f1QCD.setConstant();
    f2QCD.setConstant();

    // follow the values
    initvalues["sigmaR_QCD"] = sR.getVal();
    initvalues["cE_QCD"] = cE.getVal();
    initvalues["sigmaG_QCD"] = sG.getVal();
    initvalues["muG_QCD"] = mG.getVal();
    initvalues["f1QCD"] = f1QCD.getVal();
    initvalues["f2QCD"] = f2QCD.getVal();
    initvalues["fInvIso"] = fInvIso.getVal();

    // save fInvIso in pars
    if (pars!=NULL)(*pars)["fInvIso"] = fInvIso.getVal();


    // ----------------------- FIT DIRECT ISO WITH FIXED QCD MODEL ---------------------------
    // 3) estimate paramaters
    //
    // 4) create roofit variables
    RooRealVar f("f","fraction",initvalues["f"],0.3,1.0);
    f.setRange(0.3,1.0);
    f.setConstant(false);

    // 5) create roo data hist
    RooDataHist HistToFit("target","hist target",x,h);
    // 6) create roo hist pdf
    // 7) create model
    RooAddPdf PdfModel("model","model",RooArgList(modelQCD,modelEwk),f);
    // 8) fit
    RooFitResult *r;
    RooPlot *frame=x.frame();
    RooMsgService::instance().setSilentMode(true);

    PdfModel.fitTo(HistToFit,
            SumW2Error(kTRUE),
            Save(), 
            PrintEvalErrors(-1),
            PrintLevel(-1),
            //Minos(kTRUE),
            Warnings(0)
            );
    r = PdfModel.fitTo(HistToFit,
            SumW2Error(kTRUE),
            Save(), 
            PrintEvalErrors(-1),
            PrintLevel(-1),
            Minos(kTRUE),
            Warnings(0)
            );
    cout <<" -------------- FLOATING -------------"<<endl;
    r-> floatParsInit() . Print("V");
    cout <<" -------------- FINAL ----------------"<<endl;
    r-> floatParsFinal() . Print("V");
    cout <<" -------------------------------------"<<endl;

    // 9) plot
    //HistToFit.plotOn(frame,DataError(RooAbsData::SumW2));
    HistToFit.plotOn(frame,DataError(RooAbsData::SumW2));
    PdfModel.plotOn(frame, LineColor(kBlack));
    PdfModel.plotOn(frame, Components(modelQCD),LineColor(kBlue)); 
    PdfModel.plotOn(frame, Components(modelEwk),LineColor(kRed),LineStyle(kDashed));

    // -------------Fit Full selection inv iso
    RooRealVar fFull("fFull","fraction",.999,0.3,1.0);
    f.setRange(0.3,1.0);
    f.setConstant(false);

    RooDataHist HistFullSel("FullSel","hist target",x,hFullInv);
    RooAddPdf PdfModelFullInv("modelFullInv","model",RooArgList(modelQCD,modelEwk),fFull);

    PdfModelFullInv.fitTo(HistFullSel);

    RooPlot*frameFull = x.frame();
    HistFullSel.plotOn(frameFull);
    PdfModelFullInv.plotOn(frameFull);
    PdfModelFullInv.plotOn(frameFull, Components(modelQCD),LineColor(kBlue)); 
    PdfModelFullInv.plotOn(frameFull, Components(modelEwk),LineColor(kRed),LineStyle(kDashed));

    // 8.5) save additional results
    if (pars != NULL ) {
        (*pars)["fracErrorHigh"] = f.getAsymErrorHi(); 
        (*pars)["fracErrorLow" ] = f.getAsymErrorLo(); 
    }

    TCanvas *c=new TCanvas((string(name)+"_canvas").c_str(),"Canvas");
    c->cd();
    c->Draw();
    frame->Draw();

    TLatex* txt = new TLatex();//Form("Fraction=%.1f\%",f.getVal()*100) ;
    txt->SetTextSize(0.03) ;
    txt->SetTextColor(kBlack) ;
    txt->SetNDC();
    txt->SetTextAlign(22);
    //txt->AddText(Form("Fraction=%.1f\%",f.getVal()*100) );
    txt->DrawLatex(.3,.85,Form("Fraction=%.1f%%",float(f.getVal()*100)));

    // get pt from name
    {
        size_t n = name.find("pt");
        float ptmin = -1;
        float ptmax = -1;
        if (n != string::npos)
        {
            string pts = "";	
            //name[n+0] == 'p'
            //name[n+1] =='t'
            n +=2;

            //all string are null terminated in C/C++
            while (	name[n] != '_' and name[n] != '\0' ) { pts+=name[n]; ++n;}
            ptmin = atof( pts.c_str() ) ;

            pts="";
            ++n;
            while (	name[n] != '_' and name[n] != '\0' ) { pts+=name[n]; ++n;}
            ptmax = atof( pts.c_str() ) ;
            txt->DrawLatex(.3,.82,Form("%.1f<P_{T}<%.1f",ptmin,ptmax));	
        }
    } // end get pt from name

    TCanvas *cInvIso=new TCanvas((string(name)+"_inviso_canvas").c_str(),"Canvas");
    cInvIso->cd();
    cInvIso->Draw();
    frameInvIso->Draw();

    TCanvas *cFull=new TCanvas((string(name)+"_invisofullsel_canvas").c_str(),"Canvas");
    cFull->cd();
    cFull->Draw();
    frameFull->Draw();

    if ( outname != "")
    {
        TFile *fOut=TFile::Open(outname.c_str(),"UPDATE");
        fOut->cd();
        c->Write();
        cInvIso->Write();
        cFull->Write();
        for( auto c : cbkgs ) c->Write();
        r->Write(Form("%s_roofit",name.c_str() ) );
        fOut->Close();

        // Save the fit  plot
        c->SaveAs( (string("plot/PurityFitAnalytic/") + c->GetName() + ".pdf").c_str());
        cInvIso->SaveAs( (string("plot/PurityFitAnalytic/") + cInvIso->GetName() + ".pdf").c_str());
        cFull->SaveAs( (string("plot/PurityFitAnalytic/") + cFull->GetName() + ".pdf").c_str());
        for( auto c : cbkgs ) c->SaveAs( (string("plot/PurityFitAnalytic/") + c->GetName() + ".pdf").c_str());

    }

    // delete the clone
    for( auto c : cbkgs ) c->Delete();
    cbkgs.clear();
    sig -> Delete();
    bkg -> Delete();
    h   -> Delete();

    Log(__FUNCTION__,"INFO",Form("fFull=%.5f",fFull.getVal()) ) ;

    (*pars)["fFull"] = fFull.getVal();
    //FIXME ERRORS
    return f.getVal();
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
