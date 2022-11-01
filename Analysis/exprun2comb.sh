#!/bin/sh
year=$1
quote=$2
fitr=$3
carddir=$4

echo 'carddir='$carddir


### datacards
echo "produce cards"
echo "BB"
echo "SR"
python ../script/bwsVBSHad.py -c "BB" -s "BDTnoBnoMET" -i "UL"$year"/HAD/HAD.root" -y $year -q $quote -r SR
echo "anti"
python ../script/bwsVBSHad.py -c "BB" -s "BDTnoBnoMET" -i "UL"$year"/HADanti/HADanti.root" -y $year -q $quote -r anti
echo "side"
python ../script/bwsVBSHad.py -c "BB" -s "BDTnoBnoMET" -i "UL"$year"/HADside/HADside.root" -y $year -q $quote -r side

echo "BBtag"
echo "SR"
python ../script/bwsVBSHad.py -c "BBtag" -s "BDTbtag" -i "UL"$year"/BHAD/BHAD.root" -y $year -q $quote -r SR
echo "anti"
python ../script/bwsVBSHad.py -c "BBtag" -s "BDTbtag" -i "UL"$year"/BHADanti/BHADanti.root" -y $year -q $quote -r anti
echo "side"
python ../script/bwsVBSHad.py -c "BBtag" -s "BDTbtag" -i "UL"$year"/BHADside/BHADside.root" -y $year -q $quote -r side

echo "BMET"
echo "SR"
python ../script/bwsVBSHad.py -c "BMET" -s "BDTwithMET" -i "UL"$year"/MET/MET.root" -y $year -q $quote -r SR
echo "side"
python ../script/bwsVBSHad.py -c "BMET" -s "BDTwithMET" -i "UL"$year"/METside/METside.root" -y $year -q $quote -r side
echo "RMET"
echo "SR"
python ../script/bwsVBSHad.py -c "RMET" -s "BDTwithMET" -i "UL"$year"/MET/MET.root" -y $year -q $quote -r SR
echo "side"
python ../script/bwsVBSHad.py -c "RMET" -s "BDTwithMET" -i "UL"$year"/METside/METside.root" -y $year -q $quote -r side

#echo "antiside"
#python script/bwsVBSHad.py -c "$cat" -s "$var" -i "UL"$year"/METantiSide/METantiSide.root" -y $year -q $quote -r antiSide

echo "combine card"

### SR ###
#text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_$var\_$cat\_SR.txt -o $carddir/workspace_$year\_$var\_$cat\_SR_VVEWK.root

### side ###
#text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_$var\_$cat\_side.txt -o $carddir/workspace_$year\_$var\_$cat\_side_VVEWK.root

### anti ###
#text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_$var\_$cat\_anti.txt -o $carddir/workspace_$year\_$var\_$cat\_anti_VVEWK.root

cd $carddir

### combine card
### SR + anti + side ### 
## BB ##
combineCards.py cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_SR.txt cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_side.txt cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_anti.txt > cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_all.txt
## BBtag ##
combineCards.py cms_vbshad_$year\_$quote\_BDTbtag_BBtag_SR.txt cms_vbshad_$year\_$quote\_BDTbtag_BBtag_side.txt cms_vbshad_$year\_$quote\_BDTbtag_BBtag_anti.txt > cms_vbshad_$year\_$quote\_BDTbtag_BBtag_all.txt

### SR + side ###
## BMET ##
combineCards.py cms_vbshad_$year\_$quote\_BDTwithMET_BMET_SR.txt cms_vbshad_$year\_$quote\_BDTwithMET_BMET_side.txt > cms_vbshad_$year\_$quote\_BDTwithMET_BMET_all.txt
## RMET ##
combineCards.py cms_vbshad_$year\_$quote\_BDTwithMET_RMET_SR.txt cms_vbshad_$year\_$quote\_BDTwithMET_RMET_side.txt > cms_vbshad_$year\_$quote\_BDTwithMET_RMET_all.txt

### SR only ###
#combineCards.py cms_vbshad_$year\_$quote\_$var\_$cat\_SR.txt  > cms_vbshad_$year\_$quote\_$var\_$cat\_all.txt

### comb all cate ###
### exp all ###
combineCards.py cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_all.txt cms_vbshad_$year\_$quote\_BDTwithMET_BMET_all.txt cms_vbshad_$year\_$quote\_BDTwithMET_RMET_all.txt cms_vbshad_$year\_$quote\_BDTbtag_BBtag_all.txt > cms_vbshad_$year\_$quote\_DNN_comb_all.txt
### unb CR comb
combineCards.py cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_side.txt cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_anti.txt cms_vbshad_$year\_$quote\_BDTwithMET_BMET_side.txt cms_vbshad_$year\_$quote\_BDTwithMET_RMET_side.txt cms_vbshad_$year\_$quote\_BDTbtag_BBtag_side.txt cms_vbshad_$year\_$quote\_BDTbtag_BBtag_anti.txt > cms_vbshad_$year\_$quote\_DNN_comb_CR.txt

cd ../..

echo "start t2w"
### comb region ###
text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_BDTnoBnoMET_BB_all.txt -o $carddir/workspace_$year\_BDTnoBnoMET_BB_all_VVEWK.root
text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_BDTbtag_BBtag_all.txt -o $carddir/workspace_$year\_BDTbtag_BBtag_all_VVEWK.root
text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_BDTwithMET_BMET_all.txt -o $carddir/workspace_$year\_BDTwithMET_BMET_all_VVEWK.root
text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_BDTwithMET_RMET_all.txt -o $carddir/workspace_$year\_BDTwithMET_RMET_all_VVEWK.root

### comb cate + region ###
### exp all ###
text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_DNN_comb_all.txt -o $carddir/workspace_$year\_DNN_comb_all_VVEWK.root

### unb CR comb
text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose --PO 'map=.*/VVEWK:r_s0[1,0,5]' $carddir/cms_vbshad_$year\_$quote\_DNN_comb_CR.txt -o $carddir/workspace_$year\_DNN_comb_CR_VVEWK.root


### 2D
#text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose \
#--PO 'map=.*/VVEWK:r_s0[1,0,5]' \
#--PO 'map=.*/VVOther:r_s1[1,0,5]' \
#$carddir/cms_vbshad_$year\_$quote\_DNN_comb_all.txt -o $carddir/workspace_$year\_DNN_comb_all_2D.root

echo "done t2w"

echo "significance"
### SR ###
### combine $carddir/workspace_$year\_$var\_$cat\_SR_VVEWK.root -M Significance -n $cat\_significance_$var\_SR_VVEWK --setParameters r_s0=1 -t -1
### side ###
#combine $carddir/workspace_$year\_$var\_$cat\_side_VVEWK.root -M Significance -n $cat\_significance_$var\_side_VVEWK
### anti ###
#combine $carddir/workspace_$year\_$var\_$cat\_anti_VVEWK.root -M Significance -n $cat\_significance_$var\_anti_VVEWK


if [ "$fitr" = "exp" ]; then 
	### comb region ###
	echo "BB"
	combine $carddir/workspace_$year\_BDTnoBnoMET_BB_all_VVEWK.root -M Significance -n BB_significance_DNN_all_VVEWK --setParameters r_s0=1 -t -1
	echo "BBtag"
	combine $carddir/workspace_$year\_BDTbtag_BBtag_all_VVEWK.root -M Significance -n BBtag_significance_DNN_all_VVEWK --setParameters r_s0=1 -t -1
	echo "BMET"
	combine $carddir/workspace_$year\_BDTwithMET_BMET_all_VVEWK.root -M Significance -n BMET_significance_DNN_all_VVEWK --setParameters r_s0=1 -t -1
	echo "RMET"
	combine $carddir/workspace_$year\_BDTwithMET_RMET_all_VVEWK.root -M Significance -n RMET_significance_DNN_all_VVEWK --setParameters r_s0=1 -t -1

	### comb region+cate ####
	echo "comb"
	combine $carddir/workspace_$year\_DNN_comb_all_VVEWK.root -M Significance -n comb_significance_DNN_all_VVEWK --setParameters r_s0=1 -t -1
else
	echo "run unb CR postfit later"
	### unb CR comb
	#combine $carddir/workspace_$year\_DNN_comb_CR_VVEWK.root -M Significance -n comb_significance_DNN_CR_VVEWK
fi;



### 2D scan ###
#combine -M MultiDimFit $carddir/workspace_$year\_DNN_comb_all_2D.root -n DNN_comb_all_2D_re --redefineSignalPOIs r_s0,r_s1  -P r_s0 -P r_s1 --floatOtherPOIs 1 --algo grid --points=2500 --setParameters r_s0=1,r_s1=1 -t -1

echo "FitDiagnostics"
if [ "$fitr" = "exp" ]; then
	combine -M FitDiagnostics -t -1 $carddir/workspace_$year\_DNN_comb_all_VVEWK.root --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations  --setParameters r_s0=1 --rMin -5 --rMax 5 -n final_$year\_DNN_comb_all_VVEWK
else
	combine -M FitDiagnostics $carddir/workspace_$year\_DNN_comb_CR_VVEWK.root --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations --rMin 0 --rMax 5 -n final_$year\_DNN_comb_CR_unb_VVEWK --cminDefaultMinimizerStrategy 1 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.5
fi;
##SR##
#combine -M FitDiagnostics -t -1 $carddir/workspace_$year\_$var\_$cat\_all_VVEWK.root --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations  --setParameters r_s0=1 --rMin -5 --rMax 5 -n carddir_$year\_$var\_$cat\_all_VVEWK

##side##
#combine -M FitDiagnostics $carddir/workspace_$year\_$var\_$cat\_side_VVEWK.root --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations --rMin 0 --rMax 5 -n carddir_$year\_$var\_$cat\_side_unb_VVEWK --cminDefaultMinimizerStrategy 1 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.5

##anti###
#combine -M FitDiagnostics  $carddir/workspace_$year\_$var\_$cat\_anti_VVEWK.root --robustFit=1  --X-rtd MINIMIZER_analytic --saveShapes --saveWithUncertainties --saveNormalizations --rMin 0 --rMax 20 -n carddir_$year\_$var\_$cat\_anti_unb_VVEWK --cminDefaultMinimizerStrategy 1 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.5

