#!env python
import os,sys
import re

from optparse import OptionParser
usage='''compareSync.py file1 file2
	the files need to have run[,] lumi[,] event
	MIT can be parsed with:
	cat log.txt | grep Trigger | cut -d' ' -f 2 | tr -d '(' | tr -d ')' | sed 's/,/, /g' > trigger-mit.txt 
	'''
parser = OptionParser(usage=usage)
(opts,args) = parser.parse_args()


def ReadFile(name):
	R=set([])
	In=open(name)
	for line in In:
		l=line.split('#')[0]
		l=re.sub(',','',l)
		l=re.sub('\n','',l)
		try:
			run=l.split()[0]
			lumi=l.split()[1]
			e=l.split()[2]
			R.add( (run,lumi,e) )
		except IndexError:
			print "Ignoring line: '"+line+"'"
	return R
def PrintSet(s):
	for key in s:
		print ', '.join(key)
	return

s1=ReadFile(args[0])
s2=ReadFile(args[1])


print "Elements in",args[0],"and not in",args[1]
PrintSet(s1-s2)
print "Elements in",args[1],"and not in",args[0]
PrintSet(s2-s1)

print "------ SUMMARY ----"
print "common",len( s1&s2) 
print "only s1",len(s1-s2)
print "only s2",len(s2-s1)
print "-------------------"

