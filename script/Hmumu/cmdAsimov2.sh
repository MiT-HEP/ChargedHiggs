#!/bin/bash

[ "$WORKDIR" == "" ] && { export WORKDIR="/tmp/amarini/bias/"; mkdir $WORKDIR; }

BASE=/afs/cern.ch/user/a/amarini/work/ChHiggs2016/CMSSW_9_1_0_pre2/src/ChargedHiggs/test/bias/2017_07_09/

cd ${BASE}
eval `scramv1 runtime -sh`

TOYS=-1
SEED=123456
MH=125

[ "$1" == "" ]  || export SEED="$1"
[ "$2" == "" ]  || export MH="$2"


cd $WORKDIR

echo "Using TOYS=$TOYS"
echo "Using MH=$MH"



#mkdir splitted
#cp -v $BASE/splitted/*root ./splitted

#EXTRA="_115_140_exp"
EXTRA=""
mkdir splitted${EXTRA}
cp -v $BASE/splitted${EXTRA}/*root ./splitted${EXTRA}
TOYSNUIS=--toysFrequentist
#TOYSNUIS=--toysNoSystematics

for NUM in {0..12}; 
do
    OUT=$BASE/results_zmod2${EXTRA}_asimov_${MH}/$SEED/cat${NUM}
    mkdir -p $OUT
    DATACARDGEN=splitted${EXTRA}/catMultiPdf${NUM}.root
    DATACARDFIT=splitted${EXTRA}/catMultiPdf${NUM}.root

    for index in {0..7};
    do
        rm -v mlfit* higgs* *png
        
        echo "generating $index cat$NUM" 
        set -v
        combine ${DATACARDGEN} -M GenerateOnly ${TOYSNUIS} -t ${TOYS} --expectSignal 1 --saveToys -m ${MH} -n _cat${NUM}_gen_$index --setPhysicsModelParameters pdfindex_cat${NUM}=${index}  --freezeNuisances=pdfindex_cat${NUM} -s $SEED  ; 
    
        echo "fitting default against gen=$index"         
        combine ${DATACARDFIT} -M MaxLikelihoodFit  --freezeNuisances=MH,scale_cat0_procGluGlu,smear_cat0_procGluGlu,pdfindex_cat${NUM} -m ${MH} --toysFile higgsCombine_cat${NUM}_gen_${index}.GenerateOnly.mH${MH}.${SEED}.root --setPhysicsModelParameters pdfindex_cat${NUM}=7 -t ${TOYS} --rMin=-20 --rMax=50 -n  _cat${NUM}_mu_1_gen_${index}_fit_default  --robustFit=1 --plots --saveShapes --saveWithUncertainties --saveNormalizations --saveWorkspace ; 
        set +v

        cp -v mlfit* $OUT
        cp -v higgs* $OUT
        #[ "${TOYS}" == "-1" ] &&  cp *png $OUT
        [ "${TOYS}" == "-1" ] && { for f in *png ; do cp -v $f $OUT/${f%%.png}.gen_${index}.png; done ; }

    done 
done

ls
#cp -v mlfit* $OUT
#cp -v higgs* $OUT

