#!/bin/bash

v=$1
t=$2
echo "Excuting training in condor for arguments $@"
echo " --  v=${v} t=${t}"
#  -v 0,1,2,3,4
# -t 0,1,2,3,4,5,6


source /cvmfs/cms.cern.ch/cmsset_default.sh
scram p CMSSW_9_4_1
cd CMSSW_9_4_1/src
eval `scram runtime -sh`
cd -

#virtualenv
source /home/amarini/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/venv/bin/activate

echo " ---------- LS ------"
ls -l | sed 's:^:LS:'
echo " --------------------"
echo " PYTHON:"
which python
echo " PWD:${PWD}"
echo " --------------------"

echo " ------- TRAIN -------"
python train_hmm.py -o output_var_${v}_train_${t}.root -d dataset_var_${v}_train_${t} -v $v -t $t

echo " --------------------"

echo " ---------- TRANSFER ------"
DIR=/home/amarini/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/test/condor_train
mkdir -p $DIR/output/
cp -v output_var_${v}_train_${t}.root  $DIR/output/
rsync -avP dataset_var_${v}_train_${t} $DIR/output/
echo " --------------------"


echo " ---------- END ------"
ls -l | sed 's:^:LS:'
echo " --------------------"


