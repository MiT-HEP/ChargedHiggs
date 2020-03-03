#include "interface/GeneralFunctions.hpp"
#include "interface/Davismt2.hpp"
#include "TMath.h"
#include <cmath>

float ChargedHiggs::deltaPhi(const float phi1,const float phi2)
{
    float dphi = phi1 - phi2;
    while (dphi > TMath::Pi() ) dphi -= TMath::TwoPi(); 
    while (dphi < -TMath::Pi() ) dphi += TMath::TwoPi();
    return dphi;
}

float ChargedHiggs::mt( const float pt1,  const float pt2, const  float phi1, const float phi2)
{
    if (TMath::IsNaN(pt1) or TMath::IsNaN(pt2) or TMath::IsNaN(phi1) or TMath::IsNaN(phi2) ) return 0.;
    return TMath::Sqrt( 2.* fabs(pt1) * fabs(pt2) * fabs( 1.-TMath::Cos(ChargedHiggs::deltaPhi(phi1,phi2)) ) );
}

float ChargedHiggs::mtMassive( const TLorentzVector p1,  const TLorentzVector p2)
{
    return TMath::Sqrt( (p1.Et() + p2.Et())*(p1.Et() + p2.Et()) - ((p1 + p2).Px()*(p1 + p2).Px() + (p1 + p2).Py()*(p1 + p2).Py()) );

    //   2* pt1 * pt2 * ( 1.-TMath::Cos(ChargedHiggs::deltaPhi(phi1,phi2)) ) );
    //    return TMath::Sqrt( 2* pt1 * pt2 * ( 1.-TMath::Cos(ChargedHiggs::deltaPhi(phi1,phi2)) ) );
}


///https://github.com/cms-sw/cmssw/blob/09c3fce6626f70fd04223e7dacebf0b485f73f54/TopQuarkAnalysis/TopEventSelection/src/TtSemiLRSignalSelObservables.cc#L356
float ChargedHiggs::FW_momentum( const std::vector<TLorentzVector> jets, int index)
{

    float FW_momentum_0=0, FW_momentum_1=0, FW_momentum_2=0, FW_momentum_3=0, FW_momentum_4=0, FW_momentum_5=0, FW_momentum_6=0;

    double HT_alljets=0;
    for(unsigned int i=0;i<jets.size();i++)
        {
            HT_alljets += jets[i].Et();
        }

    for(unsigned int i=0;i<jets.size();i++)
        {
            for(unsigned int j=0;j<jets.size();j++)
                {
                    double ET_ij_over_ETSum2= jets[i].Et()*jets[j].Et()/(std::pow(HT_alljets,2));
                    double cosTheta_ij = (jets[i].Px()*jets[j].Px()+
                                          jets[i].Py()*jets[j].Py()+
                                          jets[i].Pz()*jets[j].Pz())/(jets[i].P()*jets[j].P());
                    FW_momentum_0 += ET_ij_over_ETSum2;
                    FW_momentum_1 += ET_ij_over_ETSum2 * cosTheta_ij;
                    FW_momentum_2 += ET_ij_over_ETSum2 * 0.5   * (  3*std::pow(cosTheta_ij,2)- 1);
                    FW_momentum_3 += ET_ij_over_ETSum2 * 0.5   * (  5*std::pow(cosTheta_ij,3)-  3*cosTheta_ij);
                    FW_momentum_4 += ET_ij_over_ETSum2 * 0.125 * ( 35*std::pow(cosTheta_ij,4)- 30*std::pow(cosTheta_ij,2)+3);
                    FW_momentum_5 += ET_ij_over_ETSum2 * 0.125 * ( 63*std::pow(cosTheta_ij,5)- 70*std::pow(cosTheta_ij,3)+15*cosTheta_ij);
                    FW_momentum_6 += ET_ij_over_ETSum2 * 0.0625* (231*std::pow(cosTheta_ij,6)-315*std::pow(cosTheta_ij,4)+105*std::pow(cosTheta_ij,2)-5);
                }
        }

    float FW_momentum[7] = {FW_momentum_0, FW_momentum_1, FW_momentum_2, FW_momentum_3, FW_momentum_4, FW_momentum_5, FW_momentum_6};

    if(index>6) return 0;
    return FW_momentum[index];

}


float ChargedHiggs::mt2( const TLorentzVector visa,  const TLorentzVector visb,  const TLorentzVector metVec)
{

    Davismt2 davismt2;

    // Format: M, px, py
    double visaVector[3] = { visa.M() , visa.Px(), visa.Py() };
    double visbVector[3] = { visb.M(), visb.Px(), visb.Py() };
    double metVector[3] = { 0, metVec.Px(), metVec.Py() };

    double mn    = 0.; // Neutrino mass

    davismt2.set_momenta(visaVector,visbVector,metVector);
    davismt2.set_mn(0);

    return davismt2.get_mt2();

}

#include "TLorentzVector.h"
namespace{
    double CosThetaCS(const TLorentzVector *v1, const TLorentzVector*v2, float sqrtS)
    {
        
        TLorentzVector m1s =*v1;  
        TLorentzVector m2s =*v2;  
        TLorentzVector mm= (*v1)+(*v2);
        m1s.Boost(-mm.BoostVector());
        m2s.Boost(-mm.BoostVector());
        double hmmthetas = m1s.CosTheta();
        double hmmphics   = m1s.DeltaPhi(mm);
        return hmmthetas;
    }

    double PhiCS(const TLorentzVector *v1, const TLorentzVector*v2, float sqrtS)
    {
        TLorentzVector m1s =*v1;  
        TLorentzVector m2s =*v2;  
        TLorentzVector mm= (*v1)+(*v2);
        m1s.Boost(-mm.BoostVector());
        m2s.Boost(-mm.BoostVector());
        double hmmthetas = m1s.CosTheta();
        double hmmphics   = m1s.DeltaPhi(mm);
        return hmmphics;
    }
}

double ChargedHiggs::CosThetaCS( const TLorentzVector *v1, const TLorentzVector *v2,float sqrtS){

        TLorentzVector b1,b2,v;
        b1.SetPx(0); b1.SetPy(0);
        b2.SetPx(0); b2.SetPy(0);
        double beamE = 500.*sqrtS; // 1/2 sqrtS in GeV
        b1.SetPz( std::hypot(beamE,ChargedHiggs::pMass)); b1.SetE(beamE);
        b2.SetPz(-std::hypot(beamE,ChargedHiggs::pMass)); b2.SetE(beamE);

        v=*v1+*v2;
        TVector3 boostToVFrame = -v.BoostVector();

        // Boost to higgs frame
        TLorentzVector refV_v1 = *v1; refV_v1.Boost(boostToVFrame);
        TLorentzVector refV_b1 = b1; refV_b1.Boost(boostToVFrame);
        TLorentzVector refV_b2 = b2; refV_b2.Boost(boostToVFrame);

        // Getting beam 3-vector from 4-vectors
        TVector3 refV_vb1_direction = refV_b1.Vect().Unit();
        TVector3 refV_vb2_direction = refV_b2.Vect().Unit();

        // Definition of zz directions
        TVector3 direction_cs = (refV_vb1_direction - refV_vb2_direction).Unit(); // CS direction

        return TMath::Cos(direction_cs.Angle(refV_v1.Vect()));

    }

double ChargedHiggs::PhiCS( const TLorentzVector *v1, const TLorentzVector *v2,float sqrtS){ // probably to debug more
    TLorentzVector b1,b2,v;
    b1.SetPx(0); b1.SetPy(0);
    b2.SetPx(0); b2.SetPy(0);
    double beamE = 500.*sqrtS; // 1/2 sqrtS in GeV
    b1.SetPz( std::hypot(beamE,ChargedHiggs::pMass)); b1.SetE(beamE);
    b2.SetPz(-std::hypot(beamE,ChargedHiggs::pMass)); b2.SetE(beamE);

    v=*v1+*v2;
    TVector3 boostToVFrame = -v.BoostVector();

    // Boost to higgs frame
    TLorentzVector refV_v1 = *v1; refV_v1.Boost(boostToVFrame);
    TLorentzVector refV_b1 = b1; refV_b1.Boost(boostToVFrame);
    TLorentzVector refV_b2 = b2; refV_b2.Boost(boostToVFrame);

    // Getting beam 3-vector from 4-vectors
    TVector3 refV_vb1_direction = refV_b1.Vect().Unit();
    TVector3 refV_vb2_direction = refV_b2.Vect().Unit();

    // Definition of zz directions
    TVector3 direction_cs = (refV_vb1_direction - refV_vb2_direction).Unit(); // CS direction
    TVector3 xAxis_cs = (refV_vb1_direction + refV_vb2_direction).Unit(); // CS direction //??
    TVector3 yAxis_cs = direction_cs.Cross(xAxis_cs);

    double phi=std::atan2(refV_v1.Vect()*yAxis_cs , refV_v1.Vect() *xAxis_cs) ;  // domain is -pi,pi

    //if(phi<0) return phi + 2*TMath::Pi(); // domain 0-pi
    return phi;
}


// this should be ~CosThetaStarCS in the center of mass
double ChargedHiggs::CosThetaStar(const TLorentzVector*v1, const TLorentzVector*v2)
{
    TLorentzVector v=*v1+*v2;
    const double deta= fabs(v1->Eta() - v2->Eta() );
    const double M=v.M();
    const double Pt = v.Pt();
    return fabs(std::sinh(deta)) / std::sqrt( 1+ std::pow(Pt/M,2)) * 2 * v1->Pt() * v2->Pt() / std::pow(M,2);
}

#include "interface/Lepton.hpp"
double ChargedHiggs::CosThetaCSPos(Lepton*a, Lepton*b){ 
    if (a->Charge() < 0) return ChargedHiggs::CosThetaCS(&a->GetP4(),&b->GetP4()) ;
    else return ChargedHiggs::CosThetaCS(&b->GetP4(),&a->GetP4());
}
double ChargedHiggs::PhiCSPos(Lepton*a, Lepton*b){ 
    if (a->Charge() < 0) return ChargedHiggs::PhiCS(&a->GetP4(),&b->GetP4()) ; 
    else return ChargedHiggs::PhiCS(&b->GetP4(),&a->GetP4());
}


// where is Form?
#include "TH1D.h"
// --------------- BINS ------------
int Binning::findBin(const std::vector<float> &v, float x)
{
    int R=-1; // Can be binary search
    for( int i=0; i<v.size() -1 ; ++i){
        if (v[i]<=x and x<v[i+1]) { R=i; break;}
    }
    return R;
}

std::string Binning::findBinStr(const std::vector<float> &v, float x, const char* precision)
{
    int bin= findBin(v,x);
    if (bin<0) return "NotFound";
    return Form(precision, v[bin],v[bin+1]);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
