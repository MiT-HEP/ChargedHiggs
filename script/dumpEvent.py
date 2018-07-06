#!/usr/bin/env python

import os, sys
from optparse import OptionParser, make_option

ROOTimported = False

import ROOT
ROOT.gROOT.SetBatch();

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

#----------------------------------------------------------------------
# main
#----------------------------------------------------------------------    
from subprocess import call, check_output
import re
EOS = "/usr/bin/eos"

if __name__ == "__main__" :
    parser = OptionParser(option_list=[
        ## doesn't work
        make_option("-d", "--sourcedir",
                    action="store", type="string", dest="sourcedir",
                    default="",
                    help="", metavar=""
                    ),
        make_option("-e", "--eos",
                    action="store", type="string", dest="eos",
                    default="",
                    help="", metavar=""
                    ),
        make_option("-o", "--output",
                    action="store", type="string", dest="outname",
                    default="out.txt",
                    help="", metavar=""
                    ),
        ])
    
    (options, args) = parser.parse_args()

    ## should work but doesn't unmount 
    if options.eos is not "":
        cmd = EOS+ " find -f " + options.eos
        outputList = check_output(cmd,shell=True)
        fileList0 = outputList.split() ## change lines into list
        removed = [ f for f in fileList0 if '/failed/' in f ]
        for f in removed:print>>sys.stderr, "ignoring failed file:",f
        filelist = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]
    else:
        filelist = sys.argv[1:]
   
    print >>sys.stderr, "FileList len is",len(filelist)


    out = open(options.outname,"w") 
    nentries=0
    for ifile,f in enumerate(filelist):
        print_progress(ifile,len(filelist),30,10)
        fIn=ROOT.TFile.Open(f)
        if fIn==None: raise IOError("Unable to open file:"+f)
        t=fIn.Get("nero/all")
        if t==None: raise IOError("Unable to fetch nero/all tree")
        nentries+=t.GetEntries()
        for entry in range(0,t.GetEntries()):
            t.GetEntry(entry)
            print >> out,t.runNum,t.lumiNum,t.eventNum
    print
    print "Run on",nentries,"ntries"
    out.close()
