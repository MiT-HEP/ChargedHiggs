#!/bin/bash

# Usage of this scipt
#./s_sb_errorbands.sh <index> <working directory> <MHhat> <r=MuHat
# your input rool file must be called inputfile.root
#./s_sb_errorbands.sh <index> <working directory> <MHhat> <Muhat>
nToys=1
MHhat=$3
#Muhat=$4
PhysModParams=$4
dir=$2 
#datacard=cms_datacard_hmumu_cat15_fixed.root 
## float mH
postfit=higgsCombinePostFit.MultiDimFit.mH120.root
((startIndex=$1*10000))
index=$startIndex

cd $dir
echo $PWD
source $VO_CMS_SW_DIR/cmsset_default.sh
eval `scramv1 runtime -sh`


while (( $index < $startIndex+ $nToys)); 
do
echo "---> combine $dir/$postfit -m $MHhat --snapshotName MultiDimFit -M GenerateOnly --saveWorkspace --toysFrequentist --bypassFrequentistFit -t 1 --setPhysicsModelParameters="$PhysModParams" -n combout_step0_$index -s -1"
combine $dir/$postfit  -m $MHhat --snapshotName MultiDimFit -M GenerateOnly --saveWorkspace --toysFrequentist --bypassFrequentistFit -t 1 --setPhysicsModelParameters="$PhysModParams" -n combout_step0_$index -s -1

ls $dir/higgsCombinecombout_step0_${1}*.root
mv $dir/higgsCombinecombout_step0_${1}*.root $dir/higgsCombinecombout_step0_done_$index.root

echo "---> combine $dir/higgsCombinecombout_step0_done_$index.root -m $MHhat -M MultiDimFit --floatOtherPOIs=1 --saveWorkspace --toysFrequentist --bypassFrequentistFit -t 1 --setPhysicsModelParameters="$PhysModParams" -n combout_step1_$index -s -1"
combine $dir/higgsCombinecombout_step0_done_$index.root -m $MHhat -M MultiDimFit --floatOtherPOIs=1 --saveWorkspace --toysFrequentist --bypassFrequentistFit -t 1 --setPhysicsModelParameters="$PhysModParams" -n combout_step1_$index -s -1
#combine higgsCombinecombout_step0_done_$index.root -m $MHhat -M MultiDimFit --floatOtherPOIs=1 --saveWorkspace --toysFrequentist --bypassFrequentistFit  --expectSignal=$Muhat -n combout_step1_$index 

ls $dir/higgsCombinecombout_step1_${1}*.root
mv $dir/higgsCombinecombout_step1_${1}*.root $dir/higgsCombinecombout_step1_done_$index.root

echo "---> combine $dir/higgsCombinecombout_step1_done_$index.root -m $MHhat --snapshotName MultiDimFit -M GenerateOnly --saveToys --toysFrequentist --bypassFrequentistFit -t -1 -n combout_step2_$index"
combine $dir/higgsCombinecombout_step1_done_$index.root -m $MHhat --snapshotName MultiDimFit -M GenerateOnly --saveToys --toysFrequentist --bypassFrequentistFit -t -1 -n combout_step2_$index

ls $dir/higgsCombinecombout_step2_${1}*.root
mv $dir/higgsCombinecombout_step2_${1}*.root $dir/higgsCombinecombout_step2_done_$index.root

(( index=$index+1))
done;

