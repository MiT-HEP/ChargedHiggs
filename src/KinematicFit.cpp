#include "interface/KinematicFit.hpp"

#include "TMatrixD.h"
#include "TVectorD.h"
#include <iostream>

void KinematicFit::addGhost()
{
    if (not doGhost) return;
    if (ghostAdded) return;

    for(int i=0;i<nGhost;++i)
    {
        TLorentzVector v;
        v.SetPtEtaPhiM( ptGhost,0, i*2*TMath::Pi()/nGhost,0);
        p4.push_back(v);
        sigma.push_back(sigmaGhost);
    }
    ghostAdded=true;
    return;
}

void KinematicFit::run(){

        addGhost();
        // this code minimize: 
        // \sum alpha_i p_i = -pH
        // min \sum_i (a_i-1)^2 / s^2
        
        int N=p4.size();
        TVectorD x(N+2),c(N+2);
        TMatrixD A(N+2,N+2);
        
        float pHx = H.Px();
        float pHy = H.Py();
        c(0) = -pHx;
        c(1) = -pHy;
        for(int i=0;i<N;++i) c(i+2)   = 2./(var(i));
        for(int i=0;i<N;++i) A(0,i+2) = p4[i].Px();
        for(int i=0;i<N;++i) A(1,i+2) = p4[i].Py();
        for(int i=0;i<N;++i) A(i+2,0) = -p4[i].Px();
        for(int i=0;i<N;++i) A(i+2,1) = -p4[i].Py();
        for(int i=0;i<N;++i) A(i+2,i+2)= 2./(var(i));
        
        A.Invert();
        x=A*c;

        alpha.clear();
        alpha.reserve(N); // one for each p4. Not copying lagrange mult
        for(int i=0;i<p4.size();++i)
        {
            alpha.push_back(x(i+2));
        }
        return;
}

#include <gsl/gsl_math.h> // -lgsl ?
#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_multimin.h>
#include <assert.h>

class info {
    public:
        TLorentzVector p4;
        float sigma;
        int constant;// for H

        // in this way one should be able to set it
        static float lambda ;
        static bool doNorm;
};
// init static
float info::lambda=1.;
bool info::doNorm=true;

int func(const gsl_vector * beta, void * p, gsl_vector * f)
{
    // beta = [ lx, ly, beta i]
    // f = [ Px, Py, i]
    std::vector<info> *infos = (std::vector<info>*)p;
    int N=infos->size();
    // 0 1 projections in x/y
    double Sx=0.,Sy=0.;
    double lx = gsl_vector_get(beta,0);
    double ly = gsl_vector_get(beta,1);
    for(int i=0;i<N;++i)  // the N-1 is H (constant)
    {
        double bi = 0.;
        if (not infos->at(i).constant) bi=gsl_vector_get(beta,i+2);
        Sx += infos->at(i).p4.Px() * std::exp(bi);
        Sy += infos->at(i).p4.Py() * std::exp(bi);
    }
    gsl_vector_set(f,0,Sx); 
    gsl_vector_set(f,1,Sy); 

    assert(infos->at(N-1).constant); // the last sholud be the only one constant

    double pHx = infos->at(N-1).p4.Px();
    double pHy = infos->at(N-1).p4.Py();
    for(int i=0;i<N-1;++i)
    {
        double bi = gsl_vector_get(beta,i+2);
        double si = infos->at(i).sigma;
        double fi=2*(bi)/(si*si) - lx*std::exp(bi)*pHx - ly*std::exp(bi)*pHy;
        gsl_vector_set(f,i+2,fi); 
    }

    return GSL_SUCCESS;
} // function


// this function implements the minimization 
void func2 (const gsl_vector * alpha, void * params, double * f, gsl_vector * g)
{
    std::vector<info> *infos = (std::vector<info>*)params;
    int N=infos->size();
    double pHx = infos->at(N-1).p4.Px();
    double pHy = infos->at(N-1).p4.Py();

    double lambda=infos->at(N-1).lambda; //static
    bool doNorm=infos->at(N-1).doNorm;
    double norm=1.;

    int Na=N-1; // number of params to minimize

    // compute Etmiss
    double metx=pHx , mety=pHy;
    if (doNorm) norm += infos->at(N-1).p4.Pt();
    for(unsigned i=0;i<Na;++i)
    {
        double ai= gsl_vector_get(alpha,i);
        metx += ai*infos->at(i).p4.Px();
        mety += ai*infos->at(i).p4.Py();
        if (doNorm) norm += infos->at(i).p4.Pt();
    }

    // compute f and derivative
    double value=0.0;
    for(unsigned i=0;i<Na;++i)
    {
        double ai= gsl_vector_get(alpha,i);
        //if (ai <=0) GSL_NAN;
        double lai= std::log(ai);
        double si = infos->at(i).sigma;

        value+= std::pow( lai/si,2);

        double dfi=
            double(2.)* ( lai )/(ai*si*si) + 
            lambda/norm*double(2.) * metx * infos->at(i).p4.Px() +
            lambda/norm*double(2.) * mety * infos->at(i).p4.Py();
        if (g != NULL) gsl_vector_set(g,i,dfi);
    }

    //double val2=value; //DEBUG
    value += lambda/norm * std::pow(metx,2) + lambda/norm*std::pow(mety,2);

    if (f!=NULL) *f=value;
   
    /* 
    if(true){ // DEBUG
        printf("--> func2 \n");
        printf("norm=%d\n",doNorm);
        printf("norm=%lf\n",norm);
        printf("lambda=%lf\n",lambda);
        printf("metx=%lf\n",metx);
        printf("mety=%lf\n",mety);
        printf("f=%lf\n",value);
        printf("f=(pen) %lf +(metx) %lf +(mety) %lf\n",val2,lambda/norm * std::pow(metx,2),lambda/norm*std::pow(mety,2));
        if (g != NULL){
            printf("df= (");
            for(int i=0;i<Na;++i) printf("%.5lf,",gsl_vector_get(g,i));
            printf(")\n");
        }
        printf("a= (");
        for(int i=0;i<Na;++i) printf("%.5lf,",gsl_vector_get(alpha,i));
        printf(")\n");
    }
    */
    return ;
}

double func2_f (const gsl_vector * alpha, void * params) //, gsl_vector * g)
{
    double f;
    func2(alpha,params,&f,(gsl_vector*)NULL);
    return f;
}
void func2_df (const gsl_vector * alpha, void * params,gsl_vector * g)
{
    func2(alpha,params,(double*)NULL,g);
}

void KinematicFit::runGeneric(){
    /* This function use a generic minimization form which is base on a log-normal prior.
     * It should be better as multiplicative prior, because it has a boundary at 0.
     */

    addGhost();
    info::doNorm=doNorm;
    info::lambda=lambda;

    std::vector<info> myinfos;
    for(int i=0;i<p4.size();++i)
    {
        info a;
        a.p4=p4[i];
        a.sigma=sigma[i];
        a.constant=0;
        myinfos.push_back(a);
    }

    // last position is H
    info a;
    a.p4=H;
    a.sigma=0;
    a.constant=1;
    myinfos.push_back(a);

    if (genericType==0)
    {
        // Find ROOTS
        gsl_multiroot_function F;

        F.f = &func;
        //F.n = p4.size()+2; // == myinfos.size()-1 + 2
        F.n = p4.size()+2;
        F.params = &myinfos;

        // solver
        const gsl_multiroot_fsolver_type *T;
        gsl_multiroot_fsolver *s;

        // init: [1,1,0...] corresponding to lx, ly, bi
        double x_init[p4.size()+2];
        gsl_vector *x = gsl_vector_alloc (p4.size()+2);
        gsl_vector_set(x,0,1);
        gsl_vector_set(x,1,1);
        for(int i=0;i<p4.size();++i)gsl_vector_set(x,i+2,0.); //exp(0.)=1 -> No correction

        T = gsl_multiroot_fsolver_hybrids;
        s = gsl_multiroot_fsolver_alloc (T, x->size);
        gsl_multiroot_fsolver_set (s, &F, x);

        int status;
        size_t iter=0;
        //print_state (iter, s);
        do
        {
            iter++;
            status = gsl_multiroot_fsolver_iterate (s);

            //print_state (iter, s);

            if (status)   /* check if solver is stuck */
                break;

            status =
                gsl_multiroot_test_residual (s->f, 1e-3);
        }
        while (status == GSL_CONTINUE && iter < maxIteration);

        //printf ("status = %s\n", gsl_strerror (status));
        if (status != GSL_SUCCESS) std::cout<<"[WARNING] Unable to find minimum"<<std::endl;

        // copy values  beta-> alpha
        alpha.clear();
        alpha.reserve(p4.size());
        for(int i=0;i<p4.size();++i)
        {
            alpha.push_back(std::exp(gsl_vector_get (s->x, i+2)));
        }
        gsl_multiroot_fsolver_free (s);
        gsl_vector_free (x);
    }
    else if (genericType==1)
    {
        gsl_multimin_function_fdf F;
        F.n = p4.size();
        F.f = &func2_f;
        F.df = &func2_df;
        F.fdf = &func2;
        F.params = &myinfos;
        gsl_vector *x = gsl_vector_alloc (p4.size());
        // init 
        for(unsigned i=0;i<p4.size();++i) gsl_vector_set(x,i,1.);

        // minimizer
        size_t iter = 0;
        int status;

        const gsl_multimin_fdfminimizer_type *T;
        gsl_multimin_fdfminimizer *s;
        //T = gsl_multimin_fdfminimizer_conjugate_fr;
        T = gsl_multimin_fdfminimizer_steepest_descent;
        s = gsl_multimin_fdfminimizer_alloc (T, p4.size());

        gsl_multimin_fdfminimizer_set (s, &F, x, 1e-2, 0.1); // first step-size and tolerance = decrease parameter

        do
        {
            iter++;
            status = gsl_multimin_fdfminimizer_iterate (s);
            //printf("* Status of iterate is %d.\n",status);

            if (status) break; // exit on error

            status = gsl_multimin_test_gradient (s->gradient, 1e-3); // absolute tolerance
            
            /* DEBUG
            printf("-> status of gradient is %d\n",status);
            printf("-> gradient is (");
            
            for(unsigned i=0;i<p4.size();++i)
                printf("%.5f,",gsl_vector_get(s->gradient,i));
            printf(")\n");
            
            if (status == GSL_SUCCESS) 
                printf ("Minimum found at:\n");
            if (status == GSL_ENOPROG) 
                printf ("Unable to progress towards a solution:\n");

            printf("-> iter %5d (",iter); 
            for(unsigned i=0;i<p4.size();++i)printf("%.5f,",gsl_vector_get (s->x, i));
            printf(") %10.5f\n",s->f);
            */

        }
        while (status == GSL_CONTINUE && iter < maxIteration);

        if (status != GSL_SUCCESS) std::cout<<"[WARNING] Unable to find minimum"<<std::endl;

        //copy back alpha vector
        alpha.clear();
        alpha.reserve(p4.size());
        for(int i=0;i<p4.size();++i)
        {
            float ai= gsl_vector_get (s->x, i);
            alpha.push_back(ai);
        }
        
        //dealloc
        gsl_multimin_fdfminimizer_free (s);
        gsl_vector_free (x);
    }
    else {
        std::cout<<"[ERROR]:: Unsupported type"<<std::endl;
    }
}

