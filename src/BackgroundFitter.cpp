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

#include "TLegend.h"

using namespace RooFit;

class PdfModelBuilder
{
    RooRealVar *obs_var;
    map<string,RooRealVar*> params;
    map<string,RooFormulaVar*> prods;
    map<string,RooAbsPdf*> pdfs;
    map<string,RooDataHist*> hists;
    public:
        PdfModelBuilder(){};
        ~PdfModelBuilder(){};
        //
        void setObsVar(RooRealVar *var){obs_var=var;};
        RooAbsPdf* getBernstein(string prefix, int order);
        RooAbsPdf* getModBernstein(string prefix, int order);
        //RooAbsPdf* getChebychev(string prefix, int order);
        //RooAbsPdf* getPowerLaw(string prefix, int order);
        //RooAbsPdf* getPowerLawSingle(string prefix, int order);
        RooAbsPdf* getPowerLawGeneric(string prefix, int order);
        //RooAbsPdf* getExponential(string prefix, int order);
        RooAbsPdf* getExponentialSingle(string prefix, int order);
        RooAbsPdf* getLaurentSeries(string prefix, int order);
        RooAbsPdf* getLogPol(string prefix, int order);
        RooAbsPdf* getExpPol(string prefix, int order); // e^pol
        RooAbsPdf* getDYBernstein(string prefix, int order,TH1D*dy);

        // get the order for the ftest
        // prefix are bern, powlaw, exp, lau
        // return the relevalt to be included in the multipdf
        RooAbsPdf* getPdf(const string& prefix,int order );
        RooAbsPdf* getPdf(const string& prefix,int order, TH1D*h ) { 
            if (prefix.find( "dybern")!=string::npos) return getDYBernstein(prefix+ Form("_ord%d",order),order,h); else return getPdf(prefix,order);
        }
        RooAbsPdf* fTest(const string& prefix,RooDataHist*,int *ord=NULL,const string&plotDir="",TH1D*h=NULL);
        void runFit(RooAbsPdf *pdf, RooDataHist *data, double *NLL, int *stat_t, int MaxTries);
        double getProbabilityFtest(double chi2, int ndof);

};

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
        minnll = fitTest->minNll();
        if (stat!=0) params_test->assignValueOnly(fitTest->randomizePars());
        ntries++; 
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
    if (prefix.find("powlaw") != string::npos )
        return getPowerLawGeneric(prefix+ Form("_ord%d",order),order);
    if (prefix.find("exppol")!=string::npos)
        return getExpPol(prefix+ Form("_ord%d",order),order);
    if (prefix.find("exp")!=string::npos)
        return getExponentialSingle(prefix+ Form("_ord%d",order),order);
    if (prefix.find("lau") != string::npos)
        return getLaurentSeries(prefix+ Form("_ord%d",order),order);
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
        prevNll=thisNll;
        prevOrder = order;
        //delete prevPdf; // destroed here
        prevPdf = model;
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
                s.second->plotOn(p,RooFit::Layout(0.34,0.96,0.89),RooFit::Format("NEA",AutoPrecision(1)),LineColor(col));
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

RooAbsPdf* PdfModelBuilder::getBernstein(string prefix, int order){
  
  RooArgList *coeffList = new RooArgList();
  //coeffList->add(RooConst(1.0)); // no need for cnstant in this interface
  for (int i=0; i<order; i++){
    string name = Form("%s_p%d",prefix.c_str(),i);
    RooRealVar *param = new RooRealVar(name.c_str(),name.c_str(),0.1*(i+1),-5.,5.);
    RooFormulaVar *form = new RooFormulaVar(Form("%s_sq",name.c_str()),Form("%s_sq",name.c_str()),"@0*@0",RooArgList(*param));
    //RooFormulaVar *form = new RooFormulaVar(Form("%s_fv",name.c_str()),Form("%s_fv",name.c_str()),"@0",RooArgList(*param));
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


void BackgroundFitter::fit(){
    Log(__FUNCTION__,"INFO","fit");

    PdfModelBuilder modelBuilder;
    modelBuilder.setObsVar(x_);

    TFile *fInput = TFile::Open(inname.c_str() );

    if ( fInput == NULL ) 
        Log(__FUNCTION__,"ERROR","No such file or directory: '"+ inname + "'");



    // * take TH1F and make RooDataHist
    for(int cat=0;cat < int(inputMasks.size()); ++cat)
    {
        cout<<"* Getting"<<inputMasks[cat]<<endl;
        TH1D *h = (TH1D*)fInput ->Get( inputMasks[cat].c_str() ) ;
        h->Rebin(rebin);

        if (h == NULL) 
            Log(__FUNCTION__,"ERROR","No such histogram: mask="+inputMasks[cat]);

        // -- Construct RooDataHist
        string name =  Form("dataHist_cat_%d",cat);
        hist_ [ name ] = new RooDataHist(
                Form(datasetMask_.c_str(),cat),
                Form(datasetMask_.c_str(),cat),
                *x_,
                Import( *h ) 
                );


        if (writeDatasets_)w_ -> import( *hist_[ name ] ); // write datasets in the workspace

        RooArgList storedPdfs(Form("store_cat%d",cat));

        // F-Test
        //
        int bernOrd;
        RooAbsPdf* bern = modelBuilder.fTest(Form("bern_cat%d",cat) ,hist_[name],&bernOrd,plotDir + "/bern");
        storedPdfs.add(*bern);

        int dybernOrd;
        string mask= inputMasks[cat];
        string toReplace="Data";
        mask.replace(mask.find(toReplace), toReplace.length(),"DY");
        cout<<"-> Getting DY from "<<mask<<endl;
        TH1D *dy = (TH1D*)fInput ->Get( mask.c_str() ) ;
        if (dy==NULL) cout<<"  and hist doesn't exist"<<endl;
        dy->Scale(35867); // lumi
        dy->Rebin(10); // lumi
        //dy->Smooth(1); // lumi
        RooAbsPdf* dybern = modelBuilder.fTest(Form("dybern_cat%d",cat) ,hist_[name],&dybernOrd,plotDir + "/dybern",dy);
        storedPdfs.add(*dybern);

        //int exppolOrd;
        //RooAbsPdf* exppol = modelBuilder.fTest(Form("exppol_cat%d",cat) ,hist_[name],&exppolOrd,plotDir + "/exppol");
        //storedPdfs.add(*exppol);

        //int logpolOrd;
        //RooAbsPdf* logpol = modelBuilder.fTest(Form("logpol_cat%d",cat) ,hist_[name],&logpolOrd,plotDir + "/logpol");
        //storedPdfs.add(*logpol);

        int modbernOrd;
        RooAbsPdf* modbern = modelBuilder.fTest(Form("modbern_cat%d",cat) ,hist_[name],&modbernOrd,plotDir + "/modbern");
        storedPdfs.add(*modbern);

        int powlawOrd;
        RooAbsPdf* powlaw = modelBuilder.fTest(Form("powlaw_cat%d",cat) ,hist_[name],&powlawOrd, plotDir+"/powlaw");
        storedPdfs.add(*powlaw);

        int expOrd;
        RooAbsPdf* exp = modelBuilder.fTest(Form("exp_cat%d",cat) ,hist_[name],&expOrd,plotDir+"/exp");
        storedPdfs.add(*exp);

        int lauOrd;
        RooAbsPdf* lau = modelBuilder.fTest(Form("lau_cat%d",cat) ,hist_[name],&lauOrd,plotDir+"/lau");
        storedPdfs.add(*lau);

        // construct final model
        cout<<" -> Constructing Final model for cat"<<cat<<endl;

        RooCategory pdf_cat(Form("pdfindex_cat%d",cat),Form("pdfindex_cat%d",cat));
        //RooMultiPdf multipdf;
        RooMultiPdf pdf_bkg(Form("pdf_cat%d_bkg",cat),"multipdf",pdf_cat,storedPdfs);
        RooRealVar pdf_norm(Form("pdf_cat%d_bkg_norm",cat),"norm", hist_[name]->sumEntries()) ;
        pdf_norm.setConstant();

        w_ -> import (pdf_bkg,RecycleConflictNodes());  
        w_ -> import (pdf_norm,RecycleConflictNodes()); 
        w_ -> import (pdf_cat,RecycleConflictNodes()); 

        // -- Plot
        if (plot ) {
            x_->setRange("unblindReg_1",xmin,120);
            x_->setRange("unblindReg_2",130,xmax);
            x_->setRange("tot",xmin,xmax);
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

            bern->plotOn(p);
            TObject *bernLeg = p->getObject(int(p->numItems()-1));
            leg->AddEntry(bernLeg,Form("bern ord=%d",bernOrd),"L");

            dybern->plotOn(p,LineColor(kGray),LineStyle(kDashed));
            TObject *dybernLeg = p->getObject(int(p->numItems()-1));
            leg->AddEntry(dybernLeg,Form("dybern ord=%d",dybernOrd),"L");

            //exppol->plotOn(p,LineColor(kOrange));
            //TObject *exppolLeg = p->getObject(int(p->numItems()-1));
            //leg->AddEntry(exppolLeg,Form("exppol ord=%d",exppolOrd),"L");

            //logpol->plotOn(p,LineStyle(kDashed));
            //TObject *logpolLeg = p->getObject(int(p->numItems()-1));
            //leg->AddEntry(logpolLeg,Form("logpol ord=%d",logpolOrd),"L");

            //modbern->plotOn(p,LineColor(kGreen),LineStyle(kDashed));
            //TObject *modbernLeg = p->getObject(int(p->numItems()-1));
            //leg->AddEntry(modbernLeg,Form("modbern ord=%d",modbernOrd),"L");

            powlaw->plotOn(p,LineColor(kRed));
            TObject *powlawLeg = p->getObject(int(p->numItems()-1));
            leg->AddEntry(powlawLeg,Form("powlaw ord=%d",powlawOrd),"L");

            exp->plotOn(p,LineColor(kGreen));
            TObject *expLeg = p->getObject(int(p->numItems()-1));
            leg->AddEntry(expLeg,Form("exp ord=%d",expOrd),"L");

            lau->plotOn(p,LineColor(kMagenta));
            TObject *lauLeg = p->getObject(int(p->numItems()-1));
            leg->AddEntry(lauLeg,Form("lau ord=%d",lauOrd),"L");
    
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
