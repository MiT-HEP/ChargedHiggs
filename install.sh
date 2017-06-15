#!/bin/bash

BASE=$PWD

PARALLEL=8

echo "-> Installing Nero"
#git clone git@github.com:MiT-HEP/NeroProducer.git
git clone https://github.com/MiT-HEP/NeroProducer.git
cd NeroProducer/Core
make -j $PARALLEL
cd $BASE

echo "-> Installing Combine"
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
git checkout v6.3.0 
#git remote add amarini git@github.com:amarini/HiggsAnalysis-CombinedLimit.git
git remote add amarini https://github.com/amarini/HiggsAnalysis-CombinedLimit.git
git fetch amarini
git merge amarini/cmssw80x
source env_standalone.sh 
cd $BASE
cd HiggsAnalysis/CombinedLimit
make -j $PARALLEL ; make  ## second will fix errors in the first

cd $BASE
echo "->DONE"

ls -ltr
