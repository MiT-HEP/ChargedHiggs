#include "interface/AnalysisScikit.hpp"
#include "TROOT.h"
#include "TRandom3.h"
#include "TPython.h"

void SciKitAnalysis::Init(){
    py . reset(new TPython);

    py -> Exec("import numpy as np");
    py -> Exec("import keras");
    py -> Exec("kmodel=keras.models.load_model('aux/tb_trainings/tb_1l_keras_trained_model.h5')");
    py -> Exec("kmodel.summary()");

    // bind x to x
    py -> Exec("x = ROOT.vector(float)()");

    x = (std::vector<float>*) py->Eval("x");

    
    // maybe there is a way with this. 
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
    x->resize(2);
    (*x)[0]=gRandom->Gaus();
    (*x)[1]=gRandom->Gaus();


    float y = py->Eval("kmodel.predict(np.array([ [ x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15] ] ] ))[0][0]");
    cout <<"["<<(*x)[0]<<","<<(*x)[1]<<"]  -> "<<y<<endl;

}


void SciKitAnalysis::End(){

}
