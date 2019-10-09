
from optparse import OptionParser

usage='''Usage: %prog [options] inputFile.
        Use e.g. https://dreampuf.github.io/GraphvizOnline/ (fdp) to convert to image.
        Original Author: Andrea Carlo Marini. July 2018.

        Known limitation/TODO:
        * loadSnapshot
        * if '_' are present in proc/channels may not fetch correctly names
    '''
parser=OptionParser(usage=usage)
parser.add_option("-w","--wsname",help="Ws name [%default]",default=None)
parser.add_option("-m","--modelname",help="Model Name [%default]",default="model_s")
parser.add_option("-x","--exclude",help="Exclude Regexp. can be specified multiple times.",action='append',default=[])
parser.add_option("-v","--verbose",help="Verbosity [0-3] [%default]",type='int',default=1)
parser.add_option("-L","--lib",action='append',help="Load libraries (specify more times) [%default]",default=[])
#parser.add_option("-i","--input",help="Input File [%default]",default=None)
opts,args=parser.parse_args()

import sys,os,re

sys.argv=[]
import ROOT
for lib in opts.lib:
    print "-> Loading library",lib
    ROOT.gSystem.Load(lib)

#################### UTILS & FUNCTIONS ##################33
def Log(level, msg):
    level=max(min(level,3),0)
    if level >= opts.verbose: return
    lvToStr=["ERROR","WARNING","INFO","DEBUG"]
    fout=sys.stderr
    print>>fout, "["+lvToStr[level]+"]",msg

def pruneVars(l):
    ''' Prune list of vars'''
    retval=[]
    for v in l:
        if re.search('_In$',v): continue
        to_exclude=False
        for x in opts.exclude: 
            if re.search(x,v): to_exclude=True
        if to_exclude:continue
        retval.append(v)
    return retval

def getClients(node):
    clients = []
    iter = node.clientIterator()
    while True:
        client = iter.Next()
        if client == None:
            break
        clients.append(client)
    return clients

def getServers(node):
    servers = []
    iter = node.serverIterator()
    while True:
        server = iter.Next()
        if server == None:
            break
        servers.append(server)
    return servers

def rooArgSetToList(argset):
    """creates a python list with the contents of argset (which should be a RooArgSet)"""
    it = argset.createIterator()

    retval = []
    while True:
        obj = it.Next()

        if obj == None:
            break

        retval.append(obj)

    return retval

def findWorkspaces(topdir):
    retval=[]
    for key in topdir.GetListOfKeys():
        obj = topdir.Get(key.GetName())
        if isinstance(obj,ROOT.RooWorkspace):
            retval.append(obj)
        if isinstance(obj,ROOT.TDirectory):
            print "-> entering directory",key
            retval.extend( findWorkspace(obj))
    return retval

def getObj(ws, name):
    Log(3,"Getting obj:"+name)
    obj = ws.obj(name)

    if obj == None:
        raise IOError("object '" + name + "' not found in workspace " + ws.GetName())

    return obj

def getAllVars(name):
    ''' Return a list of RooRealVars that the object name depends on'''
    deps=[]
    if isinstance(name,ROOT.RooRealVar): deps.append(name.GetName()) ## if name is a var, include it
    servers=getServers(name)
    for v in servers:
        if isinstance(v,ROOT.RooRealVar):
            deps.append(v.GetName())
        else:
            deps_v = getAllVars(v)
            deps.extend(deps_v)
        deps=list(set(deps)) #remove duplicates
    return pruneVars(list(set(deps)) )

def isConstrained(v):
    '''Return true if variable is (directly) Constrained by SimpleGaussianConstrain'''
    for c in getClients(v):
        if isinstance(c,ROOT.SimpleGaussianConstraint): return True
    return False


########### GET WS #################
fname=args.pop(0)
fIn=ROOT.TFile.Open(fname)
if fIn==None: raise IOError("Unable to open file:"+fname)
if opts.wsname==None:
    workspaces=findWorkspaces(fIn)
    if len(workspaces) !=1 : raise ValueError( "Ws error. Found" + "%d"%len(workspaces) + "workspaces instead of 1. Use -w/--wsname option.")
    ws=workspaces[0]
else:
    ws=fIn.Get(opts.wsname)
    if ws==None: raise IOError("Unable to get '"+opts.wsname + "' in '"+fname+"'")

## now ws is defined
model=getObj(ws,opts.modelname)

multicat=False
indexname=None
for server in getServers(model):
    #(0x63c02e0,V-) RooCategory::CMS_channel "CMS_channel"
    if isinstance(server,ROOT.RooCategory):
        indexname=server.GetName()
        multicat=True
        break

datacard={}
## get POIs
datacard['poi']=[x.GetName() for x in rooArgSetToList(ws.set("POI"))]
datacard['vars']=getAllVars(model)
datacard['counts']={} ## how many processes share this var

## get channels
if multicat:
    chan=getObj(ws,indexname)

maxch= chan.numTypes() if multicat else 1
for ich in range (0,maxch):
    datacard[ich]={"ich":ich}
    if multicat:
        chan.setIndex(ich)
        Log(2,"Channel %d %s"%(ich,chan.getLabel()))
        model_cat = model.getPdf(chan.getLabel())
        datacard[ich]["ch_name"]=chan.getLabel()
    else:
        Log(2,"No Channel")
        model_cat = model
        datacard[ich]["ch_name"]="No Channels"
    
    ##
    datacard[ich]["ch_vars"]=getAllVars(model_cat)

    model_addpdf=None
    model_realsum=None
    isRooRealSum=False
    for pdf in getServers(model_cat):
        if isinstance(pdf,ROOT.RooAddPdf):
            model_addpdf=pdf
            break
        if isinstance(pdf,ROOT.RooRealSumPdf):
            for x in getServers(pdf):
                if isinstance(x,ROOT.CMSHistErrorPropagator):
                    model_addpdf=x
                    model_realsum=pdf
                    isRooRealSum=True
                    break
            break
    if model_addpdf==None: raise ValueError("Unable to find RooAddPdf in ich=%(ich)d,ch_name=%(ch_name)s"%datacard[ich])

    procs=[]
    pdf_type=[]
    dependances=[] #double list
    signals=[]

    coef_list=model_addpdf.coefList()
    if isRooRealSum:
        pdf_list=model_addpdf.funcList()
    else:
        pdf_list=model_addpdf.pdfList()
    
    for ipdf in range(0,len(pdf_list)):
        pdf= pdf_list[ipdf]
        norm= coef_list[ipdf]
        name=pdf.GetName()
        if name.split('_')[-1]=='morph':
            proc = name.split('_')[2]  ## TH1
        else:
            proc = name.split('_')[1] 

        procs.append(proc)
        pdf_type.append(pdf.ClassName())
        varlist=list(set(getAllVars(pdf)+getAllVars(norm)))
        dependances.append(varlist) 
        issig=False
        for sig in datacard['poi']:
            Log(3,"pdf is:"+name +"\n"+"  :"+sig+" in "+str(dependances))
            if sig in varlist:
                issig=True
                break
        signals.append(issig)

        for v in varlist:
            if v not in datacard['counts']: datacard['counts'][v]=0
            datacard['counts'][v] +=1
    #procs=list(set(procs))
    datacard[ich]["processes"]=procs
    datacard[ich]["proc_vars"]=dependances
    datacard[ich]["proc_pdf"]=pdf_type
    datacard[ich]["proc_sig"]=signals

#GetName ClassName (node)
#Compute unique vars

##########
#import json
print "----------- DATACARD -----------"
print datacard
print "--------------------------------"

fout = sys.stdout
print "----------- GRAPHVIZ -----------"
print>>fout, "digraph g{"
print >>fout,"label = <Credits to: Andrea Carlo Marini<br/>July 2018>"
print >>fout,"labelloc = \"b\""
print >>fout,"labeljust=\"l\""
#print>>fout, "rankdir = TB"
print >>fout, "outputorder=\"edgesfirst\""
print >>fout
print >>fout,"subgraph cluster_vars{"
print >>fout,"label=\"\""

for var in datacard["vars"]:
    v=ws.var(var)
    if var not in datacard['counts']: datacard['counts'][var]=0
    if datacard['counts'][var]==1: continue ## exclude unique
    if datacard['counts'][var]==0: continue ## exclude orphans
    color="lightgrey"
    if v.isConstant(): color='gold'
    if var in datacard['poi']: color='palegreen'
    #print >>fout, "%s [label=\"%s\\n%g [%g,%g] %s\",style=filled, color=%s]"%(var,var,v.getVal(),v.getMin(),v.getMax(), "C" if v.isConstant() else "F",color)
    fontcolor="color=\"blue\"" if isConstrained(v) else "color=\"black\""
    print >>fout, "%s [label=<<font %s>%s<br/>%g [%g,%g] %s</font>>,style=filled, color=%s]"%(var,fontcolor,var,v.getVal(),v.getMin(),v.getMax(), "C" if v.isConstant() else "F",color)

print "}" ## end subgraph

print >>fout

ch_colors=["khaki","palegreen","paleturquoise","lavender","plum","mistyrose"]

## line colors
#line_colors=["salmon","chocolate","orange","gold","forestgreen","aquamarine","deepskyblue","darkblue","purple","indigo","deeppink"]
line_colors=['deepskyblue', 'salmon', 'forestgreen', 'purple', 'aquamarine', 'indigo', 'deeppink', 'chocolate', 'darkblue', 'gold','orange']

#### print channels
for ich in range (0,maxch):
    print>>fout, "subgraph cluster_ch%d{"%ich
    print>>fout, "label=%s"%datacard[ich]["ch_name"]
    print>>fout, "labelloc = \"t\""
    print>>fout, "labeljust=\"c\""
    print>>fout, "style=filled"
    print>>fout, "color="+ch_colors[ich%len(ch_colors)]
    #print>>fout, "ch%d [label=\"%s\"]"%(ich,datacard[ich]["ch_name"])
    for iproc in range(0,len(datacard[ich]["processes"])):
        color="lightblue"
        if datacard[ich]["proc_sig"][iproc]: color="pink"
        print>>fout, "ch%d_proc%d [label=\"%s\\n%s\", style=filled, color=%s]"%(ich,iproc,datacard[ich]["processes"][iproc],datacard[ich]["proc_pdf"][iproc],color)
        #print>>fout, "ch%d -> ch%d_proc%d"%(ich,ich,iproc) ## create a channel node
    print>>fout, "}"
    
    for iproc in range(0,len(datacard[ich]["processes"])):
        # create list of uniques
        has_unique=False
        for var in datacard[ich]["proc_vars"][iproc]:
            v=getObj(ws,var)
            if datacard['counts'][var] != 1 : continue

            if not has_unique: print >> fout,"unique_ch%d_proc%d [ shape=none,label=<<table border=\"0\" cellspacing=\"0\">"%(ich,iproc),

            has_unique=True
            color_tag=""
            if v.isConstant(): color_tag="bgcolor=\"gold\""
            if v.GetName() in datacard['poi']: color_tag="bgcolor=\"palegreen\""
            fontcolor="color=\"blue\"" if isConstrained(v) else "color=\"black\""
            #print >>fout,"<tr>","<td",color_tag," border=\"1\"> ","%s"%var,"</td> <td",color_tag," border=\"1\">"," \"%g[%g,%g] %s\"</td>"%(v.getVal(),v.getMin(),v.getMax(), "C" if v.isConstant() else "F"),"</tr>",
            print >>fout,"<tr>","<td",color_tag," border=\"1\"> ","<font %s>"%fontcolor,"%s"%var,"</font></td> <td",color_tag," border=\"1\">","<font %s>"%fontcolor," %g[%g,%g] %s</font></td>"%(v.getVal(),v.getMin(),v.getMax(), "C" if v.isConstant() else "F"),"</tr>",
        if has_unique:print >>fout ,"</table>>]"

        for var in datacard[ich]["proc_vars"][iproc]:
            if datacard['counts'][var] == 1 : continue
            print >>fout,"ch%d_proc%d -> %s"%(ich,iproc,var), "[color=%s arrowhead=open arrowsize=0.8]"%(line_colors[iproc%len(line_colors)])
        if has_unique:
            print >>fout,"ch%d_proc%d -> unique_ch%d_proc%d"%(ich,iproc,ich,iproc),"[color=%s arrowhead=open arrowsize=1.0]"%(line_colors[iproc%len(line_colors)])

    ## print unique variables to ch, but not to process. Orphans
    has_ch_orphans=False
    for var in datacard[ich]['ch_vars']:
        if datacard['counts'][var] > 0 : continue

        v=getObj(ws,var)
        if not has_ch_orphans: print >> fout,"orphans_ch%d [ shape=none,label=<<table border=\"0\" cellspacing=\"0\">"%(ich),
        has_ch_orphans=True
        color_tag=""
        if v.isConstant(): color_tag="bgcolor=\"gold\""
        if v.GetName() in datacard['poi']: color_tag="bgcolor=\"palegreen\""
        fontcolor="color=\"blue\"" if isConstrained(v) else "color=\"black\""
        print >>fout,"<tr>","<td",color_tag," border=\"1\"> ","<font %s>"%fontcolor,"%s"%var,"</font></td> <td",color_tag," border=\"1\">","<font %s>"%fontcolor," %g[%g,%g] %s</font></td>"%(v.getVal(),v.getMin(),v.getMax(), "C" if v.isConstant() else "F"),"</tr>",
    if has_ch_orphans:print >>fout ,"</table>>]"
    if has_ch_orphans:print >>fout,"cluster_ch%d -> orphans_ch%d"%(ich,ich)

print>>fout, "}"
print "--------------------------------"
