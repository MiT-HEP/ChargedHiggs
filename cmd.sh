#python script/submit.py -n 100 -q 1nd -i dat/configHmumu.dat -d Hmumu/fwk/$(date +%Y_%m_%d)_FirstDataMC 

## SYNC  AND TRAIN TREES
#python script/submit.py -n 200 -q 1nd -i dat/configHmumu.dat -d /eos/user/a/amarini/Hmumu/fwk/$(date +%Y_%m_%d)_ExclusiveCategoriesAndTree --config='addConfig=HmumuAnalysis|catType=3' --config="addConfig=HmumuAnalysis|doSync=1"
#python script/submit.py -n 50 -q 1nd -i dat/configHmumuData.dat -d /eos/user/a/amarini/Hmumu/fwk/$(date +%Y_%m_%d)_SyncData --config='addConfig=HmumuAnalysis|catType=0' --config="addConfig=HmumuAnalysis|doSync=1"
#python script/submit.py -n 40 -q 1nd -i dat/configHmumuSync.dat -d /eos/user/a/amarini/Hmumu/fwk/$(date +%Y_%m_%d)_SyncDataRunB --config='addConfig=HmumuAnalysis|catType=0' --config="addConfig=HmumuAnalysis|doSync=1"
#python script/submit.py -n 100 -q 1nd -i dat/configHmumu.dat -d /eos/user/a/amarini/Hmumu/fwk/$(date +%Y_%m_%d)_MITBdt --config='addConfig=HmumuAnalysis|catType=4'
python script/submit.py -n 100 -q 1nd -i dat/configHmumu.dat -d /eos/user/a/amarini/Hmumu/fwk/$(date +%Y_%m_%d)_MITBdtBoostedCatRocco --config='addConfig=HmumuAnalysis|catType=5'

#python script/submit.py -n 100 -q 1nd -i dat/configHmumu.dat -d /eos/user/a/amarini/Hmumu/fwk/$(date +%Y_%m_%d)_ExclusiveCategoriesDeepB2 --config='addConfig=HmumuAnalysis|catType=3'
#python script/submit.py -n 100 -q 1nd -i dat/configHmumu.dat -d /eos/user/a/amarini/Hmumu/fwk/$(date +%Y_%m_%d)_OldCategories --config='addConfig=HmumuAnalysis|catType=2'

#addConfig=HmumuAnalysis|catType=2
#addConfig=HmumuAnalysis|doSync=1


#python script/Hmumu/makeFinalPlotsHmm.py --noSig -v Mmm -c '' -o Hmumu/plot/ -i Hmumu/fwk/2018_01_19_FirstDataMC/2018_01_19_FirstDataMC.root
