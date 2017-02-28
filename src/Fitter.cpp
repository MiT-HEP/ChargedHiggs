#include "interface/Fitter.hpp"

#ifdef HAVE_COMBINE

#include "HiggsAnalysis/CombinedLimit/interface/RooSpline1D.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooBernsteinFast.h"
#include "RooGenericPdf.h"
#include "RooExponential.h"
#include <cstdio>
#include <cstdlib>

using namespace RooFit;

Fitter::Fitter(){
    w_ = NULL;
    inputMasks.push_back("ChargedHiggsTauNu/Vars/Mt_HplusToTauNu_M-%.0f_13TeV_amcatnlo");
    datasetMask_ = "hist_%s_cat%d_M%.0f";
    //xsecMask_ = "xsec_Mt_cat%d";
    xsecMask_ = "xsec_%s_cat%d";
    eaMask_ = "ea_sigmodel_%s_cat%d";
    normMask_ = "pdf_sigmodel_%s_cat%d_norm";
    modelMask_ = "pdf_sigmodel_%s_cat%d";
    massMask_ = "%.0f";
    writeDatasets_ = true;
    plot = true;
    plotDir = "plot/sigfit";

    xmin=60;
    xmax=150;

    verbose=false;

    outname = "sigfit.root";
    inname = "ChHiggs.root";
}

void Fitter::info(){
    cout<<"----------- FITTER INFO -----------"<<endl;
    cout<<"nGaussians : ";
       for(const auto &i : nGaussians  ) cout<< "cat="<<i.first.first<<", proc="<<i.first.second<<" = "<<i.second<<endl;
    cout<<endl;
    //cout<<"nBernstein = "<<nBernstein <<endl;
    //cout<<"Gaus*poln"<<endl;
    cout<<"xMin="<<xmin <<endl;
    cout<<"xMax="<<xmax <<endl;
    cout<<"SYST="<<systLabel_<<endl;
    cout<<"Dataset M="<<datasetMask_ <<endl;
    if (doXsec) cout<<"XSEC M="<<xsecMask_ <<endl;
    cout<<"EA M="<<eaMask_ <<endl;
    cout<<"NORM M="<<normMask_ <<endl;
    cout<<"MODEL M="<<modelMask_ <<endl;
    for(auto &s : inputMasks) cout <<"mask = "<<s<<endl;
    cout<<"mIn = ";for(auto &m : mIn ) cout <<m<<","; cout<<endl;
    cout <<"procs = ";for(auto &p : processes) cout <<p<<",";cout<<endl;
    cout<<"** ttH125 ONLY **"<<endl;
    cout<<"-----------------------------------"<<endl;
}

void Fitter::init(){

    gROOT-> SetBatch();

    info();

    if (not verbose) RooMsgService::instance().setSilentMode(true);

    TFile *fInput = TFile::Open(inname.c_str() );

    if ( fInput == NULL ) 
        Log(__FUNCTION__,"ERROR","No such file or directory: '"+ inname + "'");

    if(w_==NULL) w_ = new RooWorkspace("w","workspace") ;

    if(mh_==NULL) mh_ = new RooRealVar("MH","MH",mhmin,mhmax);
    if(x_==NULL) x_ = new RooRealVar("mmm","mmm",xmin,xmax);
    mh_->setConstant();


    // * take TH1F and make RooDataHist
    cout<<"TH1F->RooDataHist"<<endl;
    for(int cat=0;cat < int(inputMasks.size()); ++cat)
    for(const string& proc:processes)
    {
        vector<float> xSec_x;
        vector<float> xSec_y;
        vector<float> ea_y;

        map<string,vector<float> > eaSystMu_y;

        for( float& m : mIn)
        {
            if (proc == "ttH" and fabs(m-125)> 0.1) continue;//ttH125 FIXME

            string mass = Form(massMask_.c_str() ,m);
            TH1D *h = (TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(),proc.c_str(), m) ) ;

            if (h == NULL) 
                Log(__FUNCTION__,"ERROR","No such histogram: mask="+inputMasks[cat]+ Form("mass=%.1f proc=%s",m,proc.c_str()));

            //h->Rebin(5); //FIXME REMOVEME


            // -- Construct RooDataHist
            string name =  Form("%s_cat_%d_mass_%s%s",proc.c_str(),cat,mass.c_str(),systLabel_.c_str());
            hist_ [ name ] = new RooDataHist(
                    Form(datasetMask_.c_str(),proc.c_str(),cat,m),
                    Form(datasetMask_.c_str(),proc.c_str(),cat,m),
                    *x_,
                    Import( *h ) 
                    );

            if (nGaussians.find( pair<int,string>(cat,proc) ) == nGaussians.end()) 
            {
                nGaussians[pair<int,string>(cat,proc)] = 3; // default
            }


            initPars_[Form("c0_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetBinCenter(h->GetMaximumBin());
            //if (initPars_[Form("c0_%s_cat%d_%.0f",proc.c_str(),cat,m)] <120) initPars_[Form("c0_%s_cat%d_%.0f",proc.c_str(),cat,m)]= h->GetMean();
            //initPars_[Form("c0_%s_cat%d_%.0f",proc.c_str(),cat,m)]= m;
            initPars_[Form("c1_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetRMS()*.5;
            initPars_[Form("c2_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetMean()*.95;
            initPars_[Form("c3_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetRMS();
            initPars_[Form("c4_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetMean()*.9;
            initPars_[Form("c5_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetRMS()*2;

            initPars_[Form("c6_%s_cat%d_%.0f",proc.c_str(),cat,m)] = .6;
            initPars_[Form("c7_%s_cat%d_%.0f",proc.c_str(),cat,m)] = .2;

            if (nGaussians[pair<int,string>(cat,proc)] ==2)
            {
                initPars_[Form("c4_%s_cat%d_%.0f",proc.c_str(),cat,m)] = .9;
                //initPars_[Form("c5_%s_cat%d_%.0f",proc.c_str(),cat,m)] = .1;
            }


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
                cout <<"->Getting Histo:"<<Form( (inputMasks[cat] +"_"+ syst+ "Up").c_str(),proc.c_str(), m)<<endl;
                TH1D *hSUp = (TH1D*)fInput ->Get( Form( (inputMasks[cat] +"_"+ syst+ "Up").c_str(),proc.c_str(), m) ) ;
                TH1D *hSDown = (TH1D*)fInput ->Get( Form( (inputMasks[cat] +"_"+ syst+ "Down").c_str(),proc.c_str(), m) ) ;
           
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
        RooSpline1D *xsSpline {NULL};
        if (doXsec)
        {
            string xsecName =  Form(xsecMask_.c_str() , proc.c_str(),cat) ;
            xsSpline = new RooSpline1D(
                    xsecName.c_str(),
                    xsecName.c_str(),
                    *mh_,
                    xSec_x.size(),
                    &(xSec_x[0]),
                    &(xSec_y[0])
                    );
            splines_[ xsecName ] = xsSpline;
            w_ -> import ( *xsSpline ,RecycleConflictNodes() );
        }

        string eaName =  Form(eaMask_.c_str() , proc.c_str(),cat) ;

        RooAbsReal* eaSpline;

        if (proc == "ttH")//ttH125 FIXME
        {
            eaSpline = new RooRealVar(eaName.c_str(),eaName.c_str(),ea_y[0]);
            ((RooRealVar*)eaSpline)->setConstant();
        }
        else{
              eaSpline =  new RooSpline1D(
                    eaName.c_str(),
                    eaName.c_str(),
                    *mh_,
                    xSec_x.size(),
                    &(xSec_x[0]),
                    &(ea_y[0])
                    );
        }
        splines_[ eaName ] = eaSpline;
        w_ -> import ( *eaSpline ,RecycleConflictNodes() );

        //RooArgList normList(*eaSpline,*xsSpline);
        RooArgList normList(*eaSpline);
        string normFormula = "@0";
        int count=1;
        if (doXsec) 
        {
            normList.add(*xsSpline) ;
            normFormula += "*@1";
            count+=1;
        }

        normFormula+="* (1.";

        for(auto& syst: systIn) // compute norm difference for syst
        {
            string eaName =  Form((eaMask_+"_Mu_"+syst).c_str() , proc.c_str(),cat) ;
            RooAbsReal *eaSpline;
            if (proc == "ttH")//ttH125 FIXME
            {
                eaSpline = new RooRealVar(eaName.c_str(),eaName.c_str(),eaSystMu_y[syst][0]);
                ((RooRealVar*)eaSpline)->setConstant();
            }
            else{
                eaSpline=new RooSpline1D(
                    eaName.c_str(),
                    eaName.c_str(),
                    *mh_,
                    xSec_x.size(),
                    &(xSec_x[0]),
                    &(eaSystMu_y[syst][0])
                    );
            }
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
        RooFormulaVar norm( Form(normMask_.c_str(),proc.c_str(),cat) ,
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

    // non interpolated
    for(int i=0;i<=20;++i)
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
    // RooAddPdf fitModel("model","model",RooArgList(g1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ;
    std::unique_ptr<RooAbsPdf>fitModel;
    fitModel.reset( new RooAddPdf("model","model",RooArgList(g1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ) ; 


    // -- Fit Model to MC
    cout<<"Fit Model to MC"<<endl;
    for(int cat=0; cat < int(inputMasks.size()) ;++cat)
    for(const string& proc:processes)
    {
        cout<<"-> Doing cat"<<cat<<" proc"<<proc<< " nG="<<nGaussians[pair<int,string>(cat,proc)]<<endl;


        if (nGaussians[pair<int,string>(cat,proc)]==1)
            fitModel.reset( new RooGaussian("g1","g1",*x_,pars[0],pars[1]) ) ; 
        else if (nGaussians[pair<int,string>(cat,proc)]==2)
            fitModel.reset( new RooAddPdf("model","model",RooArgList(g1,g2),RooArgList(pars[4]), kTRUE ) ) ; 
        else if (nGaussians[pair<int,string>(cat,proc)]==3)
            fitModel.reset( new RooAddPdf("model","model",RooArgList(g1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ) ; 


        for(int i=0;i<pars.size() ;++i) pars[i].setConstant(kFALSE);

        // reset last mass for ttH
        lastMass=-1;
        for( auto & m: mIn )
        {
            // set par ranges mass dependent
            int pos=0;
            for(int ig =0 ;ig< nGaussians[pair<int,string>(cat,proc)] ;ig++)
            {
                //mean and sigma
                pars[pos+0].setRange(m-5*(ig+1),m+5*(ig+1));
                pars[pos+1].setRange(0.5*(ig+1),10*(ig+1));
                pos+=2;
            }
            // fractions (last is the closing)
            for(int ig =0 ;ig< nGaussians[pair<int,string>(cat,proc)] -1 ;ig++)
            {
                pars[pos].setRange(0,1);
                pos+=1;
            }
            // return if ttH !=125
            if (proc == "ttH" and fabs(m-125)> 0.1) continue;//ttH125 FIXME
            //mean and sigma
           // pars[pos+0].setRange(0,125);

            for(int i=0;i<pars.size() ;++i) 
            {
                pars[i].setVal(initPars_[Form("c%d_%s_cat%d_%.0f",i,proc.c_str(),cat,m)] );
                //cout <<"INIT "<<i<<": "<<initPars_[Form("c%d_%.0f",i,m)] <<endl;
                // follow
                if (lastMass >0 )
                {
                    if (i%2==0 and i < nGaussians[pair<int,string>(cat,proc)]*2) // mean
                        pars[i].setVal(initPars_[Form("c%d_%s_cat%d_%.0f",i,proc.c_str(),cat,lastMass)] + (m-lastMass) ); 
                    else
                        pars[i].setVal(initPars_[Form("c%d_%s_cat%d_%.0f",i,proc.c_str(),cat,lastMass)] ); 
                }
            }
            lastMass=m; // set last Mass
            // ------------------------- FIT ----------------
            string mass = Form( massMask_.c_str(), m );
            string name =  Form("%s_cat_%d_mass_%s%s",proc.c_str(), cat,mass.c_str(),systLabel_.c_str());  

            cout <<" *-> name is "<<name<<endl; // DEBUG
            if (hist_.find(name) == hist_.end() ) cout <<"Unable to find this hist: check names"<<endl;

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

            // -- print or follow
            if (saveFit) cout <<"Cat="<<cat<<" Proc="<<proc<<" nG="<<nGaussians[pair<int,string>(cat,proc)]<<endl;
            for(int i=0;i<pars.size() ;++i) 
                {
                // set to the current fitted value
                if(saveFit) cout <<"-> Init Parameter "<<i<<": "<<initPars_[Form("c%d_%s_cat%d_%.0f",i,proc.c_str(),cat,m)]<<" --> "<<pars[i].getVal()<<endl;
                initPars_[Form("c%d_%s_cat%d_%.0f",i,proc.c_str(),cat,m)] = pars[i].getVal();
                }

            // -- Plot
            if (plot ) {
                TCanvas *c = new TCanvas(Form("c_%s_%s_cat%d%s",mass.c_str(),proc.c_str(),cat,systLabel_.c_str()),"c");
                RooPlot *p = x_ -> frame();
                hist_[ name ] -> plotOn(p,DataError(RooAbsData::SumW2));
                fitModel -> plotOn(p);

                if (nGaussians[pair<int,string>(cat,proc)] >=2)
                {
                    fitModel->plotOn(p,Components(g1),LineStyle(kDashed),LineColor(kRed+2)); 
                    fitModel->plotOn(p,Components(g2),LineStyle(kDashed),LineColor(kGreen+2)); 
                }
                if (nGaussians[pair<int,string>(cat,proc)] >=3)
                    fitModel->plotOn(p,Components(g3),LineStyle(kDashed),LineColor(kMagenta)); 



                //c->SetLogy();
                p -> Draw();
                c -> Update() ;
               
                string dirName= plotDir + "/" + proc + "/" + mass; 
                system(Form("mkdir -p %s",dirName.c_str()));
                c -> SaveAs( Form("%s/fit_mh_%s_%s_cat%d%s.pdf",dirName.c_str(),mass.c_str(),proc.c_str(),cat,systLabel_.c_str()) );
                c -> SaveAs( Form("%s/fit_mh_%s_%s_cat%d%s.png",dirName.c_str(),mass.c_str(),proc.c_str(),cat,systLabel_.c_str()) );

                //
                delete p;
                delete c;
            } // end plots
            // -- Save coefficients vs mh
            //
            //for(int i=0;i<pars.size();++i)
            for(int i=0;i<3*nGaussians[pair<int,string>(cat,proc)]-1;++i)
             fitParameters_[ Form("fit_%s_cat%d_mass_%s_c%d%s",proc.c_str(),cat,mass.c_str(),i,systLabel_.c_str()) ] =  pars[i].getVal();
        } // end mass

        // -- Interpolate coefficients
        cout<<"Interpolate ---- "<<endl;
        for(int i=0;i<3*nGaussians[pair<int,string>(cat,proc)]-1;++i)
        {
            vector<float> mpoints;
            vector<float> c;

            for( auto & m: mIn )
            {
                if (proc == "ttH" and fabs(m-125)> 0.1) continue;//ttH125 FIXME

                string mass=Form(massMask_.c_str(),m);
                mpoints.push_back(m);
                c.push_back( fitParameters_[Form("fit_%s_cat%d_mass_%s_c%d%s",proc.c_str(),cat,mass.c_str(),i,systLabel_.c_str())] ) ;
                cout <<" adding points to splin: "<<(m)<<" "<<fitParameters_[Form("fit_%s_cat%d_mass_%s_c%d%s",proc.c_str(),cat,mass.c_str(),i,systLabel_.c_str())]<<endl;
            }

            //interpolate model pars
            string splname=Form("sigmodel_%s_cat%d_c%d%s",proc.c_str(),cat,i,systLabel_.c_str());
            if (proc == "ttH")//ttH125 FIXME
            {
                RooRealVar par( Form("fit_%s_cat%d_mass_%s_c%d%s",proc.c_str(),cat,"125",i,systLabel_.c_str()),
                                Form("fit_%s_cat%d_mass_%s_c%d%s",proc.c_str(),cat,"125",i,systLabel_.c_str()),
                                fitParameters_[Form("fit_%s_cat%d_mass_%s_c%d%s",proc.c_str(),cat,"125",i,systLabel_.c_str())]
                        );
                par.setConstant();
                RooFormulaVar *c =new RooFormulaVar ( splname.c_str(),splname.c_str(),"@0", par);
                if (i==0) c  =new RooFormulaVar ( splname.c_str(),splname.c_str(),"@0 +(@1-125)", RooArgList(par , *w_->var("MH"))); // just put a shift
                w_->import(*c, RecycleConflictNodes());
            }
            else{
                RooSpline1D splineC(
                        splname.c_str(),
                        splname.c_str(),
                        *mh_,
                        mpoints.size(),
                        &(mpoints[0]),
                        &(c[0])
                        ); 
                w_->import(splineC,RecycleConflictNodes());
            }
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
    for(const string& proc:processes)
    {

        string name = Form(modelMask_.c_str(),proc.c_str(),cat);

        cout <<"->Trying to access vars:"<<Form("sigmodel_%s_cat%d_c0%s",proc.c_str(),cat,systLabel_.c_str())<<endl;
        w_->Print();
        RooGaussian *g1 = new RooGaussian("g1","g1",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c0%s",proc.c_str(),cat,systLabel_.c_str()))), *(w_->function(Form("sigmodel_%s_cat%d_c1%s",proc.c_str(),cat,systLabel_.c_str()))));

        RooGaussian *g2{NULL},*g3{NULL};
        if (nGaussians[pair<int,string>(cat,proc)] >= 2 )g2=new RooGaussian("g2","g2",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c2%s",proc.c_str(),cat,systLabel_.c_str()))), *(w_->function(Form("sigmodel_%s_cat%d_c3%s",proc.c_str(),cat,systLabel_.c_str()))));
        if (nGaussians[pair<int,string>(cat,proc)] >= 3 )g3= new RooGaussian("g3","g3",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c4%s",proc.c_str(),cat,systLabel_.c_str()))), *(w_->function(Form("sigmodel_%s_cat%d_c5%s",proc.c_str(),cat,systLabel_.c_str()))));

        std::unique_ptr<RooAbsPdf> sigModel;
       
        if (nGaussians[pair<int,string>(cat,proc)] ==1) 
            sigModel.reset(new RooGaussian("g1","g1",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c0%s",proc.c_str(),cat,systLabel_.c_str()))), *(w_->function(Form("sigmodel_%s_cat%d_c1%s",proc.c_str(),cat,systLabel_.c_str())))) ) ;
        else if (nGaussians[pair<int,string>(cat,proc)] ==2) 
        {
           sigModel.reset(new RooAddPdf(name.c_str(),"model",RooArgList(*g1,*g2),
                RooArgList( *(w_->function(Form("sigmodel_%s_cat%d_c4%s",proc.c_str(),cat,systLabel_.c_str())))
                    ),
                kTRUE
                ));
        }
        else if (nGaussians[pair<int,string>(cat,proc)] ==3) 
           sigModel.reset(new RooAddPdf(name.c_str(),"model",RooArgList(*g1,*g2,*g3),
                RooArgList( *(w_->function(Form("sigmodel_%s_cat%d_c6%s",proc.c_str(),cat,systLabel_.c_str()))),
                    *(w_->function(Form("sigmodel_%s_cat%d_c7%s",proc.c_str(),cat,systLabel_.c_str())))
                    ),
                kTRUE
                ));

        w_ -> import( *sigModel ,RecycleConflictNodes());
        // -- Plot
        if (plot ) {
            TCanvas *c = new TCanvas();
            RooPlot *p = x_ -> frame();
            for(float mh=120;mh<130;mh+= 1)
            {
                cout <<"Interpolating at MH="<<mh<<endl;
                mh_ -> setVal(mh);
                w_ -> var("MH") ->setVal(mh);
                sigModel->plotOn(p);
            }
            p -> Draw();
            c -> SaveAs( Form("%s/interpolation_%s_cat%d%s.pdf",plotDir.c_str(),proc.c_str(),cat,systLabel_.c_str()) );
            c -> SaveAs( Form("%s/interpolation_%s_cat%d%s.png",plotDir.c_str(),proc.c_str(),cat,systLabel_.c_str()) );
            delete p;
            delete c;
        } // end plot
        
        //ea
        if (plot ) {
            TCanvas *c = new TCanvas();
            RooPlot *p = w_->var("MH") -> frame();
            w_ -> function( (name+"_norm").c_str()) ->plotOn(p);
            p -> Draw();
            c -> SaveAs( Form("%s/norm_%s_cat%d%s.pdf",plotDir.c_str(),proc.c_str(),cat,systLabel_.c_str()) );
            c -> SaveAs( Form("%s/norm_%s_cat%d%s.png",plotDir.c_str(),proc.c_str(),cat,systLabel_.c_str()) );
            delete p;
            delete c;
        }
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
