#!env python
import os,sys
import re

from optparse import OptionParser
usage='''compareSync.py file1 file2
    Hmm version. files have only eventNum
	the files need to have event[,] [field1] [field2]
	separator: , space :
	'''
parser = OptionParser(usage=usage)
parser.add_option("-k","--field",type="int",help="field to compare [%default]",default=-1) 
(opts,args) = parser.parse_args()


def ReadFile(name,field=-1):
	R={}
	In=open(name)
	for line in In:
		l=line.split('#')[0]
		l=re.sub(',',' ',l)
		l=re.sub(':',' ',l)
		l=re.sub('\n','',l)
		try:
			e=l.split()[0]
			#lumi=l.split()[1]
			#e=l.split()[2]
			if field>=0: 	
				val = float(l.split()[0+field])
				R[ (e) ] = val
			else:
				R[ (e) ] = 1
		except IndexError:
			print "Ignoring line: '"+line+"'"
	return R

def PrintSet(s):
	for key in s:
		print key
	return

d1=ReadFile(args[0],opts.field)
d2=ReadFile(args[1],opts.field)

s1=set(d1.keys())
s2=set(d2.keys())

print "Elements in",args[0],"and not in",args[1]
PrintSet(s1-s2)
print "Elements in",args[1],"and not in",args[0]
PrintSet(s2-s1)

maxDiff=0.
DiffKey=None
maxRelDiff=0.
RelDiffKey=None

if opts.field >=0:
	print "------ DIFF -------"
	for key in (s1&s2) :
		print "Key=(",key,") Delta=",d1[key]-d2[key]
		if maxDiff< d1[key]-d2[key]:
			DiffKey=key
			maxDiff=max(maxDiff,d1[key]-d2[key])
		if maxRelDiff<2*(d1[key]-d2[key])/(d1[key]+d2[key]):
			DiffRelKey=key
			maxRelDiff=max(maxRelDiff, 2*(d1[key]-d2[key])/(d1[key]+d2[key]) )
	print "-------------------"

print "------ SUMMARY ----"
print "common",len( s1&s2) 
print "only s1",len(s1-s2)
print "only s2",len(s2-s1)
if opts.field >=0:
	print "maxDiff=",maxDiff, "on key=",DiffKey
	print "maxRelDiff=",maxRelDiff*100,"%","on key",DiffRelKey
print "-------------------"


