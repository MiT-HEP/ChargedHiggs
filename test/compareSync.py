#!env python
import os,sys
import re

from optparse import OptionParser
usage='''compareSync.py file1 file2
	the files need to have run[,] lumi[,] event[,] [field1] [field2]
	MIT can be parsed with:
	cat log.txt | grep Trigger | cut -d' ' -f 2 | tr -d '(' | tr -d ')' | sed 's/,/, /g' > trigger-mit.txt 
	separator: , space :
	'''
parser = OptionParser(usage=usage)
parser.add_option("-k","--field",type="int",help="field to compare [%default]",default=-1) 
parser.add_option("","--select",type="string",help="min,max in the first file field for plot[%default]",default="") 
(opts,args) = parser.parse_args()


def ReadFile(name,field=-1):
    First=None
    R={}
    In=open(name)
    for line in In:
        if line.startswith('#') and First==None and field >=0: 
            l=re.sub('#\+','#',line)
            l=l.split('#')[1]
            l=re.sub(',',' ',l)
            l=re.sub(':',' ',l)
            l=re.sub('\|',' ',l)
            l=re.sub('\n','',l)
            l=re.sub('\ +',' ',l)
            try:
                print "-> try to First read line",l,l.split(), 2+field
                First=l.split()[2+field]
            except:pass
        l=line.split('#')[0]
        l=re.sub(',',' ',l)
        l=re.sub(':',' ',l)
        l=re.sub('\n','',l)
        try:
            run=l.split()[0]
            lumi=l.split()[1]
            e=l.split()[2]
            if field>=0:
                val = float(l.split()[2+field])
                R[ (run,lumi,e) ] = val
            else:
                R[ (run,lumi,e) ] = 1
        except IndexError:
            print "Ignoring line: '"+line+"'"
            print "-> Requested field",field+2,"while available are only",len(l.split())
    return R,First

def ReadFileOld(name):
	R=set([])
	In=open(name)
	for line in In:
		l=line.split('#')[0]
		l=re.sub(',',' ',l)
		l=re.sub(':',' ',l)
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

d1,f1=ReadFile(args[0],opts.field)
d2,f2=ReadFile(args[1],opts.field)

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
        print "Key=("+','.join(key)+") Delta=",d1[key]-d2[key]
        if maxDiff< d1[key]-d2[key]:
            DiffKey=key
            maxDiff=max(maxDiff,d1[key]-d2[key])
        try:
            if maxRelDiff<2*(d1[key]-d2[key])/(d1[key]+d2[key]):
                DiffRelKey=key
                maxRelDiff=max(maxRelDiff, 2*(d1[key]-d2[key])/(d1[key]+d2[key]) )
        except ZeroDivisionError:
            pass
    import ROOT
    #delta=ROOT.TH1D("delta","delta",100,-maxDiff,maxDiff)
    delta=ROOT.TH1D("delta","delta",1000,-1,1)
    for key in (s1&s2): 
        if d1[key] == 0 or d2[key]==0: continue
        if opts.select !="" and (d1[key] < float(opts.select.split(',')[0]) or d1[key] > float(opts.select.split(',')[1])): continue
        delta.Fill(d1[key]-d2[key])
    delta.Draw("HIST")
    print "Draw delta betewen", f1,f2
    if f1 and f2: delta.GetXaxis().SetTitle(f1+"-"+f2)
    raw_input("ok?")
    print "-------------------"

print "------ SUMMARY ----"
print "common",len( s1&s2) 
print "only s1",len(s1-s2)
print "only s2",len(s2-s1)
if opts.field >=0:
    print "maxDiff=",maxDiff, "on key=",DiffKey
    print "maxRelDiff=",maxRelDiff*100,"%","on key",DiffRelKey
print "-------------------"


