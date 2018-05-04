import ROOT
import os,sys
## runs over the sync files

from optparse import OptionParser,OptionGroup
parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [%default]", default="HmmSync.root")
parser.add_option("-o","--outdir",type='string',help="OutputDir. [%default]", default="sync")
opts, args = parser.parse_args()

from subprocess import call

## extra for print_progress
from datetime import datetime
time_=None
def print_progress(done,tot,n=30,every=10000):
    global time_
    if done % every != 0  and done < tot-1: return
    a=int(float(done)*n/tot )
    l="\r["
    if a==n: l += "="*n
    elif a> 0: l +=  "="*(a-1) + ">" + " "*(n-a)
    else: l += " "*n
    l+="] %.1f%%"%(float(done)*100./tot)
    if time_ == None: time_ = datetime.now()
    else: 
        new = datetime.now()
        delta=(new-time_)
        H=delta.seconds/3600 
        M=delta.seconds/60 -H*60
        S=delta.seconds - M*60-H*3600
        H+= delta.days*24
        if H>0:
            l+= " in %dh:%dm:%ds "%(H,M,S) 
        else:
            l+= " in %dm:%ds "%(M,S) 
        S=int( (delta.seconds+24*3600*delta.days)*float(tot-done)/float(done) )
        M= S/60
        S-=M*60
        H= M/60
        M-=H*60
        l+= " will end in %dh:%dm:%ds                      "%(H,M,S)
    if a==n: l+="\n"
    print l,
    sys.stdout.flush()
    return 


dir=opts.outdir+"/"
call(["mkdir","-p", dir])

#fIn=ROOT.TFile.Open(opts.input)
#t=fIn.Get("hmm")
t=ROOT.TChain("hmm")
nfiles= t.Add(opts.input)
print "-> Considering",nfiles,"in the chain"

vbf0 = open(dir+"vbf0.txt","w")
oneb = open(dir+"oneb.txt","w")
gf = open(dir+"gf.txt","w")
vbf1 = open(dir+"vbf1.txt","w")
untag0 = open(dir+"untag0.txt","w")
untag1 = open(dir+"untag1.txt","w")
All = open(dir+"all.txt","w")

nentries=t.GetEntries()
print "Runnig over",t.GetEntries(),"entries"
for ientry in range(0,t.GetEntries()):
    t.GetEntry(ientry)
    print_progress(ientry,nentries)

    if not t.pass_all: continue
    if not t.pass_leptonveto: continue
    if t.mass <80 or t.mass > 85 : continue ## sync
    if t.nbjets >0 : continue
    print >>All,t.eventNum
    if t.cat %6==0:
    	print >> vbf0 , t.eventNum
    if t.cat %6==1:
    	print >> oneb , t.eventNum
    if t.cat %6==2:
    	print >> gf , t.eventNum
    if t.cat %6==3:
    	print >> vbf1 , t.eventNum
    if t.cat %6==4:
    	print >> untag0 , t.eventNum
    if t.cat %6==5:
    	print >> untag1 , t.eventNum

##$( root HmmSync.root > all_roc.txt)<<EOF
##hmm->Scan("eventNum","pass_all")
##.q
##EOF

