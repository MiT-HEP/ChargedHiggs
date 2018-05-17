#!/bin/bash
## generate  toys, accordingly to a known distribution
#combine -M GenerateOnly -t 10 --saveToys -d datacard.root -S 0
echo "-> cleaning directory"
rm -v fitD*root higgs*root combine* template.root rootstats*

echo "-> producing histo"
python src/produce_histo.py 

## create datacard that have has bkg that toys
#--keyword-value FIT=1 --keyword-value GEN=1 --setParameters bkg_norm=xxx

# create asimov from one, and fit from the other
# all possible pertations
# plot deltaR 

echo "-> fitting"
for fit in {0..9};do
for gen in {0..9}; do
    #combine -M FitDiagnostics -t 0 --keyword-value FIT=${fit} --keyword-value GEN=${gen}  -n _fit${fit}_gen${gen} -d src/datacard_hist.txt -S 0 --rMin=-2.e2 --rMax=2.e2 #--robustFit=1 --saveNormalizations --saveShapes 
    combine -M AsymptoticLimits -t 0 --keyword-value FIT=${fit} --keyword-value GEN=${gen}  -n _fit${fit}_gen${gen} -d src/datacard_hist.txt -S 0 --rMin=-2.e2 --rMax=2.e2 
    #--stepSize=0.01000001
    #--cminFallbackAlgo "Minuit2,migrad,0:0.1"  --cminApproxPreFitTolerance=100 --X-rtd MINIMIZER_MaxCalls=9999999 --saveNormalizations --saveShapes
done
done


