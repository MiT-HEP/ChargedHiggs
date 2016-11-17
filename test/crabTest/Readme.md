#Instructions for running on crab

* Make loose requirements on the n. of events processed per second (in python/Loop.py)
* Make a dryrun submit directory 
* Copy inside the following files:

> crabChargedHiggs.py
> ChargedHiggsCrab.sh
> pset.py
> change.sh

* Edit crabChargedHiggs.py to have a suitable output dir, white list, ecc...
* Edit change.sh to have the proper AAA redirect (US, EU) ... 

> ./change.sh

* Submit on crab 

> crab submit crabChargedHiggs.py
