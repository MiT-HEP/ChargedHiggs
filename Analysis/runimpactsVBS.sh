#!/bin/sh
card=$1
cate=$2
region=$3

echo $card

echo "step 1"
##expected
combineTool.py -M Impacts -d $card -m 125 --setParameters r_s0=1 -t -1 --doInitialFit --robustFit 1 --rMin=-10 --rMax=10 --cminDefaultMinimizerStrategy 0 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.1 --cminFallbackAlgo \"Minuit,0:0.001\"
###observed
#combineTool.py -M Impacts -d $card -m 125 --doInitialFit --robustFit 1 --rMin=-10 --rMax=10 --cminDefaultMinimizerStrategy 1 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.5 --cminFallbackAlgo \"Minuit2,0:0.0001\"

echo "step 2"
##expected
combineTool.py -M Impacts -d $card -m 125 --setParameters r_s0=1 -t -1 --doFits --robustFit 1 --rMin=-10 --rMax=10 --cminDefaultMinimizerStrategy 0 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.1 --cminFallbackAlgo \"Minuit,0:0.001\"
###observed
#combineTool.py -M Impacts -d $card -m 125 --doFits --robustFit 1 --rMin=-10 --rMax=10 --cminDefaultMinimizerStrategy 1 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --X-rtd MINIMIZER_analytic --cminDefaultMinimizerTolerance=0.5 --cminFallbackAlgo \"Minuit2,0:0.0001\"

echo "step 3"
combineTool.py -M Impacts -d $card -m 125 -o impacts_NanoV9V10unb15_$cate\_$region\.json

echo "step 4"
plotImpacts.py -i impacts_NanoV9V10unb15_$cate\_$region\.json -o outputNanoV9V10unb15_$cate\_$region\.json
