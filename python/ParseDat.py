import os,sys,re
from subprocess import call,check_output


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
	return config;


def BoolKey(value):
	value = value.lowercase()
	if 'no' in value: return False
	if 'yes' in value: return True 
	if 'false' in value : return False
	if 'true' in value : return True

def StringKey(value):
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
		######### STRING ###########
		if key == 'MCDB' or key =='SFDB' or key =='Output' or key == 'pileup' :
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
				else:
					config[key] = R[key]
	return config

def FindEOS(name,mount=""):
	''' EOS PATH should be followed. The mount option will assume that eos is mounted in ~/eos '''
	EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
	# should match wildcard, that for some reason new root does not
	if '/store/' not in name: return [name]
	if '/eos/cms/store/' in name: return [name] # likely already parsed
	if 'root://eoscms//' in name: return [name] # already parsed

	cmd = EOS + ' find -f ' + name
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
		fileList = [ re.sub("/eos/cms", mount + "/cms",f) for f in list ]
	else:
		fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in list ]

	return fileList

def PrintDat(config):
	''' Print configuration'''

	print "##########################"
	for key in config:
		if key == 'config':
			for type in config[key]:
				print key,'=',type,'|'," ".join(config[key][type])
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
		R[label] = (dir,entries,xsec)
	return R

def ReadSFDB(file):
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

		if label == 'include':
			tmp = ReadSFDB(type)
			#remove from L all the key with the same label as in tmp
			labels = set([])
			new = []
			for key in tmp:
				labels.add( key['label'])
			for key in L:
				if key['label'] not in labels:
					new.append( key ) 
			# merge L and tmp
			L = new[:]

		if type == 'pteta':
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

		elif type == 'base':
			sf  = float ( l.split(' ') [2] )
			err = float ( l.split(' ') [3] )
		else:
			print "type",type,"not supported in the sf database"
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
