#include "interface/NegativeWeightInterpolator.hpp"
#include "TMath.h"

// ------------------------------ ROOFIT -----------------------------
using namespace RooFit;
// -------------------------------------------------------------------

TH1* NegativeWeightInterpolator::fit( const TH1 * h )
{
    if (verbose_>0) cout<<"[NegativeWeightInterpolator]::[fit]::[DEBUG] Reset"<<endl;
    // 1) reset , fit is deterministic
    reset();
    guess(h);
    if (verbose_>0) cout<<"[NegativeWeightInterpolator]::[fit]::[DEBUG] 2) Create target dataset"<<endl;
    // 2) create target dataset
    float xmin = h->GetBinLowEdge(1);
    float xmax = h->GetBinLowEdge( h->GetNbinsX() +1 ) ;
    vars["x"] -> setRange(xmin,xmax);
    RooDataHist HistToFit("target","hist target",*vars["x"],h); 
    if (verbose_>0) cout<<"[NegativeWeightInterpolator]::[fit]::[DEBUG] 3) delete"<<endl;
    // 3) fit
    RooMsgService::instance().setSilentMode(true);
    if (result!=NULL)  delete result;
    if (verbose_>0) cout<<"[NegativeWeightInterpolator]::[fit]::[DEBUG] 3) fit"<<endl;
    if (verbose_ >1 ) {
        cout <<"\t data = "<<endl; HistToFit . Print();
        cout <<"\t model = "<<endl; model -> Print();
        }
    result = model->fitTo(HistToFit, 
		    SumW2Error(kTRUE),// mc has weights!
		    Save(),
		    PrintEvalErrors(-1),
		    Warnings(0)
		    );
    if (verbose_>0) cout<<"[NegativeWeightInterpolator]::[fit]::[DEBUG] 4) propagate"<<endl;
    // 4) propagate to the histogram
    TH1 * r = (TH1*) h -> Clone (Form("%s_interpolated", h->GetName() ));

    float N = h ->Integral("width");
    //float dN;
    //float N = h ->IntegralAndError(1,h->GetNbinsX(),dN,"width");


    for(int i=0 ; i <= h->GetNbinsX() ; ++i)
   	{
		double x = h->GetBinCenter(i);
		double w = h->GetBinWidth(i);
		double low = h->GetBinLowEdge(i);
		double high = h->GetBinLowEdge(i+1);
		string range= Form("range_bin%d",i);
		vars["x"] -> setRange ( range.c_str(), low, high);
		RooRealVar * v = (RooRealVar*)model -> createIntegral( *vars["x"], NormSet( *vars["x"] ), Range(range.c_str() ));
		double area =  v -> getVal();
        double error = v -> getError(); // this needs error propagation ! -> numeric integration, toys?
		delete v;
		r -> SetBinContent(i, area * N ) ;
        r -> SetBinError(i, error *N ) ; // TODO, TOCHECK, + area*dN
    	}
    if (verbose_>0) cout<<"[NegativeWeightInterpolator]::[fit]::[DEBUG] 5) return success"<<endl;
    return r;
}

TH1 * NegativeWeightInterpolator::add(const TH1* pos, const TH1*neg)
{
	TH1* neg2  = (TH1*) neg -> Clone( Form("%s_neg", neg->GetName() ) );
	neg2 -> Scale(-1);


	TH1* pos_smooth = fit(pos);
	TH1* neg_smooth = fit(neg2);

	for(int i=1;i<= pos_smooth->GetNbinsX() ;++i)
	{
	double y1 = pos_smooth->GetBinContent(i);
	double y2 = neg_smooth->GetBinContent(i);
	double y = y1-y2;
	if (y<0) {
		cout<<"[NegativeWeightInterpolator]::[add]::[warning]: negative bin "<<i<<" set to 0"<<endl;
		y=0;
		}
	pos_smooth->SetBinContent(i, y);
	}
	return pos_smooth;
}

void NegativeWeightInterpolator::guess(const TH1*h)
{
     // guess initial values of the parameters.
     double mean = h->GetMean();
     double rms  = h->GetRMS();
     // -- rayleigh
     vars["s1"] -> setVal( mean / TMath::Sqrt( TMath::Pi()/2.0 )  );
     vars["s1"] -> setRange( vars["s1"]->getVal()/3.0 , vars["s1"]->getVal()*3.0 ) ;
     // -- expo
     vars["l3"] -> setVal( -1./mean ) ;
     vars["l3"] -> setRange( -10000., -(1./mean)/3.0 ) ;
}

void NegativeWeightInterpolator::reset(){
	vars["x"] -> setVal(50);
	vars["x"] -> setRange(0,100);

	// vars["m2"] -> setVal(50);
	// vars["m2"] -> setRange(0,100);

	// vars["s2"] -> setVal(50);
	// vars["s2"] -> setRange(0,100);

	vars["s1"] -> setVal(50);
	vars["s1"] -> setRange(0,100);

	vars["l3"] -> setVal(50);
	vars["l3"] -> setRange(0,100);

	vars["alpha"] -> setVal(0.3);
	vars["alpha"] -> setRange(0.05,0.95);

	// vars["beta"] -> setVal(0.3);
	// vars["beta"] -> setRange(0.05,0.95);
}

// --- void NegativeWeightInterpolator::constructModel(){
// --- 	RooRealVar *x = new RooRealVar("x","x",100,0,100);
// --- 	vars["x"] = x;
// --- 	//-- rayleigh
// --- 	RooRealVar *s1 = new RooRealVar("s1","s1",50,0,100);
// --- 	vars[s1->GetName()] = s1;
// --- 
// --- 	RooGenericPdf *rayleigh = new RooGenericPdf("rayleigh","@0/(@1*@1)*TMath::Exp(-@0*@0/(2*@1*@1))", RooArgList(*x,*s1) );
// ---     pdfs [ rayleigh -> GetName() ] = rayleigh;
// --- 
// --- 	// gaus
// --- 	RooRealVar *m2 = new RooRealVar("m2","m2",50,0,100);
// --- 	RooRealVar *s2 = new RooRealVar("s2","s2",50,0,100);
// --- 	vars[m2->GetName()] = m2;
// --- 	vars[s2->GetName()] = s2;
// --- 	RooGaussian *gaus = new RooGaussian("gaus","gaus",*x,*m2,*s2);
// ---     pdfs [ gaus->GetName() ] = gaus;
// --- 	// exp
// --- 	RooRealVar *l3 = new RooRealVar("l3","l3",50,0,100);
// --- 	vars[l3->GetName()] = l3;
// --- 	RooExponential *expo = new RooExponential("expo","expo",*x,*l3);
// ---     pdfs[ expo->GetName() ] = expo;
// --- 
// --- 
// --- 	// model
// ---     RooRealVar *a = new RooRealVar("alpha","alpha",1.0,0.05,0.95);
// ---     RooRealVar *b = new RooRealVar("beta","beta",1.0,0.05,0.95);
// --- 	vars[a->GetName()] = a;
// --- 	vars[b->GetName()] = b;
// --- 	model = new RooAddPdf("model","model", RooArgList(*rayleigh,*gaus,*expo) , RooArgList(*a,*b) );
// --- 
// ---     // workspace
// ---     w_->import( *model );
// --- }

void NegativeWeightInterpolator::constructModel(){
	RooRealVar *x = new RooRealVar("x","x",100,0,100);
	vars["x"] = x;
	//-- rayleigh
	RooRealVar *s1 = new RooRealVar("s1","s1",50,0,100);
	vars[s1->GetName()] = s1;

	RooGenericPdf *rayleigh = new RooGenericPdf("rayleigh","@0/(@1*@1)*TMath::Exp(-@0*@0/(2*@1*@1))", RooArgList(*x,*s1) );
    pdfs [ rayleigh -> GetName() ] = rayleigh;

	// exp
	RooRealVar *l3 = new RooRealVar("l3","l3",50,0,100);
	vars[l3->GetName()] = l3;
	RooExponential *expo = new RooExponential("expo","expo",*x,*l3);
    pdfs[ expo->GetName() ] = expo;


	// model
    RooRealVar *a = new RooRealVar("alpha","alpha",1.0,0.05,0.95);
	vars[a->GetName()] = a;
	model = new RooAddPdf("model","model", RooArgList(*rayleigh,*expo) , RooArgList(*a) );

    // workspace
    //w_->import( *model );
}

void NegativeWeightInterpolator::print(){
    cout <<"---------- VARS ----------"<<endl;
    for(auto &x : vars)
    {
        cout<<"*) "<<x.first <<endl;
        x.second->Print();
    }
    cout <<"---------- PDFS ----------"<<endl;
    for(auto &x : pdfs)
    {
        cout<<"*) "<<x.first <<endl;
        x.second->Print();
    }
    cout <<"---------- MODEL ----------"<<endl;
    model->Print();
    cout <<"--------------------------"<<endl;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
