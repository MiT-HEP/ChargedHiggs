
#std bias
#for i in {0..29} ; do combine cat${i}.root -M GenerateOnly --setPhysicsModelParameters pdfindex_cat$i=1 --toysFrequentist -t 1000 --expectSignal 0 --saveToys -m 125 --freezeNuisances pdfindex_cat$i -n _cat${i}_truth_idx1_mu0 ; done
#for i in {0..29}; do combine cat${i}.root -M MaxLikelihoodFit  --setPhysicsModelParameters pdfindex_cat${i}=0 --freezeNuisances=pdfindex_cat${i} --toysFile higgsCombine_cat${i}_idx1_mu1.GenerateOnly.mH125.123456.root  -t 1000 -n _cat${i}_idx1_fit0_mu0 ; done


for i in 3 ; do combine cat${i}_bkg.root -M GenerateOnly --toysFrequentist -t 1000 --expectSignal 1 --saveToys -m 125 -n _cat${i}_amcatnlo_mu1 ; done

for i in 3; do combine cat${i}.root -M MultiDimFit  --setPhysicsModelParameters MH=125 -m 125 --freezeNuisances=MH --toysFile higgsCombine_cat${i}_amcatnlo_mu1.GenerateOnly.mH125.123456.root -t 1000 -n  _cat${i}_mu1   --robustFit=1 --forceRecreateNLL --cminRunAllDiscreteCombinations --cminDiscreteMinTol=0.0001 ; done | tee multidim.log

cat multidim.log | grep RooRealVar | tr -s ' ' | cut -d ' ' -f 3,5 >> multidimtree.txt


root [0] TTree *t=new TTree("t","t")
root [1] t->ReadFile("multidimtree.txt","mu:muErr",' ')
