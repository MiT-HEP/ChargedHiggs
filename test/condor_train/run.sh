#!/bin/bash

v=$1
t=$2
echo "Excuting training in condor for arguments $@"
echo " --  v=${v} t=${t}"
#  -v 0,1,2,3,4
# -t 0,1,2,3,4,5,6

python train_hmm.py \
 -o output_var_${v}_train_${t}.root \
 -d dataset_var_${v}_train_${t} \
 -v $v \
 -t $t
