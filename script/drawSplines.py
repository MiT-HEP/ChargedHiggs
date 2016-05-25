import sys, os
import re
from optparse import OptionParser,OptionGroup

parser=OptionParser()
parser.add_option("","--sfdb" ,dest='sfdb',type='string',help="SF DB [%default]",default="aux/RMet80.txt")

opts,args=parser.parse_args()

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')

from ParseDat import *
import ROOT as r
r.gROOT.SetBatch()

sfdb=ReadSFDB(opts.sfdb)
