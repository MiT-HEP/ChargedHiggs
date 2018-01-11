#!/bin/bash

echo "-> TXT2WS"
[ -f cms_datacard_hmumu_cat14_bias.root ] && echo "1. using existing txt2ws" || text2workspace.py -m 125 cms_datacard_hmumu_cat14_bias.txt
[ -f cms_datacard_hmumu_cat14_fixed.root ] && echo "2. using existing txt2ws" || text2workspace.py -m 125 cms_datacard_hmumu_cat14_fixed.txt
[ -f cms_datacard_hmumu_cat14_bias_betterres.root ]  && echo "3. using existing txt2ws" || text2workspace.py -m 125 cms_datacard_hmumu_cat14_bias_betterres.txt
[ -f cms_datacard_hmumu_cat14_bias_stat.root ] && echo "4. using existing txt2ws" || text2workspace.py -m 125 cms_datacard_hmumu_cat14_bias_stat.txt

[ -f cms_datacard_hmumu_cat14_bias_rvrf.root ] && echo "5. using existing txt2ws" || text2workspace.py -m 125 cms_datacard_hmumu_cat14_bias.txt -P HiggsAnalysis.CombinedLimit.PhysicsModel:rVrFXSHiggs -o cms_datacard_hmumu_cat14_bias_rvrf.root



LUMINOW=35867
for LUMITARGET in 35.867 50 100 200 300 500 750 1000 1500 2000 2500 3000; 
do
    echo "-> SUBMITTING LUMITARGET"
    ##default
    #bsub -q 8nh -J proj_fixed_$LUMITARGET  "cd $PWD; eval \`scram runtime -sh\`;  combine -M MultiDimFit -t -1 -m 125 --algo=grid --points=100 --rMin=-1 --rMax=3 --cminDefaultMinimizerType=Minuit2 --expectSignal=1 --setPhysicsModelParameters lumiscale=$(echo "$LUMITARGET*1000/$LUMINOW" | bc -l ) -n fixed_${LUMITARGET}fb cms_datacard_hmumu_cat14_fixed.root "
    ## bias
    #bsub -q 8nh -J proj_bias_$LUMITARGET "cd $PWD; eval \`scram runtime -sh\`; combine -M MultiDimFit -t -1 -m 125 --algo=grid --points=100 --rMin=-1 --rMax=3 --cminDefaultMinimizerType=Minuit2 --expectSignal=1 --setPhysicsModelParameters lumiscale=$(echo "$LUMITARGET*1000/$LUMINOW" | bc -l ) -n bias_${LUMITARGET}fb cms_datacard_hmumu_cat14_bias.root "
    ## bias + 10% better muon resolution on the signal
    #bsub -q 8nh -J proj_bias_betterres_$LUMITARGET "cd $PWD; eval \`scram runtime -sh\`; combine -M MultiDimFit -t -1 -m 125 --algo=grid --points=100 --rMin=-1 --rMax=3 --cminDefaultMinimizerType=Minuit2 --expectSignal=1 --setPhysicsModelParameters lumiscale=$(echo "$LUMITARGET*1000/$LUMINOW" | bc -l ) -n bias_betterres_${LUMITARGET}fb cms_datacard_hmumu_cat14_bias_betterres.root "
    # statonly
    #bsub -q 8nh -J proj_statonly_$LUMITARGET  "cd $PWD; eval \`scram runtime -sh\`;  combine -M MultiDimFit -t -1 -m 125 --algo=grid --points=100 --rMin=-1 --rMax=3 --cminDefaultMinimizerType=Minuit2 --expectSignal=1 --setPhysicsModelParameters lumiscale=$(echo "$LUMITARGET*1000/$LUMINOW" | bc -l ) -n StatOnly_${LUMITARGET}fb -S 0 cms_datacard_hmumu_cat14_fixed.root "
    # statonly + bias
    #bsub -q 8nh -J proj_stat_bias_$LUMITARGET  "cd $PWD; eval \`scram runtime -sh\`;  combine -M MultiDimFit -t -1 -m 125 --algo=grid --points=100 --rMin=-1 --rMax=3 --cminDefaultMinimizerType=Minuit2 --expectSignal=1 --setPhysicsModelParameters lumiscale=$(echo "$LUMITARGET*1000/$LUMINOW" | bc -l ) -n StatOnlyBias_${LUMITARGET}fb cms_datacard_hmumu_cat14_bias_stat.root "
    # everything freezed, includig background
    ## RV / RF
    #bsub -q 8nh -J proj_rv_$LUMITARGET "cd $PWD; eval \`scram runtime -sh\`; combine -M MultiDimFit -t -1 -m 125 --algo=grid --points=100 -P RV --setPhysicsModelParameterRanges RV=-2,8:RF=-2,8 --floatOtherPOIs=1  --cminDefaultMinimizerType=Minuit2 --expectSignal=1 --setPhysicsModelParameters lumiscale=$(echo "$LUMITARGET*1000/$LUMINOW" | bc -l ) -n rv_${LUMITARGET}fb cms_datacard_hmumu_cat14_bias_rvrf.root "
    #bsub -q 8nh -J proj_rf_$LUMITARGET "cd $PWD; eval \`scram runtime -sh\`; combine -M MultiDimFit -t -1 -m 125 --algo=grid --points=100 -P RF --setPhysicsModelParameterRanges RV=-2,8:RF=-2,8 --floatOtherPOIs=1  --cminDefaultMinimizerType=Minuit2 --expectSignal=1 --setPhysicsModelParameters lumiscale=$(echo "$LUMITARGET*1000/$LUMINOW" | bc -l ) -n rf_${LUMITARGET}fb cms_datacard_hmumu_cat14_bias_rvrf.root "

    ## LOCAL
done
