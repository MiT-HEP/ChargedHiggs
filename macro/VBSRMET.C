/*
 * root -l -q -b VBSRMET.C+'("MET.root", "METanti.root")'
 */

#include <TROOT.h>
#include <TMVA/DataLoader.h>
#include <TMVA/Factory.h>
#include <TMVA/Types.h>
#include <TFile.h>
#include <TCut.h>
#include <TTree.h>
#include <TString.h>

void VBSRMET(
  TString inputFileName1,
  TString inputFileName2, 
  TString extraString="" 
) {
  gROOT->ProcessLine("TMVA::gConfig().GetVariablePlotting().fMaxNumOfAllowedVariablesForScatterPlots = 50");
  TFile *output_file;
  TMVA::Factory *factory;
  
  // Determine the input trees
  TFile *inputFile1 = TFile::Open(inputFileName1,"READ");
  TFile *inputFile2 = TFile::Open(inputFileName2,"READ");
  TTree *mvaTree1 = (TTree*)inputFile1->Get("tree_vbs");
  TTree *mvaTree2 = (TTree*)inputFile2->Get("tree_vbs");
  
  // Initialize the factory
  TString trainName="BDT_VBSRMET";
  output_file=TFile::Open(trainName+".root", "RECREATE");
  //factory = new TMVA::Factory("bdt", output_file, "!V:!Silent:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Multiclass");
  TString factoryOptions="!V:!Silent:DrawProgressBar";
  //TString factoryOptions="!V:!Silent:!DrawProgressBar";

  
  //else              factoryOptions += ":Transformations=I";
  factory = new TMVA::Factory("bdt", output_file, factoryOptions);
  TMVA::DataLoader *dataloader=new TMVA::DataLoader("MitRMETAnalysis");
  
    //TCut cutTrainSignal1 = "ana_category == 4 && (mc == 4 && bosGen == 1) && evt % 2 != 1";
    //TCut cutTrainSignal2 = "ana_category == 4 && (mc == 3 && bosGen == 1) && evt % 2 != 1";
    TCut cutTrainSignal = "ana_category ==4 && ((mc ==3 || mc == 4) && bosGen == 1) && evt % 2 != 1";
    TCut cutTrainBkg    = "ana_category == 4 && (mc > 50 && mc!=202 && mc <500) && evt % 2 != 1";
    TCut cutTestSignal1  = "ana_category == 4 && (mc == 3 && bosGen ==1) && evt % 2 == 1";
    //TCut cutTestSignal2  = "ana_category == 4 && (mc == 4 && bosGen ==1) && evt % 2 == 1";
    TCut cutTestBkg     = "ana_category == 4 && (mc > 50 && mc!=202 && mc <500) && evt % 2 == 1";
    
    TCut cutTrainQCDBkg   = "ana_category == 4 && mc == 500 && evt % 2 != 1";
    TCut cutTestQCDBkg    = "ana_category == 4 && mc == 500 && evt % 2 == 1";
    
    dataloader->AddTree(mvaTree1, "Background", 1.0, cutTrainBkg   , "train");
    //dataloader->AddTree(mvaTree1, "Signal"    , 1.0, cutTrainSignal1, "train");
    //dataloader->AddTree(mvaTree1, "Signal"    , 1.0, cutTrainSignal2, "train");
    dataloader->AddTree(mvaTree1, "Signal"    , 1.0, cutTrainSignal, "train");
    dataloader->AddTree(mvaTree1, "Background", 1.0, cutTestBkg   , "test");
    dataloader->AddTree(mvaTree1, "Signal"    , 1.0, cutTestSignal1, "test");
    //dataloader->AddTree(mvaTree1, "Signal"    , 1.0, cutTestSignal2, "test");
    dataloader->AddTree(mvaTree2, "Background", 1.0, cutTrainQCDBkg, "train");
    dataloader->AddTree(mvaTree2, "Background", 1.0, cutTestQCDBkg, "test");
 

    dataloader->SetWeightExpression("abs(weight)", "Signal");
    dataloader->SetWeightExpression("abs(weight*(.0042*(mc == 500)+1*(mc != 500)))", "Background");
 
 
    dataloader->AddVariable("varMjj"    ,"varMjj"    , "", 'F');
    dataloader->AddVariable("varDetajj"    ,"varDetajj"    , "", 'F');
    //dataloader->AddVariable("abs(varDphijj)"    ,"abs(varDphijj)"    , "", 'F');
    //dataloader->AddVariable("abs(varJet2Eta)"    ,"abs(varJet2Eta)"    , "", 'F');
    dataloader->AddVariable("varJet2Pt"    ,"varJet2Pt"    , "", 'F');
    //dataloader->AddVariable("varMVVgen"    ,"varMVVgen"    , "", 'F');
    dataloader->AddVariable("varMVV"    ,"varMVV"    , "", 'F');
    //dataloader->AddVariable("varPTVV"    ,"varPTVV"    , "", 'F');
    //dataloader->AddVariable("varPTV1"    ,"varPTV1"    , "", 'F');
    dataloader->AddVariable("varPTV2"    ,"varPTV2"    , "", 'F');
    //dataloader->AddVariable("varDetaVV"    ,"varDetaVV"    , "", 'F');
    //dataloader->AddVariable("varPetaVV"    ,"varPetaVV"    , "", 'F');
    //dataloader->AddVariable("varEtaMinV"    ,"varEtaMinV"    , "", 'F');
    //dataloader->AddVariable("varEtaMaxV"    ,"varEtaMaxV"    , "", 'F');
    //dataloader->AddVariable("varCen"    ,"varCen"    , "", 'F');
    //dataloader->AddVariable("varcenPtVVjj"    ,"varcenPtVVjj"    , "", 'F');
    dataloader->AddVariable("varzepVB"    ,"varzepVB"    , "", 'F');
    //dataloader->AddVariable("varzepVV"    ,"varzepVV"    , "", 'F');
    //dataloader->AddVariable("varDRVj"    ,"varDRVj"    , "", 'F');
    dataloader->AddVariable("varnormPTVVjj"    ,"varnormPTVVjj"    , "", 'F');
    dataloader->AddVariable("bosV2chi2"    ,"bosV2chi2"    , "", 'F');
    dataloader->AddVariable("bosV2mass"    ,"bosV2mass"    , "", 'F');
    dataloader->AddVariable("varFW2j"    ,"varFW2j"    , "", 'F');
    
  
  TString prepareOptions="NormMode=None";
    prepareOptions+=":SplitMode=Block"; // use e.g. all events selected by trainTreeEventSplitStr for training
    prepareOptions+=":MixMode=Random";
  dataloader->PrepareTrainingAndTestTree("", prepareOptions);
  
  // for resolved
  //TString hyperparameters=
  //"!H:!V:BoostType=AdaBoost:MinNodeSize=5%:NegWeightTreatment=IgnoreNegWeightsInTraining:SeparationType=MisClassificationError:NTrees=500:MaxDepth=3:AdaBoostBeta=0.12:nCuts=10000";

  //TString hyperparameters="!H:!V:NTrees=500:MinNodeSize=5%:MaxDepth=3:BoostType=Grad:Shrinkage=0.1:nCuts=30:PruneMethod=CostComplexity";
  TString hyperparameters="!H:!V:NTrees=1200:NegWeightTreatment=Pray:MinNodeSize=4%:MaxDepth=3:BoostType=Grad:Shrinkage=0.01:nCuts=30";
  // for boosted
  //TString hyperparameters="!H:!V:NTrees=1000:NegWeightTreatment=Pray:SeparationType=MisClassificationError:MinNodeSize=5%:MaxDepth=2:BoostType=Grad:Shrinkage=0.05:nCuts=1000";
  //TString hyperparameters="!H:!V:NTrees=1000:NegWeightTreatment=Pray:SeparationType=MisClassificationError:MinNodeSize=5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.12:nCuts=1000";
  //if(useGaussDeco) hyperparameters += ":VarTransform=G,D";
  factory->BookMethod(dataloader, TMVA::Types::kBDT, trainName, hyperparameters);
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  output_file->Close();
}
