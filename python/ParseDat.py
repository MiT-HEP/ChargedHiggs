import os,sys,re
import math
from subprocess import call,check_output
import ROOT


def FindBasePath(verbose=False):
     if verbose: print "-> Looking for basepath"
     basepath = ""
     mypath = os.path.abspath(os.getcwd())
     while mypath != "" and mypath != "/":
             if "ChargedHiggs" in os.path.basename(mypath):
                     basepath = os.path.abspath(mypath)
             mypath = os.path.dirname(mypath)

     if verbose: print "-> Base Path is " + basepath
     sys.path.insert(0,basepath)
     sys.path.insert(0,"./")
     return basepath

def Default():
	config = {}
	config['config'] = {}
	config['Loader'] = "LoadNero"
	return config;


def BoolKey(value):
	value = value.lower()
	if 'no' in value: return False
	if 'yes' in value: return True 
	if 'false' in value : return False
	if 'true' in value : return True
	if value =='n' : return False
	if value =='y': return True
	return False

## TODO, make a class for the parser
subdict={}

def StringKey(value):
	try:
		if '%' in value: value = value%subdict
	except ValueError:
		print "Unable to substitute in value",value,"with dict",subdict
		raise ValueError
	value = re.sub('\n','',value)
	return value

def FloatKey(value):
	return float(value)

def IntKey(value):
	return int(value)

def vStringKey(value):
	r=[]
	for s in value.split(','):
		r.append( StringKey(s) )
	return r

def vFloatKey(value):
	s = vStringKey(value)
	r = [ FloatKey(v) for v in s if v!= '' ]
	return r

def vIntKey(value):
	s = vStringKey(value)
	r = [ IntKey(v) for v in s if v!= '' ]
	return r

def ParseDat(name):
	''' Parse configuratino File '''
	print "<-> Parsing input file", name

	try:
		stream = open(name)
	except IOError:
		base= FindBasePath() ## make sure to be able to open file correctly
		stream = open( base +"/" + name) 

	config = Default()
	for line in stream:
		l = line.split('#')[0]
		l = re.sub(' ' ,'', l )
		if len(l.split()) == 0 : continue;
		l.split()[0]
		if  l =='': continue;

		key = l.split('=')[0]
		value = ''
		if '=' in l : value = '='.join(l.split('=')[1:])
		value=re.sub('~',' ',value)
		######### BOOL  ###########
		if key == "Dump" or key=="Final":
			config[key]=BoolKey(value)
		######### SUB ######
		if key == 'sub':
			## make sure that is well formatted
			if value.split('|')[0] in subdict and subdict[value.split('|')[0] ] != StringKey(value.split('|')[1]):
				print "WARNING: overriding substitution:",value.split('|')[0],"-> '" + StringKey(value.split('|')[1]) + "' (old value was '"+ subdict[value.split('|')[0] ]+"')" 
			subdict[value.split('|')[0] ]= StringKey( value.split('|')[1] )
		######### STRING ###########
		if key == 'MCDB' or key =='SFDB' or key =='Output' or key == 'pileup' or key =='DumpDir' or key == 'Loader':
			config[key] = StringKey(value)

		####### V STRING ##########
		if      key=='Files' \
			or key == 'Analysis' \
			or key == 'Smear'  \
			or key == 'Correct'  \
			or key == 'Branches': 
			config[key] =  vStringKey(  value   )
	
		if key=='addfiles':
			if 'Files' not in config: config['Files']=[]
			config['Files'].extend(vStringKey( value ))
		####### FLOAT ##########
		if key == 'Lumi':
			config[key]=FloatKey(value)
		####### INT ##########
		if key == 'EntryPerSecond':
			config[key]=IntKey(value)
		####### V FLOAT ##########
		if key == 'pileupLumi':
			config[key]=vFloatKey(value)

		####### V INT ##########
		if key == 'pileupRun':
			config[key]=vIntKey(value)

		########## DICT ###########
		if key == 'config': # dict
			type= value.split('|')[0]
			config[key][type] = vStringKey(  '|'.join(value.split('|')[1:])  )  
		if key == 'addConfig': # dict
			key = 'config'
			type= value.split('|')[0]
			config[key][type].extend(vStringKey(  '|'.join(value.split('|')[1:])  )  )
		
		######## INCLUDE ###########
		if key == 'include':
			R = ParseDat( StringKey(value) )
			for key in R:
				if key == 'config':
					for type in R[key]:
						config[key][type] = R[key][type]
				elif key == 'Files' and 'Files' in config: ## if already exists, extend it, otherwise cgo to default (sub)
					config['Files'].extend( R['Files'] )
				elif key == 'Loader' : # LoadNero is the default and all the dat have it. FIXME
					if R[key] != "LoadNero": config[key] = R[key]
				else:
					config[key] = R[key]
	return config

def FindHadoop(name,mount="/mnt/hadoop/cms"):
	if os.path.isfile(name): return [name] ## file exists
	cmd = "find %s/%s -type f "%(mount,name)
	list=check_output(cmd, shell=True).split()
	removed = [f for f in list if '/failed/' in f ]
	for f in removed:
		print "ParseDat.py - FindHadoop: Ignoring failed file: '"+ f + "'"
	list = [ f for f in list if '/failed/' not in f ]
	return list

def FindEOS(name,mount=""):
	''' EOS PATH should be followed. The mount option will assume that eos is mounted in ~/eos '''
	#EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
	#EOS = "/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select"
	EOS = "/usr/bin/eos"
	# should match wildcard, that for some reason new root does not
	if '/store/' not in name and '/eos/user' not in name: return [name]
	if '/eos/cms/store/' in name: return [name] # likely already parsed
	if 'root://eoscms//' in name: return [name] # already parsed
	if 'root://eosuser//' in name: return [name] # already parsed
	if 'root://' in name: return [name] # already parsed
	if os.path.isfile(name): return [name] ## file exists
	
	userInstance=""
	if '/eos/user' in name:
		userInstance=" root://eosuser"
	cmd = EOS + userInstance+ ' find -f ' + name
	print "Runnig command:",cmd
	list = check_output(cmd ,shell=True).split()
	
	# print removed
	tot = len(list)
	removed = [ f for f in list if '/failed/' in f ] 
	for f in removed:
		print "ParseDat.py - FindEOS: Ignoring failed file: '"+ f + "'"
	# remove failed directories from crab submission
	list = [ f  for f in list if '/failed/' not in f ]

	if mount != "":
		if userInstance != "": 
			print "UNSUPPORTED: mount + instance"
			raise ValueError
		fileList = [ re.sub("/eos/cms", mount + "/cms",f) for f in list ]
	else:
		if userInstance =="":
			fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in list ]
		else:
			fileList = [ re.sub("/eos/user","root://eosuser///eos/user",f) for f in list ]

	return fileList

def FindDataset(name,mount=""): 
    DASclient = "dasgoclient -query '%(query)s'"
    cmd= DASclient%{'query':'file dataset=%s'%name}
    if mount =="":
        o=check_output(cmd,shell=True).split()
        fileList=[ 'root://eoscms//'+x for x in o ]
        if fileList == []: return
        ## try to open file0 -> Dynamically moving to AAA
        f0= ROOT.TFile.Open(fileList[0])
        if f0== None: fileList=[ 'root://xrootd-cms.infn.it//'+x for x in o]
    elif mount =="full-check":
        o=check_output(cmd,shell=True).split()
        fileList=[ 'root://eoscms//'+x for x in o ]
        if fileList == []: return
        ## try to open all files -> Dynamically moving to AAA
        for i in range(0,len(o)):
            f0= ROOT.TFile.Open(fileList[i])
            if f0== None: fileList[i]='root://xrootd-cms.infn.it//'+o[i]
    elif mount == "aaa" or mount == "AAA":
        fileList=[ 'root://xrootd-cms.infn.it//'+x for x in check_output(cmd,shell=True).split() ]
    else:
        fileList=[ mount+x for x in check_output(cmd,shell=True).split() ]
    return fileList

def PrintDat(config):
	''' Print configuration'''

	print "##########################"
	for key in config:
		if key == 'config':
			for type in config[key]:
				print key,'=',type,'|',",".join(config[key][type])
		######### STRING ###########
		elif key == 'MCDB' or key =='SFDB' or key == 'Output' or key == 'pileup':
			print key ,'=',config[key]
		######### V STRING ###########
		elif key =='Files' \
			or key == 'Analysis' \
			or key == 'Smear' \
			or key == 'Correct' \
			or key == 'Branches' :
			print key, '=', ','.join(config[key])
		######### V FLOAT/INT #########
		elif key == 'pileupLumi' \
			or key == 'pileupRun':
			tmp= [ str(n) for n in config[key] ]
			print key, '=', ','.join(tmp)

		else:
			print key , '=' , config[key]
	print "##########################"

def PrintUsage():
	print '###Configuration file###'
	print 'include = filename.dat'
	print 'Loader = LoadNero'
	print 'Files = file1,file2,file3'
	print 'addfiles = file4'
	print 'Output = file'
	print 'MCDB = file'
	print 'SFDB = file'
	print 'pileup = file'
	print 'pileupRun  = ""/ -1 / 1,10,100,100'
	print 'pileupLumi = ""/ -1 /  1.2,2.3,4.5 # partial luminosity '
	print 'branches = brancfile'
	print 'Analysis = AnalysisBase,Analysis2 ..'
	print 'Smears = @SmearBase,JER,JES'
	print 'Correct = @CorrectorBase'
	print 'config = AnalysisBase|a=1,b=2,c(3)'

def ReadMCDB(file):
	'''read and parse the MCDB file:
	    \t\t### LABEL dir Entries xSec
	'''
	R={}
	f = open(file)
	for line in f:
		l = line.split('#')[0]
		l = re.sub('\n','',l)
		if l == "": continue
		l=re.sub('^ *','',l)
		label= l.split(' ')[0]
		dir= l.split(' ')[1]
		entries = l.split(' ')[2]
		xsec = l.split(' ')[3]
		scales=[]
		pdfs=[]
		current=4
		while len(l.split(' ') )>current:
		   what=l.split(' ')[current]
		   current+=1
		   if 'PDFS' in what :
		   	for i in range(0,30):## MC_MAX_PDFS
				pdfs.append(float(l.split(' ')[current]))
				current+=1
		   elif 'SCALES' in what:
		   	for i in range(0,6): ## MC_MAX_SCALESS
				scales.append(float(l.split(' ')[current]))
				current+=1
		  
		if label in R :
			print "WARNING: Duplicate label",label
		R[label] = (dir,entries,xsec,scales,pdfs)
	return R

import json
def ReadSFDB(file,verbose=False):
	'''read and parse the SFDB file:
	    \t\t### LABEL type -- -- --- --- 
	'''
	L=[]## this is a list of sf bins
	f = open(file)
	for line in f:
	   try:
		l = line.split('#')[0]
		l = l.replace('\n','') ## delete end of line
		if l == "": continue
		l=re.sub('^ *','',l) ## remove space at the beginning
		l=re.sub('\ +',' ',l) ## squeeze
		if l == "": continue
		label= l.split(' ')[0]
		type= l.split(' ')[1]
		R={}
		R['label']= label
		R['type'] = type

		if label.lower() == 'include':
			if verbose: print "-> Reading SF from file",type ### DEBUG
			tmp = ReadSFDB(type)
			#remove from L all the key with the same label as in tmp
			labels = set([])
			new = []

			for key in tmp:
				## DEBUG
				if verbose: print " * found key:",key ### DEBUG
				labels.add( key['label'])
				new.append(key)

			for key in L:
				if key['label'] not in labels:
					new.append( key ) 
					if verbose: print " * copying key :",key ### DEBUG
				else:
					if verbose: print " * overwriting key :",key ### DEBUG
			# merge L and tmp
			L = new[:]

		elif type == 'pteta':
			pt1  = float ( l.split(' ')[2] )
			pt2  = float ( l.split(' ')[3] )
			eta1 = float ( l.split(' ')[4] )
			eta2 = float ( l.split(' ')[5] )
			sf   = float ( l.split(' ')[6] )
			err  = float ( l.split(' ')[7] )
			R['pt1']=pt1
			R['pt2']=pt2
			R['eta1']=eta1
			R['eta2']=eta2
		elif type == 'spline':
			pt = float( l.split(' ')[2])
			sf   = float ( l.split(' ')[3] )
			err  = float ( l.split(' ')[4] )
			R['pt']=pt

		elif type == 'th2f':
			R['filename'] = l.split(' ' )[2]
			if len(l.split(' ')) >= 5:
				R['type'] ='th2eff'
				type = 'th2eff'
				R['effdata'] = l.split(' ')[3]
				R['effmc'] = l.split(' ')[4]
				R['errdata'] = ""
				R['errmc'] = ""
				if len(l.split(' ')) >= 7:
					R['errdata'] = l.split(' ')[5]
					R['errmc'] = l.split(' ')[6]
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'base':
			sf  = float ( l.split(' ') [2] )
			err = float ( l.split(' ') [3] )

		elif type == 'wg1':
			sf  = 1.0
			err = 0.0

		elif type == 'nnlops':
			sf  = 1.0
			err = 0.0
			R['filename'] = l.split(' ')[2]

		elif type == 'csv':
			R['filename'] = l.split(' ')[2]
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'csv-reweight':
			#R['csv-reweight'] 
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'tf1' or type=='tf2':
			R['formula'] = l.split(' ' )[2]
			try: R['errFormula'] = l.split(' ')[3]
			except: R['errFormula']=''
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'inverse-eff-xinmei':
			R['filename'] = l.split(' ')[2]
			R['type'] = 'pteta'
			fIn = ROOT.TFile.Open(R['filename'])
			hEff= fIn.Get("hEffEtaPt")
			hErr= fIn.Get("hErrhEtaPt") ### high and symm
			for xBin in range(1,hEff.GetXaxis().GetNbins()+1):
			   for yBin in range(1,hEff.GetYaxis().GetNbins()+1):
			      R['pt1'] = hEff.GetYaxis().GetBinLowEdge(yBin)
			      R['pt2'] = hEff.GetYaxis().GetBinLowEdge(yBin+1)
			      R['eta1'] = hEff.GetXaxis().GetBinLowEdge(xBin)
			      R['eta2'] = hEff.GetXaxis().GetBinLowEdge(xBin+1)
			      R['sf'] = 1./hEff.GetBinContent(xBin,yBin)
			      R['err'] = hErr.GetBinContent(xBin,yBin)/(hEff.GetBinContent(xBin,yBin)**2)

			      R1={} ## copy 
			      for key in R: R1[key] = R[key]
			      L.append(R1)
			R=None
			continue ## lines loop

		elif type == 'json-sami':
			R['filename'] = l.split(' ')[2]
			R['type'] = 'pteta'

			doEffOnly=False
			if 'effonly' in ' '.join(l.split()[3:]): 
				doEffOnly=True
				if verbose: print "  * EffOnly. MC eff=1"
			try: 
			   jstring = open(R['filename']).read()
			except IOError as e:
			   print "-> Error in reading SF",R['filename']
			   raise e
		        j=json.loads( jstring )
			"dataParameters"
			"mcParameters"
			#run="runs_256629_260627"
			run="runs_271036_279588"
			runMC="2016"
			for idx in range(3,len(l.split())):
				if 'run=' in l.split()[idx]: run=l.split()[idx].replace('run=','')
				if 'runMC=' in l.split()[idx]: runMC=l.split()[idx].replace('runMC=','')
			for idx,params in enumerate(j["dataParameters"][run]["bins"]):
				R["pt1"] =params["pt"] 
				if len(j["dataParameters"][run]["bins"]) > idx+1:
					next=j["dataParameters"][run]["bins"][idx+1]
					R["pt2"] = next["pt"]
				else:
					R["pt2"] = 8000.
				R["eta1"] =  -10
				R["eta2"] = 10
				effD = params["efficiency"]
				## TODO: implement Asymm
				errD = params["uncertaintyPlus"]
				mc=j["mcParameters"][runMC]["bins"]
				effMc= mc[idx]["efficiency"]
				errMc = mc[idx]["uncertaintyPlus"] 

				if effMc == 0. : continue # bin not well populated

				if doEffOnly:
					effMc=1
					errMc=0

				try:
					R["sf"] = effD/effMc
					R["err"] = math.sqrt((errD/effD)**2 + (errMc/effMc)**2)* R["sf"]
				except ZeroDivisionError:
					R["sf"] =1.0
					R["err"] =0.0

				print "  * Loading SF: ",R["pt1"],R["pt2"]," -- ",R["sf"],"+/-",R["err"]
				R1={} ## copy 
				for key in R: R1[key] = R[key]
				L.append(R1)
			R=None
			continue ## lines loop

		elif type == 'json-muon':
			#usage json-muon aux/mu/RunBC_data_ID.json aux/mu/RunBC_mc_ID.json NUM_SoftID_DEN_genTracks
			# assuming same name in data and mc
			R['filename-data'] = l.split(' ')[2]
			R['filename-mc'] = l.split(' ')[3]
			R['id-type'] = l.split(' ')[4] 
			R['type'] = 'ptetaeff'
			jstring = open(R['filename-data']).read()
			jData=json.loads( jstring )
			jstring = open(R['filename-mc']).read()
			jMc = json.loads(jstring)
			print "* Reading SF from",R['filename-data'],R["filename-mc"],R['id-type']
			if R['id-type'] not in jData:
				print "[ERROR] unknown id",R['id-type'], "in",R['filename-data']
				print "available id are:", ','.join([k for k in jData])
				raise ValueError
			if R['id-type'] not in jMc:
				print "[ERROR] unknown id",R['id-type'], "in",R['filename-mc']
				print "available id are:", ','.join([k for k in jMc])
				raise ValueError
			idData = jData[R['id-type']]
			idMc = jMc[R['id-type']]
			if 'abseta_pt' not in idData:
				print "[ERROR]",'abseta_pt',"not in json"
				print "available sf are", ','.join([k for k in idData]),"in",R["filename-data"]
				raise ValueError
			if 'abseta_pt' not in idMc:
				print "[ERROR]",'abseta_pt',"not in json"
				print "available sf are", ','.join([k for k in idMc]),"in",R["filename-mc"]
				raise ValueError

			for etarange in idData['abseta_pt']:
				if etarange not in idMc['abseta_pt']:
					print "[ERROR] eta range",etarange,"not common between data and mc files"
					raise ValueError
				etaStr= re.sub("abseta:\\[","", re.sub('\\]','',etarange))
				R['eta1'] = float(etaStr.split(',')[0])
				R['eta2'] = float(etaStr.split(',')[1])
				for ptrange in idData['abseta_pt'][etarange]:
					if ptrange not in idMc['abseta_pt'][etarange]:
					    print "[ERROR] pt range",ptrange,"not common between data and mc files in eta range",etarange
					    raise ValueError
					ptStr = re.sub('pt:\\[','',re.sub('\\]','',ptrange))
					R['pt1'] = float(ptStr.split(',')[0])
					R['pt2'] = float(ptStr.split(',')[1])

					dataEff = idData['abseta_pt'][etarange][ptrange]['value']
					dataErr = idData['abseta_pt'][etarange][ptrange]['error']
					mcEff = idMc['abseta_pt'][etarange][ptrange]['value']
					mcErr = idMc['abseta_pt'][etarange][ptrange]['error']
                    
					R["dataEff"]=dataEff
					R["dataErr"]=dataErr
					R["mcEff"]=mcEff
					R["mcErr"]=mcErr

					print "  * Loading Effiencies: ",R["pt1"],R["pt2"],R['eta1'],R['eta2']," -- ",R["dataEff"],R["mcEff"]
					R1={} ## copy 
					for key in R: R1[key] = R[key]
					L.append(R1)
			R=None
			continue ## lines loop
			 
		else:
			print "sf",label,"of type",type,"not supported in the sf database"
		R['sf'] =sf
		R['err'] =err
		L.append(R);

	   except:
		   print "Unable to parse line:"
		   print "'" + line.replace('\n','') + "'"
		   raise ## re-raise exception
	return L

def ReadBranches(fileName):
    ''' Read Branches from a files '''
    R=[]
    if fileName =="": 
        print "[WARNING]: Branch file not provided. Returning empty list."
        return R
    f =open(fileName,"r")
    for line in f:
        l = line.split('#')[0]
        l = re.sub('\n','',l)
        if l == "": continue
        l=re.sub('^ *','',l).split()[0]
        b = StringKey(l)
        R.append(b)
    return R

def chunkIt(seq, num):
  ''' Take a list seq and return a n lists'''
  R=[]
  for i in range(0,num): R.append([])
  for j in range(0,len(seq) ):
        R[ j%num ].append(seq[j])
  return R


from datetime import datetime
class DirectoryStore:
    ''' This class aims to identify which directories are used and by whom. Using datetime, json, os'''
    def __init__(self):
        self.fname="./.nero.json"
        self.isinit=False
        self.db={}
        self.verbose=False

    def init(self):
        ''' Read the database '''
        self.db ={} 
        self.user = os.environ['USER']
        if self.verbose: print "[DEBUG]: User is",self.user
        self.isinit=True

        if self.verbose: print "[DEBUG]: Reading File:",self.fname
        try: 
            jstring = open(self.fname).read()
        except IOError as e:
            print "[ERROR] Unable to open file",self.fname,"assuming it doesn't exist"
            return self

        self.db=json.loads( jstring )
        return self

    def add(self,d):
        ''' Add or update directory usage '''
        if d not in self.db:
            self.db[d]={}
        #if self.user not in self.db[d]:self.db[d][self.user] =""
        ## assuming that now is always an update
        self.db[d][self.user] = "%d/%d/%d"%(datetime.today().year,datetime.today().month,datetime.today().day)
        return self
    def end(self):
        ''' write the database'''
        jstring = json.dumps(self.db,sort_keys=True, indent=4)
        fd=open(self.fname,"write")
        fd.write(jstring)
        fd.close()
        self.isinit=False
        return self

