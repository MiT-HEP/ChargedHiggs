#!/bin/bash

#DIR="/eos/user/d/dalfonso/AnalysisVBS/NANO/APR10"
#DIR="/eos/user/d/dalfonso/AnalysisVBS/NANO/APR12syst"
#DIR="/eos/user/d/dalfonso/AnalysisVBS/NANO/MAY24SYST"
#DIR="/eos/user/d/dalfonso/AnalysisVBS/NANO/MAY25SYSTaqgc"
DIR="/eos/user/d/dalfonso/AnalysisVBS/NANO/MAY25SYST"

for year in "2016APV" "2016" "2017" "2018";
do

for type in "HADside" "BHADside" "METside" "HADanti" "BHADanti" "METanti"  "HAD" "BHAD" "MET" ;

do

#ls /eos/user/d/dalfonso/AnalysisVBS/NANO/MAY25SYST/UL2016APV/BHAD/BHAD.root
#ls /eos/user/d/dalfonso/AnalysisVBS/NANO/MAY25SYST/UL2016APV/BHAD/VBS*.root | wc

TMP1="${DIR}/UL${year}/${type}/${type}.root"
TMP2="${DIR}/UL${year}/${type}/VBS*.root"
#echo $TMP1
#echo $TMP2
ls $TMP1
ls $TMP2 | wc

done

done
