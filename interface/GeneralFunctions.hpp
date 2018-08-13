#ifndef GENERAL_FUNCT_H
#define GENERAL_FUNCT_H

#ifndef NULL
    #define NULL 0
#endif

#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <TLorentzVector.h>
//#include "interface/Davismt2.hpp"

namespace ChargedHiggs{

    // --- delta phi between -pi and pi
    float deltaPhi(const float phi1,const float phi2);

    // --- call delete and set to null
    template<typename T> 
    void Delete(T& x) { delete x; x=NULL; }

    // compute mt for massive vectors
    float mtMassive( const TLorentzVector p1,  const TLorentzVector p2);
    // mt for massless particles
    float mt(const float pt1, const float pt2, const float phi1, const float phi2);
    // compute mt2
    float mt2( const TLorentzVector visa,  const TLorentzVector visb,  const TLorentzVector metVec);
    // compute general functions Fox-Wolfram momenta
    float FW_momentum( const std::vector<TLorentzVector> jet, int index);

    // use this for massive objects
    template <typename T>
    inline float mt( const T& obj1, const T& obj2)
    {
        float constexpr pi = float(std::acos(-1));
        const float m1 = obj1.M();
        const float pt1= obj1.Pt();
        const float m2 = obj2.M();
        const float pt2= obj2.Pt();
        const float dphi = deltaPhi(obj1.Phi() , obj2.Phi() );

        const float pt12 = std::pow(pt1,2) + std::pow(pt2,2) - 2*pt1*pt2 * std::cos( pi  - dphi);

        const float et1 = std::sqrt( std::pow(m1,2) + std::pow(pt1,2) );
        const float et2 = std::sqrt( std::pow(m2,2) + std::pow(pt2,2) );
        return std::sqrt( std::pow(et1 + et2,2) - pt12);
    }

    double CosThetaCS(const TLorentzVector *v1, const TLorentzVector*v2, float sqrtS=13) ;
    double CosThetaStar(const TLorentzVector *v1, const TLorentzVector*v2) ;

    constexpr float Mw = 80.385;
    constexpr float Mh = 125.;
    constexpr float Mtop = 173.34;

   template<typename T>
   std::string printBinary(const T& x){ std::bitset<sizeof(T)*8> b(x); return b.to_string() ; }

   template<typename T>
   std::string printBinary(const T& x,unsigned max){ std::bitset<sizeof(T)*8> b(x); std::string s= b.to_string(); return s.substr(b.size()-max) ;  }

};

#include <map>
#include <memory>

namespace Binning{
    // can I template this ? 
    // bins, find the bin, return i if in [i,i+1)
    int findBin(const std::vector<float> &v, float x);
    // return the formmatted string "bin0_bin1"
    std::string findBinStr(const std::vector<float> &v, float x,const char* precision="%.0f_%.0f");

    // create a class that can do binary search on bins
    template <typename T>
    class bintree
    {
        private:
            enum COLOR {black,red};

            class node{     
                        public:
                            node(){ left.reset(NULL); right.reset(NULL); parent=NULL;}
                            std::vector< std::pair<float,float> > x; 
                            COLOR c{black}; 
                            T info;
                            std::unique_ptr< node > left;
                            std::unique_ptr< node > right;
                            node* parent;
                            const bool operator<( const node& y){ 
                                for(size_t d =0 ;d < x.size() ;++d)
                                {
                                    if (y.x.size() < d ) return false;
                                    if ( x[d].first < y.x[d].first ) return true;
                                    if ( x[d].first > y.x[d].first ) return false;
                                    if ( x[d].second < y.x[d].second) return true;
                                    if ( x[d].second > y.x[d].second) return false;
                                }
                                if (y.x.size() > x.size() ) return true;
                                return false;
                            }
                            const bool operator<(const std::vector<float> &y){
                                for(size_t d =0 ;d < x.size() ;++d)
                                {
                                    if (y.size()<d) return false; // at least equal will work ==
                                    if ( y[d] < x[d].first) return true;
                                    if ( y[d] > x[d].second) return false;
                                }
                                return false;
                            }
                            const bool operator>(const std::vector<float> &y){
                                for(size_t d =0 ;d < x.size() ;++d)
                                {
                                    if (y.size()<d) return false; // at least equal will work ==
                                    if ( y[d] < x[d].first) return false;
                                    if ( y[d] > x[d].second) return true;
                                }
                                return false; // ==
                            }
                        };


            std::unique_ptr< node > tree;

            // find the leaf smallest than a, return a
            node * dirtyInsert_ (node *base, node *a ) 
            {
                if ( base == NULL) return base;
                if ( *a < *base ) {
                        if (base->left.get() == NULL ) {  base->left -> reset (a) ; a->parent=base; return a;}
                        return  dirtyInsert_ (base->left.get(), a);
                        }
                if ( *a > *base ) {
                       if (base->right.get() == NULL ) { base->right -> reset (a) ; a->parent=base; return a;} 
                        return dirtyInsert_(base->right.get(), a);
                }
                return NULL; // already there
            }
          
            node* granparent( node *n ) { return n->parent->parent;}
            node* uncle ( node *n) {     
                if (n->parent == grandparent(n)->left.get()) return grandparent(n)->right.get();
                else return grandparent(n)->left.get();
            }
            //insert cases
            void insert_case1(node *n) 
            {
                //root
                if ( n->parent == NULL) {  n->c = black; return ;}
                // parent is black
                return insert_case2 (n);
            }
            void insert_case2(node *n)
            {
                if (n->parent->c == black ) { return; } // nothing to be done
                return insert_case3 (n);
            }
       
            void insert_case3(node *n) { // recolor
                    node *u = uncle(n);
                    node *g = grandparent(n);
                    if (u != NULL and u->c == red) {
                    n->parent->c = black;
                    u->c = black;
                    g->c = red;
                    insert_case1(g); // recolor grandparent
                }
                else
                    insert_case4(n);     
            }

            void rotate_left(node *a){

                    // referred to a, if it is a left or right child
                    bool isRoot=false,isLeft=false, isRight=false; // keep track of memory

                    node *p = a->parent;

                    if (p == NULL) {isRoot=true;}
                    else {
                        if (p->left.get() == a) { isLeft=true; p->left.release(); }
                        else { isRight=true;p->right.release(); } 
                    }

                    node *r = a->right.release();  // release n from it's parent
                    node *l = r->left.release();

                    r->parent=NULL; // reset to the root
                    a->parent=r;

                    if (isRight)  { p->right.reset(r); }
                    if (isLeft)   { p->left.reset(r); }
                    r->left.reset(a);
                    a->right.reset(l);
            }
            void rotate_right(node *a){

                    // referred to a, if it is a right or left child
                    bool isRoot=false,isLeft=false, isRight=false; // keep track of memory

                    node *p = a->parent;

                    if (p == NULL) {isRoot=true;}
                    else {
                        if (p->right.get() == a) { isRight=true; p->right.release(); }
                        else { isLeft=true;p->left.release(); } 
                    }

                    node *l = a->left.release();  // release n from it's parent
                    node *r = r->right.release();

                    l->parent=NULL; // reset to the root
                    a->parent=l;

                    if (isLeft)  { p->left.reset(l); }
                    if (isRight)   { p->right.reset(l); }
                    l->right.reset(a);
                    a->left.reset(r);
            }

            void insert_case4(node *n){
                if (n == n->parent->right.get() and n->parent == grandparent(n)->left.get()) {
                    rotate_left(n->parent);
                    n = n->left.get(); // update pointers to update the tree
                } else if (n == n->parent->left.get() && n->parent == grandparent(n)->right.get()) {
                    rotate_right(n->parent);
                    n = n->right.get();
                }
                insert_case5(n);
            } 

            void insert_case5(node *n)
            {
                node *g = grandparent(n);

                n->parent->c = black;
                g->c = red;
                if (n == n->parent->left)
                    rotate_right(g);
                else
                    rotate_left(g);
                return;
            }


            T find_( const std::vector<float> &x,node *t) const
                {
                    //if (t==NULL) throw std::out_of_range();
                    if (t==NULL) throw 10;
                    if (x<*t) return find_(t->left.get());
                    if (x>*t) return find_(t->right.get());
                    // FOUND!!
                    return t->info;
                }
        public:
            // binary search 1. find bin
            bintree(){ tree.reset(NULL);}

            T find( const std::vector<float> &x) const 
                {
                    return find_(x,tree.get());
                }
            void add ( std::vector< std::pair<float,float> > &x, T& info) {
                    //1. construct a
                    auto a= new node();
                    a->info = info;
                    a->x = x;
                    // check root
                    if (tree.get() == NULL){
                        tree.reset(a);
                        return ;
                    }
                    // 2. insert it and color it red
                    a->c = red;
                    if ( dirtyInsert_(tree.get(), a) == NULL ) return; // aleady there
                    // 3. check parent color
                    insert_case1(a);
                    }; 
    };

    //bintree<float> testTree; // this is for testing compilation
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
