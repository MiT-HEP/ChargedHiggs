#/bin/bash
#best for EU
#xrootd-cms.infn.it
# best for USA
#cmsxrootd.fnal.gov

REDIRECTOR=cms-xrd-global.cern.ch 

for i in input*dat;
do
	sed -i'' "s:eoscms:$REDIRECTOR:g" $i 
done
