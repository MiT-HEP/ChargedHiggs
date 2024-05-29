#!/bin/bash

SUFFIX="aug12"
DIR="AUG4SYST"
aQGC="AUG12SYSTaqgc"

NJOBS=15

for cat in HAD BHAD MET ; do
#for reg in SR anti side antiSide; do
for reg in SR anti side; do
for year in 2016 2016APV 2017 2018 ; do
   oyear=${year}
   [ "$year" == "2016" ] && oyear="2016";
   [ "$year" == "2016APV" ] && oyear="2016APV";
   ireg=${reg}
   [ "${reg}" == "SR" ] && ireg="";

   [ "$cat" != "HAD" ] && [ "$cat" != "BHAD" ] && [ "$reg" == "antiSide" ] && {  echo "no $cat $reg"; continue ; } ## no anti side
   sem -j ${NJOBS} cp -v /eos/user/d/dalfonso/AnalysisVBS/NANO/${DIR}/UL${year}/${cat}${ireg}/${cat}${ireg}.root ./${cat}${reg}_${oyear}_${SUFFIX}.root
   #aqgc
   [ "$reg" == "antiSide" ] && { echo "no aqgc for $reg" ; continue;} # no aqgc for antiSide
   #[ "$year" == "2016APV" ] && { ln -s ./${cat}${reg}_22016_${SUFFIX}aqgc.root  ./${cat}${reg}_12016_${SUFFIX}aqgc.root; } ## 2016APV is the same as 2016
   sem -j ${NJOBS} cp -v /eos/user/d/dalfonso/AnalysisVBS/NANO/${aQGC}/UL${year}/${cat}${ireg}/${cat}${ireg}.root ./${cat}${reg}_${oyear}_${SUFFIX}aqgc.root

done
done
done

sem -j ${NJOBS} --wait
#### 2016 and 22016 are identical, but we will apply different luminosity
#for x in *_22016_*.root ; do
#    ln -s $x ${x//22016/2016}
#done



