#include "interface/PurityFit.hpp"


void PurityFit::init(){
    fIn_ = TFile::Open(inname.c_str() );
    if (fIn_== NULL) cout<<"[PurityFit]::[init]::[ERROR] no input file:"<<inname<<endl;
}

void PurityFit::fit(){
    string signame="ChargedHiggsQCDPurity/Vars/EtMiss_pt%.0f_%.0f_IsoInv_Data";
    string bkgname="ChargedHiggsQCDPurity/Vars/EtMiss_pt%.0f_%.0f_%s";
    string bkgnameInv="ChargedHiggsQCDPurity/Vars/EtMiss_pt%.0f_%.0f_IsoInv_%s";
    string targetname="ChargedHiggsQCDPurity/Vars/EtMiss_pt%.0f_%.0f_Data";

    vector<string> bkglabels;
        bkglabels.push_back("WJets");
        bkglabels.push_back("TTJets");
        bkglabels.push_back("WW");
        bkglabels.push_back("WZ");
        bkglabels.push_back("ZZ");
        bkglabels.push_back("DY");

    // reset output file
    TFile *fOut= TFile::Open(outname.c_str(),"RECREATE");
    fOut->Close();

    for (size_t iBin=0;iBin+1<PtBins.size() ;++iBin)
    {
        TCanvas *cEWK= new TCanvas(Form("EWK_control_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ));
        TCanvas *cQCD= new TCanvas(Form("QCD_control_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ));

        TH1D *h   = (TH1D*) fIn_ -> Get( Form(targetname.c_str(), PtBins[iBin],PtBins[iBin+1])  ) -> Clone();  // EWK
        TH1D *sig = (TH1D*) fIn_ -> Get( Form(signame.c_str(), PtBins[iBin],PtBins[iBin+1]) ) -> Clone();// QCD
       
        if ( h != NULL and sig != NULL and h->Integral() >0 and sig->Integral() >0)  // control plots QCD
        {
            cQCD->cd();

            sig->DrawNormalized("P");

            TH1D * qcd = (TH1D*)  fIn_ ->Get( Form( bkgname.c_str() , PtBins[iBin],PtBins[iBin+1],"QCD") );
            TH1D * qcdInv = (TH1D*)  fIn_ ->Get( Form( bkgnameInv.c_str() , PtBins[iBin],PtBins[iBin+1],"QCD") ) ;
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
        for (string& s : bkglabels)
        {
            TH1D *bkg_tmp = (TH1D*)  fIn_ ->Get( Form( bkgname.c_str() , PtBins[iBin],PtBins[iBin+1],s.c_str()) );
            if ( bkg_tmp == NULL )  cout <<"[PurityFit]::[fit]::[ERROR] histo "<<  Form( bkgname.c_str() , PtBins[iBin],PtBins[iBin+1],s.c_str()) << " is NULL"<<endl;

            bool first = false;
            if (bkg==NULL) {
                bkg = (TH1D*) bkg_tmp->Clone( Form( bkgname.c_str(), PtBins[iBin],PtBins[iBin+1], "EWK") );
                first = true;
                }
            else bkg->Add(bkg_tmp);
      
            // control plots EWK
            if (s == "DY") bkg_tmp->SetLineColor(kCyan);
            else if (s == "TTJets") bkg_tmp->SetLineColor(kMagenta+2);
            else if (s == "WJets" ) bkg_tmp->SetLineColor(kGreen+2);
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
     
        float f = fit_specific( h,sig,bkg, Form("fit_pt%.0f_%.0f",PtBins[iBin],PtBins[iBin+1] ), outname);
    } // bin loop

}


// ------------------------------ ROOFIT -----------------------------
using namespace RooFit;
// -------------------------------------------------------------------

float PurityFit::fit_specific( TH1* h, TH1* sig, TH1* bkg, 
        string name, // unique name of the result
        string outname , // output file name, where to save results
        map<string,float> *pars	 // to gather additional params
        )
{
    // 1) perform preliminary checks
    if ( h== NULL ) { cout<<"[PurityFit]::[fit_specific]::[ERROR] no target histogram"<<endl; return -1;}
    if ( sig== NULL ) { cout<<"[PurityFit]::[fit_specific]::[ERROR] no sig histogram"<<endl; return -1;}
    if ( bkg== NULL ) { cout<<"[PurityFit]::[fit_specific]::[ERROR] no bkg histogram"<<endl; return -1;}

    if (sig->Integral() == 0 ) { cout<<"[PurityFit]::[fit_specific]::[ERROR] sig integrall is NULL"<<endl; return -2;}
    if (bkg->Integral() == 0 ) { cout<<"[PurityFit]::[fit_specific]::[ERROR] bkg integrall is NULL"<<endl; return -2;}

    if (verbose_ >0) cout <<"[PurityFit]::[fit_specific]::[INFO] fitting "<<h->GetName() << " " << sig->GetName()<<" "<<bkg->GetName()<<endl;
    // 2) scale templates: template normalization is meaningless
    sig->Sumw2();	
    bkg->Sumw2();

    sig -> Scale( 1./sig->Integral() );
    bkg -> Scale( 1./bkg->Integral() );

    // 3) estimate paramaters
    float fracEstimator=0.5;
    int   nbins = h->GetNbinsX();
    float xmin = h->GetBinLowEdge(1);
    float xmax = h->GetBinLowEdge(nbins+1);
        {
             // 3.1) compute frac as around the max of sig 
             float sigMax= sig->GetMaximum();
             int sigBinMax= sig->GetMaximumBin();
             float sigInt= sig->Integral();
             float bkgUnderMax=bkg->GetBinContent(sigBinMax);
             float targetMax = h->GetMaximum();
             float targetInt = h->Integral();
             float frac1=(targetMax/targetInt)/( (sigMax-bkgUnderMax)/sigInt);
             // 3.2) compute frac as tail
             float bkgInt= bkg->Integral();
             int bkgN=bkg->GetNbinsX();
             int nTailSum= bkgN/10;
             float bkgTail=0;for(int i=0;i<nTailSum;i++) bkgTail+= bkg->GetBinContent(bkgN-i);
             int targetN=h->GetNbinsX();
             float targetTail=0;for(int i=0;i<nTailSum;i++) targetTail+= h->GetBinContent(targetN-i);
             float frac2=1.- (targetTail/targetInt)/(bkgTail/bkgInt);
             // 3.3) set estimator 
             fracEstimator = frac1;
             if(fracEstimator<0.05 or fracEstimator>0.95) fracEstimator=frac2;
             if(fracEstimator<0.05 or fracEstimator>0.95) fracEstimator=0.8;
        }
    // 4) create roofit variables
    RooRealVar f("f","fraction",fracEstimator,0.05,0.95);
    RooRealVar x("x","EtMiss",xmin,xmax);
    // 5) create roo data hist
    RooDataHist HistSig("sig","hist sig",x,sig);
    RooDataHist HistBkg("bkg","hist bkg",x,bkg);
    RooDataHist HistToFit("target","hist target",x,h);
    // 6) create roo hist pdf
    RooHistPdf PdfSig("pdfsig","pdfsig",x,HistSig,0);
    RooHistPdf PdfBkg("pdfbkg","pdfbkg",x,HistBkg,0); //last number is interpolation
    // 7) create model
    RooAddPdf PdfModel("model","model",RooArgList(PdfSig,PdfBkg),f);
    // 8) fit
    RooFitResult *r;
    RooPlot *frame=x.frame();
    RooMsgService::instance().setSilentMode(true);
    r = PdfModel.fitTo(HistToFit,
            SumW2Error(kTRUE),
            Save(), 
            PrintEvalErrors(-1),
            PrintLevel(-1),
            Minos(kTRUE),
            Warnings(0)
            );
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
        r->Write(Form("%s_roofit",name.c_str() ) );
        fOut->Close();
    }

    return f.getVal();
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
