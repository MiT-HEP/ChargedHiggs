#!/bin/bash

[ "$WORKDIR" == "" ] && export WORKDIR="/tmp/amarini/bias/"

BASE=/afs/cern.ch/user/a/amarini/work/ChHiggs2016/CMSSW_9_1_0_pre2/src/ChargedHiggs/test/bias/2017_07_09/

cd ${BASE}
eval `scramv1 runtime -sh`

catList=12
TOYS=50
SEED=123456
MH=120

[ "$1" == "" ]  || export catList="$1"
[ "$2" == "" ]  || export SEED="$2"


cd $WORKDIR
echo "Using TOYS=$TOYS"
echo "Using MH=$MH"

EXTRA=""
mkdir splitted${EXTRA}
cp -v $BASE/splitted${EXTRA}/*root ./splitted${EXTRA}
TOYSNUIS=--toysFrequentist

for NUM in $catList ; do 

    OUT=$BASE/results_2d_finebin_v3_MH${MH}/$SEED/cat$NUM
    mkdir -p $OUT
    DATACARDGEN=splitted${EXTRA}/catMultiPdf${NUM}.root
    DATACARDFIT=splitted${EXTRA}/catMultiPdf${NUM}.root

    cp -v $BASE/splitted${EXTRA} ./splitted$EXTRA/

    for index in {1..11};
    do
        rm -v mlfit* higgs* *png
        echo "generating $index cat$NUM" 
        set -v
        combine $DATACARDGEN -M GenerateOnly --toysFrequentist -t ${TOYS} --expectSignal 1 --saveToys -m ${MH} -n _cat${NUM}_gen_$index --setPhysicsModelParameters pdfindex_cat${NUM}=${index} --freezeNuisances=MH,pdfindex_cat${NUM},scale_cat0_procGluGlu,smear_cat0_procGluGlu -s $SEED ; 
    
        #for jindex in `seq 1 7`; 
        for jindex in `seq 1 11`; 
        do
                echo "fitting ${jindex} against gen=$index"         
                combine ${DATACARDFIT} -M MaxLikelihoodFit  --setPhysicsModelParameters MH=${MH},pdfindex_cat${NUM}=${jindex} -m ${MH} --freezeNuisances=MH,pdfindex_cat${NUM} --toysFile higgsCombine_cat${NUM}_gen_${index}.GenerateOnly.mH${MH}.${SEED}.root -t ${TOYS} --rMin=-20 --rMax=50 -n  _cat${NUM}_mu_1_gen_${index}_fit_${jindex}   --robustFit=1; 
                cp -v mlfit* $OUT
        done
        set +v
    done 
    ls
done

