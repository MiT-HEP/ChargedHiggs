#Post-fit translation on additional distributions

## Requirements
* Nominal datacard
* Second datacard with the nuisances you want to account for
* Run the postfit point.

## Run the toys 
* Get the postfit distribution of the nominal datacard

~~~bash
combine -M MultiDimFit [options] --saveWorkspace -n postfit datacard
combine -M MultiDimFit --rMin=-20 --rMax=20  --setParameters r=0 --freezeParameters r --saveWorkspace -n postfit datacard.root
~~~

* Throw the toys to draw the post fit distributions postfit-translation.sh

~~~bash
./post_fit_translation.sh index $PWD mhhat <params> <freeze> <excludeNuis>
mkdir toys
for i in {0.1000}; do bsub -q 8nh $PWD/post_fit_translation.sh $index $PWD/toys 125 r=0 r ; done
~~~

## Make the plot
*
