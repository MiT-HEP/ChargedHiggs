#cp -v /eos/user/d/dalfonso/AnalysisVBS/NANO/SEPT22aqgc/UL18/HAD/HAD.root Datacards/inputs/HADaqgc.root
#/eos/user/d/dalfonso/AnalysisVBS/NANO/SEPT3/UL18/HAD/HAD.root

#Copy file in Datacards/inputs/ or /eos/user/h/hum/VBSHad
python script/bwsVBSHad.py -q 1 -s mVV -y 2018 -c BB -i HADSR_2018_sep12.root

python script/bwsVBSHad.py -q 5 --aqgc -s MVV -y 2018 -c BB -i HADSR_2018_sep12.root




SM_VALUE="0p00"
combine -M GenerateOnly -t -1 --expectSignal=1 -n "_AsimovSM" --keyword-value VALUE=${SM_VALUE} -d cms_vbshad_2018_5_aqgc_ft7_MVV_BB.txt --saveToys


#for val in "5p20" ; do
cat cms_vbshad_2018_5_aqgc_ft7_MVV_BB.txt | tail -n 1 | cut -d ' ' -f 3 | tr ',' '\n' | while read val; do
echo " --- Doing $val --- "
# baseline r=0
combine -M MultiDimFit -t -1 --toysFile  higgsCombine_AsimovSM.GenerateOnly.mH120.VALUE0p00.123456.root  --keyword-value VALUE=$val -n "_baseline"  -d cms_vbshad_2018_5_aqgc_ft7_MVV_BB.txt --saveWorkspace --setParameters r=0 --freezeParameters r 
# deltaNLL wrt baseline, i.e. at r=1
combine -M MultiDimFit -t -1 --toysFile  higgsCombine_AsimovSM.GenerateOnly.mH120.VALUE0p00.123456.root  --keyword-value VALUE=$val -n "_ft7_$val" -d higgsCombine_baseline.MultiDimFit.mH120.VALUE${val}.root  --skipInitialFit --algo fixed --fixedPointPOIs r=1 --snapshotName MultiDimFit
done


hadd higgsCombine_tmp_ft7.root higgsCombine_ft7_*.MultiDimFit.mH120.VALUE*.root
python ../../script/makeNLLPlots.py --xtitle ft7 -o deltaNLL_ft7 -p VALUE -f higgsCombine_tmp_ft7.root --tobaseline r --nosmooth --xrange -16,30.
