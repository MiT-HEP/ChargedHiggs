#include "interface/Fitter.hpp"

#ifdef HAVE_COMBINE

#include "HiggsAnalysis/CombinedLimit/interface/RooSpline1D.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooBernsteinFast.h"
#include "RooGenericPdf.h"
#include "RooExponential.h"

using namespace RooFit;

Fitter::Fitter(){
    w_ = NULL;
    inputMasks.push_back("ChargedHiggsTauNu/Vars/Mt_HplusToTauNu_M-%.0f_13TeV_amcatnlo");
    datasetMask_ = "hist_Mt_M%.0f";
    //xsecMask_ = "xsec_Mt_cat%d";
    xsecMask_ = "xsec";
    eaMask_ = "ea_sigmodel_cat%d";
    normMask_ = "pdf_sigmodel_cat%d_norm";
    modelMask_ = "pdf_sigmodel_cat%d";
    massMask_ = "%.0f";
    writeDatasets_ = true;
    plot = true;
    plotDir = "plot/sigfit";

    xmin=0;
    xmax=4000;

    verbose=false;

    outname = "sigfit.root";
    inname = "ChHiggs.root";
}

void Fitter::info(){
    cout<<"----------- FITTER INFO -----------"<<endl;
    //cout<<"nGaussians = "<<nGaussians <<endl;
    //cout<<"nBernstein = "<<nBernstein <<endl;
    cout<<"Gaus*poln"<<endl;
    cout<<"xMin="<<xmin <<endl;
    cout<<"xMax="<<xmax <<endl;
    cout<<"SYST="<<systLabel_<<endl;
    cout<<"Dataset M="<<datasetMask_ <<endl;
    cout<<"XSEC M="<<xsecMask_ <<endl;
    cout<<"EA M="<<eaMask_ <<endl;
    cout<<"NORM M="<<normMask_ <<endl;
    cout<<"MODEL M="<<modelMask_ <<endl;
    for(auto &s : inputMasks) cout <<"mask = "<<s<<endl;
    for(auto &m : mIn ) cout <<"mIn = "<<m<<endl;
    cout<<"-----------------------------------"<<endl;
}

void Fitter::init(){

    gROOT-> SetBatch();

    info();

    TFile *fInput = TFile::Open(inname.c_str() );

    if ( fInput == NULL ) 
        Log(__FUNCTION__,"ERROR","No such file or directory: '"+ inname + "'");

    if(w_==NULL) w_ = new RooWorkspace("w","workspace") ;

    if(mh_==NULL) mh_ = new RooRealVar("MH","MH",200,500);
    if(x_==NULL) x_ = new RooRealVar("mt","mt",xmin,xmax);
    mh_->setConstant();


    // * take TH1F and make RooDataHist
    for(int cat=0;cat < int(inputMasks.size()); ++cat)
    {
        vector<float> xSec_x;
        vector<float> xSec_y;
        vector<float> ea_y;

        map<string,vector<float> > eaSystMu_y;

        for( float& m : mIn)
        {
            string mass = Form(massMask_.c_str() ,m);
            TH1D *h = (TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(), m) ) ;

            if (h == NULL) 
                Log(__FUNCTION__,"ERROR","No such histogram: mask="+inputMasks[cat]+ Form("mass=%.1f",m));

            h->Rebin(5); //FIXME REMOVEME


            // -- Construct RooDataHist
            string name =  Form("cat_%d_mass_%s%s",cat,mass.c_str(),systLabel_.c_str());
            hist_ [ name ] = new RooDataHist(
                    Form(datasetMask_.c_str(),m),
                    Form(datasetMask_.c_str(),m),
                    *x_,
                    Import( *h ) 
                    );

            initPars_[Form("c0_%.0f",m)] = h->GetBinCenter(h->GetMaximumBin());
            if (initPars_[Form("c0_%.0f",m)] <50) initPars_[Form("c0_%.0f",m)]= h->GetMean();
            initPars_[Form("c1_%.0f",m)] = h->GetRMS()*.5;
            initPars_[Form("c2_%.0f",m)] = h->GetMean()*.8;
            initPars_[Form("c3_%.0f",m)] = h->GetRMS();
            initPars_[Form("c4_%.0f",m)] = 0.;
            initPars_[Form("c5_%.0f",m)] = h->GetRMS();

            initPars_[Form("c6_%.0f",m)] = .6;
            initPars_[Form("c7_%.0f",m)] = .2;



            if (writeDatasets_)w_ -> import( *hist_[ name ] ); // write datasets in the workspace

            // -- Construct Normalization Splines
            xSec_x . push_back( m );
            int bin0 = h->FindBin(xmin);
            int bin1 = h->FindBin(xmax);
            float xsec=1;
            switch (int (m) )  // hard coded for now
            {
                case 200: {xsec = 0.02952842256; break; }
                case 300: {xsec = 0.002366; break; }
                case 350: {xsec = 0.0013458646; break; }
                case 400: {xsec = 0.000836289779; break; }
                case 500: {xsec = 0.000382537512; break; }
                case 180: {xsec = 0.0599630833405; break; }
                case 220: {xsec = 0.; break; } // FIXME
                case 250: {xsec = 0.005344; break; }
            }
            xSec_y . push_back( xsec );
            ea_y . push_back( h->Integral()/xsec );

            for(auto& syst: systIn) // compute norm difference for syst
            {
                cout <<"->Getting Histo:"<<Form( (inputMasks[cat] +"_"+ syst+ "Up").c_str(), m)<<endl;
                TH1D *hSUp = (TH1D*)fInput ->Get( Form( (inputMasks[cat] +"_"+ syst+ "Up").c_str(), m) ) ;
                TH1D *hSDown = (TH1D*)fInput ->Get( Form( (inputMasks[cat] +"_"+ syst+ "Down").c_str(), m) ) ;
           
                float ea= h->Integral()/xsec ; 
                float eaUp= hSUp->Integral()/xsec;
                float eaDown= hSDown->Integral()/xsec;
                eaSystMu_y[syst].push_back( (eaUp/ea + ea/eaDown)/2 -1.);

                cout<<"SYST="<<syst<<" Mass="<<m<<" EA="<<ea<<" eaUp"<<eaUp<<" eaDown="<<eaDown<<" mu="<<(eaUp/ea + ea/eaDown)/2 -1.<<endl;
            }


        }
        //xSec_x . clear();
        //xSec_y . clear();
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
        w_ -> import ( *xsSpline ,RecycleConflictNodes() );

        string eaName =  Form(eaMask_.c_str() , cat) ;
        RooSpline1D *eaSpline = new RooSpline1D(
                eaName.c_str(),
                eaName.c_str(),
                *mh_,
                xSec_x.size(),
                &(xSec_x[0]),
                &(ea_y[0])
                );
        splines_[ eaName ] = eaSpline;
        w_ -> import ( *eaSpline ,RecycleConflictNodes() );

        RooArgList normList(*eaSpline,*xsSpline);
        string normFormula="@0*@1 * (1.";
        int count=2;

        for(auto& syst: systIn) // compute norm difference for syst
        {
            string eaName =  Form((eaMask_+"_Mu_"+syst).c_str() , cat) ;
            RooSpline1D *eaSpline=new RooSpline1D(
                    eaName.c_str(),
                    eaName.c_str(),
                    *mh_,
                    xSec_x.size(),
                    &(xSec_x[0]),
                    &(eaSystMu_y[syst][0])
                    );
            splines_[ eaName ] = eaSpline;
            w_ -> import ( *eaSpline ,RecycleConflictNodes() );
            normList.add( *eaSpline);
            normFormula += Form( "+@%d*@%d",count ,count+1) ;
            count +=2;

            RooRealVar *vSyst = new RooRealVar(syst.c_str(),syst.c_str(),0); // var for syst shift
            normList.add(*vSyst);

            w_ -> import(*vSyst);
        }
        normFormula+=")";
        RooFormulaVar norm( Form(normMask_.c_str(),cat) ,
                "norm",
                normFormula.c_str(), normList
                );
        w_ -> import (norm,RecycleConflictNodes());//xsec and ea
    }
    fInput -> Close();

}

void Fitter::fit(){
    // sort mass points
    sort( mIn.begin(), mIn.end() );
    // -- Construct Model
    //
    //
    vector<RooRealVar> pars;

    for(int i=0;i<=7;++i)
        pars.push_back(RooRealVar(Form("c%d",i),Form("c%d"),0));

    pars[0].setRange(0,1000);
    pars[1].setRange(5,1000);
    pars[2].setRange(0,1000);
    pars[3].setRange(5,1000);
    pars[4].setRange(0,100);
    pars[5].setRange(5,1000);
    pars[6].setRange(0,1);
    pars[7].setRange(0,1);
    //G+G+E
    RooGaussian g1("g1","g1",*x_,pars[0],pars[1]);
    RooGaussian g2("g2","g2",*x_,pars[2],pars[3]);
    RooGaussian g3("g3","g3",*x_,pars[4],pars[5]);

    //RooGenericPdf  fitModel("model","TMath::Exp(-TMath::Power((@0-@1)/@2,2))*(1+@3*@0+@4*@0*@0)",RooArgList(*x_,mean,sigma,c1,c2));
    //RooGenericPdf  fitModel("model","TMath::Exp(-TMath::Power((@0-@1)/@2,2))+@4*TMath::Exp(-@0*@3)",RooArgList(*x_,mean,sigma,c1,c2,c3));
    RooAddPdf fitModel("model","model",RooArgList(g1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ;

    for(int i=0;i<pars.size() ;++i) pars[i].setConstant(kFALSE);

    // -- Fit Model to MC
    for(int cat=0; cat < int(inputMasks.size()) ;++cat)
    {
        for( auto & m: mIn )
        {
            for(int i=0;i<pars.size() ;++i) 
                {
                pars[i].setVal(initPars_[Form("c%d_%.0f",i,m)] );
                //cout <<"INIT "<<i<<": "<<initPars_[Form("c%d_%.0f",i,m)] <<endl;
                }
            // ------------------------- FIT ----------------
            string mass = Form( massMask_.c_str(), m );
            string name =  Form("cat_%d_mass_%s%s",cat,mass.c_str(),systLabel_.c_str());  

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
            fitModel.fitTo( *hist_[ name ] ,SumW2Error(kTRUE), PrintEvalErrors(errlevel),PrintLevel(printlevel),Warnings(warnlevel) );
            fitModel.fitTo( *hist_[ name ] ,SumW2Error(kTRUE), PrintEvalErrors(errlevel),PrintLevel(printlevel),Warnings(warnlevel) );
            // -- print or follow
            //for(int i=0;i<pars.size() ;++i) 
            //    {
            //    cout <<"FIT "<<i<<": "<<pars[i].getVal() <<endl;
            //    }

            // -- Plot
            if (plot ) {
                TCanvas *c = new TCanvas(Form("c_%s_cat%d%s",mass.c_str(),cat,systLabel_.c_str()),"c");
                RooPlot *p = x_ -> frame();
                hist_[ name ] -> plotOn(p,DataError(RooAbsData::SumW2));
                fitModel . plotOn(p);

                //c->SetLogy();
                p -> Draw();
                c -> Update() ;

                c -> SaveAs( Form("%s/fit_mh_%s_cat%d%s.pdf",plotDir.c_str(),mass.c_str(),cat,systLabel_.c_str()) );
                c -> SaveAs( Form("%s/fit_mh_%s_cat%d%s.png",plotDir.c_str(),mass.c_str(),cat,systLabel_.c_str()) );

                //
                delete p;
                delete c;
            } // end plots
            // -- Save coefficients vs mh
            //
            for(int i=0;i<pars.size();++i)
             fitParameters_[ Form("fit_cat%d_mass_%s_c%d%s",cat,mass.c_str(),i,systLabel_.c_str()) ] =  pars[i].getVal();
        } // end mass

        // -- Interpolate coefficients
        for(int i=0;i<pars.size();++i)
        {
            vector<float> mpoints;
            vector<float> c;

            for( auto & m: mIn )
            {
                string mass=Form(massMask_.c_str(),m);
                mpoints.push_back(m);
                c.push_back( fitParameters_[Form("fit_cat%d_mass_%s_c%d%s",cat,mass.c_str(),i,systLabel_.c_str())] ) ;
                cout <<" adding points to splin: "<<(m)<<" "<<fitParameters_[Form("fit_cat%d_mass_%s_c%d%s",cat,mass.c_str(),i,systLabel_.c_str())]<<endl;
            }

            //interpolate model pars
            string splname=Form("sigmodel_cat%d_c%d%s",cat,i,systLabel_.c_str());
            RooSpline1D splineC(
                    splname.c_str(),
                    splname.c_str(),
                    *mh_,
                    mpoints.size(),
                    &(mpoints[0]),
                    &(c[0])
                    ); 
            w_->import(splineC,RecycleConflictNodes());
            mpoints.clear();
            c.clear();
        }
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

void Fitter::end(){
    // -- Save final model 
    // final model uses splines to get the values of the multigaussians
    // it will also include nuisances variables if necessary
    //
    for( int cat=0;cat<int(inputMasks.size()) ;++cat)
    {

        string name = Form(modelMask_.c_str(),cat);
        //RooGenericPdf *sigModel = new RooGenericPdf(name.c_str(),"TMath::Exp(-TMath::Power((@0-@1)/@2,2))*(1+@3*@0+@4*@0*@0)",
        //RooGenericPdf *sigModel = new RooGenericPdf(name.c_str(),"TMath::Exp(-TMath::Power((@0-@1)/@2,2))+@4*TMath::Exp(-@0*@3)",
        //        RooArgList(*x_,
        //           * (w_->var(Form("sigmodel_cat%d_mean",cat)) ),
        //           * (w_->var(Form("sigmodel_cat%d_sigma",cat)) ),
        //           * (w_->var(Form("sigmodel_cat%d_c1",cat)) ),
        //           * (w_->var(Form("sigmodel_cat%d_c2",cat)) ),
        //           * (w_->var(Form("sigmodel_cat%d_c3",cat)) )
        //            ) 
        //        );
        cout <<"->Trying to acess vars:"<<Form("sigmodel_cat%d_c0%s",cat,systLabel_.c_str())<<endl;
        w_->Print();
        RooGaussian g1("g1","g1",*x_,  * (w_->function(Form("sigmodel_cat%d_c0%s",cat,systLabel_.c_str()))), *(w_->function(Form("sigmodel_cat%d_c1%s",cat,systLabel_.c_str()))));
        RooGaussian g2("g2","g2",*x_,  * (w_->function(Form("sigmodel_cat%d_c2%s",cat,systLabel_.c_str()))), *(w_->function(Form("sigmodel_cat%d_c3%s",cat,systLabel_.c_str()))));
        RooGaussian g3("g3","g3",*x_,  * (w_->function(Form("sigmodel_cat%d_c4%s",cat,systLabel_.c_str()))), *(w_->function(Form("sigmodel_cat%d_c5%s",cat,systLabel_.c_str()))));

        RooAddPdf *sigModel=new RooAddPdf(name.c_str(),"model",RooArgList(g1,g2,g3),
                RooArgList( *(w_->function(Form("sigmodel_cat%d_c6%s",cat,systLabel_.c_str()))),
                    *(w_->function(Form("sigmodel_cat%d_c7%s",cat,systLabel_.c_str())))
                    ),
                kTRUE
                );

        w_ -> import( *sigModel ,RecycleConflictNodes());
        // -- Plot
        if (plot ) {
            TCanvas *c = new TCanvas();
            RooPlot *p = x_ -> frame();
            for(float mh=200;mh<501;mh+= 20)
            {
                cout <<"Interpolating at MH="<<mh<<endl;
                mh_ -> setVal(mh);
                sigModel->plotOn(p);
            }
            p -> Draw();
            c -> SaveAs( Form("%s/interpolation_cat%d%s.pdf",plotDir.c_str(),cat,systLabel_.c_str()) );
            c -> SaveAs( Form("%s/interpolation_cat%d%s.png",plotDir.c_str(),cat,systLabel_.c_str()) );
            delete p;
            delete c;
        } // end plot
    } //end cat
}

void Fitter::write(){
    w_ -> writeToFile(outname.c_str() );
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
