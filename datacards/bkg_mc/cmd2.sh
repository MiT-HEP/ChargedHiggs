
BASE=$PWD

export DIR2016=Hmumu/2019_07_25_Hmm2016_BdtUCSD_Mjj400_JetPt25
export DIR2017=Hmumu/2019_07_25_Hmm2017_BdtUCSD_Mjj400_JetPt25
export DIR2018=Hmumu/2019_07_25_Hmm2018_BdtUCSD_Mjj400_JetPt25
export DIRRunII=Hmumu/2019_07_25_HmmAll_BdtUCSD_Mjj400_JetPt25

cd RunII_mc
for cat in {0..13}; do
    text2workspace.py --X-allow-no-signal --for-fits -m 125 -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so cat${cat}.txt ;
    combine -M AsymptoticLimits -L `root-config --libdir`/libPyROOT.so -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so -t -1 --rMin=0 --rMax=50 --expectSignal=0 cat${cat}.root -n cat${cat} -m 125 ;
done
combineCards.py -S cat{0..8}.txt > cms_datacard_hmumu_RunII.txt
text2workspace.py --X-allow-no-background --for-fits -m 125 -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so cms_datacard_hmumu_RunII.txt
combine -M AsymptoticLimits -L `root-config --libdir`/libPyROOT.so -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so -t -1 --rMin=0 --rMax=10 --expectSignal=0 -n All -m 125 cms_datacard_hmumu_RunII.root
python $BASE/../../test/makeCombinePlotByCat.py -a higgsCombinecat{0..8}.Asymptotic*mH125.root higgsCombineAll.AsymptoticLimits.mH125.root
cd $BASE

OUT=Mjj400_JetPt25
mkdir $OUT
for dir in RunII_mc; do
    mkdir $OUT/$dir
    mv -v $dir/{higgs*,limit*} ${OUT}/$dir/
done

################## PARTIAL ASIMOV: mc is mc and data is asimov
cd RunII_mc
for cat in {0..13}; do
    text2workspace.py --X-allow-no-signal --for-fits -m 125 -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so cat${cat}.Asimov.txt ;
    combine -M AsymptoticLimits -L `root-config --libdir`/libPyROOT.so -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so -t 0 --rMin=0 --rMax=50 --expectSignal=0 cat${cat}.Asimov.root -n cat${cat}_Asimov -m 125 ;
done
combineCards.py -S cat{0..8}.Asimov.txt > cms_datacard_hmumu_RunII.Asimov.txt
text2workspace.py --X-allow-no-background --for-fits -m 125 -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so cms_datacard_hmumu_RunII.Asimov.txt
combine -M AsymptoticLimits -L `root-config --libdir`/libPyROOT.so -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so -t 0 --rMin=0 --rMax=10 --expectSignal=0 -n All_Asimov -m 125 cms_datacard_hmumu_RunII.Asimov.root
python $BASE/../../test/makeCombinePlotByCat.py -a higgsCombinecat{0..8}_Asimov.Asymptotic*mH125.root higgsCombineAll_Asimov.AsymptoticLimits.mH125.root
cd $BASE

OUT=Mjj400_JetPt25_MCandAsimov
mkdir $OUT
for dir in RunII_mc; do
    mkdir $OUT/$dir
    mv -v $dir/{higgs*,limit*} ${OUT}/$dir/
done

################## PARTIAL and NORM: mc is mc and data is asimov
cd RunII_mc
for cat in {0..13}; do
    text2workspace.py --X-allow-no-signal --for-fits -m 125 -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so cat${cat}_norm.Asimov.txt ;
    combine -M AsymptoticLimits --cminDefaultMinimizerStrategy=0 --cminFallbackAlgo Minuit2,Migrad,0:0.1 -L `root-config --libdir`/libPyROOT.so -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so -t 0 --rMin=0 --rMax=50 --expectSignal=0 cat${cat}_norm.Asimov.root -n cat${cat}_norm_Asimov -m 125;
done
combineCards.py -S cat{0..8}_norm.Asimov.txt > cms_datacard_hmumu_RunII.norm.Asimov.txt
text2workspace.py --X-allow-no-background --for-fits -m 125 -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so cms_datacard_hmumu_RunII.norm.Asimov.txt
combine -M AsymptoticLimits --cminDefaultMinimizerStrategy=0 --cminFallbackAlgo Minuit2,Migrad,0:0.1 -L `root-config --libdir`/libPyROOT.so -L /afs/cern.ch/user/a/amarini/work/ChHiggs2017/CMSSW_9_4_1/src/ChargedHiggs/bin/libChargedHiggs.so -t 0 --rMin=0 --rMax=10 --expectSignal=0 -n All_norm_Asimov -m 125 cms_datacard_hmumu_RunII.norm.Asimov.root
python $BASE/../../test/makeCombinePlotByCat.py -a higgsCombinecat{0..8}_norm_Asimov.Asymptotic*mH125.root higgsCombineAll_norm_Asimov.AsymptoticLimits.mH125.root
cd $BASE

OUT=Mjj400_JetPt25_MCNormandAsimov
mkdir $OUT
for dir in RunII_mc; do
    mkdir $OUT/$dir
    mv -v $dir/{higgs*,limit*} ${OUT}/$dir/
done

