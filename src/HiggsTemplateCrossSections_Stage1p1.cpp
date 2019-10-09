
#include "interface/HiggsTemplateCrossSections_Stage1p1.hpp"
#include "interface/Handlers.hpp"

using namespace std;
using namespace STXS;
// -*- C++ -*-

int STXS::getBin(double x, std::vector<double> bins) {
    for (size_t i=1;i<bins.size();++i)
        if (x<bins[i]) return i-1;
    return bins.size()-1;
}

/// @brief Return invariant mass of the two leading jets
double STXS::getMjj (const  std::vector<TLorentzVector> & jets)
{
    double mjj = jets.size()>1 ? (jets[0]+jets[1]).M():0;
    return mjj;
}

/// @brief get pt of Higgs plus jj system
double STXS::getPtHjj(const std::vector<TLorentzVector> &jets, const TLorentzVector &higgs) {
    if (jets.size() <2) return 0;
    //const FourMomentum &j1=jets[0].momentum(), &j2=jets[1].momentum();
    return (jets[0]+jets[1]+higgs).Pt();
}

int STXS::vbfTopology(const std::vector<TLorentzVector> &jets, const TLorentzVector &higgs) {
    if (jets.size()<2) return 0;
    TLorentzVector j1=jets[0], j2=jets[1];
    double mjj = (j1+j2).M();
    if(mjj>350 && mjj<=700) return ((j1+j2+higgs).Pt()<25) ? 1 : 2;
    else if(mjj>700) return ((j1+j2+higgs).Pt()<25) ? 3 : 4;
    else return 0;
}

/// @brief Whether the Higgs is produced in association with a vector boson (VH)
bool STXS::isVH(STXS::HiggsProdMode p) { return p==STXS::WH || p==STXS::QQ2ZH || p==STXS::GG2ZH; }


STXS::Stage1p1::Category STXS::getStage1p1Category(const STXS::HiggsProdMode prodMode,
        const TLorentzVector &higgs,
        const std::vector<TLorentzVector> &jets,
        const TLorentzVector &V, bool quarkDecayed) {
    using namespace STXS::Stage1p1;
    int Njets=jets.size(), ctrlHiggs = std::abs(higgs.Rapidity())<2.5, fwdHiggs = !ctrlHiggs;

    double mjj=getMjj(jets);
    double ptHjj= getPtHjj(jets, higgs);

    // 1. GGF Stage 1 categories
    if (prodMode==STXS::GGF || (prodMode==STXS::GG2ZH && quarkDecayed) ) {
        if (fwdHiggs)        return GG2H_FWDH;

        if (higgs.Pt() >= 200) return GG2H_PTH_GT200;
        if (Njets==0)
        {
            if(higgs.Pt() <10) return  GG2H_0J_PTH_0_10;
            else return GG2H_0J_PTH_GT10;
        }

        if (Njets==1)
        {    
            if (higgs.Pt()<60 ) return GG2H_1J_PTH_0_60;
            else if (higgs.Pt()<120) return GG2H_1J_PTH_60_120;
            else return GG2H_1J_PTH_120_200;
        }

        if (Njets >=2)
        {
            if (mjj <350){
                if (higgs.Pt()<60) return GG2H_GE2J_MJJ_0_350_PTH_0_60;
                else if (higgs.Pt()<120) return GG2H_GE2J_MJJ_0_350_PTH_60_120;
                else return GG2H_GE2J_MJJ_0_350_PTH_120_200;
            }

            if (mjj>=350 and mjj<700){
                if (ptHjj< 25) return  GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25;
                if (ptHjj >=25) return GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25;
            }
            if (mjj >=700){
                if (ptHjj< 25) return  GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25;
                if (ptHjj >=25) return GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25;
            }
        }

    } // ggh
    // 2. Electroweak qq->Hqq Stage 1 categories
    else if (prodMode==STXS::VBF || ( isVH(prodMode) && quarkDecayed) ) {
        if (std::abs(higgs.Rapidity())>2.5) return QQ2HQQ_FWDH;
        if (Njets==0) { return QQ2HQQ_0J;}
        if (Njets==1) { return QQ2HQQ_1J;}
        double mjj=getMjj(jets);
        double ptHjj= getPtHjj(jets, higgs);
        int vbfTopo = vbfTopology(jets,higgs);

        if (Njets>=2) {
            if ( mjj < 60 )      return QQ2HQQ_GE2J_MJJ_0_60;
            else if ( 60 < mjj and mjj < 120 ) return QQ2HQQ_GE2J_MJJ_60_120;
            else if ( 120 < mjj and mjj < 350 ) return QQ2HQQ_GE2J_MJJ_120_350;
            else if (  mjj > 350 ) {
                if (higgs.Pt()>200) return QQ2HQQ_GE2J_MJJ_GT350_PTH_GT200;
                if(vbfTopo) return Category(QQ2HQQ_GE2J_MJJ_GT350_PTH_GT200+vbfTopo);
            }
        }


    } // VBF
    // 3. WH->Hlv categories
    else if (prodMode==STXS::WH) {
        if (fwdHiggs) return QQ2HLNU_FWDH;
        else if (V.Pt()<75) return QQ2HLNU_PTV_0_75;
        else if (V.Pt()<150) return QQ2HLNU_PTV_75_150;
        else if (V.Pt()>250) return QQ2HLNU_PTV_GT250;
        // 150 < pTV/GeV < 250
        return jets.empty() ? QQ2HLNU_PTV_150_250_0J : QQ2HLNU_PTV_150_250_GE1J;
    }
    // 4. qq->ZH->llH categories
    else if (prodMode==STXS::QQ2ZH) {
        if (fwdHiggs) return QQ2HLL_FWDH;
        else if (V.Pt()<75) return QQ2HLL_PTV_0_75;
        else if (V.Pt()<150) return QQ2HLL_PTV_75_150;
        else if (V.Pt()>250) return QQ2HLL_PTV_GT250;
        // 150 < pTV/GeV < 250
        return jets.empty() ? QQ2HLL_PTV_150_250_0J : QQ2HLL_PTV_150_250_GE1J;
    }
    // 5. gg->ZH->llH categories
    else if (prodMode==STXS::GG2ZH ) {
        if (fwdHiggs) return GG2HLL_FWDH;
        if      (V.Pt()<75) return GG2HLL_PTV_0_75;
        else if (V.Pt()<150) return GG2HLL_PTV_75_150;
        else if (V.Pt()<250){
            if (jets.empty()) return GG2HLL_PTV_150_250_0J;
            else return GG2HLL_PTV_150_250_GE1J;
        }
        else return GG2HLL_PTV_GT250;
    }
    // 6.ttH,bbH,tH categories
    else if (prodMode==STXS::TTH) return Category(TTH_FWDH+ctrlHiggs);
    else if (prodMode==STXS::BBH) return Category(BBH_FWDH+ctrlHiggs);
    else if (prodMode==STXS::TH ) return Category(TH_FWDH+ctrlHiggs);
    return STXS::Stage1p1::UNKNOWN;
}

// experimental! -> and will not work unless with the same name
#define STXSString(name) std::string(#name)
using namespace STXS;
using namespace std;
#include <iostream>
std::map<int,std::string>  STXS::stxs_to_string;

std::string STXS::ENumToString(STXS::Stage1p1::Category x)
{
    if (STXS::stxs_to_string.find(int(x))!= STXS::stxs_to_string.end())
    { return STXS::stxs_to_string[int(x)] ;}
    
    STXS::stxs_to_string[0] = "UNKNOWN";
    //STXSString(name)
    //STXS::stxs_to_string[int()]=STXSString();
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_FWDH                                 )]=STXSString(GG2H_FWDH                                 );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_PTH_GT200                            )]=STXSString(GG2H_PTH_GT200                            );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_0J_PTH_0_10                          )]=STXSString(GG2H_0J_PTH_0_10                          );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_0J_PTH_GT10                          )]=STXSString(GG2H_0J_PTH_GT10                          );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_1J_PTH_0_60                          )]=STXSString(GG2H_1J_PTH_0_60                          );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_1J_PTH_60_120                        )]=STXSString(GG2H_1J_PTH_60_120                        );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_1J_PTH_120_200                       )]=STXSString(GG2H_1J_PTH_120_200                       );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_GE2J_MJJ_0_350_PTH_0_60              )]=STXSString(GG2H_GE2J_MJJ_0_350_PTH_0_60              );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_GE2J_MJJ_0_350_PTH_60_120            )]=STXSString(GG2H_GE2J_MJJ_0_350_PTH_60_120            );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_GE2J_MJJ_0_350_PTH_120_200           )]=STXSString(GG2H_GE2J_MJJ_0_350_PTH_120_200           );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25)]=STXSString(GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25)]=STXSString(GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25  )]=STXSString(GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25  );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25  )]=STXSString(GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25  );

    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_FWDH                                 )]=STXSString(QQ2HQQ_FWDH                                 );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_0J                                   )]=STXSString(QQ2HQQ_0J                                   );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_1J                                   )]=STXSString(QQ2HQQ_1J                                   );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_0_60                        )]=STXSString(QQ2HQQ_GE2J_MJJ_0_60                        );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_60_120                      )]=STXSString(QQ2HQQ_GE2J_MJJ_60_120                      );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_120_350                     )]=STXSString(QQ2HQQ_GE2J_MJJ_120_350                     );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_GT350_PTH_GT200             )]=STXSString(QQ2HQQ_GE2J_MJJ_GT350_PTH_GT200             );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25)]=STXSString(QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25)]=STXSString(QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25  )]=STXSString(QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25  );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25  )]=STXSString(QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25  );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLNU_FWDH)]=STXSString(QQ2HLNU_FWDH);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLNU_PTV_0_75)]=STXSString(QQ2HLNU_PTV_0_75);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLNU_PTV_75_150)]=STXSString(QQ2HLNU_PTV_75_150);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLNU_PTV_150_250_0J)]=STXSString(QQ2HLNU_PTV_150_250_0J);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLNU_PTV_150_250_GE1J)]=STXSString(QQ2HLNU_PTV_150_250_GE1J);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLNU_PTV_GT250)]=STXSString(QQ2HLNU_PTV_GT250);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLL_FWDH)]=STXSString(QQ2HLL_FWDH);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLL_PTV_0_75)]=STXSString(QQ2HLL_PTV_0_75);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLL_PTV_75_150)]=STXSString(QQ2HLL_PTV_75_150);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLL_PTV_150_250_0J)]=STXSString(QQ2HLL_PTV_150_250_0J);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLL_PTV_150_250_GE1J)]=STXSString(QQ2HLL_PTV_150_250_GE1J);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::QQ2HLL_PTV_GT250)]=STXSString(QQ2HLL_PTV_GT250);

    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2HLL_FWDH            )]=STXSString(GG2HLL_FWDH            );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2HLL_PTV_0_75        )]=STXSString(GG2HLL_PTV_0_75        );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2HLL_PTV_75_150      )]=STXSString(GG2HLL_PTV_75_150      );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2HLL_PTV_150_250_0J  )]=STXSString(GG2HLL_PTV_150_250_0J  );
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2HLL_PTV_150_250_GE1J)]=STXSString(GG2HLL_PTV_150_250_GE1J);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::GG2HLL_PTV_GT250       )]=STXSString(GG2HLL_PTV_GT250       );

    STXS::stxs_to_string[int(STXS::Stage1p1::Category::TTH_FWDH)]=STXSString(TTH_FWDH);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::TTH)]=STXSString(TTH);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::BBH_FWDH)]=STXSString(BBH_FWDH);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::BBH)]=STXSString(BBH);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::TH_FWDH)]=STXSString(TH_FWDH);
    STXS::stxs_to_string[int(STXS::Stage1p1::Category::TH)]=STXSString(TH);

    if (STXS::stxs_to_string.find(int(x))!= STXS::stxs_to_string.end())
    { return STXS::stxs_to_string[int(x)] ;}

    std::cout<<"ERROR: Unable to convert to string STXS::bin"<<x<<std::endl;
    throw abortException();
}




