#!env python

#import os,sys
from subprocess import call, check_output
#import re
import threading
import time

from optparse import OptionParser, OptionGroup

usage = "usage: %prog [options] exe1 exe2 ..."
parser=OptionParser(usage=usage)
parser.add_option("-n","--ncore" ,dest='ncore',type='int',help="Input Datacard [Default=%default]",default=10)

def call_exe(exe):
	print " -> Calling "+ exe
	call(exe,shell=True)
	print " -> Done "+exe

(opts,args)=parser.parse_args()

threads=[]

for exe in args:
	while threading.activeCount() >= opts.ncore: 
		print "sleep ...",
		time.sleep(3)
		print "wake up"
	t = threading.Thread(target=call_exe, args= (exe,)  )
	t.start()
	threads.append( t )

for t in threads:
	t.join()

print "Done"

