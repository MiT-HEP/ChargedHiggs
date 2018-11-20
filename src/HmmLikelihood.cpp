#include "interface/HmmLikelihood.hpp"
using namespace std;


float HmmLikelihood::likelihood(float Hpt, float Heta,float mjj_1, float mjj_2, float detajj_1,float detajj_2, float ncentjets,float firstQGL,float maxDeepB,float softHt1)
{
    float r = 0.;
    r += log(Eval("Hpt",Hpt));
    r += log(Eval("Heta",Heta));
    r += log(Eval("mjj_1",mjj_1));
    r += log(Eval("mjj_2",mjj_2));
    //if (detajj_1 >0) r += log(Eval("detajj_1",detajj_1));
    r += log(Eval("detajj_1",max(float(0.),detajj_1)));
    r += log(Eval("detajj_2",max(float(0.),detajj_2)));
    r += log(Eval("ncentjets",ncentjets));
    //r += log(Eval("deta",deta));
    //r += log(Eval("dphi",dphi));
    r += log(Eval("firstQGL",max(float(0.),firstQGL)));
    r += log(Eval("maxDeepB",max(float(0.),maxDeepB)));
    r += log(Eval("softHt1",max(float(0.),softHt1)));

    return r;
}

// Destructor. Clean resources in store
HmmLikelihood::~HmmLikelihood()
{
    for(auto it=store_.begin();it!=store_.end();it++)
    {
        delete it->second;
    }
    store_.clear();
}

void HmmLikelihood::init()
{
    store_["Hpt"] = new TF1("l_Hpt","[0]*(1-TMath::Exp(-(x-[2])/[1]))",0,10000);
    {
        store_["Hpt"] -> SetParameter(0,4.36965e-01/0.548788196776);
        store_["Hpt"] -> SetParameter(1,5.69864e+01);
        store_["Hpt"] -> SetParameter(2,-4.82624e+01);
    }

    store_["mjj_1"] = new TF1("l_mjj_1","[0]*(1-TMath::Exp(-(x-[2])/[1]))",0,10000);
    {
        store_["mjj_1"] -> SetParameter(0,8.45516e-01/0.884316087678);
        store_["mjj_1"] -> SetParameter(1,4.09109e+02);
        store_["mjj_1"] -> SetParameter(2,-2.67768e+02);
    }

    store_["mjj_2"] = new TF1("l_mjj_2","[0]*(1-TMath::Exp(-(x-[2])/[1]))",0,10000);
    {
        store_["mjj_2"] -> SetParameter(0,9.34002e-01/0.958604638626 );
        store_["mjj_2"] -> SetParameter(1,9.50970e+02);
        store_["mjj_2"] -> SetParameter(2,-5.06520e+02);
    }

    store_["detajj_1"] = new TF1("l_detajj_1","[0] + [1]*x",0,10000);
    {
        store_["detajj_1"] -> SetParameter(0,1.66935e-02/0.0347395227069 );
        store_["detajj_1"] -> SetParameter(1,2.27238e-03/0.0347395227069 );
    }

    store_["detajj_2"] = new TF1("l_detajj_2","[0] + [1]*x",0,10000);
    {
        store_["detajj_2"] -> SetParameter(0,2.25440e-02/0.0476201487887 );
        store_["detajj_2"] -> SetParameter(1,2.48477e-03/0.0476201487887 );
    }

    store_["softHt1"] = new TF1("l_detajj_2","(x<[4])*([0]+[1]*x+[2]*x*x)/[5] + (x>=[4])*([0]+[1]*[4]+[2]*[4]*[4] +[3]*(x-[4]))/[5]",0,1000);
    {
        store_["softHt1"] -> SetParameter(0,1.17263e-01 );
        store_["softHt1"] -> SetParameter(1,4.84488e-04 );
        store_["softHt1"] -> SetParameter(2,-1.51827e-05);
        store_["softHt1"] -> SetParameter(3,-5.22670e-04 );
        store_["softHt1"] -> SetParameter(4,5.99980e+01 );
        store_["softHt1"] -> SetParameter(5,0.232663602607);
    }
    
    store_["maxDeepB"] = new TF1("l_maxDeepB","([0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x)/[5]",0,1);
    {
        store_["maxDeepB"] -> SetParameter(0,0.139212);
        store_["maxDeepB"] -> SetParameter(1,-0.490601);
        store_["maxDeepB"] -> SetParameter(2,0.919301);
        store_["maxDeepB"] -> SetParameter(3,-0.849076);
        store_["maxDeepB"] -> SetParameter(4,0.297767);
        store_["maxDeepB"] -> SetParameter(5,0.263761673781);
    }
    
    store_["Heta"] = new TF1("l_Heta","([0]+[1]*abs(x)+[2]*x*x+[3]*abs(x*x*x)+[4]*x*x*x*x)/[5]",0,10);
    {// abs Heta
        store_["Heta"] -> SetParameter(0,0.0198072 );
        store_["Heta"] -> SetParameter(1,-0.000121444 );
        store_["Heta"] -> SetParameter(2,-0.000714758);
        store_["Heta"] -> SetParameter(3,3.73193e-05);
        store_["Heta"] -> SetParameter(4,1.6555e-05);
        store_["Heta"] -> SetParameter(5,0.0398648773619);
    }

    store_["deltaeta"] = new TF1("l_deltaeta","([0]+[1]*(-x)+[2]*x*x+[3]*(-x*x*x)+[4]*x*x*x*x)/[5]",0,10);
    { // x -> -x
        store_["deltaeta"] -> SetParameter(0,0.0148901);
        store_["deltaeta"] -> SetParameter(1,-0.000609412);
        store_["deltaeta"] -> SetParameter(2,-0.000885679 );
        store_["deltaeta"] -> SetParameter(3,-0.00028292);
        store_["deltaeta"] -> SetParameter(4,-2.78719e-05);
        store_["deltaeta"] -> SetParameter(5,0.0282519526408);
    }

    store_["deltaphi"] = new TF1("l_deltaphi","([0]+[1]*(-x)+[2]*x*x+[3]*(-x*x*x)+[4]*x*x*x*x +[5]*pow(-x,5) + [6]*pow(x,6))/[7]",0,10);
    {
        store_["deltaphi"] -> SetParameter(0,0.0932959);
        store_["deltaphi"] -> SetParameter(1,-0.00019284);
        store_["deltaphi"] -> SetParameter(2,0.0033782);
        store_["deltaphi"] -> SetParameter(3,-0.00341211);
        store_["deltaphi"] -> SetParameter(4,-0.00789946);
        store_["deltaphi"] -> SetParameter(5, -0.00379672);
        store_["deltaphi"] -> SetParameter(6,-0.00057899);
        store_["deltaphi"] -> SetParameter(7,0.136165972503);
    }
    store_["ncentjets"]=new TF1("l_ncentjets","(x<0.5)*0.457405+(0.5<x&&x<1.5)*0.628932+(1.5<x&&x<2.5)*0.528874+(x>2.5)*0.285878",0,10);
    {/*nothing*/}

    store_["firstQGL"]=new TF1("l_firstqgl","([0]+[1]*x)/[2]",0,1);
    {
        store_["firstQGL"] -> SetParameter(0,0.0455312 );
        store_["firstQGL"] -> SetParameter(1,-0.00285187 ); 
        store_["firstQGL"] -> SetParameter(2,0.0844472821156);
    }
}

