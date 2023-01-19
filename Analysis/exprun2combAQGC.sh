#!/bin/sh
year=$1
quote=$2
fitr=$3
carddir=$4

echo 'carddir='$carddir


UPdir='/afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_10_2_13/src/ChargedHiggs/Datacards/AUG12_interpolate'
carddir='AQGC'
fitr="exp"
year='2021'

#quote='5_aqgc_fm2'
#string='0p40'

quote='5_aqgc_ft7'
string='0p40'

echo "-- AQGC --"
echo "start t2w"

cat $UPdir/cms_vbshad_$year\_$quote\_MVV_BB_SR.txt | sed -e s/'$VALUE'/$string/g > $carddir/cms_vbshad_$year\_$quote\_MVV_BB_SR.txt
cat $UPdir/cms_vbshad_$year\_$quote\_MVV_BB_side.txt | sed -e s/'$VALUE'/$string/g > $carddir/cms_vbshad_$year\_$quote\_MVV_BB_side.txt
cat $UPdir/cms_vbshad_$year\_$quote\_MVV_BB_anti.txt | sed -e s/'$VALUE'/$string/g > $carddir/cms_vbshad_$year\_$quote\_MVV_BB_anti.txt

cp $UPdir/cms_vbshad_$year\_$quote\_MVV_BB_SR.inputs.root $carddir
cp $UPdir/cms_vbshad_$year\_$quote\_MVV_BB_side.inputs.root $carddir
cp $UPdir/cms_vbshad_$year\_$quote\_MVV_BB_anti.inputs.root $carddir


cd $carddir

combineCards.py cms_vbshad_$year\_$quote\_MVV_BB_SR.txt cms_vbshad_$year\_$quote\_MVV_BB_side.txt cms_vbshad_$year\_$quote\_MVV_BB_anti.txt > cms_vbshad_$year\_$quote\_MVV_BB_all.txt

cd ../ 

### comb region ###
text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VV_AQGC_EWK:r_s0[1,0,5]' --keyword-value VALUE=0p00 $carddir/cms_vbshad_$year\_$quote\_MVV_BB_all.txt -o $carddir/workspace_$year\_$quote\_MVV_BB_all.root

echo "done t2w"

echo "significance"


if [ "$fitr" = "exp" ]; then 
	### comb region ###
	echo "BB all - exp"
	combine $carddir/workspace_$year\_$quote\_MVV_BB_all.root -M Significance -n BB_significance_MVV_BB_all_VVEWK --setParameters r_s0=1 -t -1 

else
	echo "run unb CR postfit later"
	### unb CR comb
	#combine $carddir/workspace_$year\_DNN_comb_CR_VVEWK.root -M Significance -n comb_significance_DNN_CR_VVEWK
fi;

echo "done significance"


echo "FitDiagnostics"

if [ "$fitr" = "exp" ]; then
	echo "BB all - exp"
	combine -M FitDiagnostics -t -1 $carddir/workspace_$year\_$quote\_MVV_BB_all.root  --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations  --setParameters r_s0=1 --rMin -5 --rMax 5 -n final_$year\_$quote\_MVV_BB_all
#	combine -M FitDiagnostics -t -1 $carddir/workspace_$year\_DNN_comb_all_VVEWK.root --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations  --setParameters r_s0=1 --rMin -5 --rMax 5 -n final_$year\_DNN_comb_all_VVEWK
else
	combine -M FitDiagnostics $carddir/workspace_$year\_DNN_comb_CR_VVEWK.root --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations --rMin 0 --rMax 5 -n final_$year\_DNN_comb_CR_unb_VVEWK --cminDefaultMinimizerStrategy 1 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.5
fi;

echo "done FitDiagnostics"
