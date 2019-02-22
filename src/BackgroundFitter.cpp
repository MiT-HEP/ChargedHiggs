#include "interface/BackgroundFitter.hpp"

#ifdef HAVE_COMBINE

#include "RooGenericPdf.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooHistPdf.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooSpline1D.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooBernsteinFast.h"
#include "HiggsAnalysis/CombinedLimit/interface/RooMultiPdf.h"
#include "interface/Logger.hpp"
#include <cstdio>
#include <cstdlib>

#include "HiggsAnalysis/CombinedLimit/interface/HGGRooPdfs.h"

#include "TArrow.h"

#include <iostream>
#include <fstream>

using namespace RooFit;


double PdfModelBuilder::getGoodnessOfFit(RooRealVar *mass, RooAbsPdf *mpdf, RooDataHist *data, std::string name,bool blind){

    double prob;
    int ntoys = 500;

    // Routine to calculate the goodness of fit. 
    name+="_gofTest.pdf";
    RooRealVar norm("norm","norm",data->sumEntries(),0,10E6);
    //norm.removeRange();

    RooExtendPdf *pdf = new RooExtendPdf("ext","ext",*mpdf,norm);

    // get The Chi2 value from the data
    RooPlot *plot_chi2 = mass->frame();
    //if (blind)
    //{
    //    data->plotOn(plot_chi2,Name("data"), CutRange("unblindReg_1"));
    //    data->plotOn(plot_chi2,Name("data"), CutRange("unblindReg_2"));
    //    data->plotOn(plot_chi2,Name("data"),Invisible());
    //}
    //else
    float xmin=obs_var->getMin();
    float xmax=obs_var->getMax();
    data->plotOn(plot_chi2,Name("data"),RooFit::Binning(160,xmin,xmax));

    pdf->plotOn(plot_chi2,Name("pdf"));
    int np = pdf->getParameters(*data)->getSize();

    double chi2 = plot_chi2->chiSquare("pdf","data",np);
    std::cout << "Calculating GOF for pdf " << pdf->GetName() << ", using " <<np << " fitted parameters" <<std::endl;

    // The first thing is to check if the number of entries in any bin is < 5 
    // if so, we don't rely on asymptotic approximations
    int nBinsForMass = data->numEntries(); // nentries = nbins

    if ((double)data->sumEntries()/nBinsForMass < 5  ){ // not for wo

        std::cout << "Running toys for GOF test " << std::endl;
        // store pre-fit params 
        RooArgSet *params = pdf->getParameters(*data);
        RooArgSet preParams;
        params->snapshot(preParams);
        int ndata = data->sumEntries();

        if (RandomGen == NULL ) RandomGen = new TRandom3();

        int npass =0;
        std::vector<double> toy_chi2;
        for (int itoy = 0 ; itoy < ntoys ; itoy++){
            std::cout << Form("\t.. %.1f %% complete\r",100*float(itoy)/ntoys) << std::flush;
            params->assignValueOnly(preParams);
            int nToyEvents = RandomGen->Poisson(ndata);
            RooDataHist *binnedtoy = pdf->generateBinned(RooArgSet(*mass),nToyEvents,0,1);
            pdf->fitTo(*binnedtoy,RooFit::Minimizer("Minuit2","minimize"),RooFit::Minos(0),RooFit::Hesse(0),RooFit::PrintLevel(-1),RooFit::Strategy(0));

            RooPlot *plot_t = mass->frame();
            binnedtoy->plotOn(plot_t);
            pdf->plotOn(plot_t);//,RooFit::NormRange("fitdata_1,fitdata_2"));

            double chi2_t = plot_t->chiSquare(np);
            if( chi2_t>=chi2) npass++;
            toy_chi2.push_back(chi2_t*(nBinsForMass-np));
            delete plot_t;
        }
        std::cout << "complete" << std::endl;
        prob = (double)npass / ntoys;

        TCanvas *can = new TCanvas();
        double medianChi2 = toy_chi2[(int)(((float)ntoys)/2)];
        double rms = TMath::Sqrt(medianChi2);

        TH1F toyhist(Form("gofTest_%s.pdf",pdf->GetName()),";Chi2;",50,medianChi2-5*rms,medianChi2+5*rms);
        for (std::vector<double>::iterator itx = toy_chi2.begin();itx!=toy_chi2.end();itx++){
            toyhist.Fill((*itx));
        }
        toyhist.Draw();

        TArrow lData(chi2*(nBinsForMass-np),toyhist.GetMaximum(),chi2*(nBinsForMass-np),0);
        lData.SetLineWidth(2);
        lData.Draw();
        can->SaveAs(name.c_str());

        // back to best fit     
        params->assignValueOnly(preParams);
    } else {
        prob = TMath::Prob(chi2*(nBinsForMass-np),nBinsForMass-np);
    }
    std::cout << "Chi2 in Observed =  " << chi2*(nBinsForMass-np) << std::endl;
    std::cout << "p-value  =  " << prob << std::endl;
    delete pdf;
    //return prob;
    return chi2;
}

RooAbsPdf* PdfModelBuilder::getZModExp(string prefix, int order){
    if ( order != 1 and order !=2 and order !=3) return NULL;

    RooArgList *plist = new RooArgList();
    plist->add(*obs_var); //@0
    string pname;

        pname = prefix + "_a";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),1.,-5.0, 5.0 );
        plist->add(*params[pname]); // @1

        pname = prefix + "_b";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),.1,-2, 2 );
        plist->add(*params[pname]); // @2

        pname = prefix + "_c";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),-.1,-2, 2.0 );
        plist->add(*params[pname]); // @3

        //plist->add(*prods["obs_range"]); // @4

        //pname = prefix + "_d";
        //params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),0.0,-.1,.1 );
        //plist->add(*params[pname]); // @5
        //
        bool firstFound=false;
        for( int cat=12 ; cat>=0 ;--cat)
        {
            bool found =false;
            for( const string&suf : {"_a","_b","_c"} )
            {
                string pname = prefix+suf;
                string old = pname;
                size_t pos=pname.find("cat");
                string thisCat="cat";
                thisCat += Form("%c",pname[pos+3]);
                if ( pname[pos+4] >='0' and pname[pos+4]<='9') thisCat += Form("%c",pname[pos+4]);
                replace(old,thisCat ,Form("cat%d",cat));
                if (params.find(old) != params.end())
                {
                    cout<<"TAKING PARAMS FROM "<<old<<" TO "<<pname<<endl;
                    params[pname] -> setVal( params[old]->getVal());
                    found=true;
                }
            }
            if (found and not firstFound) { firstFound=true; found=false;}
            if (found ) break;
        }

        if (order >1)
        {
            pname = prefix + "_Zwidth";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),2.5,0,30);
            plist->add(*params[pname]); //@5
        }

        if (order>2)
        {
            pname = prefix + "_mZ";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),91.2,90,92);
            plist->add(*params[pname]); //@6
        }


    // the envelope does getVariables->size to compute the penalization term
    RooAbsPdf *zmod=NULL;
    //if (order ==3 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),"TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-@6),@1)+TMath::Power(@5/2.,@1))",*plist); //FIXME
    //if (order ==2 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),"TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(@5/2.,@1))",*plist);//FIXME
    if (order ==1 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),"TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) ",*plist);

    return zmod;
}


RooAbsPdf* PdfModelBuilder::getZModExp2(string prefix, int order){
    if ( order >5) return NULL;

    RooArgList *plist = new RooArgList();
    plist->add(*obs_var); //@0
    string pname;

        pname = prefix + "_a";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),1.,-5.0, 5.0 );
        plist->add(*params[pname]); // @1

        pname = prefix + "_b";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),.1,-2, 2 );
        plist->add(*params[pname]); // @2

        pname = prefix + "_c";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),-.1,-2, 2.0 );
        plist->add(*params[pname]); // @3

        if (order >=2)
        {
            plist->add(*prods["obs_range"]); // @4

            pname = prefix + "_e";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),0.0,-0.05, 0.05 );
            plist->add(*params[pname]); // @5
        }

        if (order >=3)
        {
            pname = prefix + "_f";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),0.0,-0.05, 0.05 );
            plist->add(*params[pname]); // @6
        }
        if (order >=4)
        {
            pname = prefix + "_g";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),0.0,-0.05, 0.05 );
            plist->add(*params[pname]); // @7
        }

        if (order >=5)
        {
            pname = prefix + "_h";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),0.0,-0.05, 0.05 );
            plist->add(*params[pname]); // @8
        }
    
    for( const string&suf : {"_a","_b","_c"} )
    {
        string pname = prefix+suf;
        string old = pname;
        replace(old,"zmod2","zmod");
        replace(old,Form("ord%d",order),"ord1");
        if (params.find(old) != params.end()) params[pname] -> setVal( params[old]->getVal());
        else cout <<"WARNING UNABLE TO FIND REPLACEMENT FROM "<<old<<" TO "<<pname<<endl;
        if (order !=1) params[pname] -> setConstant();
    }
    //if (order ==1 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),"TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) * (1 + @5*TMath::Sin(TMath::Sqrt(@4+.2)*4.5*3.1415 ))",*plist);


    // the envelope does getVariables->size to compute the penalization term
    RooAbsPdf *zmod=NULL;
    if (order ==1 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),
            "TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) ",*plist);
    if (order ==2 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),
            "TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) * (1+ @5*(@4-.5))",*plist);
    if (order ==3 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),
            "TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) * ( 1+ @5*3*@4*@4 + @6*6*@4*(1-@4)+ (-@5-@6)*3*(1-@4)*(1-@4) )",*plist);
    if (order ==4 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),
            "TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) * ( 1+ @5*4*@4*@4*@4 + @6*@4*@4*(1-@4) + @7*@4*(1-@4)*(1-@4) + (-@5-@6-@7)*(1-@4)*(1-@4)*(1-@4) )",*plist);
    if (order ==5 )zmod = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),
            "TMath::Exp(@2*@0/100. +(@0/100.)*(@0/100.)*@3 )/(TMath::Power((@0-91.2),@1)+TMath::Power(2.5/2.,@1)) * (1+ @5*5*pow(@4,4)+20*@6*pow(@4,3)*(1-@4)+@7*30*@4*@4*(1-@4)*(1-@4) + @8 *20*@4*pow(1-@4,3)+(-@5-@6-@7-@8)*5*pow(1-@4,4))",*plist);

    return zmod;
}

RooAbsPdf* PdfModelBuilder::getBWZ(string prefix, int order){
    if (order != 1 and order !=2 and order !=3) return NULL;

    RooArgList *plist = new RooArgList();
    plist->add(*obs_var); //@0
    string pname;

        pname = prefix + "_a";
        //params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),1.,-5.0, 5.0 );
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),-1e-03,-1e-02,-1e-5 );
        plist->add(*params[pname]); // @1

        if (order>1)
        {
            pname = prefix + "_Zwidth";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),2.5,0,30.);
            plist->add(*params[pname]); //@2
        }

        if (order>2){
            pname = prefix + "_mZ";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),91.2,90.,92.);
            plist->add(*params[pname]); //@3
        }

    RooAbsPdf *bwz=NULL;
    //if (order==3) bwz = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),"TMath::Exp(@0*@1)/((@0-@3)*(@0-@3)+0.25*@2*@2)",*plist);
    //if (order==2) bwz = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),"TMath::Exp(@0*@1)/((@0-91.2)*(@0-91.2)+0.25*@2*@2)",*plist);
    if (order==1) bwz = new RooGenericPdf((prefix).c_str(),(prefix).c_str(),"TMath::Exp(@0*@1)/((@0-91.2)*(@0-91.2)+1.5625)",*plist);

    return bwz;
}

RooAbsPdf* PdfModelBuilder::getZPhotonRun1(string prefix, int order){ // BWZGamma
    if ( order !=1 and order !=2 and order !=3 ) return NULL;

    RooArgList *plist = new RooArgList();
    plist->add(*obs_var); //@0
    string pname;

        pname = prefix + "_a";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),-0.0053,-0.0073,-0.0033 );
        plist->add(*params[pname]); // @1

        if (order>1){
            pname = prefix + "_Zwidth";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),2.5,0,30);
            plist->add(*params[pname]); //@2
        }

        if (order>2){
            pname = prefix + "_mZ";
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),91.2,90,92);
            plist->add(*params[pname]); //@3
        }

    RooAbsPdf *part1=NULL;
    //if (order==3) part1 = new RooGenericPdf((prefix+"_pdf1").c_str(),(prefix+"_pdf1").c_str(),
    //        "TMath::Exp(@1*@0)/((@0-@3)*(@0-@3)+@2*@2/4.)",*plist);
    //if (order==2) part1 = new RooGenericPdf((prefix+"_pdf1").c_str(),(prefix+"_pdf1").c_str(),
    //        "TMath::Exp(@1*@0)/((@0-91.2)*(@0-91.2)+@2*@2/4.)",*plist);
    if (order==1) part1 = new RooGenericPdf((prefix+"_pdf1").c_str(),(prefix+"_pdf1").c_str(),
            "TMath::Exp(@1*@0)/((@0-91.2)*(@0-91.2)+2.5*2.5/4.)",*plist);

    if (part1==NULL) return NULL;

    pdfs[part1->GetName() ] = part1;

    plist = new RooArgList();
    plist->add(*obs_var); //@0

        pname = prefix + "_a";
        plist->add(*params[pname]); //@1

    RooGenericPdf *part2 = new RooGenericPdf((prefix+"_pdf2").c_str(),(prefix+"_pdf2").c_str(),"TMath::Exp(@1*@0)/(@0*@0)",*plist);
    pdfs[part2->GetName() ] = part2;

        pname = prefix + "_f";
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),0.379,0.2,1 );

    RooAddPdf *zpho = new RooAddPdf(prefix.c_str(),prefix.c_str(),RooArgList(*part1,*part2),RooArgList(*params[pname]),0 ) ;

    return zpho;
}


RooAbsPdf* PdfModelBuilder::getDYBernstein(string prefix, int order,TH1D*dy){
    //DY roohistpdf modified with a bernstein poly
    if (dy==NULL) cout<<"[PdfModelBuilder]::[getDYBernstein]::[ERROR] dy histogram is NULL"<<endl;
    RooDataHist *dh=new RooDataHist((prefix + "_dy_dh").c_str(),(prefix + "_dy_dh").c_str(),*obs_var,Import(*dy));
    hists[dh->GetName()] = dh;
    RooHistPdf *dypdf = new RooHistPdf((prefix +"_dy_pdf").c_str(),(prefix +"_dy_pdf").c_str(),*obs_var,*dh,2); // last number= interpolation
    if (order ==0 ) return dypdf;
    dypdf->Print("V");
    pdfs[dypdf->GetName() ] = dypdf;
    RooAbsPdf *bern= getBernstein(prefix+"_bern_",order);
    if (bern == NULL) return NULL;
    pdfs[bern->GetName() ] = bern;
    RooRealVar *f= new RooRealVar((prefix + "_f").c_str(),(prefix + "_f").c_str(),.5,0.8,1.0);// fraction
    RooAddPdf *dybern = new RooAddPdf(prefix.c_str(),prefix.c_str(),RooArgList(*dypdf,*bern),RooArgList(*f),0 ) ;
    return dybern;
} 
RooAbsPdf* PdfModelBuilder::getPolyTimesFewz(string prefix,int order,string fname="test/bias/FEWZ/h2mu-dsdm-13tev-xs-lux-1jet-nnlo-hp.dat")
{
    //RooAbsPdf* basePdf = getPdf(prefix + "_base", 0 );
    vector<float> mass;
    vector<float> xsec;
    ifstream dat (fname); 
    if ( dat.is_open() )
    {
        float m, y, e;
        while (!dat.eof() )
        {
           dat >> m;
           dat >> y; 
           dat >> e;
           mass.push_back(m);
           xsec.push_back(y);
           cout <<"[PdfModelBuilder]::[getPolyTimesFewz]::[DEBUG] inserting "<<m<<" "<<y<<endl;
        }
        dat.close();
    }

    float m0 = 110.-0.1;     

    if (mass[0] > m0)
    {
        float y0 = (xsec[1] - xsec[0]) / (mass[1]-mass[0])*(m0 - mass[0]) + xsec[0];

        cout <<"[PdfModelBuilder]::[getPolyTimesFewz]::[DEBUG] inserting (extra) "<<m0<<" "<<y0<<endl;
        mass.insert(mass.begin(), m0);
        xsec.insert(xsec.begin(),y0);
    }

    cout <<"[PdfModelBuilder]::[getPolyTimesFewz]::[DEBUG] constructing spline size="<<mass.size()<<endl;

    // TODO -1?
    RooSpline1D *spl=new RooSpline1D( (prefix +"_spl").c_str(),(prefix +"_spl").c_str(),*obs_var,mass.size()-1, &(mass[0]),&(xsec[0]) );

    //for(float m= 110; m<150 ;m+=0.5)
    //{
    //    obs_var->setVal(m);
    //    cout <<"[PdfModelBuilder]::[getPolyTimesFewz]::[DEBUG] spline eval m="<<m<<" "<<spl->getVal()<<endl;
    //    spl->Print("V");
    //}


    if (order ==0 ) 
    {
           cout <<"[PdfModelBuilder]::[getPolyTimesFewz]::[DEBUG] order 0"<<endl;
        RooGenericPdf *basePdf = new RooGenericPdf(prefix.c_str(),prefix.c_str(),"@0",RooArgList(*spl));
        return basePdf;
    }

    RooArgList *plist = new RooArgList();
    plist->add(*obs_var); //@0
    plist->add(*spl); //@1
    string formula = "@1*( 1.  ";
    float xmin=obs_var->getMin();
    float xmax=obs_var->getMax();
    string x0=Form("(@0-%f)/(%f-%f)",xmin,xmax,xmin); // 1+x because tot norm is always free!
    string x=x0; // 1+x because tot norm is always free!
    for(int i=0;i< order ;++i)
    {
            string pname = prefix + Form("_poly_order%d_p%d",order,i);
            params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),0,-5,5);
            plist->add(*params[pname]); //@2
            formula += Form(" + @%d",i+2);
            if (x!= "" )formula += "*("+x+") ";
            x+="*"+x0;
    }
    formula += ")";

    cout <<"[PdfModelBuilder]::[getPolyTimesFewz]::[DEBUG] order "<<order<<" : formula="<<formula<<endl;
    RooGenericPdf *func = new RooGenericPdf(prefix.c_str(),prefix.c_str(),formula.c_str(),*plist);
    return func;
}

RooAbsPdf* PdfModelBuilder::getLogPol(string prefix, int order){
    // poly in log-log scale
    if (order <=0 ) return NULL;
    RooArgList *plist = new RooArgList();
    string pname = prefix + "_p0";
    params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),2.28,-10, 10. );
    plist->add(*obs_var);
    plist->add(*params[pname]);
    string formula = "@1";
    formula+=" + TMath::Log( 1. "; 
    for (int i=1; i<=order; i++){
        float sign = -2; // even are doubled
        if (i%2==1) sign =-1; // odd are negative
        pname = prefix + Form("_p%d",i);
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),sign*1.,-15, 15. );
        plist->add(*params[pname]);
        formula += Form(" + @%d*TMath::Exp(%d * @0/110.)",(i+1),i);
    }
    formula+=")"; // log
    RooGenericPdf *logpol = new RooGenericPdf(prefix.c_str(),prefix.c_str(),formula.c_str(),*plist);
    logpol->Print("v");
    return logpol;
}

RooAbsPdf* PdfModelBuilder::getLaurentSeries(string prefix, int order){
 
  int nlower=int(ceil(order/2.));
  int nhigher=order-nlower;
  // first do 0th order
  RooArgList *pows = new RooArgList();
  RooArgList *plist = new RooArgList();
  string pname =  Form("%s_pow0",prefix.c_str());
  float central= -4 ; // central point
  pdfs.insert(pair<string,RooAbsPdf*>(pname, new RooPower(pname.c_str(),pname.c_str(),*obs_var,RooConst(central))));
  pows->add(*pdfs[pname]);

  // even terms
  for (int i=1; i<=nlower; i++){
    string name = Form("%s_l%d",prefix.c_str(),i);
    params.insert(pair<string,RooRealVar*>(name, new RooRealVar(name.c_str(),name.c_str(),0.25/order,0.000001,0.999999)));
    plist->add(*params[name]);
    string pname =  Form("%s_powl%d",prefix.c_str(),i);
    pdfs.insert(pair<string,RooAbsPdf*>(pname, new RooPower(pname.c_str(),pname.c_str(),*obs_var,RooConst(central-i))));
    pows->add(*pdfs[pname]);
  }
  // odd terms
  for (int i=1; i<=nhigher; i++){
    string name = Form("%s_h%d",prefix.c_str(),i);
    params.insert(pair<string,RooRealVar*>(name, new RooRealVar(name.c_str(),name.c_str(),0.25/order,0.000001,0.999999)));
    plist->add(*params[name]);
    string pname =  Form("%s_powh%d",prefix.c_str(),i);
    pdfs.insert(pair<string,RooAbsPdf*>(pname, new RooPower(pname.c_str(),pname.c_str(),*obs_var,RooConst(central+i))));
    pows->add(*pdfs[pname]);
  }
  RooAddPdf *pdf = new RooAddPdf(prefix.c_str(),prefix.c_str(),*pows,*plist,true);
  return pdf;
  //bkgPdfs.insert(pair<string,RooAbsPdf*>(pdf->GetName(),pdf));
}

RooAbsPdf* PdfModelBuilder::getExpPol(string prefix, int order){ // e^pol
    string formula="TMath::Exp(";

    RooArgList *plist = new RooArgList();
    plist->add(*obs_var);
    string pname;

    for (int i=0; i<=order; i++){
        pname = prefix + Form("_p%d",i);
        params[pname] = new RooRealVar(pname.c_str(),pname.c_str(),1.,-15, 15. );
        plist->add(*params[pname]);
        if (i>=1) formula += "+";
        if (i==0) formula += Form("@%d",(i+1));
        else if (i==1) formula += Form("@%d*@0/110.",(i+1));
        else formula += Form(" @%d * TMath::Power(@0/110.,%d)",(i+1),i);
    }
    formula+=")"; // exp
    RooGenericPdf *logpol = new RooGenericPdf(prefix.c_str(),prefix.c_str(),formula.c_str(),*plist);
    return logpol;
}

void PdfModelBuilder::runFit(RooAbsPdf *pdf, RooDataHist *data, double *NLL, int *stat_t, int MaxTries){

	int ntries=0;
  	RooArgSet *params_test = pdf->getParameters((const RooArgSet*)(0));
	//std::cout << " BEFORE ITERATIONS-------------------------------" << std::endl;
	//params_test->Print("v");
	int stat=1;
	double minnll=10e8;

    pdf->fitTo(*data,
                RooFit::Minimizer("Minuit2","minimize"),
                RooFit::SumW2Error(kTRUE) // for linear fit should be easier to estimate the parameters
                );

	while (stat!=0){
        if (ntries>=MaxTries) break;
        RooFitResult *fitTest = pdf->fitTo(*data,
                RooFit::Save(1),
                RooFit::Minimizer("Minuit2","minimize")
                );
        stat = fitTest->status();
        bool nan=false;
        for(TIterator *it=pdf->getVariables()->createIterator(); ;)
        {
            RooRealVar* var = (RooRealVar*) **it;
            if(var == NULL) break;
            if (std::isnan(var->getVal())) {
                nan=true;
                if (params.find(var->GetName()) != params.end() ) params[var->GetName()]->setVal(0.); //reset
                cout <<"[PdfModelBuilder]::[runFit]::[WARNING] found NAN value"<<endl;
            }
            it->Next();
        }
        
        if (nan) stat=10;
        minnll = fitTest->minNll();
        ntries++; 
        if (stat!=0 and ntries < MaxTries) params_test->assignValueOnly(fitTest->randomizePars());
    }
	*stat_t = stat;
	*NLL = minnll;

}


RooAbsPdf* PdfModelBuilder::getPdf(const string& prefix,int order )
{
    if (prefix.find("modbern") != string::npos)
        return getModBernstein(prefix+ Form("_ord%d",order),order);
    if (prefix.find("logpol") != string::npos)
        return getLogPol(prefix+ Form("_ord%d",order),order);
    if (prefix.find("bern") != string::npos)
        return getBernstein(prefix+ Form("_ord%d",order),order);
    if (prefix.find("zpho") != string::npos)
        return getZPhotonRun1(prefix+ Form("_ord%d",order),order);
    if (prefix.find("zmod2") != string::npos)
        return getZModExp2(prefix+ Form("_ord%d",order),order);
    else if (prefix.find("zmod") != string::npos)
        return getZModExp(prefix+ Form("_ord%d",order),order);
    if (prefix.find("bwz") != string::npos)
        return getBWZ(prefix+ Form("_ord%d",order),order);
    if (prefix.find("powlaw") != string::npos )
        return getPowerLawGeneric(prefix+ Form("_ord%d",order),order);
    if (prefix.find("exppol")!=string::npos)
        return getExpPol(prefix+ Form("_ord%d",order),order);
    if (prefix.find("exp")!=string::npos)
        return getExponentialSingle(prefix+ Form("_ord%d",order),order);
    if (prefix.find("lau") != string::npos)
        return getLaurentSeries(prefix+ Form("_ord%d",order),order);
    if (prefix.find("fewz_1j") != string::npos)
        return getPolyTimesFewz(prefix + Form("_ord%d",order),order,"aux/fewz/h2mu-dsdm-13tev-xs-lux-1jet-nnlo-hp.dat");
    if (prefix.find("fewz_2j") != string::npos)
        return getPolyTimesFewz(prefix + Form("_ord%d",order),order,"aux/fewz/h2mu-dsdm-13tev-xs-lux-2jet-nnlo-hp.dat");
    if (prefix.find("fewz_full") != string::npos)
        return getPolyTimesFewz(prefix + Form("_ord%d",order),order,"aux/fewz/h2mu-dsdm-13tev-xs-lux-full-nnlo-hp.dat");
    cout <<"PdfModelBulider::getPdf::[ERROR] unknow prefix"<<endl;
    return NULL;
}

double PdfModelBuilder::getProbabilityFtest(double chi2, int ndof){
 
  double prob_asym = TMath::Prob(chi2,ndof);
  return prob_asym;
}


RooAbsPdf* PdfModelBuilder::fTest(const string& prefix,RooDataHist*dh,int *ord,const string& plotDir,TH1D*h)
{ // fTest Based on 
    Logger::getInstance().Log("PdfModelBuilder",__FUNCTION__,"INFO","Starting fTest: " + prefix);
    const int maxOrder=15;
    const float alpha=0.05;
    double prevNll = -1;
    RooAbsPdf * prevPdf = NULL;
    int prevOrder =0;
    double prob =0.;
    map<string,RooAbsPdf*> store;
    for(int order =1 ;order<= maxOrder and prob<alpha;++order)
    {
        double thisNll;
        int fitStatus;

        RooAbsPdf* model = getPdf(prefix,order,h); 
        if (model==NULL) Logger::getInstance().Log("PdfModelBuilder",__FUNCTION__,"WARNING",Form("Unable to construct model for order %d and prefix '%s'",order, prefix.c_str()));
        if (model == NULL ) continue;
        runFit( model ,dh,&thisNll,&fitStatus,3);
        if (fitStatus!=0) std::cout << "Warning -- Fit status for " << model->GetName() << " at " << fitStatus <<std::endl;
        double chi2 = 2.*(prevNll-thisNll);
        if (chi2<0. && order>1) chi2=0.;
        if (prevPdf!=NULL){
            prob = getProbabilityFtest(chi2,order-prevOrder);
            std::cout << " F-test Prob(chi2>chi2(data) == " << prob << std::endl;
        } else {
            prob = 0;
        }
	    double gofProb=0;
        if (prob< alpha){
            prevNll=thisNll;
            prevOrder = order;
            //delete prevPdf; // destroed here
            prevPdf = model;
        }
        store[model->GetName()]=model;
    }
    if (plotDir != "")
    {
            system(Form("mkdir -p %s",plotDir.c_str()));
            int color[7] = {kBlue,kRed,kMagenta,kGreen+1,kOrange+7,kAzure+10,kBlack};
            int styles[] = {0,1,2};
            bool blind=true;
            TCanvas *c = new TCanvas();
            TLegend *leg = new TLegend(0.6,0.65,0.89,0.89);
            leg->SetFillColor(0);
            leg->SetLineColor(0);
            RooPlot *p = obs_var -> frame();
            if (blind){
                dh->plotOn(p,CutRange("unblindReg_1"));
                dh->plotOn(p,CutRange("unblindReg_2"));
                dh->plotOn(p,Invisible()); // normalization
                p->SetMinimum(0.1);
            }
            else dh->plotOn(p);
            int i=0, col,style;
            for (auto &s : store){
                if (i<=6) {col=color[i];i++;}
                else {col=kBlack; style++; i=0;}
                s.second->plotOn(p,LineColor(col));
                TObject *pdfLeg = p->getObject(int(p->numItems()-1));
                leg->AddEntry(pdfLeg,s.first.c_str(),"L");
            }
            p->Draw();
            leg->Draw("SAME");
            c->SaveAs((plotDir +"/"+ prefix + ".pdf"  ).c_str());
            c->SaveAs((plotDir +"/"+ prefix + ".png"  ).c_str());

    }

    cout <<" ---- Return pdf "<< prevPdf->GetName()<<endl;
    if (ord!=NULL) *ord=prevOrder;
    Logger::getInstance().Log("PdfModelBuilder",__FUNCTION__,"INFO",Form("fTest ---> %s: %d",prefix.c_str(),prevOrder));
    for (auto &p : store) if (p.first != prevPdf->GetName()) delete p.second; // delete all except this
    return prevPdf;
}

RooAbsPdf* PdfModelBuilder::getModBernstein(string prefix, int order){
    /*Return a Bernstein + exp*/

  RooRealVar *tau=new RooRealVar(Form("%s_t",prefix.c_str()),Form("%s_t",prefix.c_str()),-1.,-10,-.1);
  RooRealVar *f =new RooRealVar(Form("%s_f",prefix.c_str()),Form("%s_f",prefix.c_str()),.9,0.00001,.99999);
  params.insert(pair<string,RooRealVar*>(tau->GetName(),tau));
  params.insert(pair<string,RooRealVar*>(f->GetName(),f));
  //RooExponential *exp=new RooExponential(Form("%s_e",prefix.c_str()),"exp",*obs_var,*tau);
  //RooPower *exp= new RooPower(Form("%s_e",prefix.c_str()),"exp",*obs_var,RooConst(-1) ) ;
  RooPower *exp= new RooPower(Form("%s_e",prefix.c_str()),"exp",*obs_var,*tau ) ;
  pdfs.insert(pair<string,RooAbsPdf*>(exp->GetName(),exp));
  RooArgList *coeffList = new RooArgList();
  for (int i=0; i<order; i++){
    string name = Form("%s_p%d",prefix.c_str(),i);
    RooRealVar *param = new RooRealVar(name.c_str(),name.c_str(),0.1*(i+1),-5.,5.);
    RooFormulaVar *form = new RooFormulaVar(Form("%s_sq",name.c_str()),Form("%s_sq",name.c_str()),"@0*@0",RooArgList(*param));
    params.insert(pair<string,RooRealVar*>(name,param));
    prods.insert(pair<string,RooFormulaVar*>(name,form));
    coeffList->add(*prods[name]);
  }
  RooAbsPdf *bern=NULL;
  if (order==1) {
	bern = new RooBernsteinFast<1>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  } else if (order==2) {
	bern = new RooBernsteinFast<2>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  } else if (order==3) {
	bern = new RooBernsteinFast<3>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  } else if (order==4) {
	bern = new RooBernsteinFast<4>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  } else if (order==5) {
	bern = new RooBernsteinFast<5>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  } else if (order==6) {
	bern = new RooBernsteinFast<6>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  } else if (order==7) {
	bern = new RooBernsteinFast<7>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  } else {
    Logger::getInstance().Log("PdfModelBuilder",__FUNCTION__,"ERROR",Form("Unable to construct bern of order %d",order));
	return NULL;
  }
  pdfs.insert(pair<string,RooAbsPdf*>(bern->GetName(),bern));
  RooAddPdf *modBern = new RooAddPdf(prefix.c_str(),prefix.c_str(),RooArgList(*exp,*bern),RooArgList(*f),true);
  return modBern;
}

RooAbsPdf* PdfModelBuilder::getBernstein(string prefix, int order,bool positive){
  
  RooArgList *coeffList = new RooArgList();
  //coeffList->add(RooConst(1.0)); // no need for cnstant in this interface
  for (int i=0; i<order; i++){
    string name = Form("%s_p%d",prefix.c_str(),i);
    RooRealVar *param = new RooRealVar(name.c_str(),name.c_str(),0.1*(i+1),-5.,5.);
    //const bool freeze = true;
    //if (freeze){
    //    double min=obs_var->getMin();
    //    double max=obs_var->getMax();
    //    double peak= min + (max-min)*i/float(order-1);
    //    if ( peak>122. and peak <128.) {
    //        param->setVal(0.);
    //        param->setConstant();
    //    }
    //}
    RooFormulaVar *form=NULL;
    if (positive)
        form = new RooFormulaVar(Form("%s_sq",name.c_str()),Form("%s_sq",name.c_str()),"@0*@0",RooArgList(*param));
    else
        form = new RooFormulaVar(Form("%s_fv",name.c_str()),Form("%s_fv",name.c_str()),"@0",RooArgList(*param));
    params.insert(pair<string,RooRealVar*>(name,param));
    prods.insert(pair<string,RooFormulaVar*>(name,form));
    coeffList->add(*prods[name]);
  }

  if (order==1) {
	RooBernsteinFast<1> *bern = new RooBernsteinFast<1>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  	return bern;
  } else if (order==2) {
	RooBernsteinFast<2> *bern = new RooBernsteinFast<2>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  	return bern;
  } else if (order==3) {
	RooBernsteinFast<3> *bern = new RooBernsteinFast<3>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  	return bern;
  } else if (order==4) {
	RooBernsteinFast<4> *bern = new RooBernsteinFast<4>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  	return bern;
  } else if (order==5) {
	RooBernsteinFast<5> *bern = new RooBernsteinFast<5>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  	return bern;
  } else if (order==6) {
	RooBernsteinFast<6> *bern = new RooBernsteinFast<6>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  	return bern;
  } else if (order==7) {
	RooBernsteinFast<7> *bern = new RooBernsteinFast<7>(prefix.c_str(),prefix.c_str(),*obs_var,*coeffList);
  	return bern;
  } else {
    Logger::getInstance().Log("PdfModelBuilder",__FUNCTION__,"ERROR",Form("Unable to construct bern of order %d",order));
	return NULL;
  }
  //return bern;
  //bkgPdfs.insert(pair<string,RooAbsPdf*>(bern->GetName(),bern));

}

RooAbsPdf* PdfModelBuilder::getPowerLawGeneric(string prefix, int order){
  
  if (order%2==0){
    cerr << "ERROR -- addPowerLaw -- only odd number of params allowed" << endl;
    return NULL;
  }
  else {
    int nfracs=(order-1)/2;
    int npows=order-nfracs;
    assert(nfracs==npows-1);
    string formula="";
    RooArgList *dependents = new RooArgList();
    dependents->add(*obs_var);
    // first do recursive fraction
    if (order>1) {
      formula += "(1.-";
      for (int i=1; i<=nfracs; i++){
        if (i<nfracs) formula += Form("@%d-",i);
        else formula += Form("@%d)*",i);
        string name =  Form("%s_f%d",prefix.c_str(),i);
        params.insert(pair<string,RooRealVar*>(name, new RooRealVar(name.c_str(),name.c_str(),0.1,0.,1.)));
        dependents->add(*params[name]);
      }
    }
    for (int i=1; i<=npows; i++){
      string pname =  Form("%s_p%d",prefix.c_str(),i);
      string fname =  Form("%s_f%d",prefix.c_str(),i-1);
      params.insert(pair<string,RooRealVar*>(pname, new RooRealVar(pname.c_str(),pname.c_str(),TMath::Max(-10.,-2.*(i+1)),-10.,0.)));
      if (i==1) {
        formula += Form("TMath::Power(@0,@%d)",nfracs+i);
        dependents->add(*params[pname]);
      }
      else {
        formula += Form(" + @%d*TMath::Power(@0,@%d)",i-1,nfracs+i);
        dependents->add(*params[pname]);
      }
    }
    cout << "FORM -- " << formula << endl;
    dependents->Print("v");
    RooGenericPdf *pow = new RooGenericPdf(prefix.c_str(),prefix.c_str(),formula.c_str(),*dependents);
    pow->Print("v");
    return pow;
    //bkgPdfs.insert(pair<string,RooAbsPdf*>(pow->GetName(),pow));

  }
}

RooAbsPdf* PdfModelBuilder::getExponentialSingle(string prefix, int order){
  
  if (order%2==0){
    cerr << "ERROR -- addExponential -- only odd number of params allowed" << endl;
    return NULL;
  }
  else {
    int nfracs=(order-1)/2;
    int nexps=order-nfracs;
    assert(nfracs==nexps-1);
    RooArgList *fracs = new RooArgList();
    RooArgList *exps = new RooArgList();
    for (int i=1; i<=nfracs; i++){
      string name =  Form("%s_f%d",prefix.c_str(),i);
      params.insert(pair<string,RooRealVar*>(name, new RooRealVar(name.c_str(),name.c_str(),0.9-float(i-1)*1./nfracs,0.0001,0.9999)));
      fracs->add(*params[name]);
    }
    for (int i=1; i<=nexps; i++){
      string name =  Form("%s_p%d",prefix.c_str(),i);
      string ename =  Form("%s_e%d",prefix.c_str(),i);
      params.insert(pair<string,RooRealVar*>(name, new RooRealVar(name.c_str(),name.c_str(),TMath::Max(-1.,-0.04*(i+1)),-1.,0.)));
      pdfs.insert(pair<string,RooAbsPdf*>(ename, new RooExponential(ename.c_str(),ename.c_str(),*obs_var,*params[name])));
      exps->add(*pdfs[ename]);
    }

    // reset parameters  to decent values
    if (order > 3) //5
    {
        for(int i=1; i<=nfracs;++i){ //
            string oldName=Form("%s_f%d",prefix.c_str(),i);
            string newName=Form("%s_f%d",prefix.c_str(),i);
            bool success=replace( oldName, Form("ord%d",order), Form("ord%d",order-2) );
            if (not success) continue;
            if (i==nfracs) {oldName=Form("%s_f%d",prefix.c_str(),i-1);params[newName] -> setVal( params[oldName]->getVal()*.7);}
            else params[newName] -> setVal( params[oldName]->getVal()*.9); // slightly smaller fractions !
            
        } 
        for (int i=1; i<=nexps; i++){ // and now parameters,
          string newName =  Form("%s_p%d",prefix.c_str(),i);
          string oldName = newName;
          bool success=replace( oldName, Form("ord%d",order), Form("ord%d",order-2) );
          if (not success) continue;
          if (i==nexps) { oldName = Form("%s_p%d",prefix.c_str(),i-1); params[newName] -> setVal( params[oldName]->getVal()*.8); }
          else params[newName] -> setVal( params[oldName]->getVal()*1.1 ) ;
        }
    }
    //fracs->Print("v");
    //exps->Print("v");
    RooAbsPdf *exp = new RooAddPdf(prefix.c_str(),prefix.c_str(),*exps,*fracs,true);
    //exp->Print("v");
    cout << "--------------------------" << endl;
    return exp;
    //bkgPdfs.insert(pair<string,RooAbsPdf*>(exp->GetName(),exp));

  }
}

// ------------- TODO ---------------


BackgroundFitter::BackgroundFitter(){
    w_ = NULL;
    inputMasks.push_back("ChargedHiggsTauNu/Vars/Mt_Data");
    datasetMask_ = "dataObs_cat%d";
    normMask_ = "pdf_bkgmodel_cat%d_norm";
    modelMask_ = "pdf_bkgmodel_cat%d";
    writeDatasets_ = true;
    plot = true;
    plotDir = "plot/bkgfit";

    xmin=60;
    xmax=150;

    verbose=false;

    outname = "bkgfit.root";
    inname = "ChHiggs.root";
}

void BackgroundFitter::info(){
    cout<<"----------- FITTER INFO -----------"<<endl;
    cout<<"xMin="<<xmin <<endl;
    cout<<"xMax="<<xmax <<endl;
    cout<<"Dataset M="<<datasetMask_ <<endl;
    cout<<"NORM M="<<normMask_ <<endl;
    cout<<"MODEL M="<<modelMask_ <<endl;
    cout<<"OutName="<<outname<<endl;
    cout<<"InName="<<inname<<endl;
    for(auto &s : inputMasks) cout <<"mask = "<<s<<endl;
    cout<<"-----------------------------------"<<endl;
}

void BackgroundFitter::init(){
    gROOT-> SetBatch();

    info();

    if (not verbose) RooMsgService::instance().setSilentMode(true);

    if(w_==NULL) w_ = new RooWorkspace("w","workspace") ;

    if(x_==NULL) x_ = new RooRealVar("mmm","mmm",xmin,xmax);

}

void BackgroundFitter::plotOnFrame(RooPlot *p,RooAbsPdf*pdf, int color, int style, const string & legend,TLegend*leg)
{
    pdf -> plotOn(p, LineColor(color),LineStyle(style) );
    TObject *pdfLeg = p->getObject(int(p->numItems()-1));
    leg->AddEntry(pdfLeg,legend.c_str(),"L"); 
}


void BackgroundFitter::fit(){
    Log(__FUNCTION__,"INFO","fit");

    PdfModelBuilder modelBuilder;
    modelBuilder.setObsVar(x_);

    TFile *fInput = TFile::Open(inname.c_str() );

    if ( fInput == NULL ) 
        Log(__FUNCTION__,"ERROR","No such file or directory: '"+ inname + "'");

    if (plot ) {
            x_->setRange("unblindReg_1",xmin,120);
            x_->setRange("unblindReg_2",130,xmax);
            x_->setRange("tot",xmin,xmax);
    }


    // * take TH1F and make RooDataHist
    for(int cat=0;cat < int(inputMasks.size()); ++cat)
    {
        cout<<"* Getting"<<inputMasks[cat]<<endl;
        string name =  Form("dataHist_cat_%d",cat);

        if ( inputMasks[cat].find(":") == string::npos )
        {
            TH1D *h = (TH1D*)fInput ->Get( inputMasks[cat].c_str() ) ;
            h->Rebin(rebin);

            if (h == NULL) 
                Log(__FUNCTION__,"ERROR","No such histogram: mask="+inputMasks[cat]);

            // -- Construct RooDataHist
            hist_ [ name ] = new RooDataHist(
                    Form(datasetMask_.c_str(),cat),
                    Form(datasetMask_.c_str(),cat),
                    *x_,
                    Import( *h ) 
                    );
        }
        else { // it's already a RooDataHist
            string mask=  inputMasks[cat];
            TFile *fLocal=NULL;
            long sepfile= mask.find("|") ;
            if (sepfile == string::npos){
                sepfile=-1;
                fLocal=fInput;
            }
            else{
                string fName = mask.substr(0, sepfile);
                fLocal=TFile::Open(fName.c_str());
                if (fLocal==NULL) Log(__FUNCTION__,"ERROR","Unable to open File: '"+fName+"'");
                mask = mask.substr(sepfile+1,mask.size() );

            }

            size_t sep= mask.find(":") ;
            RooWorkspace * w_local = (RooWorkspace*) fInput -> Get(  mask.substr(0, sep).c_str() );
            if (w_local==NULL)
                Log(__FUNCTION__,"ERROR","Unable to find workspaces from: '"+inputMasks[cat]+"' -> w: '"+mask.substr(0, sep)+"'");
            RooDataHist *dh_local=(RooDataHist*) w_local->data( mask.substr(sep+1, mask.size() ).c_str() )->Clone(name.c_str());

            if (dh_local==NULL)
                Log(__FUNCTION__,"ERROR","Unable to find data hist from: '"+inputMasks[cat]+"' -> dh: '"+mask.substr(sep+1, mask.size() )+"'");

            string old= RooArgList(*dh_local->get(1))[0].GetName();
            dh_local->changeObservableName(old.c_str() , x_->GetName());
            // -- Construct RooDataHist
            hist_ [ name ] = dh_local; 
        }


        if (writeDatasets_)w_ -> import( *hist_[ name ] ); // write datasets in the workspace

        RooArgList storedPdfs(Form("store_cat%d",cat));

        // F-Test
        //
        cout<<"*** Fitting Bernstein ***"<<endl;
        int bernOrd;
        RooAbsPdf* bern = modelBuilder.fTest(Form("bern_cat%d",cat) ,hist_[name],&bernOrd,plotDir + "/bern");
        storedPdfs.add(*bern); // 0

        cout<<"*** Fitting DY Bernstein ***"<<endl;
        int dybernOrd;
        string mask= inputMasks[cat];
        string toReplace="Data";
        if (mask.find(toReplace) != string::npos) mask.replace(mask.find(toReplace), toReplace.length(),"DY");
        cout<<"-> Getting DY from "<<mask<<endl;
        TH1D *dy = (TH1D*)fInput ->Get( mask.c_str() ) ;
        if (dy==NULL) cout<<"  and hist doesn't exist"<<endl;
        RooAbsPdf* dybern = NULL;
        if (dy != NULL){
            dy->Scale(35867); // lumi
            dy->Rebin(10); // lumi
            //dy->Smooth(1); // lumi
            dybern = modelBuilder.fTest(Form("dybern_cat%d",cat) ,hist_[name],&dybernOrd,plotDir + "/dybern",dy);
            //storedPdfs.add(*dybern);
        }

        cout<<"*** Fitting ZPHO ***"<<endl;
        int zphoOrd;
        RooAbsPdf* zpho = modelBuilder.fTest(Form("zpho_cat%d",cat) ,hist_[name],&zphoOrd,plotDir + "/zpho");
        storedPdfs.add(*zpho); //1

        cout<<"*** Fitting ZMOD ***"<<endl;
        int zmodOrd;
        RooAbsPdf* zmod = modelBuilder.fTest(Form("zmod_cat%d",cat) ,hist_[name],&zmodOrd,plotDir + "/zmod");
        storedPdfs.add(*zmod);//2

        cout<<"*** Fitting ZRED ***"<<endl;
        int bwzOrd;
        RooAbsPdf* bwz = modelBuilder.fTest(Form("bwz_cat%d",cat) ,hist_[name],&bwzOrd,plotDir + "/bwz");
        //storedPdfs.add(*bwz);

        cout<<"*** Fitting MOD BERN ***"<<endl;
        int modbernOrd;
        RooAbsPdf* modbern = modelBuilder.fTest(Form("modbern_cat%d",cat) ,hist_[name],&modbernOrd,plotDir + "/modbern");
        //storedPdfs.add(*modbern);

        //cout<<"*** Fitting POWLAW ***"<<endl;
        //int powlawOrd;
        //RooAbsPdf* powlaw = modelBuilder.fTest(Form("powlaw_cat%d",cat) ,hist_[name],&powlawOrd, plotDir+"/powlaw");
        //storedPdfs.add(*powlaw);

        cout<<"*** Fitting EXP ***"<<endl;
        int expOrd;
        RooAbsPdf* exp = modelBuilder.fTest(Form("exp_cat%d",cat) ,hist_[name],&expOrd,plotDir+"/exp");
        storedPdfs.add(*exp); // 3

        //cout<<"*** Fitting LAU ***"<<endl;
        //int lauOrd;
        //RooAbsPdf* lau = modelBuilder.fTest(Form("lau_cat%d",cat) ,hist_[name],&lauOrd,plotDir+"/lau");
        //storedPdfs.add(*lau);
        //
        //
        cout<<"*** Fitting fewz_1j ***"<<endl;
        int fewz_1jOrd;
        RooAbsPdf* fewz_1j = modelBuilder.fTest(Form("fewz_1j_cat%d",cat) ,hist_[name],&fewz_1jOrd,plotDir + "/fewz_1j");
        storedPdfs.add(*fewz_1j); //4

        cout<<"*** Fitting fewz_2j ***"<<endl;
        int fewz_2jOrd;
        RooAbsPdf* fewz_2j = modelBuilder.fTest(Form("fewz_2j_cat%d",cat) ,hist_[name],&fewz_2jOrd,plotDir + "/fewz_2j");
        storedPdfs.add(*fewz_2j); //5

        cout<<"*** Fitting fewz_full ***"<<endl;
        int fewz_fullOrd;
        RooAbsPdf* fewz_full = modelBuilder.fTest(Form("fewz_full_cat%d",cat) ,hist_[name],&fewz_fullOrd,plotDir + "/fewz_full");
        storedPdfs.add(*fewz_full);//6

        cout<<"*** Fitting ZMOD2 ***"<<endl;
        //int zmod2Ord;
        //RooAbsPdf* zmod2 = modelBuilder.fTest(Form("zmod2_cat%d",cat) ,hist_[name],&zmod2Ord,plotDir + "/zmod2");
        //storedPdfs.add(*zmod2); //7

        {
            cout<<" --- Fitting ZMOD2 ORDER 5 ---"<<endl;
            int order=5; double nll; int fitStatus;
            RooAbsPdf *zmod2_ord5=modelBuilder.getZModExp2(Form("zmod2_cat%d_ord%d",cat,order),order);
            modelBuilder.runFit( zmod2_ord5 , hist_[name],&nll,&fitStatus,3);
            storedPdfs.add(*zmod2_ord5); //7

            cout<<" --- Fitting ZMOD2 ORDER 4 ---"<<endl;
            order=4;
            RooAbsPdf* zmod2_ord4=modelBuilder.getZModExp2(Form("zmod2_cat%d_ord%d",cat,order),order);
            modelBuilder.runFit( zmod2_ord4 , hist_[name],&nll,&fitStatus,3);
            storedPdfs.add(*zmod2_ord4); //8

            cout<<" --- Fitting ZMOD2 ORDER 3 ---"<<endl;
            order=3;
            RooAbsPdf* zmod2_ord3=modelBuilder.getZModExp2(Form("zmod2_cat%d_ord%d",cat,order),order);
            modelBuilder.runFit( zmod2_ord3 , hist_[name],&nll,&fitStatus,3);
            storedPdfs.add(*zmod2_ord3); //9

            cout<<" --- Fitting ZMOD2 ORDER 2 ---"<<endl;
            order=2;
            RooAbsPdf *zmod2_ord2=modelBuilder.getZModExp2(Form("zmod2_cat%d_ord%d",cat,order),order);
            modelBuilder.runFit( zmod2_ord2 , hist_[name],&nll,&fitStatus,3);
            storedPdfs.add(*zmod2_ord2); //10

            cout<<" --- Fitting ZMOD2 ORDER 1 == ZMOD ---"<<endl;
            order=1;
            RooAbsPdf *zmod2_ord1=modelBuilder.getZModExp2(Form("zmod2_cat%d_ord%d",cat,order),order);
            modelBuilder.runFit( zmod2_ord1 , hist_[name],&nll,&fitStatus,3);
            storedPdfs.add(*zmod2_ord1); //11

            vector<RooAbsPdf*> bern2;
            if (cat >=14){ // for 14 and 15 save all the bern 1 2 3 4
                cout<<" --- Fitting BERN2 ORDER 1 ---"<<endl;
                int order=1;
                RooAbsPdf *bern_ord1=modelBuilder.getBernstein(Form("bern2_cat%d_ord%d",cat,order),order);
                modelBuilder.runFit( bern_ord1 , hist_[name],&nll,&fitStatus,3);
                storedPdfs.add(*bern_ord1); //12
                bern2.push_back(bern_ord1);

                cout<<" --- Fitting BERN2 ORDER 2 ---"<<endl;
                order=2;
                RooAbsPdf *bern_ord2=modelBuilder.getBernstein(Form("bern2_cat%d_ord%d",cat,order),order);
                modelBuilder.runFit( bern_ord2 , hist_[name],&nll,&fitStatus,3);
                storedPdfs.add(*bern_ord2); //13
                bern2.push_back(bern_ord2);

                cout<<" --- Fitting BERN2 ORDER 3 ---"<<endl;
                order=3;
                RooAbsPdf *bern_ord3=modelBuilder.getBernstein(Form("bern2_cat%d_ord%d",cat,order),order);
                modelBuilder.runFit( bern_ord3 , hist_[name],&nll,&fitStatus,3);
                storedPdfs.add(*bern_ord3); //14
                bern2.push_back(bern_ord3);

                cout<<" --- Fitting BERN2 ORDER 4 ---"<<endl;
                order=4;
                RooAbsPdf *bern_ord4=modelBuilder.getBernstein(Form("bern2_cat%d_ord%d",cat,order),order);
                modelBuilder.runFit( bern_ord4 , hist_[name],&nll,&fitStatus,3);
                storedPdfs.add(*bern_ord4); //15
                bern2.push_back(bern_ord4);

                cout<<" --- Fitting BERN2 ORDER 5 ---"<<endl;
                order=5;
                RooAbsPdf *bern_ord5=modelBuilder.getBernstein(Form("bern2_cat%d_ord%d",cat,order),order);
                modelBuilder.runFit( bern_ord5 , hist_[name],&nll,&fitStatus,3);
                storedPdfs.add(*bern_ord5); //16
                bern2.push_back(bern_ord5);

            }

            if (plot){
                TCanvas *c = new TCanvas();
                TLegend *leg = new TLegend(0.6,0.65,0.89,0.89);
                leg->SetFillColor(0);
                leg->SetLineColor(0);
                RooPlot *p = x_ -> frame();
                if (blind){
                    hist_[name]->plotOn(p,CutRange("unblindReg_1"));
                    hist_[name]->plotOn(p,CutRange("unblindReg_2"));
                    hist_[name]->plotOn(p,Invisible()); // normalization
                    p->SetMinimum(0.0001);
                }
                else
                    hist_[name]->plotOn(p);

                TObject *datLeg = p->getObject(int(p->numItems()-1));
                leg->AddEntry(datLeg,Form("Data - cat%d",cat),"LEP");

                if(cat <14){
                    plotOnFrame( p, zmod2_ord5, kRed, kSolid,Form("zmod2 ord=%d chi2=%.2f",5,modelBuilder.getGoodnessOfFit(x_, zmod2_ord5, hist_[name], plotDir +"/zmod2/chosen",blind) ),leg);
                    plotOnFrame( p, zmod2_ord4, kBlue, kSolid,Form("zmod2 ord=%d chi2=%.2f",4,modelBuilder.getGoodnessOfFit(x_, zmod2_ord4, hist_[name], plotDir +"/zmod2/chosen",blind) ),leg);
                    plotOnFrame( p, zmod2_ord3, kGreen, kSolid,Form("zmod2 ord=%d chi2=%.2f",3,modelBuilder.getGoodnessOfFit(x_, zmod2_ord3, hist_[name], plotDir +"/zmod2/chosen",blind) ),leg);
                    plotOnFrame( p, zmod2_ord2, kOrange, kSolid,Form("zmod2 ord=%d chi2=%.2f",2,modelBuilder.getGoodnessOfFit(x_, zmod2_ord2, hist_[name], plotDir +"/zmod2/chosen",blind) ),leg);
                    plotOnFrame( p, zmod2_ord1, kCyan, kSolid,Form("zmod2 ord=%d chi2=%.2f",1,modelBuilder.getGoodnessOfFit(x_, zmod2_ord1, hist_[name], plotDir +"/zmod2/chosen",blind) ),leg);
                }
                else{
                    plotOnFrame( p, bern2[0], kRed, kSolid,Form("bern2 ord=%d chi2=%.2f",1,modelBuilder.getGoodnessOfFit(x_, bern2[0], hist_[name], plotDir +"/bern2/chosen",blind) ),leg);
                    plotOnFrame( p, bern2[1], kBlue, kSolid,Form("bern2 ord=%d chi2=%.2f",2,modelBuilder.getGoodnessOfFit(x_, bern2[1], hist_[name], plotDir +"/bern2/chosen",blind) ),leg);
                    plotOnFrame( p, bern2[2], kGreen, kSolid,Form("bern2 ord=%d chi2=%.2f",3,modelBuilder.getGoodnessOfFit(x_, bern2[2], hist_[name], plotDir +"/bern2/chosen",blind) ),leg);
                    plotOnFrame( p, bern2[3], kOrange, kSolid,Form("bern2 ord=%d chi2=%.2f",4,modelBuilder.getGoodnessOfFit(x_, bern2[3], hist_[name], plotDir +"/bern2/chosen",blind) ),leg);
                    plotOnFrame( p, bern2[4], kCyan, kSolid,Form("bern2 ord=%d chi2=%.2f",5,modelBuilder.getGoodnessOfFit(x_, bern2[4], hist_[name], plotDir +"/bern2/chosen",blind) ),leg);
                }

    
                p -> Draw();
                leg->Draw("same");

                if (cat<14){
                    system(Form("mkdir -p %s/zmod2/",plotDir.c_str()));
                    c -> SaveAs( Form("%s/zmod2/zmod2_all_cat%d.pdf",plotDir.c_str(),cat) );
                    c -> SaveAs( Form("%s/zmod2/zmod2_all_cat%d.png",plotDir.c_str(),cat) );
                }
                else{
                    system(Form("mkdir -p %s/bern2/",plotDir.c_str()));
                    c -> SaveAs( Form("%s/bern2/bern2_all_cat%d.pdf",plotDir.c_str(),cat) );
                    c -> SaveAs( Form("%s/bern2/bern2_all_cat%d.png",plotDir.c_str(),cat) );
                }

                delete p;
                delete c;
            }
        }


        // construct final model
        cout<<" -> Constructing Final model for cat"<<cat<<endl;

        RooCategory pdf_cat(Form("pdfindex_cat%d",cat),Form("pdfindex_cat%d",cat));
        //pdf_cat.setIndex(1);

        //RooMultiPdf multipdf;
        RooMultiPdf pdf_bkg(Form("pdf_cat%d_bkg",cat),"multipdf",pdf_cat,storedPdfs);
        RooRealVar pdf_norm(Form("pdf_cat%d_bkg_norm",cat),"norm", hist_[name]->sumEntries(), hist_[name]->sumEntries()/2.,hist_[name]->sumEntries()*2.) ;
        //pdf_norm.setConstant();

        w_ -> import (pdf_bkg,RecycleConflictNodes());  
        w_ -> import (pdf_norm,RecycleConflictNodes()); 
        w_ -> import (pdf_cat,RecycleConflictNodes()); 

        // -- Plot
        if (plot ) {
            //x_->setRange("unblindReg_1",xmin,120);
            //x_->setRange("unblindReg_2",130,xmax);
            //x_->setRange("tot",xmin,xmax);
            TCanvas *c = new TCanvas();
            TLegend *leg = new TLegend(0.6,0.65,0.89,0.89);
            leg->SetFillColor(0);
            leg->SetLineColor(0);
            RooPlot *p = x_ -> frame();
            if (blind){
                hist_[name]->plotOn(p,CutRange("unblindReg_1"));
                hist_[name]->plotOn(p,CutRange("unblindReg_2"));
                hist_[name]->plotOn(p,Invisible()); // normalization
                p->SetMinimum(0.0001);
            }
            else
                hist_[name]->plotOn(p);

            TObject *datLeg = p->getObject(int(p->numItems()-1));
            leg->AddEntry(datLeg,Form("Data - cat%d",cat),"LEP");

            //chi2=modelBuilder.getGoodnessOfFit(x_, bern, hist_[name], plotDir +"/bern/chosen",blind);
            plotOnFrame( p, bern, kBlue, kSolid,Form("bern ord=%d chi2=%.2f",bernOrd, modelBuilder.getGoodnessOfFit(x_, bern, hist_[name], plotDir +"/bern/chosen",blind) ),leg);
            //if (dybern != NULL)
            //    plotOnFrame( p, dybern, kGray+2, kDashed,Form("dybern ord=%d chi2=%.2f",dybernOrd,modelBuilder.getGoodnessOfFit(x_, dybern, hist_[name], plotDir +"/dybern/chosen",blind) ),leg);
            plotOnFrame( p, zpho, kOrange, kSolid,Form("zpho ord=%d chi2=%.2f",zphoOrd,modelBuilder.getGoodnessOfFit(x_, zpho, hist_[name], plotDir +"/zpho/chosen",blind) ),leg);
            plotOnFrame( p, zmod, kRed+2, kDashed,Form("zmod ord=%d chi2=%.2f",zmodOrd,modelBuilder.getGoodnessOfFit(x_, zmod, hist_[name], plotDir +"/zmod/chosen",blind) ),leg);
            //plotOnFrame( p, zmod2, kRed, kSolid,Form("zmod2 ord=%d chi2=%.2f",zmod2Ord,modelBuilder.getGoodnessOfFit(x_, zmod2, hist_[name], plotDir +"/zmod2/chosen",blind) ),leg);
            plotOnFrame( p, bwz, kCyan, kSolid,Form("bwz ord=%d chi2=%.2f",bwzOrd,modelBuilder.getGoodnessOfFit(x_, bwz, hist_[name], plotDir +"/bwz/chosen",blind) ),leg);
            //plotOnFrame( p, powlaw, kRed, kSolid,Form("powlaw ord=%d chi2=%.2f",powlawOrd,modelBuilder.getGoodnessOfFit(x_, powlaw, hist_[name], plotDir +"/powlaw/chosen",blind) ),leg);
            plotOnFrame( p, exp, kGreen, kSolid,Form("exp ord=%d chi2=%.2f",expOrd,modelBuilder.getGoodnessOfFit(x_, exp, hist_[name], plotDir +"/exp/chosen",blind) ),leg);
            //plotOnFrame( p, lau, kMagenta, kSolid,Form("lau ord=%d chi2=%.2f",lauOrd,modelBuilder.getGoodnessOfFit(x_, lau, hist_[name], plotDir +"/lau/chosen",blind) ),leg);
            plotOnFrame( p, fewz_1j, kGreen+2, kSolid,Form("fewz_1j ord=%d chi2=%.2f",fewz_1jOrd, modelBuilder.getGoodnessOfFit(x_, fewz_1j, hist_[name], plotDir +"/fewz_1j/chosen",blind) ),leg);
            plotOnFrame( p, fewz_2j, kMagenta+2, kDashed,Form("fewz_2j ord=%d chi2=%.2f",fewz_2jOrd, modelBuilder.getGoodnessOfFit(x_, fewz_2j, hist_[name], plotDir +"/fewz_2j/chosen",blind) ),leg);
            plotOnFrame( p, fewz_full, kCyan+2, kSolid,Form("fewz_full ord=%d chi2=%.2f",fewz_fullOrd, modelBuilder.getGoodnessOfFit(x_, fewz_full, hist_[name], plotDir +"/fewz_full/chosen",blind) ),leg);

    
            p -> Draw();
            leg->Draw("same");

            system(Form("mkdir -p %s",plotDir.c_str()));
            c -> SaveAs( Form("%s/bkg_cat%d.pdf",plotDir.c_str(),cat) );
            c -> SaveAs( Form("%s/bkg_cat%d.png",plotDir.c_str(),cat) );
            delete p;
            delete c;
        } // end plot
        
    } //end cat
}

void BackgroundFitter::write(){
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
