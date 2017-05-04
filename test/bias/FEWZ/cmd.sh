
catList=12
#muList=0 1 8 20
muList="0 1 8 20"

for mu in ${muList}; 
do
    for i in ${catList} ; do 
        combine cat${i}_bkg.root -M GenerateOnly --toysFrequentist -t 1000 --expectSignal ${mu} --saveToys -m 125 -n _cat${i}_amcatnlo_mu${mu}  &
    done
done

wait

for mu in ${muList}; 
do
    for i in $catList; 
    do 
        combine cat${i}.root -M MultiDimFit  --setPhysicsModelParameters MH=125 -m 125 --freezeNuisances=MH --toysFile higgsCombine_cat${i}_amcatnlo_mu${mu}.GenerateOnly.mH125.123456.root -t 1000 -n  _cat${i}_mu${mu}  --robustFit=1 --forceRecreateNLL --cminRunAllDiscreteCombinations --cminDiscreteMinTol=0.0001 --rMin=-20 --rMax=50 &  
    done | tee multidim${mu}.log

done

wait

for mu in ${muList};
do 
    cat multidim${mu}.log | grep RooRealVar | tr -s ' ' | cut -d ' ' -f 3,5 > multidimtree${mu}.txt

    echo 
    echo  "TTree *t${mu}=new TTree(\"t${mu}\",\"t${mu}\")"
    echo  "t${mu}->ReadFile(\"multidimtree${mu}.txt\",\"mu:muErr\",' ')"

done
