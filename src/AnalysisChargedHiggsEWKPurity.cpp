#include "interface/AnalysisChargedHiggsEWKPurity.hpp"
#include "interface/GeneralFunctions.hpp"
#include "interface/CutSelector.hpp"
#include "interface/AnalysisChargedHiggsTauNu.hpp"
#include <string>

#ifdef HAVE_PYTHIA
    #include "Pythia8/Pythia.h"
    #include "TF1.h"
#endif

void ChargedHiggsEWKPurity::Init()
{
    // -- simplified selection
    for ( string& l : AllLabel()  ) 
    {
            string hist= "EtMiss_Full";
            Book( dir + hist+"_"+ l  , ("EtMissFull"+ l).c_str(),1000,0.,1000.); 
            hist= "EtMiss_Loose";
            Book( dir + hist+"_"+ l  , ("EtMissLoose"+ l).c_str(),1000,0.,1000.); 

            hist= "Recoil_Full";
            Book( dir + hist+"_"+ l  , ("RecoilFull"+ l).c_str(),1000,0.,1000.); 

            hist= "HT_Full";
            Book( dir + hist+"_"+ l  , ("HTFull"+ l).c_str(),1000,0.,1000.); 

            hist= "Mt";
            Book( dir + hist+"_"+ l  , ("Mt"+ l).c_str(),1000,0.,1000.); 

            hist= "Mt_Embed";
            Book( dir + hist+"_"+ l  , ("MtEmbed"+ l).c_str(),1000,0.,1000.); 
    }

    if (doPythia)
    {
        #ifdef HAVE_PYTHIA 

        // create objects
        fMasterGen.reset(new Pythia8::Pythia);
        random.reset(new TRandom3);
        //randomEngine.reset(new CLHEP::HepRandomEngine);
        //
        fMasterGen->settings.addFlag("ResonanceDecayFilter:filter",false);
        fMasterGen->settings.addFlag("ResonanceDecayFilter:exclusive",false);
        fMasterGen->settings.addFlag("ResonanceDecayFilter:eMuAsEquivalent",false);
        fMasterGen->settings.addFlag("ResonanceDecayFilter:eMuTauAsEquivalent",false);
        fMasterGen->settings.addFlag("ResonanceDecayFilter:allNuAsEquivalent",false);
        fMasterGen->settings.addMVec("ResonanceDecayFilter:mothers",std::vector<int>(),false,false,0,0);
        fMasterGen->settings.addMVec("ResonanceDecayFilter:daughters",std::vector<int>(),false,false,0,0);  
        //fMasterGen->setRndmEnginePtr( randomEngine.get() ); 
        fMasterGen->readString(Form("Random:seed=%u",random->Integer(999999999))); 
        fMasterGen->readString("ParticleDecays:sophisticatedTau = 2");
        fMasterGen->readString("ParticleDecays:tauPolarization = 0");
        fMasterGen->readString("15:onMode = on"); // all taus !! -> eff
        //fMasterGen->readString("");
        fMasterGen->readString("Next:numberShowEvent = 0"); 
        #else
            Log(__FUNCTION__,"ERROR","Cannot do pythia w/o CMSSW");
            throw abort;
        #endif
    }
}

int ChargedHiggsEWKPurity::analyze(Event*e,string systname)
{
    string label = GetLabel(e);
    e->ApplyTopReweight();
    e->ApplyWReweight();
    CutSelector muon; 
    muon.SetMask(ChargedHiggsTauNu::MaxCut-1);
    muon.SetCut(ChargedHiggsTauNu::Selection(e,true,true));
    bool passLoose=muon.passAllUpTo(ChargedHiggsTauNu::ThreeJets);
    if ( not passLoose) return EVENT_NOT_USED;
    if (not e->IsRealData()) e->ApplyBTagSF(0);// 0=loos wp

    Fill( dir + "EtMiss_Loose_" + label,systname,e->GetMet().Pt(),e->weight() );

    if (muon.passAllExcept(ChargedHiggsTauNu::Met) ) 
    {
    	Fill( dir + "EtMiss_Full_" + label,systname,e->GetMet().Pt(),e->weight() );
    	Fill( dir + "HT_Full_" + label,systname,e->Ht(),e->weight() );
        TLorentzVector r (e->GetMuon(0)->GetP4() );
        r += e->GetMet().GetP4();
    	Fill( dir + "Recoil_Full_" + label,systname,r.Pt(),e->weight() );
    
    }

    if (muon.passAll() ) 
    {
    
    	Fill( dir + "Mt_" + label,systname,e->Mt(Event::MtMuon),e->weight() );
        if ( doPythia )
        {
        #ifdef HAVE_PYTHIA
            fMasterGen->event.reset();
            int particleID = -15 * e->GetMuon(0)->Charge(); 
            double phi = e->GetMuon(0)->Phi();
            double eta = e->GetMuon(0)->Eta();
            double pt  = e->GetMuon(0)->Pt();
            double mass = (fMasterGen->particleData).m0( particleID );
            double the = 2.*atan(exp(-eta));

            double pp = pt / sin(the); // sqrt( ee*ee - mass*mass )
            double ee = sqrt( pp*pp + mass*mass );
      
            double px = pt * cos(phi);
            double py = pt * sin(phi);
            double pz = pp * cos(the);

            (fMasterGen->event).append( particleID, 1, 0, 0, px, py, pz, ee, mass );
            int eventSize = (fMasterGen->event).size()-1;
            double tauTmp = -(fMasterGen->event)[eventSize].tau0() * log(random->Uniform());
            (fMasterGen->event)[eventSize].tau( tauTmp );

            if ( !fMasterGen->next() ) {
                Log(__FUNCTION__,"WARNING","Failed to generate event");
                return 0; // GENERATE EVENT
            }  

            TLorentzVector tau;
            TLorentzVector nu;
            for (unsigned i=0; i<fMasterGen->event.size() ;++i)
            {
                const auto & p = fMasterGen->event.at(i);
                if ( p.status() != 1) continue;
                TLorentzVector myp;
                myp.SetPxPyPzE( p.px(), p.py(), p.pz(), p.e());
                if ( p.idAbs() ==11 or p.idAbs()==13 ){
                    return 0; // it's a muon/ele tau!
                }

                if ( p.idAbs() ==12 or p.idAbs()==14 or p.idAbs()==16){
                    nu += myp;  
                    }
                else{
                    tau += myp;
                    }
            }

            // ------ SORT OF FAST SIM ----
            // tau is my gen tau vis / nu is  to be added  to the met
            // [0]* ( (1-[3]) * TMath::Gaus(x,[1],[2]) + [3]*TMath::Gaus(x,[4],[5]))
            //
            //   1  p0           4.81151e+03   2.92245e+01   1.02769e-04  -2.43784e-03
            //   2  p1           9.90412e-01   1.42608e-04   2.76938e-05  -5.21294e-02
            //   3  p2           2.70180e-02   2.06939e-04   4.53012e-05  -3.73509e-02
            //   4  p3           9.13808e-02   2.78106e-03   1.31412e-04  -3.43633e-02
            //   5  p4           9.56298e-01   1.10600e-03   1.01067e-04   3.95337e-03
            //   6  p5           9.66168e-02   1.42866e-03   1.03578e-04   3.85831e-03
            //
            //
            TF1 myfunc("myfunc","[0]* ( (1-[3]) * TMath::Gaus(x,[1],[2]) + [3]*TMath::Gaus(x,[4],[5]))",0.8,1.2);
            myfunc.SetParameter(0,4.81151e+03);
            myfunc.SetParameter(1,9.90412e-01);
            myfunc.SetParameter(2,2.70180e-02);
            myfunc.SetParameter(3,9.13808e-02);
            myfunc.SetParameter(4,9.56298e-01);
            myfunc.SetParameter(5,9.66168e-02);
            double smear=myfunc.GetRandom();
            //double ptNew=random->Gaus(tau.Pt(),tau.Pt()*0.05); 
            double ptNew=smear*tau.Pt();

            TLorentzVector tauReco(tau);
            TLorentzVector metReco(tau);

            tauReco *= ptNew/tau.Pt();
            metReco *= (1.-ptNew/tau.Pt() ); // the mis reco part will go into fake met
           
            metReco += nu;  //add neutrinos
            metReco += e->GetMet().GetP4(); // add real met in the event

            //-----
            double mt=ChargedHiggs::mt(tauReco.Pt(),metReco.Pt(),tauReco.Phi(),metReco.Phi());
    	    Fill( dir + "Mt_Embed_" + label,systname,mt,e->weight() );

        #else
            Log(__FUNCTION__,"ERROR","Cannot do pythia w/o CMSSW");
            throw abort;
        #endif
        
        }
    }
    return EVENT_NOT_USED;

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
