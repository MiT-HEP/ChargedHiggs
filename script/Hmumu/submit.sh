

for s in {0..49} ; do export SEED=$RANDOM ; echo "using $SEED" ; for i in {0..12}; do bsub -q 8nh -J Bias_cat${i} $PWD/cmd_2d.sh $i $SEED ; done  ; done
