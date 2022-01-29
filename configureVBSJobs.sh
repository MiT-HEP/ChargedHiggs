#!/bin/bash

DIR="/eos/user/d/dalfonso/AnalysisVBS/NANO/JAN26"

#SYST='Smear=NONE,PU,@SmearUncluster(),@SmearL1Prefire,@SmearJesSourceAK8("aux/JEC/Nano/Summer19UL16_V7_MC_UncertaintySources_AK8PFPuppi.txt"!"Total"),@SmearJesSource("aux/JEC/Nano/Summer19UL16_V7_MC_UncertaintySources_AK4PFchs.txt"!"Total")'

SYST=''

for year in "2016" "2016APV" "2017" "2018";
do

for type in "HADside" "BHADside" "METside" "HADanti" "BHADanti" "METanti" "HAD" "BHAD" "MET" "HADantiSide" "BHADantiSide";
do

echo " --- Submitting $type $year --- "

TMPDATFILE="dat/tmp_${type}_${year}.dat"
rm -v $TMPDATFILE
echo "-> writing temporary dat in $TMPDATFILE"
echo "## --- automatically generated. Do not edit or commit ">>$TMPDATFILE
echo "include=dat/configVBShad.nanov9.${year}.dat">>$TMPDATFILE

#echo $SYST>>$TMPDATFILE
#echo "Final=yes">>$TMPDATFILE

[ "$type" == "HAD" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=1">>$TMPDATFILE ; echo "Output=VBShadSignal_HAD.root">> $TMPDATFILE ; }
[ "$type" == "BHAD" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=1,doHADAnalysis=0" >>$TMPDATFILE; echo "Output=VBShadSignal_B.root">>$TMPDATFILE ; }
[ "$type" == "MET" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=1,doBAnalysis=0,doHADAnalysis=0">>$TMPDATFILE ; echo "Output=VBShadSignal_MET.root">> $TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/Zinv_local.dat">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/Wjets_local.dat">>$TMPDATFILE ;}

[ "$type" == "HADside" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=1,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadSignal_side_HAD.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/jetHT.dat">>$TMPDATFILE ;}
[ "$type" == "BHADside" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=1,doHADAnalysis=0,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadSignal_side_B.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/jetHT.dat">>$TMPDATFILE ; }
[ "$type" == "METside" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=1,doBAnalysis=0,doHADAnalysis=0,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadSignal_side_MET.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/MET.dat">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/Zinv_local.dat">>$TMPDATFILE ;  echo "include=dat/catalog/2020.moriond/nanov9/year${year}/Wjets_local.dat">>$TMPDATFILE ;}

[ "$type" == "HADanti" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=1,doBAntiAnalysis=0">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_HAD.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/jetHT.dat">>$TMPDATFILE ; } 
[ "$type" == "BHADanti" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=0,doBAntiAnalysis=1">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_BHAD.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/jetHT.dat">>$TMPDATFILE ; } 
[ "$type" == "METanti" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doMETAntiAnalysis=1,doBAntiAnalysis=0">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_MET.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/MET.dat">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/Zinv_local.dat">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/Wjets_local.dat">>$TMPDATFILE ;}

[ "$type" == "HADantiSide" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=1,doBAntiAnalysis=0,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_HADside.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/jetHT.dat">>$TMPDATFILE ; } 
[ "$type" == "BHADantiSide" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=0,doBAntiAnalysis=1,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_BHADside.root">>$TMPDATFILE ; echo "include=dat/catalog/2020.moriond/nanov9/year${year}/jetHT.dat">>$TMPDATFILE ; } 

## only when running on aQGC
#[ "$type" == "MET" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=1,doBAnalysis=0,doHADAnalysis=0">>$TMPDATFILE ; echo "Output=VBShadSignal_MET.root">> $TMPDATFILE ; }

#[ "$type" == "HADside" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=1,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadSignal_side_HAD.root">>$TMPDATFILE ; }
#[ "$type" == "BHADside" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=1,doHADAnalysis=0,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadSignal_side_B.root">>$TMPDATFILE ; }
#[ "$type" == "METside" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=1,doBAnalysis=0,doHADAnalysis=0,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadSignal_side_MET.root">>$TMPDATFILE ; }

#[ "$type" == "HADanti" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=1,doBAntiAnalysis=0">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_HAD.root">>$TMPDATFILE ; }
#[ "$type" == "BHADanti" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=0,doBAntiAnalysis=1">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_BHAD.root">>$TMPDATFILE ; }
#[ "$type" == "METanti" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doMETAntiAnalysis=1,doBAntiAnalysis=0">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_MET.root">>$TMPDATFILE ; }

#[ "$type" == "HADantiSide" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=1,doBAntiAnalysis=0,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_HADside.root">>$TMPDATFILE ; } 
#[ "$type" == "BHADantiSide" ] && { echo "addConfig=VBShadAnalysis|doMETAnalysis=0,doBAnalysis=0,doHADAnalysis=0,doHADAntiAnalysis=0,doBAntiAnalysis=1,doSideBand=1">>$TMPDATFILE ; echo "Output=VBShadAntiSignal_BHADside.root">>$TMPDATFILE ; }

python script/submit.py --local --aaa --proxy --condor -i $TMPDATFILE -v 3 -n 500 -q 8nh -d $DIR/UL$year/$type
#python script/submit.py --local --aaa --proxy --condor -i $TMPDATFILE -v 3 -n 200 -q 8nh -d $DIR/UL$year/$type --dryrun

done

done
