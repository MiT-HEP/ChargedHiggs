#https://www.dropbox.com/s/cwccmbo5lun9kwt/electronreg.pdf?dl=0
#zcat QCDPurity_Sync/log*.txt.gz  | grep SYNC | sed 's/^.*:\ //g' | tr ',' ' ' | sort -n > runListQCD.txt

import os,sys

f1=open("test/mysubNew/runListQCD.txt")
f2=open("test/mysubNew/runListTauNu.txt")

dict1={}

for line in f1:
	run=int(line.split()[0])
	lumi=int(line.split()[1])
	event=int(line.split()[2])
	dict1[(run,lumi,event)] =1 

for line in f2:
	run=int(line.split()[0])
	lumi=int(line.split()[1])
	event=int(line.split()[2])

	if (run,lumi,event) in dict1:
		del dict1[(run,lumi,event)]
	else: 
		print "TauNu Only Event: ",run ,lumi,event

for run,lumi,event in dict1:
	print "QCD Only Event: ", run ,lumi, event


