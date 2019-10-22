#!/bin/bash

#for i in {1..13}; do cp -v cat0.txt cat${i}.txt ; sed -i'' "s/cat0/cat${i}/g" cat${i}.txt ; done

BASE=$PWD

for year in 2016 2017 2018;
do
    cd $year 
    ln -s $BASE/../Hmumu
    #export DIR=Hmumu/2019_05_27_Hmm${year}_BdtUCSD_Cat_Nvtx_zptrwgt_stxs_nobveto_opt_NoMjj
    #export DIR=Hmumu/2019_05_30_Hmm${year}_BdtUCSD_kinfit_fsrreco_opt29_Mjj400
    #export DIR=Hmumu/2019_06_24_Hmm${year}_BdtUCSD_asRaffaCat2
    #export DIR=Hmumu/2019_07_10_Hmm${year}_BdtUCSD_Mjj400
    export DIR=Hmumu/2019_07_25_Hmm${year}_BdtUCSD_Mjj400_JetPt25
    for cat in {0..13}; do 
        text2workspace.py -m 125  cat${cat}.txt
        combine -M AsymptoticLimits -t -1 --rMin=0 --rMax=50 --expectSignal=0 cat${cat}.root -n cat${cat} -m 125 ;
    done
    combineCards.py -S cat*txt > cms_datacard_hmumu_year${year}.txt
    text2workspace.py -m 125 cms_datacard_hmumu_year${year}.txt
    combine -M AsymptoticLimits -t -1 --rMin=0 --rMax=10 --expectSignal=0 -n All -m 125 cms_datacard_hmumu_year${year}.root
    python $BASE/../test/makeCombinePlotByCat.py -a higgsCombinecat{0..13}.Asymptotic*mH125.root higgsCombineAll.AsymptoticLimits.mH125.root
    cd $BASE
done
cd $BASE

#export DIR2016=Hmumu/2019_05_27_Hmm2016_BdtUCSD_Cat_Nvtx_zptrwgt_stxs_nobveto_opt_NoMjj
#export DIR2017=Hmumu/2019_05_27_Hmm2017_BdtUCSD_Cat_Nvtx_zptrwgt_stxs_nobveto_opt_NoMjj
#export DIR2018=Hmumu/2019_05_27_Hmm2018_BdtUCSD_Cat_Nvtx_zptrwgt_stxs_nobveto_opt_NoMjj
#export DIRRunII=Hmumu/2019_05_27_HmmAll_BdtUCSD_Cat_Nvtx_zptrwgt_stxs_nobveto_opt_NoMjj
#export DIR2016=Hmumu/2019_05_30_Hmm2016_BdtUCSD_kinfit_fsrreco_opt29_Mjj400
#export DIR2017=Hmumu/2019_05_30_Hmm2017_BdtUCSD_kinfit_fsrreco_opt29_Mjj400
#export DIR2018=Hmumu/2019_05_30_Hmm2018_BdtUCSD_kinfit_fsrreco_opt29_Mjj400
#export DIRRunII=Hmumu/2019_05_30_HmmAll_BdtUCSD_kinfit_fsrreco_opt29_Mjj400

#export DIR2016=Hmumu/2019_06_24_Hmm2016_BdtUCSD_asRaffaCat2
#export DIR2017=Hmumu/2019_06_24_Hmm2017_BdtUCSD_asRaffaCat2
#export DIR2018=Hmumu/2019_06_24_Hmm2018_BdtUCSD_asRaffaCat2
#export DIRRunII=Hmumu/2019_06_24_HmmAll_BdtUCSD_asRaffaCat2

#Hmumu/2019_06_24_Hmm${year}_BdtUCSD_asRaffaCat2
#2019_07_10_HmmAll_BdtUCSD_Mjj400
#export DIR2016=Hmumu/2019_07_10_Hmm2016_BdtUCSD_Mjj400
#export DIR2017=Hmumu/2019_07_10_Hmm2017_BdtUCSD_Mjj400
#export DIR2018=Hmumu/2019_07_10_Hmm2018_BdtUCSD_Mjj400
#export DIRRunII=Hmumu/2019_07_10_HmmAll_BdtUCSD_Mjj400

export DIR2016=Hmumu/2019_07_25_Hmm2016_BdtUCSD_Mjj400_JetPt25
export DIR2017=Hmumu/2019_07_25_Hmm2017_BdtUCSD_Mjj400_JetPt25
export DIR2018=Hmumu/2019_07_25_Hmm2018_BdtUCSD_Mjj400_JetPt25
export DIRRunII=Hmumu/2019_07_25_HmmAll_BdtUCSD_Mjj400_JetPt25

cd RunII
for cat in {0..13}; do 
    text2workspace.py -m 125  cat${cat}.txt
    combine -M AsymptoticLimits -t -1 --rMin=0 --rMax=50 --expectSignal=0 cat${cat}.root -n cat${cat} -m 125 ;
done
combineCards.py -S cat*txt > cms_datacard_hmumu_RunII.txt
text2workspace.py -m 125 cms_datacard_hmumu_RunII.txt
combine -M AsymptoticLimits -t -1 --rMin=0 --rMax=10 --expectSignal=0 -n All -m 125 cms_datacard_hmumu_RunII.root
python $BASE/../test/makeCombinePlotByCat.py -a higgsCombinecat{0..13}.Asymptotic*mH125.root higgsCombineAll.AsymptoticLimits.mH125.root
cd $BASE

OUT=Mjj400_JetPt25
mkdir $OUT
for dir in 2016 2017 2018 RunII; do
    mkdir $OUT/$dir
    mv -v $dir/{higgs*,limit*} ${OUT}/$dir/
done
