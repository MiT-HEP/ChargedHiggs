#!/bin/bash

READ=$HOME/work/ChHiggs2016/CMSSW_9_1_0_pre2/src/ChargedHiggs/test/YR4/read.py ;
MH=120.0
procs="qqH ggH WH ZH"

BR=$(python $READ BR Hmm $MH | grep -v Unable | sed 's/.*== //g')

for Ecm in 7TeV 8TeV; do
for proc in $procs ; do
        SCALE=$(python $READ $Ecm $proc $MH | sed 's/.*== //g')
        for i in *_${Ecm}_*txt ; do
            echo "yr4_$proc rateParam * ${proc}_hmm${Ecm} $SCALE" >> $i
            echo "yr4_BRHmm rateParam * ${proc}_hmm${Ecm} $BR"   >> $i 
        done
done
done
