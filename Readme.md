# Charged Higgs Analysis Code

### Table of Contents
- [Installation](#user-content-installation)
- [Submission](#user-content-submission-to-batch)
- [Local Run](#user-content-local-run)
- [Configuration](#user-content-configuration)
- [Datacards](#user-content-datacards)
- [Development](#user-content-development)


## Installation
* Install Nero package and compile the Core (_not_ scram)
```
BASE=$PWD [ $CMSSW_BASE/src ]
cd $BASE
git clone git@github.com:MiT-HEP/NeroProducer.git
cd NeroProducer/Core
make -j 16
```
* Install the package
```
cd $BASE
git clone git@github.com:MiT-HEP/ChargedHiggs.git ChargedHiggs
cd ChargedHiggs
make -j 16
```
* Optional: install combine
```
cd $CMSSW_BASE/src
cmsenv
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
git checkout 31cc693 ## ( 74x-root6 )
scram b -j 16 
```

## Submission to Batch
* Submission to the batch system is performed with the script/submit.py:
```
$python script/submit.py  --help
Usage: submit.py [options]

Options:
  -h, --help            show this help message and exit
  -i INPUT, --input=INPUT
                        Input Configuration file
  -d DIR, --dir=DIR     Directory where to write the configuration. WARNING OVERWRITE THE DIRECTORY!!!
  -v DEBUG, --debug=DEBUG
                        Debug Verbosity. From 0-3. Default=0
  -n NJOBS, --njobs=NJOBS
                        Number of Job to submit
  -q QUEUE, --queue=QUEUE
                        Queue
```
For example:
```
python script/submit.py -i dat/config.dat -v 3 -n 30 -q 1nh -d mysub/sub1
```

* You can check the status with the command (after the job started):
```
python script/submit.py -d mysub/sub1 --status
```

* you can resubmit jobs i-j,k or the failed job with the commands:
```
python script/submit.py -d mysub/sub1 --resubmit [ -j fail]
python script/submit.py -d mysub/sub1 --resubmit -j i-j,k
python script/submit.py -d mysub/sub1 --resubmit -j run
```

## Local run
* To run locally, you can call directly the looper with:
```
$python python/Loop.py --help
Usage: Loop.py [options]

Options:
  -h, --help         show this help message and exit
  -v, --verbose      Verbose
  -d DAT, --dat=DAT  Configuration file
```
For example:
```
python python/Loop.py -v -d dat/config.dat
```

## Configuration

### Configuration file
configuration file is a list of keys, followed by a '=' sign.
The content depends on the key itself.
* 'include': load the content of filename.dat
* 'Files': the files to be run are file1,file2,/store..
* 'addfiles': add these files to the Files to be run on.
* 'Output': output file name will be...
* 'MCDB': the Monte Carlo database file is ...
* 'SFDB': the scale factor database file is...
* 'Smears = @SmearBase,JER,JES': Book the systematics smears, by name, or by constructor (@)
* 'Analysis = AnalysisBase,Analysis2 ': run the following analysis (that inherths from AnalysisBase) in that specifici order
* 'config = AnalysisBase|a=1,b=2,c(3)': configure the analysis setting the public members, or running the public member functions ...


### MC Database:
* The MC database should contain the list of MCs with the info for the normalization. Can be dumped with the script:
```
$python python/mcDatabase.py --help
Usage: usage

Options:
  -h, --help            show this help message and exit
  -e EOS, --eos=EOS     eos directory to scout, will not read the files in the
                        pSet
  -x XSEC, --xsec=XSEC  Use external cross-section
  -l LABEL, --label=LABEL
                        MC label
  -f FILE, --file=FILE  mc_database file name
  -r, --recursive       do same for each subdir
```
e.g.:
```
python python/mcDatabase.py -r -e /store/user/amarini/Nero/v0.4/
```

### PU Reweighting
* Generate the PU distributions (1D)
```
python python/puDistributions.py -r -e /store/user/amarini/Nero/v0.4/  -f aux/pileup.root
```
* or Run Dependent( RD )
```
python python/puDistributions.py -r -e /store/user/amarini/Nero/v0.4/ --run=0,1000,2000,3000 -f aux/pileupRD.root
```

* add to these files the pileup distributions in data, with names:
   * PU-pileup
   * PU-pileupUp
   * PU-pileupDown
* or for the RD model:
   * RD-pileup_run1_run2
   * RD-pileupUp_run1_run2
   * RD-pileupDown_run1_run2

* modify the configuration file for using these files. The pileupRun and pileupLumi keys controls the RD vs 1D model.

### SF Database
The Scale factor database, contains a list of scale-factors associated to a label. This are constructed in the looper and can be used in the analysis.

### Datacards

* Merge the output files (if necessary):
```
python script/submit.py --hadd -d mysub/sub1
```
* Dump datacard **shape** based:
```
python python/dumpDatacard.py -i ChHiggs.root --hist=CutFlow/CutFlow -f datacard_2.txt -s 
```
* or dump **cut and count** datacard:
```
python python/dumpDatacard.py -v -f datacard.txt -i ChHiggs.root --hist CutFlow/CutFlow --bin 2 --syst=JER
```

## Development

### Code structure

```
**********    *********    **********
* Looper * -> * Event * -> * Weight * -> MCDB, SF
**********    *********    **********

                                   *******
                        -> vector< * Jet * >
                                   *******
                        -> ...


                      *************  
           -> vector< * SmearBase * >      : set the actual systematic smear. Object or corrector should be awared.
                      *************  

                      *************  
           -> vector< * CorrectorBase * >  : change the content of the event, correcting objects like met, energy ...
                      *************  

                      ****************
           -> vector< * AnalysisBase * >   : perform analysis. Usually do not have rw access to the objects. To check.
                      ****************

```
