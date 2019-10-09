import os,sys,re

from optparse import OptionParser

usage='''python mergesyst.py [options] 2016=file 2017=file 2018=file
Example: python mergesyst.py -u -o cms_datacard_hmumu_RunII_syst.txt jes_2016.txt jes_2017.txt jes_2018.txt
'''
parser=OptionParser(usage=usage)
parser.add_option("-u","--uncorrelate",help="Uncorrelate",action='store_true',default=False)
parser.add_option("-o","--output",help="Outputfile",default="cms_datacard_hmumu_RunII_syst.txt")
parser.add_option("","--hmm",dest="hmm",type="string",help="HmmConfig instance [%default]",default="hmmExCatMjj")
opts,args=parser.parse_args()

print "-> Looking for basepath"
basepath = ""
mypath = os.path.abspath(os.getcwd())
while mypath != "" and mypath != "/":
    if "ChargedHiggs" in os.path.basename(mypath):
        basepath = os.path.abspath(mypath)
    mypath = os.path.dirname(mypath)
print "-> Base Path is " + basepath
sys.path.insert(0,basepath)
sys.path.insert(0,basepath +"/python")
from hmm import *

#config=hmmAutoCat
config= eval(opts.hmm)
#config.Print()

print "-> Parsing datacard",opts.output
fDatacard=open(opts.output,"r")
## categories from dump syst are ordered as in python/hmm
## from 0 to N-1

m=None ## Map from ch->cat
obs=None  # ch
exp_cat=None
exp_proc=None

for line in fDatacard:
    if 'Combination of' in line: m=re.sub('\n','',re.sub('Combination of','',line)).split()
    if line.startswith('bin') and obs!=None: exp_cat=re.sub('bin\ *','',line).split()
    if line.startswith('process') and exp_proc==None: exp_proc=re.sub('process\ *','',line).split()
    if line.startswith('bin') and obs==None: obs=re.sub('bin\ *','',line).split()

fDatacard.close()

fDatacard=open(opts.output,"a")

if len(args) < 3: print "too few args", args
f2016=None
f2017=None
f2018=None

for f in args:
    if '2016' in f:f2016=open(re.sub('2016=','',f),'r')
    if '2017' in f:f2017=open(re.sub('2017=','',f),'r')
    if '2018' in f:f2018=open(re.sub('2018=','',f),'r')

smap={}
for l in f2016:
    name=l.split()[0]
    name=re.sub("cms_",'',name)
    if opts.uncorrelate:
        name+="2016"
    if name not in smap: smap[name]={}
    smap[name][2016]=l
for l in f2017:
    name=l.split()[0]
    name=re.sub("cms_",'',name)
    if opts.uncorrelate:
        name+="2017"
    if name not in smap: smap[name]={}
    smap[name][2017]=l
for l in f2018:
    name=l.split()[0]
    name=re.sub("cms_",'',name)
    if opts.uncorrelate:
        name+="2018"
    if name not in smap: smap[name]={}
    smap[name][2018]=l

#for idx,cat in enumerate(config.categories):
#   for pi,proc in enumerate(config.processes):

#m=None ## Map from ch->cat
#obs=None  # ch
#exp_cat=None
#exp_proc=None

for sname in smap:
    sline=sname + ' lnN '

    for idx in range(0,len(exp_cat)):
        catnew=exp_cat[idx]
        procnew=exp_proc[idx]
        catold=re.sub('\.txt','',m[ obs.index(catnew) ]) #ch1->cat0
        procold=re.sub('_.*','',procnew) #remove year
        procold=re.sub('qqH','VBF',procold)
        procold=re.sub('ggH','GluGlu',procold)
        
        if 'BKG' in procnew: 
            sline += ' - '
            continue

        print "-> Considering procold",procold,"procnew",procnew
        print "-> Considering catold",catold,"catnew",catnew

        year=int(re.sub('.*_','',procnew))
        procidx=config.datacard_procs.index(procold) # this has also BKG
        catidx=config.categories.index(catold)
        #processes= config.datacard_procs
        gidx=0
        for idx,cat in enumerate(config.categories):
           for pi,proc in enumerate(config.processes):
               if cat == catold and procold == proc: 
                   break
               gidx+=1
               if cat != catold and procold != proc: continue
           if cat == catold: 
               break
        if year in smap[sname]:
            value=smap[sname][year].split()[gidx+2] # name and lnN
        else: value ="-"
        sline += " "+value + " "
    print >>fDatacard,sline
