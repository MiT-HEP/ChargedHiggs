#!/bin/bash

#environment is set
#pset is technically there
echo "*** Runnig JobNumber $1 ***"

#datNum=$(echo  "$1-1"| bc )
datNum="$(($1-1))"
echo "-> datNum=$datNum"

##Create a ChHiggs Directory
mkdir ChargedHiggs
# move everything inside there
# don't move script sh
mv package* *dat *py ./ChargedHiggs

cd ChargedHiggs

## UNTAR
tar -xzf package.tar.gz

## WRITE A FILE
echo "#### Automatically created ####" > myconfig.dat
echo "include=input${datNum}.dat" >> myconfig.dat
echo "Output=ChargedHiggsFwk.root" >>myconfig.dat

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bin/bare

#ln -s interface ../
ln -s ChargedHiggs/interface ../interface

echo "-> LS"
ls -l
echo "-> LS .."
ls -l ..
echo "-> LS ../interface"
ls -l ../interface

echo  "-> Runnig Loop"

python python/Loop.py -v -d myconfig.dat 2>&1 | gzip > log.txt.gz

EXITCODE=${PIPESTATUS[0]}

echo "-> tail of log file"
zcat log.txt.gz | tail -n 100

echo "-> exiting procedure"
[ $EXITCODE != 0 ] && { echo "FAILED" ; zcat log.txt.gz | tail -n 100 | python python/FrameworkJobReportDumper.py --status=$EXITCODE  ;}

[ $EXITCODE == 0 ] && { echo "SUCCESS" ; python python/FrameworkJobReportDumper.py; }


cd ..
mv ChargedHiggs/*xml ./
mv ChargedHiggs/*json ./

echo "Finished At:"
date

#exit $EXITCODE
# no exit, otherwise will not do the json job report
