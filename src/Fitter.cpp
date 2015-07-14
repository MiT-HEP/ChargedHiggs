#include "interface/Fitter.hpp"

#ifdef HAVE_COMBINE

#include "HiggsAnalysis/CombinedLimit/interface/RooSpline1D.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooBernsteinFast.h"

using namespace RooFit;

Fitter::Fitter(){
    w_ = NULL;
    nGaussians= 3;
    nBernstein= 1;
    inputMasks.push_back("ChargedHiggsTauNu/Vars/Mt_HplusToTauNu-M%.0f");
    datasetMask_ = "hist_Mt_M%.0f";
    //xsecMask_ = "xsec_Mt_cat%d";
    xsecMask_ = "pdf_sigmodel_cat%d__norm";
    massMask_ = "%.0f";
    writeDatasets_ = true;
    plot = true;
    plotDir = "plot";

    xmin=0;
    xmax=1000;

    verbose=false;
}

void Fitter::info(){
    cout<<"----------- FITTER INFO -----------"<<endl;
    cout<<"nGaussians = "<<nGaussians <<endl;
    cout<<"nBernstein = "<<nBernstein <<endl;
    cout<<"xMin="<<xmin <<endl;
    cout<<"xMax="<<xmax <<endl;
    for(auto &s : inputMasks) cout <<"mask = "<<s<<endl;
    for(auto &m : mIn ) cout <<"mIn = "<<m<<endl;
    cout<<"-----------------------------------"<<endl;
}

void Fitter::init(){

    gROOT-> SetBatch();

    info();

    TFile *fInput = TFile::Open(inputFileName.c_str() );

    if ( fInput == NULL ) cout<<"[Fitter]::[init]::[ERROR] no such file or directory '"<<inputFileName<<"'"<<endl;

    w_ = new RooWorkspace("w","workspace") ;

    mh_ = new RooRealVar("MH","MH",200,1000);
    x_ = new RooRealVar("Mt","Mt",xmin,xmax);


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
            int bin0 = h->FindBin(xmin);
            int bin1 = h->FindBin(xmax);
            xSec_y . push_back( h->Integral(bin0,bin1) );
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
    initGaus();
    initBern();

}

void Fitter::initBern(){
    for(int i=0;i<nBernstein ;++i)
    {
        if (i== 0 ) startBern_.push_back(0.5);
        else if (i== 1 ) startBern_.push_back(2);
        else startBern_.push_back(0.1);
    }
}

void Fitter::initGaus(){
    for(int i=0;i<nGaussians ;++i)
        {
        if(i==0 ){
            startMean_  . push_back( 100 );
            startSigma_ . push_back( 100. );
            startFraction_ . push_back(.3);
            }
        if( i== 1) {
            startMean_  . push_back( 0. );
            startSigma_ . push_back( 500. );
            startFraction_ . push_back(.30);
            }
        else if (i== 2){
            startMean_  . push_back( 100);
            startSigma_ . push_back( 300);
            startFraction_ . push_back(.30);
            }
        else{
            startMean_  . push_back( 500);
            startSigma_ . push_back( 100+100*i);
            startFraction_ . push_back(.05);
            }
        }
    return;
}

void Fitter::addBernFitModel(RooArgList *pdfs,RooArgList*coeffs, bool isLast)
{
    if (nBernstein <=0 ) return;

        RooArgList * args = new RooArgList();
        for(int b=0;b<nBernstein; b++)
            {
            string name = Form("fitmodel_bern_param_%d",b);
            RooRealVar *tmp = new RooRealVar(name.c_str(), name.c_str(), startBern_[b],0.01,100.);
            vars_ [ name ] = tmp;
            args -> add( *tmp );
            }
        string name = Form("fitmodel_bern_%d",nBernstein);
        RooAbsPdf *bern; 
        if (nBernstein ==1 ) bern= new RooBernsteinFast<1>(name.c_str(),name.c_str(),*x_, *args);
        if (nBernstein ==2 ) bern= new RooBernsteinFast<2>(name.c_str(),name.c_str(),*x_, *args);
        if (nBernstein ==3 ) bern= new RooBernsteinFast<3>(name.c_str(),name.c_str(),*x_, *args);
        if (nBernstein ==4 ) bern= new RooBernsteinFast<4>(name.c_str(),name.c_str(),*x_, *args);
        if (nBernstein ==5 ) bern= new RooBernsteinFast<5>(name.c_str(),name.c_str(),*x_, *args);
        if (nBernstein ==6 ) bern= new RooBernsteinFast<6>(name.c_str(),name.c_str(),*x_, *args);
        if (nBernstein ==7 ) bern= new RooBernsteinFast<7>(name.c_str(),name.c_str(),*x_, *args);

        pdfs->add( *bern );

        if ( not isLast ){ // if I have other pdfs add fraction
            RooRealVar *f = new RooRealVar( Form("fitmodel_bern_frac") ,Form("fitmodel_bern_frac") , 0.3, 0.01,1.0 );
            vars_[ f->GetName() ] = f;
            coeffs -> add ( *f );
        }
} 

void Fitter::addGausFitModel(RooArgList *pdfs, RooArgList *coeffs, bool isLast)
{
    for( int i=0;i< nGaussians ;++i)
    {
        RooRealVar *m = new RooRealVar( Form("fitmodel_gaus_mean_%d",i),Form("fitmodel_gaus_mean_%d",i), startMean_[i] );
        RooRealVar *s = new RooRealVar( Form("fitmodel_gaus_sigma_%d",i), Form("fitmodel_gaus_sigma_%d",i), startSigma_[i] );
        RooGaussian *gaus = new RooGaussian(Form("fitmodel_gaus_g%d",i),Form("fitmodel_gaus_g%d",i),*x_, *m, *s);

        vars_[ m->GetName() ] = m;
        vars_[ s->GetName() ] = s;

        pdfs -> add( *gaus );

        if ( (i != nGaussians-1 )  or not isLast)
        {
        RooRealVar *f = new RooRealVar( Form("fitmodel_gaus_frac_%d",i) ,Form("fitmodel_gaus_frac_%d",i) , startFraction_[i], 0.01,1.0 );
        vars_ [ f->GetName() ] = f;
        coeffs -> add(   *f  ) ;
        }
    }

}

void Fitter::saveCoefficientsBern(int cat,string mass,bool isLast){
            for(int b=0;b<nBernstein ;++b)
            {
                fitParameters_[ Form("fit_cat%d_mass_%s_bern_par_%d",cat,mass.c_str(),b) ] = vars_[ Form("fitmodel_bern_param_%d",b) ]  -> getVal();
            }
            if( nBernstein >0 && not isLast ) fitParameters_[ Form("fit_cat%d_mass_%s_bern_frac",cat,mass.c_str()) ] = vars_["fitmodel_bern_frac" ]  -> getVal();

}


void Fitter::saveCoefficientsGaus(int cat,string mass,bool isLast)
{
            for(int i=0;i< nGaussians ;++i) 
            {
                fitParameters_[ Form("fit_cat%d_mass_%s_mean_%d",cat,mass.c_str(), i )] = vars_[ Form("fitmodel_gaus_mean_%d",i) ] -> getVal();
                fitParameters_[ Form("fit_cat%d_mass_%s_sigma_%d",cat,mass.c_str(), i )] = vars_[Form("fitmodel_gaus_sigma_%d",i)] -> getVal();
                if ( (i !=nGaussians -1) or not isLast ) fitParameters_[ Form("fit_cat%d_mass_%s_frac_%d",cat,mass.c_str(), i )] = vars_[ Form("fitmodel_gaus_frac_%d",i) ] -> getVal();
            }
    return;
}

void Fitter::interpolateBern(int cat,  bool isLast){

        for(int b=0;b<nBernstein ;++b)
        {
            vector<float> x;
            vector<float> frac;
            vector<float> pars;
            for( auto & m: mIn )
            {
                string mass=Form(massMask_.c_str(),m);
                x.push_back(m);
                pars.push_back( fitParameters_[Form("fit_cat%d_mass_%s_bern_par_%d",cat,mass.c_str(),b)] ) ;
                if ( isLast ) frac.push_back( fitParameters_[Form("fit_cat%d_mass_%s_bern_frac",cat,mass.c_str())] ) ;
            }
            string splname= Form("spline_cat%d_par%d_bern",cat,b);
            RooSpline1D *spline = new RooSpline1D(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        x.size(),
                        &(x[0]),
                        &(pars[0])
                        ); 
            splines_[ splname ] = spline;
            //w_ -> import ( *spline  );

            if( isLast ){
                    string splname= Form("spline_cat%d_bern_frac",cat);
                    RooSpline1D *spline = new RooSpline1D(
                                splname.c_str(),
                                splname.c_str(),
                                *mh_,
                                x.size(),
                                &(x[0]),
                                &(frac[0])
                                ); 
                    splines_[ splname ] = spline;
                    //w_ -> import ( *spline  );
            }
        
        } // end loop over bernstein
    return;
}

void Fitter::interpolateGaus(int cat, bool isLast)
{
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
            string splname= Form("spline_cat%d_g%d_mean",cat,i);
            RooSpline1D *meanSpline = new RooSpline1D(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        x.size(),
                        &(x[0]),
                        &(mean[0])
                        );
            splines_[ splname ] = meanSpline;
            //w_ -> import ( *meanSpline  );
            splname= Form("spline_cat%d_g%d_sigma",cat,i);
            RooSpline1D *sigmaSpline = new RooSpline1D(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        x.size(),
                        &(x[0]),
                        &(sigma[0])
                        );
            splines_[ splname ] = sigmaSpline;
            //w_ -> import ( *sigmaSpline  );
            if ( (i !=nGaussians -1)  or not isLast){
                splname= Form("spline_cat%d_g%d_frac",cat,i);
                RooSpline1D *fracSpline = new RooSpline1D(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        x.size(),
                        &(x[0]),
                        &(frac[0])
                        );
                splines_[ splname ] = fracSpline;
                //w_ -> import ( *fracSpline  );
            } // not last gaus
        } // nGaussians
    return;
}

void Fitter::fitSignal(){
    // sort mass points
    sort( mIn.begin(), mIn.end() );
    // -- Construct Model
    RooArgList *pdfs = new RooArgList();
    RooArgList *coeffs = new RooArgList();


    //bernstein
    addBernFitModel(pdfs,coeffs,nGaussians<=0);
    // gaussians
    addGausFitModel(pdfs,coeffs,true);

    // construct Fitmodel
    string name = "model";

    RooAddPdf *fitModel = new RooAddPdf(name.c_str(),name.c_str(),*pdfs,*coeffs,true);


    // -- Fit Model to data
    for(int cat=0; cat < int(inputMasks.size()) ;++cat)
    {
        for( auto & m: mIn )
        {
             // ------------------------- FIT ----------------
             string mass = Form( massMask_.c_str(), m );
             string name =  Form("cat_%d_mass_%s",cat,mass.c_str());  

             // switch on and off verbosity of roofit/minuit
             int errlevel = -1;
             int printlevel = -1;
             int warnlevel = 0;
             if (verbose)
             {
                errlevel = 1;
                printlevel = 1;
                warnlevel = 1;
             }
             // -- fit

             fitModel->fitTo( *hist_[ name ] ,SumW2Error(kTRUE), PrintEvalErrors(errlevel),PrintLevel(printlevel),Warnings(warnlevel) );
             fitModel->fitTo( *hist_[ name ] ,SumW2Error(kTRUE), PrintEvalErrors(errlevel),PrintLevel(printlevel),Warnings(warnlevel) );

             // -- Plot
             if (plot ) {
                 TCanvas *c = new TCanvas();
                 RooPlot *p = x_ -> frame();
                 hist_[ name ] -> plotOn(p,DataError(RooAbsData::SumW2));
                 fitModel -> plotOn(p);

                 for(int i=0;i < nGaussians ;++i)
                 {
                       fitModel -> plotOn(p, Components(Form("fitmodel_gaus_g%d",i)),LineStyle(kDashed) );
                 }

                 if( nBernstein >0 )
                 {
                fitModel -> plotOn(p, Components(Form("fitmodel_bern_%d",nBernstein)),LineStyle(kDotted) );
                 }

                 c->SetLogy();
                 p -> Draw();
                 c -> SaveAs( Form("%s/fit_mh_%s_cat%d.pdf",plotDir.c_str(),mass.c_str(),cat) );
                 c -> SaveAs( Form("%s/fit_mh_%s_cat%d.png",plotDir.c_str(),mass.c_str(),cat) );
                 delete p;
                 delete c;
                } // end plots
            // -- Save coefficients vs mh
            saveCoefficientsBern(cat,mass, nGaussians<=0);
            saveCoefficientsGaus(cat,mass,true);
        } // end mass

        // -- Interpolate coefficients
        // -- bern
        interpolateBern(cat,nGaussians<=0);
        // -- gaus
        interpolateGaus(cat,true);
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
        RooArgList *pdfs = new RooArgList();

        if(nBernstein >0 )
        {  
            RooArgList *pars=new RooArgList();
            for(int b=0;b<nBernstein; b++)
                {
                RooAbsReal *p = splines_ [ Form("spline_cat%d_par%d_bern",cat,b) ] ;
                if (p == NULL ) cout<<"[Fitter]::[finalModel]::[ERROR] bern parameter"<<b<<"for cat"<<cat<<"is NULL "<<endl;
                pars -> add( *p );
                }
            RooAbsPdf* pdf; 

            if (nBernstein==1) pdf=new RooBernsteinFast<1>(Form("bern_%d",nBernstein), Form("bern_%d",nBernstein), *x_, *pars);
            if (nBernstein==2) pdf=new RooBernsteinFast<2>(Form("bern_%d",nBernstein), Form("bern_%d",nBernstein), *x_, *pars);
            if (nBernstein==3) pdf=new RooBernsteinFast<3>(Form("bern_%d",nBernstein), Form("bern_%d",nBernstein), *x_, *pars);
            if (nBernstein==4) pdf=new RooBernsteinFast<4>(Form("bern_%d",nBernstein), Form("bern_%d",nBernstein), *x_, *pars);
            if (nBernstein==5) pdf=new RooBernsteinFast<5>(Form("bern_%d",nBernstein), Form("bern_%d",nBernstein), *x_, *pars);
            if (nBernstein==6) pdf=new RooBernsteinFast<6>(Form("bern_%d",nBernstein), Form("bern_%d",nBernstein), *x_, *pars);
            if (nBernstein==7) pdf=new RooBernsteinFast<7>(Form("bern_%d",nBernstein), Form("bern_%d",nBernstein), *x_, *pars);

            pdfs ->add( *pdf );
            if (nGaussians >0 ) coeffs -> add( *splines_[Form("spline_cat%d_bern_frac",cat)] );
        }

        for( int i=0;i< nGaussians ;++i)
        {

            RooAbsReal *m =  splines_[  Form("spline_cat%d_g%d_mean",cat,i) ] ;
            RooAbsReal *s =  splines_[  Form("spline_cat%d_g%d_sigma",cat,i) ] ;
            
            if( m== NULL ) cout<<"[Fitter]::[finalModel]::[ERROR] mean spline is NULL"<< Form("spline_cat%d_g%d_mean",cat,i)<<endl;
            if( s== NULL ) cout<<"[Fitter]::[finalModel]::[ERROR] sigma spline is NULL"<< Form("spline_cat%d_g%d_sigma",cat,i)<<endl;
            if (m== NULL or s==NULL) for(auto& p: splines_) cout<<"[Fitter]::[finalModel]::[DEBUG] splines_["<<p.first<<"] = "<<p.second<<endl;

            RooGaussian *gaus = new RooGaussian(Form("gaus_g%d",i),Form("gaus_g%d",i),*x_, *m, *s);


            pdfs -> add( *gaus );

            if (i != nGaussians-1) // sum to 1
            {
            RooAbsReal *f = splines_ [  Form("spline_cat%d_g%d_frac",cat,i) ];
            coeffs -> add(   *f  ) ;
            }
        }

        string name = Form("pdf_sigmodel_cat%d",cat);
        // ------------------------------------------------------------------------------\/recursive
        RooAddPdf *sigModel = new RooAddPdf(name.c_str(),name.c_str(),*pdfs,*coeffs,true);
        w_ -> import( *sigModel );
        // -- Plot
        if (plot ) {
            TCanvas *c = new TCanvas();
            RooPlot *p = x_ -> frame();
            for(float mh=200;mh<1000;mh+= 50)
            {
                mh_ -> setVal(mh);
                sigModel->plotOn(p);
            }
            p -> Draw();
            c -> SaveAs( Form("%s/interpolation_cat%d.pdf",plotDir.c_str(),cat) );
            c -> SaveAs( Form("%s/interpolation_cat%d.png",plotDir.c_str(),cat) );
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
