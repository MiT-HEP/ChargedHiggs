#!/bin/bash

[ "$WORKDIR" == "" ] && export WORKDIR="/tmp/amarini/bias/"

BASE=/afs/cern.ch/user/a/amarini/work/ChHiggs2016/CMSSW_9_1_0_pre2/src/ChargedHiggs/test/bias/FEWZ
cd ${BASE}
eval `scramv1 runtime -sh`

catList=12
muList="0 1 8 20"
[ "$1" == "" ]  || export catList="$1"

for cat in ${catList};
do
    cp -v ./*cat${cat}* $WORKDIR/
done

cd $WORKDIR

for mu in ${muList}; 
do
    for i in ${catList} ; do 
        combine cat${i}_bkg.root -M GenerateOnly --toysFrequentist -t 1000 --expectSignal ${mu} --saveToys -m 125 -n _cat${i}_amcatnlo_mu${mu}  &
    done
done

wait

for mu in ${muList}; 
do
    for i in $catList; 
    do 
        combine cat${i}.root -M MultiDimFit  --setPhysicsModelParameters MH=125 -m 125 --freezeNuisances=MH --toysFile higgsCombine_cat${i}_amcatnlo_mu${mu}.GenerateOnly.mH125.123456.root -t 1000 -n  _cat${i}_mu${mu}  --robustFit=1 --forceRecreateNLL --cminRunAllDiscreteCombinations --cminDiscreteMinTol=0.0001 --rMin=-20 --rMax=50 &  
    done | tee multidim${mu}_cat${i}.log

done

wait

for mu in ${muList};
do 
    for i in $catList; 
    do 
    cat multidim${mu}_cat${i}.log | grep RooRealVar | tr -s ' ' | cut -d ' ' -f 3,5 > multidimtree${mu}_cat${i}.txt

    echo  > multidimcmd.txt
    echo  "TTree *t${mu}=new TTree(\"t${mu}\",\"t${mu}\")" >> multidimcmd.txt
    echo  "t${mu}->ReadFile(\"multidimtree${mu}_cat${i}.txt\",\"mu:muErr\",' ')" >> multidimcmd.txt

    done

done

for i in $catList; 
do 
    mkdir $BASE/cat${i}
    cp -v multidim*cat${i} $BASE/cat${i}/
    cat multidimcmd.txt >> $BASE/cat${i}/multidimcmd.txt
done

