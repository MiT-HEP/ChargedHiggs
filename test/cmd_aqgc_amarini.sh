#cp -v /eos/user/d/dalfonso/AnalysisVBS/NANO/SEPT22aqgc/UL18/HAD/HAD.root Datacards/inputs/HADaqgc.root
#/eos/user/d/dalfonso/AnalysisVBS/NANO/SEPT3/UL18/HAD/HAD.root

#Copy file in Datacards/inputs/ or /eos/user/h/hum/VBSHad
python script/bwsVBSHad.py --there -q 2 -s Mjj -y 2020 -c BB -r SR -i HADSR_2020_sep23.root
python script/bwsVBSHad.py --there -q 2 -s Mjj -y 2020 -c BB -r anti -i HADanti_2020_sep23.root
python script/bwsVBSHad.py --there -q 2 -s Mjj -y 2020 -c BB -r side -i HADside_2020_sep23.root

python script/bwsVBSHad.py --there -q 5 --aqgc -s MVV -y 2018 -c BB -r SR -i HADSR_2018_sep23.root --aqgc_parameter ft7




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

python script/plotAQGC.py --more $(ls Datacards/OCT15/AQGC/higgs*all.root | while read f ; do echo  "$f:$(echo ${f%%_all.root} | sed 's:.*_::g')" ; done | tr '\n' ',' | sed 's/,$//') -o plot_aqgc/all  --xrange -16,16 -b

python script/plotAQGC_panel.py -o plot_aqgc/all

mkdir ~/www/cms-private/VBSHadronic/OCT15_rebin§
rsync -avP plot_aqgc/ ~/www/cms-private/VBSHadronic/OCT15_rebin/plot_aqgc/
