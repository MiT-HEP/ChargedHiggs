#include "interface/Fitter.hpp"

#ifdef HAVE_COMBINE

#include "HiggsAnalysis/CombinedLimit/interface/RooSpline1D.h"

using namespace RooFit;

Fitter::Fitter(){
    w_ = NULL;
    nGaussians= 3;
    inputMasks.push_back("ChargedHiggsTauNu/Vars/Mt_HplusToTauNu-M%.0f");
    datasetMask_ = "hist_Mt_M%.0f";
    //xsecMask_ = "xsec_Mt_cat%d";
    xsecMask_ = "pdf_sigmodel_cat%d__norm";
    massMask_ = "%.0f";
    writeDatasets_ = true;
    plot_ = true;
    plotDir_ = "plot";
}

void Fitter::init(){

    gROOT-> SetBatch();

    TFile *fInput = TFile::Open(inputFileName.c_str() );

    if ( fInput == NULL ) cout<<"[Fitter]::[init]::[ERROR] no such file or directory '"<<inputFileName<<"'"<<endl;

    w_ = new RooWorkspace("w","workspace") ;

    mh_ = new RooRealVar("MH","MH",200,1000);
    x_ = new RooRealVar("Mt","Mt",0,1000);


    // * take TH1F and make RooDataHist
    for(int cat=0;cat < int(inputMasks.size()); ++cat)
    {
        vector<float> xSec_x;
        vector<float> xSec_y;

        for( float& m : mIn)
        {
            string mass = Form(massMask_.c_str() ,m);
            TH1D *h = (TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(), m) ) ;
            h->Rebin(5); //FIXME REMOVEME

            if (h == NULL) cout <<"[Fitter]::[init]::[ERROR] no such histogram: mask='"<<inputMasks[cat]<<"' mass="<<m<<endl;

            // -- Construct RooDataHist
            string name =  Form("cat_%d_mass_%s",cat,mass.c_str());
            hist_ [ name ] = new RooDataHist(
                            Form(datasetMask_.c_str(),m),
                            Form(datasetMask_.c_str(),m),
                            *x_,
                            Import( *h ) 
                            );

            if (writeDatasets_)w_ -> import( *hist_[ name ] ); // write datasets in the workspace

            // -- Construct Normalization Spline
            xSec_x . push_back( m );
            xSec_y . push_back( h->Integral() );
        }
        xSec_x . clear();
        xSec_y . clear();
        // -- Construct Normalization Spline 2
        //
        string xsecName =  Form(xsecMask_.c_str() , cat) ;
        RooSpline1D *xsSpline = new RooSpline1D(
                        xsecName.c_str(),
                        xsecName.c_str(),
                        *mh_,
                        xSec_x.size(),
                        &(xSec_x[0]),
                        &(xSec_y[0])
                        );
        splines_[ xsecName ] = xsSpline;
        w_ -> import ( *xsSpline  );
    }

    fInput -> Close();
   

    // -- Init parameters
    //
    for(int i=0;i<nGaussians ;++i)
        {
        startMean_  . push_back( 100. + 30*i);
        startSigma_ . push_back( 100. + 30*i);
        startFraction_ . push_back(1./nGaussians);
        }
}


void Fitter::fitSignal(){
    // sort mass points
    sort( mIn.begin(), mIn.end() );
    // -- Construct Model
    RooArgList *gaussians = new RooArgList();
    RooArgList *coeffs = new RooArgList();

    vector<RooRealVar*> v_means; // get a direct access to the RooRealVars
    vector<RooRealVar*> v_sigmas;
    vector<RooRealVar*> v_fracs;

    for( int i=0;i< nGaussians ;++i)
    {
        RooRealVar *m = new RooRealVar( Form("fitmodel_gaus_mean_%d",i),Form("fitmodel_gaus_mean_%d",i), startMean_[i] );
        RooRealVar *s = new RooRealVar( Form("fitmodel_gaus_sigma_%d",i), Form("fitmodel_gaus_sigma_%d",i), startSigma_[i] );
        RooGaussian *gaus = new RooGaussian(Form("fitmodel_gaus_g%d",i),Form("fitmodel_gaus_g%d",i),*x_, *m, *s);

        v_means.push_back(m);
        v_sigmas.push_back(s);

        gaussians -> add( *gaus );

        if (i != nGaussians-1) // sum to 1
        {
        RooRealVar *f = new RooRealVar( Form("fitmodel_gaus_frac_%d",i) ,Form("fitmodel_gaus_frac_%d",i) , startFraction_[i], 0.01,1.0 );
        v_fracs.push_back(f);
        coeffs -> add(   *f  ) ;
        }
    }

    string name = "model";
    // ------------------------------------------------------------------------------\/recursive
    RooAddPdf *fitModel = new RooAddPdf(name.c_str(),name.c_str(),*gaussians,*coeffs,true);
    // -- Fit Model to data
    for(int cat=0; cat < int(inputMasks.size()) ;++cat)
    {
        for( auto & m: mIn )
        {
             // ------------------------- FIT ----------------
             string mass = Form( massMask_.c_str(), m );
             string name =  Form("cat_%d_mass_%s",cat,mass.c_str());  
             fitModel->fitTo( *hist_[ name ]  );

             // -- Plot
             if (plot_ ) {
                 TCanvas *c = new TCanvas();
                 RooPlot *p = x_ -> frame();
                 hist_[ name ] -> plotOn(p,DataError(RooAbsData::SumW2));
                 fitModel -> plotOn(p);
                 p -> Draw();
                 c -> SaveAs( Form("%s/fit_mh_%s_cat%d.pdf",plotDir_.c_str(),mass.c_str(),cat) );
                 c -> SaveAs( Form("%s/fit_mh_%s_cat%d.png",plotDir_.c_str(),mass.c_str(),cat) );
                 delete p;
                 delete c;
                }
            // -- Save coefficients vs mh
            for(int i=0;i< nGaussians ;++ i) 
            {
                fitParameters_[ Form("fit_cat%d_mass_%s_mean_%d",cat,mass.c_str(), i )] = v_means[i] -> getVal();
                fitParameters_[ Form("fit_cat%d_mass_%s_sigma_%d",cat,mass.c_str(), i )] = v_sigmas[i] -> getVal();
                if (i !=nGaussians -1 ) fitParameters_[ Form("fit_cat%d_mass_%s_frac_%d",cat,mass.c_str(), i )] = v_fracs[i] -> getVal();
            }
        }
        // -- Interpolate coefficients
        for(int i=0;i< nGaussians ;++i) 
        {
            vector<float> x;
            vector<float> mean;
            vector<float> sigma;
            vector<float> frac;
            for( auto & m: mIn )
            {
                string mass=Form(massMask_.c_str(),m);
                x.push_back(m);
                mean.push_back( fitParameters_[Form("fit_cat%d_mass_%s_mean_%d",cat,mass.c_str(),i)] ) ;
                sigma.push_back( fitParameters_[Form("fit_cat%d_mass_%s_sigma_%d",cat,mass.c_str(),i)] ) ;
                if (i != nGaussians -1 ) frac.push_back( fitParameters_[Form("fit_cat%d_mass_%s_frac_%d",cat,mass.c_str(),i)] ) ;
            }

            // -- Save splines 
            string splname= Form("spline_cat%d_mean",cat);
            RooSpline1D *meanSpline = new RooSpline1D(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        x.size(),
                        &(x[0]),
                        &(mean[0])
                        );
            splines_[ splname ] = meanSpline;
            w_ -> import ( *meanSpline  );
            splname= Form("spline_cat%d_sigma",cat);
            RooSpline1D *sigmaSpline = new RooSpline1D(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        x.size(),
                        &(x[0]),
                        &(sigma[0])
                        );
            splines_[ splname ] = sigmaSpline;
            w_ -> import ( *sigmaSpline  );
            if (i =nGaussians -1 ){
                splname= Form("spline_cat%d_frac",cat);
                RooSpline1D *fracSpline = new RooSpline1D(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        x.size(),
                        &(x[0]),
                        &(frac[0])
                        );
                splines_[ splname ] = fracSpline;
                w_ -> import ( *fracSpline  );
            } // not last gaus
        } // nGaussians
    }
    return;
}

// -- RooAbsReal* getMeanWithSyst(string name,RooAbsReal * mean){
// --     // Foreseen to add systematics
// --     RooArgList *depends = new RooArgList();
// --     string formula = "@0 ";
// --     depends -> add(*mean);
// -- 
// --     RooFormulaVar *formVar= new RooFormulaVar(name.c_str(), name.c_str(), formula.c_str(), depends);
// --     return formVar;
// -- }

void Fitter::finalModel(){
    // -- Save final model 
    // final model uses splines to get the values of the multigaussians
    // it will also include nuisances variables if necessary
    //
    for( int cat=0;cat<int(inputMasks.size()) ;++cat)
    {
        RooArgList *coeffs = new RooArgList();
        RooArgList *gaussians = new RooArgList();

        for( int i=0;i< nGaussians ;++i)
        {

            RooAbsReal *m =  splines_[  Form("spline_cat%d_mean",cat) ] ;
            RooAbsReal *s =  splines_[  Form("spline_cat%d_sigma",cat) ] ;
            RooGaussian *gaus = new RooGaussian(Form("gaus_g%d",i),Form("gaus_g%d",i),*x_, *m, *s);


            gaussians -> add( *gaus );

            if (i != nGaussians-1) // sum to 1
            {
            RooAbsReal *f = splines_ [  Form("spline_cat%d_frac",cat) ];
            coeffs -> add(   *f  ) ;
            }
        }

        string name = Form("pdf_sigmodel_cat%d",cat);
        // ------------------------------------------------------------------------------\/recursive
        RooAddPdf *sigModel = new RooAddPdf(name.c_str(),name.c_str(),*gaussians,*coeffs,true);
        w_ -> import( *sigModel );
        // -- Plot
        if (plot_ ) {
            TCanvas *c = new TCanvas();
            RooPlot *p = x_ -> frame();
            for(float mh=200;mh<1000;mh+= 50)
            {
                mh_ -> setVal(mh);
                sigModel->plotOn(p);
            }
            p -> Draw();
            c -> SaveAs( Form("%s/interpolation_cat%d.pdf",plotDir_.c_str(),cat) );
            c -> SaveAs( Form("%s/interpolation_cat%d.png",plotDir_.c_str(),cat) );
            delete p;
            delete c;
           } // end plot
    } //end cat

}

void Fitter::write(){
    w_ -> writeToFile(outputFileName.c_str() );
}



//end HAVE_COMBINE
#endif 

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
