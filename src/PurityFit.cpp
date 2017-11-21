#include "interface/PurityFit.hpp"
#include "TMath.h"
#include "TF1.h"
#include <fstream>

//#define VERBOSE 1


void PurityFit::init(){
    fIn_ = TFile::Open(inname.c_str() );
    if (fIn_ == NULL) Log(__FUNCTION__,"ERROR",string("No input file:") + inname );
}

void PurityFit::fit(){
    // Uperp, EtMiss
    string signame   ="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_IsoInv"+extra+"_Data";
    string bkgname   ="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_"+extra+"_%s";
    string bkgnameInv="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_IsoInv"+extra+"_%s";
    string targetname="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_"+extra+"_Data";
    string fullselInv   ="ChargedHiggsQCDPurity/Vars/"+ what + "_pt%.0f_%.0f_eta0.0_2.1_IsoInv_FullSelection"+extra+"_Data";
    string tauname="ChargedHiggsQCDPurity/Vars/TauPt_pt%.0f_%.0f_eta0.0_2.1_Data"; // use to compute the mean
    if (extra !="")Log(__FUNCTION__,"FIXME","Implement the Tau Pt for extras");

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
    fw.open(txtoutname.c_str(),ios::app|ios::out);
    fw <<"# QCD R-factor computed by "<<name()<<endl;

    for (size_t iBin=0;iBin+1<PtBins.size() ;++iBin)
    {
        TCanvas *cEWK= new TCanvas(Form("EWK_control_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ));
        TCanvas *cQCD= new TCanvas(Form("QCD_control_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ));
       
        string histname = Form(targetname.c_str(),PtBins[iBin],PtBins[iBin+1]); 
        if (verbose_ > 0 ) Log(__FUNCTION__,"INFO","Getting histogram "+ histname);
        if (fIn_ -> Get( histname.c_str()  )  == NULL){
            Log(__FUNCTION__,"WARNING","Unable to fetch: "+ histname);
            Log(__FUNCTION__,"WARNING","Trying to continue ");
            continue;
        }
        TH1D *h   = (TH1D*) fIn_ -> Get( histname.c_str()  ) -> Clone();  

        histname=Form(fullselInv.c_str(),PtBins[iBin],PtBins[iBin+1]);
        if (verbose_ > 0 ) Log(__FUNCTION__,"INFO","Getting histogram "+ histname);
        TH1D *hFullInv   = (TH1D*) fIn_ -> Get( histname.c_str()  ) -> Clone();  

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
            if (qcd == NULL)
            {
                Log(__FUNCTION__,"WARNING","Unable to fetch: "+ histname);
            }
            
            histname=Form(bkgnameInv.c_str(),PtBins[iBin],PtBins[iBin+1],"QCD");
            if (verbose_>0) Log(__FUNCTION__,"INFO","Getting histogram "+histname);
            TH1D * qcdInv = (TH1D*)  fIn_ ->Get(histname.c_str()) ;
            if (qcdInv == NULL)
            {
                Log(__FUNCTION__,"WARNING","Unable to fetch: "+ histname);
            }

            sig->SetMarkerStyle(20);
            if(qcd) qcd->SetLineColor(kRed+2);
            if (qcdInv){
                qcdInv->SetMarkerColor(kRed);
                qcdInv->SetLineColor(kRed);
                qcdInv->SetMarkerStyle(21);
            }
            
            if (qcd and qcd->Integral() >0 ) qcd->DrawNormalized("HIST SAME");
            if (qcdInv and qcdInv->Integral() > 0 ) qcdInv->DrawNormalized("P SAME");

            TLegend *l = new TLegend(0.6,.6,.9,.9);
            l->SetFillStyle(0);
            l->SetBorderSize(0);
            l->AddEntry(sig,"Data InvIso");
            if (qcd) l->AddEntry(qcd,"QCD DirectIso");
            if (qcdInv) l->AddEntry(qcdInv,"QCD InvIso");
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
            if (bkg_tmp == NULL){
                Log(__FUNCTION__,"ERROR", string("histo: ") + histname + " is NULL");
                continue;
            }

            histname=Form( bkgnameInv.c_str() , PtBins[iBin],PtBins[iBin+1],s.c_str());
            TH1D *bkgInv_tmp = (TH1D*)  fIn_ ->Get( histname.c_str());
            if (bkgInv_tmp == NULL){
                Log(__FUNCTION__,"ERROR",string("histo: ") + histname + " is NULL");
            }

            bool first = false;
            if (bkg==NULL) {
                bkg = (TH1D*) bkg_tmp->Clone( Form( bkgname.c_str(), PtBins[iBin],PtBins[iBin+1], "EWK") );
                first = true;
                }
            else bkg->Add(bkg_tmp);

            if (bkgInv==NULL) {
                bkgInv = (TH1D*) bkgInv_tmp->Clone( Form( bkgnameInv.c_str(), PtBins[iBin],PtBins[iBin+1], "EWK") );
                }
            else bkgInv->Add(bkgInv_tmp);

            if (bkg_tmp == NULL) continue;  
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
        float R = f * (hI-bI) / (sI -biI);
        float Rhi = pars["fracErrorHigh"] * (hI-bI) / (sI-biI);
        float Rlo = pars["fracErrorLow"] * (hI-bI) / (sI-biI);

        //  INFO
        cout <<"[INFO] pt "<<PtBins[iBin]<<" "<<PtBins[iBin+1]
            << " frac "<<f <<" +"<<pars["fracErrorHigh"]<< " -"<< pars["fracErrorLow"]
            << " hI = "<< hI 
            << " sI="<< sI 
            << " bI="<< bI
            << " biI="<< biI
            << " R' ="<< (hI-bI)/sI 
            << " R "<< R <<" +"<<Rhi<<" -"<<Rlo
            <<endl;
        //  for SF DB
        fw <<labelbin<<" pteta "<<PtBins[iBin]<<" "<<PtBins[iBin+1]<< " -2.1 2.1 "<<R<<" "<< (Rhi + Rlo)/2.0<<endl;
        fw <<labelspline<<" spline "<<hTau->GetMean()<< " "<<R<<" "<< (Rhi + Rlo)/2.0<<endl;

    } // bin loop

}


// ------------------------------ ROOFIT -----------------------------
using namespace RooFit;
// -------------------------------------------------------------------

float PurityFit::fit_specific( const TH1* h_, const TH1* sig_, const TH1* bkg_, 
        TH1* bkgInv_,
        TH1* hFullInv,
        string name, // unique name of the result
        string outname , // output file name, where to save results
        map<string,float> *pars	 // to gather additional params
        )
{
    Log(__FUNCTION__,"INFO","NOT FITTING ANYTHING!");
    return 1; // I should not fit

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

    // 2.5) fit background with exponential
    int   nbins = h->GetNbinsX();
    float xmin = h->GetBinLowEdge(1);
    float xmax = h->GetBinLowEdge(nbins+1);

    double prevChi2=-1;

    vector<TCanvas*> cbkgs;
    vector<TF1> expos;

    double cached_pars[50]; //cache
    for(int i=0;i< sizeof(cached_pars)/sizeof(double); ++i) cached_pars[i]=0;

    int poln=0;
    for( poln=0;poln<5; ++poln)
    {
        string formula = "TMath::Exp(-[0]*x) * ( ";
        for(int i=0;i<=poln;++i)
                {
                    if (i>0 ) formula += " + ";
                    formula += Form("[%d]",i+1);
                    if (i>0) formula += Form("*TMath::Power(x,%d)",i);
                }
        formula += ")";
        cout <<"Considering Formula"<<formula<<endl;
        TF1 expo(Form("expo%d",poln),formula.c_str(),xmin,xmax);
        expo.SetParameter(0,0.01);
        expo.SetParameter(1,0.05);

        expo.SetParLimits(0,TMath::Min(bkg->Integral()*.1,0.001), bkg->Integral()*10);
        expo.SetParLimits(1,1e-9, 1.);

        if (poln>0)for(int k=0;k < poln+1;++k) expo.SetParameter(k, cached_pars[k]);

        bkg->Fit( &expo ,"QN") ;
        bkg->Fit( &expo ,"QNM") ;
        expos.push_back(expo);

        //cache parameters
        for(int k=0;k < poln+1;++k) cached_pars[k]=expo.GetParameter(k);

        double chi2=expo.GetChisquare();
        double prob = 0 ; 
        if (poln > 0)
        {
            int n = 0;
            for(int i=1;i<=bkg->GetNbinsX();++i) if ( bkg->GetBinContent(i)> 0 ) ++n;
            int dof = n - (poln+2) + 1;
            double f = (prevChi2 - chi2) / ( chi2 /dof) ;
            prob= 1.- TMath::FDistI (f , 1, dof  ) ;
        }
        prevChi2 = chi2;

        cout<<"----------- BKG PARAMETERS ARE -------"<<endl;
        cout << "Prob = "<<prob<<endl;
        cout << "chi2 = "<<chi2<<endl;
        cout<<" 0 : "<< expo.GetParameter(0) <<endl;
        for(int i=0; i<=poln;++i)
            cout<<" "<< i+1 <<" : "<< expo.GetParameter(i+1) <<endl;
        cout<<"--------------------------------------"<<endl;
        TCanvas *cbkg=new TCanvas( (string(name)+"_bkgfit"+ Form("pol%d",poln)).c_str(),"Canvas"); cbkgs.push_back(cbkg);
        bkg->Clone("bkgForDraw")->Draw("P E"); // FIXME, memory leak
        expo.DrawClone("L SAME");

        if (prob >0.05 and poln> 0) break; // ---------------------- EXIT BEFORE UPDATING
    }
    
    poln -= 1;  // the last has a negligible improvement

    for(int i=1;i<=bkg->GetNbinsX() ;++i)
            bkg->SetBinContent(i,expos[poln].Integral( bkg->GetBinLowEdge(i),bkg->GetBinLowEdge(i+1) ) ) ;

    // remove bkgInv from Sig
    sig->Add(bkgInv,-1);

    if (sig->Integral() >0)
        sig -> Scale( 1./sig->Integral() );
    if (bkg->Integral() >0)
        bkg -> Scale( 1./bkg->Integral() );


    // 3) estimate paramaters
    //
    // 4) create roofit variables
    RooRealVar f("f","fraction",0.95,0.3,1.0);
    f.setRange(0.3,1.0);
    f.setConstant(false);

    RooRealVar x("x","EtMiss",xmin,xmax);
    // 5) create roo data hist
    RooDataHist HistSig("sig","hist sig",x,sig);
    RooDataHist HistBkg("bkg","hist bkg",x,bkg);
    RooDataHist HistToFit("target","hist target",x,h);
    // 6) create roo hist pdf
    RooHistPdf PdfSig("pdfsig","pdfsig",x,HistSig,0);
    RooHistPdf PdfBkg("pdfbkg","pdfbkg",x,HistBkg,10); //last number is interpolation
    RooRealVar norm("norm","norm", bkgIntegral) ;
    norm.setConstant();
    RooExtendPdf PdfExtBkg("pdfextbkg","pdfextbkg",PdfBkg,norm);
    // 7) create model
    RooAddPdf PdfModel("model","model",RooArgList(PdfSig,PdfExtBkg),f);
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
    // 8.5) save additional results
    if (pars != NULL ) {
        (*pars)["fracErrorHigh"] = f.getAsymErrorHi(); 
        (*pars)["fracErrorLow" ] = f.getAsymErrorLo(); 
    }
    // 9) plot
    HistToFit.plotOn(frame,DataError(RooAbsData::SumW2));
    PdfModel.plotOn(frame, LineColor(kBlack));
    PdfModel.plotOn(frame, Components(PdfBkg),LineColor(kRed)); 
    PdfModel.plotOn(frame, Components(PdfSig),LineColor(kBlue),LineStyle(kDashed));

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

    if ( outname != "")
    {
        TFile *fOut=TFile::Open(outname.c_str(),"UPDATE");
        fOut->cd();
        c->Write();
        for( auto c : cbkgs ) c->Write();
        r->Write(Form("%s_roofit",name.c_str() ) );
        fOut->Close();
    }

    // delete the clone
    for( auto c : cbkgs ) c->Delete();
    cbkgs.clear();
    sig -> Delete();
    bkg -> Delete();
    h   -> Delete();

    return f.getVal();
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
