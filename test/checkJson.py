import os,sys
import json

from optparse import OptionParser

usage="checkJson -j Json run lumi: check if run and lumi are in the json file "
parser = OptionParser(usage=usage)
parser.add_option("-j","--json",dest="json",type='string',help="Json file",default='aux/Cert_13TeV_16Dec2015ReReco_Collisions15_25ns_JSON_v2.txt')
(opts,args) = parser.parse_args()


jstring = open(opts.json).read()
goodlumis=json.loads( jstring )

run=int(args[0])
lumis=int(args[1])
print "Checking RUN=",run,"LUMIS",lumis

for run_ in goodlumis.keys():
       for lumis_ in goodlumis[run_]:
	   if int(run_)==run and  int(lumis_[0])<= lumis and lumis<=int(lumis_[1]):
		   print "OK"
		   exit(0)

print "NO"
