#include "interface/AnalysisScikit.hpp"
#include "TROOT.h"
#include "TRandom3.h"
#include "TPython.h"

void SciKitAnalysis::Init(){
    py . reset(new TPython);

    py -> Exec("import numpy as np");
    py -> Exec("from sklearn.ensemble import RandomForestClassifier");
    py -> Exec("from sklearn.calibration import CalibratedClassifierCV");
    py -> Exec("from sklearn.metrics import log_loss");
    py -> Exec("from sklearn.externals import joblib ");
    //"test/scikit/filename.pkl"
    //
    py -> Exec("clf=joblib.load('test/scikit/filename.pkl') ");
    py -> Exec("from array import array");

    //PyObject* pyx = py->ObjectProxy_FromVoidPtr(&x, "std::vector<float>");

    //PyObject* pymain = PyImport_ImportModule("__main__");

    //cout <<" pymain="<<long(pymain)<<endl;
    //cout <<" pyx="<<long(pyx)<<endl;

    //PyModule_AddObject(pymain, "x", pyx);

    //Py_DECREF(pymain);
    //py ->Bind (x1,"x1");
    //py ->Bind (x2,"x2");
}

int SciKitAnalysis::analyze(Event*e,string systname)
{
    //py -> Bind( TObject*, label);
    x.resize(2);
    x[0]=gRandom->Gaus();
    x[1]=gRandom->Gaus();


    float y = py -> Eval( "clf.predict([[x[0],x[1]]])[0]" );
    cout <<"["<<x[0]<<","<<x[1]<<"]  -> "<<y<<endl;

}


void SciKitAnalysis::End(){

}
