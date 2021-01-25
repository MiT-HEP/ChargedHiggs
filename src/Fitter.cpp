#include "interface/Fitter.hpp"

#ifdef HAVE_COMBINE

#include "HiggsAnalysis/CombinedLimit/interface/RooSpline1D.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooBernsteinFast.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooDoubleCBFast.h"
/*
              RooAbsReal& _x,
              RooAbsReal& _mean,
              RooAbsReal& _width,
              RooAbsReal& _alpha1,
              RooAbsReal& _n1,
              RooAbsReal& _alpha2,
              RooAbsReal& _n2
 */
#include "RooGenericPdf.h"
#include "RooExponential.h"
#include "interface/Handlers.hpp" // abortException
#include <cstdio>
#include <cstdlib>

using namespace RooFit;

Fitter::Fitter(string uniq){
    uniq_=uniq;
    w_ = NULL;
    inputMasks.push_back("ChargedHiggsTauNu/Vars/Mt_HplusToTauNu_M-%.0f_13TeV_amcatnlo");
    datasetMask_ = "hist_%s_cat%d_M%.0f";
    //xsecMask_ = "xsec_Mt_cat%d";
    xsecMask_ = "xsec"+uniq_+"_%s_cat%d";
    eaMask_ = "ea_sigmodel"+uniq_+"_%s_cat%d";
    normMask_ = "pdf_sigmodel"+uniq_+"_%s_cat%d_norm";
    modelMask_ = "pdf_sigmodel"+uniq_+"_%s_cat%d";
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
    cout<<"xName="<<xname <<endl;
    cout<<"Dataset M="<<datasetMask_ <<endl;
    if (doXsec) cout<<"XSEC M="<<xsecMask_ <<endl;
    cout<<"EA M="<<eaMask_ <<endl;
    cout<<"NORM M="<<normMask_ <<endl;
    cout<<"MODEL M="<<modelMask_ <<endl;
    for(auto &s : inputMasks) cout <<"mask = "<<s<<endl;
    cout<<"mIn = ";for(auto &m : mIn ) cout <<m<<","; cout<<endl;
    cout <<"procs = ";for(auto &p : processes) cout <<p<<",";cout<<endl;
    cout << " ---- SYST ---"<<endl;
    cout <<"scale: " ; for(const auto &p : scaleUnc ) cout << "("<<p.first.first <<","<<p.first.second <<") -> "<< p.second<< ", "; cout <<endl;
    cout <<"corr: "; for( const auto &p : scaleCorr) cout <<"("<<p.first.first <<","<<p.first.second <<") -> "<< "("<< p.second.first <<","<<p.second.second <<"), ";  cout<<endl;
    cout <<"smear: " ; for(const auto &p : smearUnc ) cout << "("<<p.first.first <<","<<p.first.second <<") -> "<< p.second<< ", "; cout <<endl;
    cout <<"corr: "; for( const auto &p : smearCorr) cout <<"("<<p.first.first <<","<<p.first.second <<") -> "<< "("<< p.second.first <<","<<p.second.second <<"), ";  cout<<endl;
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
    if(x_==NULL) x_ = new RooRealVar(xname.c_str(),xname.c_str(),xmin,xmax);
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
            if (proc == "bbH" and fabs(m-125)> 0.1) continue;//ttH125  bbH125
            if ( inputMasks[cat].find("HiggsZG_Zbb") != string::npos and fabs(m-125)> 0.1) continue;
            //if ((proc == "ttH" or proc=="WMinusH" or proc=="WPlusH" or proc=="ZH" or proc=="GluGluToHHTo2B2M_node_4") and fabs(m-125)> 0.1) continue;
            if (( proc=="GluGluToHHTo2B2M_node_4") and fabs(m-125)> 0.1) continue;

            string mass = Form(massMask_.c_str() ,m);
            TH1D *h = NULL;
            if (fInput==NULL and inname.find(",") == string::npos ){
                // sum over files. 
                // split by files
                vector<string> files;
                istringstream ss (inname);
                string token;
                while (std::getline(ss, token, ',')){
                    if (token == "" ) continue;
                    files.push_back(token); 
                }
                for (auto s : files){
                    fInput = TFile::Open(s.c_str() );
                    if (h== NULL)
                        h = (TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(),proc.c_str(), m) ) ->Clone();
                        if (h==NULL){Log(__FUNCTION__,"ERROR","No such histogram: mask="+inputMasks[cat]+ Form("mass=%.1f proc=%s",m,proc.c_str()) + " file="+s ); throw abortException(); }
                    else {
                        TH1D *hTmp =  (TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(),proc.c_str(), m) ) ;
                        if (hTmp==NULL){Log(__FUNCTION__,"ERROR","No such histogram: mask="+inputMasks[cat]+ Form("mass=%.1f proc=%s",m,proc.c_str()) + " file="+s ); throw abortException(); }
                        h->Add(hTmp);
                    }
                    fInput ->Close();
                } 
            }
            else{ // default
                h = (TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(),proc.c_str(), m) ) ;
            }

            if (h==NULL and proc=="WH") // sum W+ and w- for wh
            {
                h = (TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(),"WPlusH", m) ) ;
                TH1D * hTmp=(TH1D*)fInput ->Get( Form(inputMasks[cat].c_str(),"WMinusH", m) ) ;
                h->Add(hTmp);
            }

            if (h == NULL) 
            {
                Log(__FUNCTION__,"ERROR","No such histogram: mask="+inputMasks[cat]+ Form("mass=%.1f proc=%s",m,proc.c_str()));
                nGaussians[pair<int,string>(cat,proc)] = 1;
                initPars_[Form("c0_%s_cat%d_%.0f",proc.c_str(),cat,m)] = m;
                initPars_[Form("c1_%s_cat%d_%.0f",proc.c_str(),cat,m)] = 2.5;
                ea_y . push_back( 1.e-9 );
                xSec_x . push_back( m );
                xSec_y . push_back( 1 );
                continue;
            }

            // -- Construct RooDataHist
            string name =  Form("%s_cat_%d_mass_%s",proc.c_str(),cat,mass.c_str());
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

            if (xname == "mt"){ // mt is much looser 
                initPars_[Form("c0_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetBinCenter(h->GetMaximumBin());
                initPars_[Form("c1_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetRMS()*.5;

                initPars_[Form("c2_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetMean()*.7;
                initPars_[Form("c3_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetRMS()*2;

                initPars_[Form("c4_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetMean()*.5;
                initPars_[Form("c5_%s_cat%d_%.0f",proc.c_str(),cat,m)] = h->GetRMS()*4;

                initPars_[Form("c6_%s_cat%d_%.0f",proc.c_str(),cat,m)] = .6;
                initPars_[Form("c7_%s_cat%d_%.0f",proc.c_str(),cat,m)] = .2;

                if (nGaussians[pair<int,string>(cat,proc)] ==2)
                {
                    initPars_[Form("c4_%s_cat%d_%.0f",proc.c_str(),cat,m)] = .9;
                }
            }
            else{
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
            }
            if (useDCB){
                initPars_[Form("c8_%s_cat%d_%.0f",proc.c_str(),cat,m)] = 1.;
                initPars_[Form("c9_%s_cat%d_%.0f",proc.c_str(),cat,m)] = 1.;
                initPars_[Form("c10_%s_cat%d_%.0f",proc.c_str(),cat,m)] = 1.;
                initPars_[Form("c11_%s_cat%d_%.0f",proc.c_str(),cat,m)] = 1.;
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
            ea_y . push_back( h->Integral(bin0,bin1)/xsec );

        } // end mass loop

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

        //if (proc == "ttH")//ttH125  bbH125
        //if (false)//ttH as all
        //if (proc == "ttH" or proc=="WMinusH" or proc=="WPlusH" or proc=="ZH" or proc=="GluGluToHHTo2B2M_node_4")
        if (proc=="GluGluToHHTo2B2M_node_4" or proc=="bbH" or 
             (inputMasks[cat].find("HiggsZG_Zbb") != string::npos)
                )
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

        //normFormula+="* (1.";
        //normFormula+=")";

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
    pars[4].setRange(0,1000);
    pars[5].setRange(5,1000);
    pars[6].setRange(0,1);
    pars[7].setRange(0,1);

    pars[8].setRange(.5,3); //  alpha
    pars[9].setRange(0.01,5); // n -> powerlaw
    pars[10].setRange(.5,3); // alpha2
    pars[11].setRange(0.01,5);

    if (xname == "mvv" or xname =="mt") // DoublyChargedHiggs
    {
        pars[0].setRange(0,10000);
        pars[1].setRange(10,1000);
        pars[2].setRange(0,10000);
        pars[3].setRange(5,1000);
        pars[4].setRange(0,10000);
        pars[5].setRange(10,1000);
    }
    //G+G+E
    RooGaussian g1("g1","g1",*x_,pars[0],pars[1]);
    RooGaussian g2("g2","g2",*x_,pars[2],pars[3]);
    RooGaussian g3("g3","g3",*x_,pars[4],pars[5]);

    RooDoubleCBFast cb1("cb1","cb1",*x_,pars[0],pars[1],pars[8],pars[9],pars[10],pars[11]);

    //RooGenericPdf  fitModel("model","TMath::Exp(-TMath::Power((@0-@1)/@2,2))*(1+@3*@0+@4*@0*@0)",RooArgList(*x_,mean,sigma,c1,c2));
    //RooGenericPdf  fitModel("model","TMath::Exp(-TMath::Power((@0-@1)/@2,2))+@4*TMath::Exp(-@0*@3)",RooArgList(*x_,mean,sigma,c1,c2,c3));
    // RooAddPdf fitModel("model","model",RooArgList(g1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ;
    std::unique_ptr<RooAbsPdf>fitModel;
    
    if (useDCB){ // is this used?
        fitModel.reset( new RooAddPdf("model","model",RooArgList(cb1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ) ; 
    }
    else{
        fitModel.reset( new RooAddPdf("model","model",RooArgList(g1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ) ; 
    }


    // -- Fit Model to MC
    cout<<"Fit Model to MC"<<endl;
    for(int cat=0; cat < int(inputMasks.size()) ;++cat)
    for(const string& proc:processes)
    {
        cout<<"-> Doing cat"<<cat<<" proc"<<proc<< " nG="<<nGaussians[pair<int,string>(cat,proc)]<<endl;

        if (useDCB){
            if (nGaussians[pair<int,string>(cat,proc)]==1)
            {
                fitModel.reset( new RooDoubleCBFast("cb1","cb1",*x_,pars[0],pars[1],pars[8],pars[9],pars[10],pars[11]) );
            }
            else if (nGaussians[pair<int,string>(cat,proc)]==2){
                fitModel.reset( new RooAddPdf("model","model",RooArgList(cb1,g2),RooArgList(pars[4]), kTRUE ) ) ; 
            }
            else if (nGaussians[pair<int,string>(cat,proc)]==3){
                fitModel.reset( new RooAddPdf("model","model",RooArgList(cb1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ) ; 
            }

        }else{
            if (nGaussians[pair<int,string>(cat,proc)]==1){
                fitModel.reset( new RooGaussian("g1","g1",*x_,pars[0],pars[1]) ) ; }
            else if (nGaussians[pair<int,string>(cat,proc)]==2){
                fitModel.reset( new RooAddPdf("model","model",RooArgList(g1,g2),RooArgList(pars[4]), kTRUE ) ) ; }
            else if (nGaussians[pair<int,string>(cat,proc)]==3){
                fitModel.reset( new RooAddPdf("model","model",RooArgList(g1,g2,g3),RooArgList(pars[6],pars[7]), kTRUE ) ) ; }
        }


        for(int i=0;i<pars.size() ;++i) pars[i].setConstant(kFALSE);

        vector<float> mInPlusOne{mIn};
        //mInPlusOne. push_back(mIn[0]);
        mInPlusOne. push_back(mIn[mIn.size()/2]);

        // reset last mass for ttH
        lastMass=-1;
        for( auto & m: mInPlusOne )
        {
            // set par ranges mass dependent
            int pos=0;
            for(int ig =0 ;ig< nGaussians[pair<int,string>(cat,proc)] ;ig++)
            {
                //mean and sigma
                pars[pos+0].setRange(m-5*(ig+1),m+5*(ig+1));
                pars[pos+1].setRange(0.5*(ig+1),10*(ig+1));

                if (proc=="DoublyChargedHiggs" or (proc=="SinglyChargedHiggs" and xname == "mvv"))
                {
                    pars[pos+0].setRange(m-50*(ig+1),m+50*(ig+1));
                    pars[pos+1].setRange(50*(ig+1),500*(ig+1));
                    if (ig==2){ // third gaussian
                        pars[pos+0].setRange(m-500,m+500);
                    }
                }

                if (proc=="SinglyChargedHiggs" and xname == "mt")
                {
                    pars[pos+0].setRange(m-100*(ig+1),m+100*(ig+1));
                    pars[pos+1].setRange(100*(ig+1),800*(ig+1));

                    if (ig==2){ // third gaussian
                        pars[pos+0].setRange(m-500,m+500);
                    }
                }
                pos+=2;
            }
            // fractions (last is the closing)
            for(int ig =0 ;ig< nGaussians[pair<int,string>(cat,proc)] -1 ;ig++)
            {
                pars[pos].setRange(0,1);
                pos+=1;
            }

            // return if ttH !=125
            if (proc == "bbH" and fabs(m-125)> 0.1) continue;//ttH125  bbH125
            //if ((proc == "ttH" or proc=="WMinusH" or proc=="WPlusH" or proc=="ZH" or proc=="GluGluToHHTo2B2M_node_4") and fabs(m-125)> 0.1) continue;
            if (( proc=="GluGluToHHTo2B2M_node_4") and fabs(m-125)> 0.1) continue;
            if ( inputMasks[cat].find("HiggsZG_Zbb") != string::npos and fabs(m-125)> 0.1) continue;
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
            string name =  Form("%s_cat_%d_mass_%s",proc.c_str(), cat,mass.c_str());  

            cout <<" *-> name is "<<name<<endl; // DEBUG
            bool foundHist=true;
            if (hist_.find(name) == hist_.end() ) { foundHist=false;cout <<"Unable to find this hist: check names"<<endl;}

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
            if (foundHist){
                fitModel->fitTo( *hist_[ name ] ,SumW2Error(kTRUE), PrintEvalErrors(errlevel),PrintLevel(printlevel),Warnings(warnlevel) );
                fitModel->fitTo( *hist_[ name ] ,SumW2Error(kTRUE), PrintEvalErrors(errlevel),PrintLevel(printlevel),Warnings(warnlevel) );
            }

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
                TCanvas *c = new TCanvas(Form("c_%s_%s_cat%d",mass.c_str(),proc.c_str(),cat),"c");
                RooPlot *p = x_ -> frame();
                if (foundHist) hist_[ name ] -> plotOn(p,DataError(RooAbsData::SumW2));
                fitModel -> plotOn(p);

                if (nGaussians[pair<int,string>(cat,proc)] >=2)
                {
                    if (useDCB)
                        fitModel->plotOn(p,Components(cb1),LineStyle(kDashed),LineColor(kRed+2)); 
                    else
                        fitModel->plotOn(p,Components(g1),LineStyle(kDashed),LineColor(kRed+2)); 

                    fitModel->plotOn(p,Components(g2),LineStyle(kDashed),LineColor(kGreen+2)); 
                }
                if (nGaussians[pair<int,string>(cat,proc)] >=3)
                {
                    fitModel->plotOn(p,Components(g3),LineStyle(kDashed),LineColor(kMagenta)); 
                }



                //c->SetLogy();
                p -> Draw();
                c -> Update() ;
               
                string dirName= plotDir + "/" + proc + "/" + mass; 
                system(Form("mkdir -p %s",dirName.c_str()));
                c -> SaveAs( Form("%s/fit_mh_%s_%s_cat%d.pdf",dirName.c_str(),mass.c_str(),proc.c_str(),cat) );
                c -> SaveAs( Form("%s/fit_mh_%s_%s_cat%d.png",dirName.c_str(),mass.c_str(),proc.c_str(),cat) );

                //
                delete p;
                delete c;
            } // end plots
            // -- Save coefficients vs mh
            //
            //for(int i=0;i<pars.size();++i)
            //for(int i=0;i<3*nGaussians[pair<int,string>(cat,proc)]-1 + (useDCB)?4:0 ;++i)
            vector<int> params_to_interpolate;
            for(int i=0;i<3*nGaussians[pair<int,string>(cat,proc)]-1;++i) params_to_interpolate.push_back(i);
            if (useDCB) for(int i=8;i<=11;++i) params_to_interpolate.push_back(i);
            for(const int& i : params_to_interpolate)
                fitParameters_[ Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,mass.c_str(),i) ] =  pars[i].getVal();

        } // end mass

        // -- Interpolate coefficients
        cout<<"Interpolate ---- "<<endl;
        vector<int> params_to_interpolate;
        for(int i=0;i<3*nGaussians[pair<int,string>(cat,proc)]-1;++i) params_to_interpolate.push_back(i);
        if (useDCB) for(int i=8;i<=11;++i) params_to_interpolate.push_back(i);
        //for(int i=0;i<3*nGaussians[pair<int,string>(cat,proc)]-1 ;++i)
        for(const int& i : params_to_interpolate)
        {
            vector<float> mpoints;
            vector<float> c;

            for( auto & m: mIn )
            {
                if (proc == "bbH" and fabs(m-125)> 0.1) continue;//ttH125 bbH125
                //if ((proc == "ttH" or proc=="WMinusH" or proc=="WPlusH" or proc=="ZH" or proc=="GluGluToHHTo2B2M_node_4") and fabs(m-125)> 0.1) continue;
                if (( proc=="GluGluToHHTo2B2M_node_4") and fabs(m-125)> 0.1) continue;
                //if (proc=="DoublyChargedHiggs" and fabs(m-1500)>0.1) continue;
                if ( inputMasks[cat].find("HiggsZG_Zbb") != string::npos and fabs(m-125)> 0.1) continue;

                cout <<" Considering proc='"<<proc<<"' and mass = "<< m<<endl;

                string mass=Form(massMask_.c_str(),m);
                mpoints.push_back(m);
                c.push_back( fitParameters_[Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,mass.c_str(),i)] ) ;
                cout <<" adding points to splin: "<<(m)<<" "<<fitParameters_[Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,mass.c_str(),i)]<<endl;
            }

            //interpolate model pars
            string splname=Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),proc.c_str(),cat,i);
            //#warning TTH125
            //if (proc == "ttH" or proc=="WMinusH" or proc=="WPlusH" or proc=="ZH" or proc=="GluGluToHHTo2B2M_node_4")//ttH125
            if (proc=="GluGluToHHTo2B2M_node_4" or proc=="bbH" or
                    ( inputMasks[cat].find("HiggsZG_Zbb") != string::npos )
                    )//ttH125
            //if (false)//ttH all
            {
                string massexp = "125"; //if (proc=="DoublyChargedHiggs") massexp="1500";

                cout <<"DEBUG: Creating RooRealVar: "<<Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,massexp.c_str(),i) <<endl;
                cout <<"DEBUG: par name="<<Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,massexp.c_str(),i)<<endl;
                cout <<"DEBUG: par value = "<<fitParameters_[Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,massexp.c_str(),i)]<<endl;
                if (w_->var("MH") == NULL ) w_->import(*mh_) ;

                cout <<"DEBUG: MH = "<< w_->var("MH")<<endl;

                RooRealVar par( Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,massexp.c_str(),i),
                                Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,massexp.c_str(),i),
                                fitParameters_[Form("fit_%s_cat%d_mass_%s_c%d",proc.c_str(),cat,massexp.c_str(),i)]
                        );
                par.setConstant();
                RooFormulaVar *c =new RooFormulaVar ( splname.c_str(),splname.c_str(),"@0", par);
                if (i==0) c  =new RooFormulaVar ( splname.c_str(),splname.c_str(), ("@0 +(@1-"+massexp+")").c_str(), RooArgList(par , *w_->var("MH"))); // just put a shift // for ngaussians mean
                if (i==2 and nGaussians[pair<int,string>(cat,proc)] >1) c  =new RooFormulaVar ( splname.c_str(),splname.c_str(), ("@0 +(@1-"+massexp+")").c_str(), RooArgList(par , *w_->var("MH"))); // just put a shift // for ngaussians mean
                if (i==4 and nGaussians[pair<int,string>(cat,proc)] >2) c  =new RooFormulaVar ( splname.c_str(),splname.c_str(), ("@0 +(@1-"+massexp+")").c_str(), RooArgList(par , *w_->var("MH"))); // just put a shift // for ngaussians mean
                cout <<"DEBUG importing in workspace:"<< splname <<endl;
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
        } // end interpolated
    }
    return;
}


void Fitter::end(){
    // -- Save final model 
    // final model uses splines to get the values of the multigaussians
    // it will also include nuisances variables if necessary
    //
    for( int cat=0;cat<int(inputMasks.size()) ;++cat)
    for(const string& proc:processes)
    {

        string name = Form(modelMask_.c_str(),proc.c_str(),cat);

        cout <<"->Trying to access vars:"<<Form("sigmodel%s_%s_cat%d_c0",uniq_.c_str(),proc.c_str(),cat)<<endl;

        w_->Print();

       /// PUT SYST HERE 

        // construct gaussians

        //RooGaussian *g1 = new RooGaussian(Form("g1_%s_cat%d",proc.c_str(),cat),"g1",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c0",proc.c_str(),cat))), *(w_->function(Form("sigmodel_%s_cat%d_c1",proc.c_str(),cat))));
        //RooGaussian *g1 = new RooGaussian(Form("g1%s_%s_cat%d",uniq_.c_str(),proc.c_str(),cat),"g1",*x_,  *getMeanWithSyst(cat,proc,0), *getSigmaWithSyst(cat,proc,0) );
        RooAbsPdf *g1{NULL},*g2{NULL},*g3{NULL};

        string repl_proc = proc;
        int repl_cat=cat;
        if ( replace.find ( pair<int,string>(cat,proc))  != replace.end()){
            repl_proc = replace[ pair<int,string>(cat,proc) ] .second;
            repl_cat = replace[ pair<int,string>(cat,proc) ] .first;
            cout<<" [Fitter]::[for coeff] found replacement for cat: "<<cat<<", proc"<<proc<<" with -> "<<repl_cat<<", "<<repl_proc<<endl;
        }
        string splname8=Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),repl_proc.c_str(),repl_cat,8);
        string splname9=Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),repl_proc.c_str(),repl_cat,9);
        string splname10=Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),repl_proc.c_str(),repl_cat,10);
        string splname11=Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),repl_proc.c_str(),repl_cat,11);
        
        if (useDCB){

            if (w_->function(splname8.c_str()) == NULL)
            {
                Log(__FUNCTION__,"ERROR","No such function in the ws:" + splname8);
                w_->Print();
            }
            if (w_->function(splname9.c_str()) == NULL)
            {
                Log(__FUNCTION__,"ERROR","No such function in the ws:" + splname8);
                w_->Print();
            }
            if (w_->function(splname10.c_str()) == NULL)
            {
                Log(__FUNCTION__,"ERROR","No such function in the ws:" + splname8);
                w_->Print();
            }
            if (w_->function(splname11.c_str()) == NULL)
            {
                Log(__FUNCTION__,"ERROR","No such function in the ws:" + splname8);
                w_->Print();
            }

            //8 9 10 11
            g1 = new RooDoubleCBFast(Form("cb1%s_%s_cat%d",uniq_.c_str(),proc.c_str(),cat),"cb1",*x_,  *getMeanWithSyst(cat,proc,0), *getSigmaWithSyst(cat,proc,0) , *(w_->function(splname8.c_str())) , *(w_->function(splname9.c_str())),*(w_->function(splname10.c_str())),*(w_->function(splname11.c_str())) );

        } else{
            g1 = new RooGaussian(Form("g1%s_%s_cat%d",uniq_.c_str(),proc.c_str(),cat),"g1",*x_,  *getMeanWithSyst(cat,proc,0), *getSigmaWithSyst(cat,proc,0) );
        }

        //if (nGaussians[pair<int,string>(cat,proc)] >= 2 )g2=new RooGaussian(Form("g2_%s_cat%d",proc.c_str(),cat),"g2",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c2",proc.c_str(),cat))), *(w_->function(Form("sigmodel_%s_cat%d_c3",proc.c_str(),cat))));
        //if (nGaussians[pair<int,string>(cat,proc)] >= 3 )g3= new RooGaussian(Form("g3_%s_cat%d",proc.c_str(),cat),"g3",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c4",proc.c_str(),cat))), *(w_->function(Form("sigmodel_%s_cat%d_c5",proc.c_str(),cat))));
        if (nGaussians[pair<int,string>(cat,proc)] >= 2 )g2=new RooGaussian(Form("g2%s_%s_cat%d",uniq_.c_str(),proc.c_str(),cat),"g2",*x_, *getMeanWithSyst(cat,proc,1), *getSigmaWithSyst(cat,proc,1));
        if (nGaussians[pair<int,string>(cat,proc)] >= 3 )g3= new RooGaussian(Form("g3%s_%s_cat%d",uniq_.c_str(),proc.c_str(),cat),"g3",*x_,  *getMeanWithSyst(cat,proc,2), *getSigmaWithSyst(cat,proc,2));

        std::unique_ptr<RooAbsPdf> sigModel;
       
        if (nGaussians[pair<int,string>(cat,proc)] ==1) {
            if (useDCB){
                sigModel.reset( new RooDoubleCBFast(name.c_str(),"cb1",*x_, *getMeanWithSyst(cat,proc,0), *getSigmaWithSyst(cat,proc,0),
                        *(w_->function(splname8.c_str())) , *(w_->function(splname9.c_str())),*(w_->function(splname10.c_str())),*(w_->function(splname11.c_str()))
                        ));
            }
            else{
                sigModel.reset( new RooGaussian(name.c_str(),"g1",*x_,  *getMeanWithSyst(cat,proc,0), *getSigmaWithSyst(cat,proc,0) )
                    );
            }
            //sigModel.reset(new RooGaussian(name.c_str(),"g1",*x_,  * (w_->function(Form("sigmodel_%s_cat%d_c0",proc.c_str(),cat))), *(w_->function(Form("sigmodel_%s_cat%d_c1",proc.c_str(),cat)))) ) ;
        }
        else if (nGaussians[pair<int,string>(cat,proc)] ==2) 
        {
           sigModel.reset(new RooAddPdf(name.c_str(),"model",RooArgList(*g1,*g2),
                RooArgList( *(w_->function(Form("sigmodel%s_%s_cat%d_c4",uniq_.c_str(),repl_proc.c_str(),repl_cat)))
                    ),
                kTRUE
                ));
        }
        else if (nGaussians[pair<int,string>(cat,proc)] ==3) 
           sigModel.reset(new RooAddPdf(name.c_str(),"model",RooArgList(*g1,*g2,*g3),
                RooArgList( *(w_->function(Form("sigmodel%s_%s_cat%d_c6",uniq_.c_str(),repl_proc.c_str(),repl_cat))),
                    *(w_->function(Form("sigmodel%s_%s_cat%d_c7",uniq_.c_str(),repl_proc.c_str(),repl_cat)))
                    ),
                kTRUE
                ));

        w_ -> import( *sigModel ,RecycleConflictNodes());
        // -- Plot
        if (plot ) {
            TCanvas *c = new TCanvas();
            RooPlot *p = x_ -> frame();
            float deltamh=1;
            if (xname == "mvv" or xname =="mt") deltamh = 100;

            for(float mh=mhmin;mh<mhmax; mh+= deltamh)
            {
                cout <<"Interpolating at MH="<<mh<<endl;
                mh_ -> setVal(mh);
                w_ -> var("MH") ->setVal(mh);
                sigModel->plotOn(p);
            }
            p -> Draw();
            c -> SaveAs( Form("%s/interpolation_%s_cat%d.pdf",plotDir.c_str(),proc.c_str(),cat) );
            c -> SaveAs( Form("%s/interpolation_%s_cat%d.png",plotDir.c_str(),proc.c_str(),cat) );
            delete p;
            delete c;
        } // end plot
        
        //ea
        if (plot ) {
            TCanvas *c = new TCanvas();
            RooPlot *p = w_->var("MH") -> frame();
            w_ -> function( (name+"_norm").c_str()) ->plotOn(p);
            p -> Draw();
            c -> SaveAs( Form("%s/norm_%s_cat%d.pdf",plotDir.c_str(),proc.c_str(),cat) );
            c -> SaveAs( Form("%s/norm_%s_cat%d.png",plotDir.c_str(),proc.c_str(),cat) );
            delete p;
            delete c;
        }
    } //end cat
}


RooAbsReal* Fitter::getMeanWithSyst(int cat, string proc,int gaus=0){
        // Add Systematics here, because the parameter is a RooSpline, and I need to have a RooFormulaVar -> m = m0 + scale_catX_procX*scaleUnc = [0,-1,1][0.0011]
        // s = s0 + smear_catX_procX * smearUnc
        // or w->var() if correlated

        // look up for correlations
        int mycat = cat ;//  for correlation 
        string myproc = proc;
        if (scaleCorr.find(pair<int,string>(cat,proc))!= scaleCorr.end() )
        {
            mycat = scaleCorr[ pair<int,string>(cat,proc) ] .first; 
            myproc = scaleCorr[ pair<int,string>(cat,proc) ] .second;
        }

        RooRealVar *scaleNuisance =  new RooRealVar(Form("scale_cat%d_proc%s",mycat,myproc.c_str()),Form("scale_cat%d_proc%s",mycat,myproc.c_str()),0,-1,1);
        RooRealVar *scaleValue =  new RooRealVar(Form("scale_value_cat%d_proc%s",mycat,myproc.c_str()),Form("scale_value_cat%d_proc%s",mycat,myproc.c_str()),0.);

        RooRealVar *smearNuisance =  new RooRealVar(Form("smear_cat%d_proc%s",mycat,myproc.c_str()),Form("smear_cat%d_proc%s",mycat,myproc.c_str()),0,-1,1);
        RooRealVar *smearValue =  new RooRealVar(Form("smear_value_cat%d_proc%s",mycat,myproc.c_str()),Form("smear_value_cat%d_proc%s",mycat,myproc.c_str()),0.);

        string repl_proc = proc;
        int repl_cat=cat;

        if ( replace.find ( pair<int,string>(cat,proc))  != replace.end()){
            repl_proc = replace[ pair<int,string>(cat,proc) ] .second;
            repl_cat = replace[ pair<int,string>(cat,proc) ] .first;
            cout<<" [Fitter]::[getMeanWithSyst] found replacement for cat: "<<cat<<", proc"<<proc<<" with -> "<<repl_cat<<", "<<repl_proc<<endl;
        }

        string scaleFormula = "@0";
        RooArgList scaleList( *(w_->function(Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),repl_proc.c_str(),repl_cat,0+gaus*2))) );

        RooFormulaVar *deltaM=NULL;
        if (gaus >0 )
            deltaM=new RooFormulaVar(Form("deltaM_%s_%s_cat%d_g%d",uniq_.c_str(),proc.c_str(),cat,gaus),"deltaM","@1-@0",RooArgList( *w_->function(Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),proc.c_str(),cat,0) ), * w_->function(Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),proc.c_str(),cat,0+gaus*2)) ));

        if (scaleUnc.find( pair<int,string>(mycat,myproc) )   != scaleUnc.end() ) 
        {
            scaleValue->setVal( scaleUnc[pair<int,string>(mycat,myproc)]) ;
            scaleFormula += "*(1.+ (@1*@2)) ";
            scaleList.add( *scaleNuisance);
            scaleList.add( *scaleValue);
            if (deltaM != NULL) // the scaling of sigma on deltaM not on the mean
            {
                scaleFormula += "+ (@3*@4*@5) ";
                scaleList.add( *smearNuisance);
                scaleList.add( *smearValue);
                scaleList.add( *deltaM);
            }
        }

        scaleValue->setConstant();
        scaleNuisance->setConstant();

        string name=Form("sigmodel%s_%s_cat%d_g%d_mean",uniq_.c_str(),proc.c_str(),cat,gaus);
        RooFormulaVar *mean = new RooFormulaVar(name.c_str(),name.c_str(),scaleFormula.c_str(),scaleList);
        w_->import(*mean,RecycleConflictNodes());
        return mean;
}

RooAbsReal* Fitter::getSigmaWithSyst(int cat, string proc,int gaus=0){
        // Add Systematics here, because the parameter is a RooSpline, and I need to have a RooFormulaVar -> m = m0 + smear_catX_procX*smearUnc = [0,-1,1][0.0011]
        // s = s0 + smear_catX_procX * smearUnc
        // or w->var() if correlated
        int mycat = cat ;//  for correlation 
        string myproc = proc;
        if (smearCorr.find(pair<int,string>(cat,proc))!= smearCorr.end() )
        {
            mycat = smearCorr[ pair<int,string>(cat,proc) ] .first; 
            myproc = smearCorr[ pair<int,string>(cat,proc) ] .second;
        }

        RooRealVar *smearNuisance =  new RooRealVar(Form("smear_cat%d_proc%s",mycat,myproc.c_str()),Form("smear_cat%d_proc%s",mycat,myproc.c_str()),0,-1,1);
        RooRealVar *smearValue =  new RooRealVar(Form("smear_value_cat%d_proc%s",mycat,myproc.c_str()),Form("smear_value_cat%d_proc%s",mycat,myproc.c_str()),0.);

        string repl_proc = proc;
        int repl_cat=cat;
        if ( replace.find ( pair<int,string>(cat,proc))  != replace.end()){
            repl_proc = replace[ pair<int,string>(cat,proc) ] .second;
            repl_cat = replace[ pair<int,string>(cat,proc) ] .first;
            cout<<" [Fitter]::[getSigmaWithSyst] found replacement for cat: "<<cat<<", proc"<<proc<<" with -> "<<repl_cat<<", "<<repl_proc<<endl;
        }

        string smearFormula = "@0";
        RooArgList smearList( *(w_->function(Form("sigmodel%s_%s_cat%d_c%d",uniq_.c_str(),repl_proc.c_str(),repl_cat,1+gaus*2))) );

        if (smearUnc.find( pair<int,string>(mycat,myproc) )   != smearUnc.end() ) 
        {
            smearValue->setVal( smearUnc[pair<int,string>(mycat,myproc)]) ;
            smearFormula += "* (1. + (@1*@2) )";
            smearList.add( *smearNuisance);
            smearList.add( *smearValue);
        }
        smearValue->setConstant();
        smearNuisance->setConstant();

        string name=Form("sigmodel%s_%s_cat%d_g%d_sigma",uniq_.c_str(),proc.c_str(),cat,gaus);
        RooFormulaVar *sigma = new RooFormulaVar(name.c_str(),name.c_str(),smearFormula.c_str(),smearList);
        w_->import(*sigma,RecycleConflictNodes());
        return sigma;
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
