#!/bin/bash

# Usage of this scipt
#./s_sb_errorbands.sh <index> <working directory> <params>

## toys per job
nToys=1
dir=$2 
MHhat=125
[ "$3" == "" ] || MHhat=$3
PhysModParams=""
addParams=""
[ "$4" == "" ] || PhysModParams="--setParameters=$4"
[ "$4" == "" ] || addParams=",$4"
FreezeParams=""
[ "$5" == "" ] || FreezeParams="--freezeParameters=$5"
ExcludeNuis=""
[ "$6" == "" ] || ExcludeNuis="--exclude=$6"

## get post-fit distribution

postfit=higgsCombinePostFit.MultiDimFit.mH120.root
secondws=xxx.root

((startIndex=$1*10000))
index=$startIndex

cd $dir
echo $PWD
source $VO_CMS_SW_DIR/cmsset_default.sh
eval `scramv1 runtime -sh`

SDIR=${0%/*}
[ -f $postfit ] || cp -v $SDIR/$postfit ./
[ -f $secondws ] || cp -v $SDIR/$secondws ./
[ -f printSnapshot.py ] || cp -v $SDIR/printSnapshot.py ./

if (( $startIndex < 0 ));
then
    ## do it for the nominal point
    combine $dir/${secondws} -m $MHhat -M GenerateOnly --saveWorkspace --saveToys --toysFrequentist --bypassFrequentistFit -t -1 -n combout_step3_$index --setParameters=$(python $dir/printSnapshot.py ${ExcludeNuis} ${dir}/$postfit MultiDimFit )${addParams} $FreezeParams

    mv -v $dir/higgsCombinecombout_step3_${index}*.root $dir/higgsCombinecombout_step3_done_nominal.root

else

    while (( $index < $startIndex+ $nToys)); 
    do

    # toy generation from the post fit
    combine $dir/$postfit  -m $MHhat --snapshotName MultiDimFit -M GenerateOnly --saveWorkspace --toysFrequentist --bypassFrequentistFit -t 1 $PhysModParams $FreezeParams -n combout_step0_$index -s -1

    #ls $dir/higgsCombinecombout_step0_${1}*.root
    mv $dir/higgsCombinecombout_step0_${index}*.root $dir/higgsCombinecombout_step0_done_$index.root

    # toy is fit
    combine $dir/higgsCombinecombout_step0_done_$index.root -m $MHhat -M MultiDimFit --floatOtherPOIs=1 --saveWorkspace --toysFrequentist --bypassFrequentistFit -t 1 $PhysModParams $FreezeParams -n combout_step1_$index -s -1

    mv $dir/higgsCombinecombout_step1_${index}*.root $dir/higgsCombinecombout_step1_done_$index.root

    ##Use generate only from the postfit  asimov  to have a roodatahist
    combine $dir/higgsCombinecombout_step1_done_$index.root -m $MHhat --snapshotName MultiDimFit -M GenerateOnly --saveToys --toysFrequentist --bypassFrequentistFit -t -1 -n combout_step2_$index

    mv $dir/higgsCombinecombout_step2_${index}*.root $dir/higgsCombinecombout_step2_done_$index.root

    ## generate second ws
    combine $dir/${secondws} -m $MHhat -M GenerateOnly --saveToys --toysFrequentist --bypassFrequentistFit -t -1 -n combout_step3_$index --setParameters=$(python $dir/printSnapshot.py ${ExcludeNuis} $dir/higgsCombinecombout_step1_done_$index.root MultiDimFit )

    mv $dir/higgsCombinecombout_step3_${index}*.root $dir/higgsCombinecombout_step3_done_$index.root


    (( index=$index+1))
    done;

fi;
