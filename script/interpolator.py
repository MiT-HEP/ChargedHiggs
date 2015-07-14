import sys, os
from optparse import OptionParser,OptionGroup

parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="ChHiggs.root")
parser.add_option("-o","--output",type='string',help="Output ROOT file. [Default=%default]", default="ChHiggs_interpolated.root")
parser.add_option("-s","--step",type='float',help="Step [Default=%default]", default=50)
parser.add_option("-b","--begin",type='float',help="Begin [Default=%default]", default=200)
parser.add_option("-e","--end",type='float',help="End [Default=%default]", default=900)
parser.add_option("-a","--available",type='string',help="Available [Default=%default]", default="200,250,500,900")
parser.add_option("-n","--name",type='string',help="Histo Name Str [Default=%default]", default="ChargedHiggsTauNu/Vars/Mt_HplusToTauNu-M%(mass).0f")
parser.add_option("-l","--lumi",type='float',help="Lumi. Scale Histograms. [Default=%default]", default=5000)

extra = OptionGroup(parser,"Extra options:","")
extra.add_option("-r","--rebin",type='int',help = "Rebin Histograms. [Default=%default]", default=1)
extra.add_option("-p","--plot",action='store_true',help = "Make Plot. [Default=%default]", default=False)
extra.add_option("","--inject",type='float',help = "Multiply Signal by factor. [Default=%default]", default=1)

parser.add_option_group(extra)

opts, args = parser.parse_args()
opts.plotDir = "plot/interpolation"

sys.argv=[]
import ROOT
ROOT.gROOT.SetBatch()

def drange(start, stop, step):
	''' Return a floating range list. Start and stop are included in the list if possible.'''
	r = start
	while r <= stop:
		yield r
		r += step

def interpolate(x1,y1,x2,y2,x):
	''' linear interpolation between the points (x1,y1) (x2,y2) evaluated at x'''
	#y = mx +q
	if x1==x2 and y1==y2 and x==x1 : return y1

	m= (y2-y1)/(x2-x1)
	q= y2 - m*x2
	return m*x+q

# save Cumulative distributions here:

def cumulative(h):
	''' Compute Cumulative distribution. Do not change h. Bin 0 is 0.'''

	norm = h.Integral("width");
	cum = h.Clone(h.GetName() + "_cdf")
	cum.Reset("ACE")
	for i in range(1,h.GetNbinsX()+1):
		cum.SetBinContent(i, h.Integral(1,i,"width") / norm )
	return cum

def interpolateCdf(m1,h1,m2,h2,m):
	r= h1.Clone(h1.GetName() + "_interpolated_from%.0f_%.0f_to%.0f"%(m1,m2,m) )
	r.Reset("ACE")
	for i in range(1,h1.GetNbinsX()+1):
		r.SetBinContent(i, interpolate(m1, h1.GetBinContent(i), m2,h2.GetBinContent(i), m) ) 
	return r

hCdf={}
def interpolateHisto(m1,h1,m2,h2,m):
	'''Return interpolated histogram '''	
	norm1= h1.Integral()
	norm2= h2.Integral()
	if m1 not in hCdf: hCdf[m1] = cumulative(h1)
	if m2 not in hCdf: hCdf[m2] = cumulative(h2)

	cdf = interpolateCdf(m1, hCdf[m1], m2,hCdf[m2], m)
	norm = interpolate( m1,norm1,m2,norm2,m)

	h = h1.Clone("Interpolated_from%.0f_%.0f_to%.0f"%(m1,m2,m) ) 
	h.Reset("ACE")

	if cdf.GetBinContent(cdf.GetNbinsX()) != 1 : print "ERROR: CDF Last Bin = ", cdf.GetBinContent( cdf.GetNbinsX() )

	for i in range(1,h.GetNbinsX()+1):
		##   [1] [1-2] [1-3] ... [1-N]
		##   2 = bin2-bin1..
		h.SetBinContent( i,   cdf.GetBinContent(i) - cdf.GetBinContent(i-1) )
	h.Scale(norm)
	return h


###########################################################

fInput = ROOT.TFile.Open(opts.input)

fOutput= ROOT.TFile.Open(opts.output,"RECREATE")

hIn={}
hOut={}

##
if fInput == None:
	print "file ",opts.input, "doesn't exist"
	exit(1)

## LOAD INPUT HISTOGRAMS
for mass in opts.available.split(","):
	dict={"mass":float(mass)}
	objName = opts.name % dict

	hIn[ dict["mass"] ] = fInput.Get(objName)
	if hIn[ dict["mass"] ] == None:
		print "Unable to get object", fileName, "from file",opts.input
		exit(1)
	if hIn[ dict["mass"] ].Integral() == 0 :
		print "Object", fileName, " is likely to be empty"

	hIn[ dict["mass"] ].Rebin(opts.rebin)
	if opts.inject !=1 : print "[WARING] :: INJECTING x"+str(opts.inject)+" signal"
	hIn[ dict["mass"] ].Scale(opts.inject)
	hIn[ dict["mass"] ].Scale(opts.lumi)
	print "* Loaded obj: '"+objName+"'"

fOutput.cd()

if opts.inject!=1:
	n=ROOT.TNamed("inject","%.1f"%opts.inject)
	n.Write()

### Copy the following histograms
for suffix in ["Data","QCD","TTJets","WW","WZ","ZZ","DY"]:
	dataName =  '_'.join(opts.name.split("_")[:-1]) + "_"+ suffix
	targetName = dataName.split("/")[-1]
	data = fInput.Get( dataName )
	
	if data == None:
		print "* No histogram found for '"+dataName+"'"
	else:
		data = data.Clone(targetName)

		if suffix != "Data" : data.Scale(opts.lumi)

		print "* Load and copy obj obj '"+dataName+"'"
		data.Rebin(opts.rebin)
		data.Write()

#nameMask = opts.name.replace(".0f",".1f") ## increase precision to  1 digit more
nameMask = opts.name[:]
mIn = [ mX for mX in hIn]
mIn.sort() ## don't know why dict was not sorted

for mass in drange(opts.begin,opts.end,opts.step):
	# find the two mass points	
	m1 = -1
	m2 = -1

	for mX in mIn:
		if mass >= mX : m1 = mX
		if mass <= mX and m2<0 : m2 = mX

	if m1 <0 or m2 < 0:
		print "Unable to find interpolation point for m",mass,"in (",m1,",",m2,")"
	# interpolate them
	h1 = hIn[m1]
	h2 = hIn[m2]
	h= interpolateHisto(m1,h1,m2,h2,mass)

	#print "Interpolating for (",m1,",",m2,") -> ",mass

	name = nameMask.split('/')[-1] + "_interpolated"
	h.SetName(  name % {"mass":mass} )
	hOut[ mass ] = h
	h.Write()

blues = [	ROOT.kBlue+4 , 
		ROOT.kBlue+2, 
		ROOT.kBlue, 
		ROOT.kBlue-4, 
	]
reds =  [	ROOT.kRed -4 , 
		ROOT.kRed, 
		ROOT.kRed+2,
	]
greens =  [	ROOT.kGreen -4 , 
		ROOT.kGreen, 
		ROOT.kGreen+2,
	]
magentas = [	ROOT.kMagenta+2,
		ROOT.kMagenta,
		ROOT.kMagenta-4,
		]

colors= [ ]
colors.extend( blues )
colors.extend( magentas[::-1] )
colors.extend( reds )
colors.extend( greens[::-1] )

def getColor():
	c= colors.pop(0)
	colors.append(c)
	return c

if opts.plot:
	ROOT.gStyle.SetOptTitle(0)
	ROOT.gStyle.SetOptStat(0)
	c=ROOT.TCanvas("plot","canvas",800,800)
	# draw axis
	dummy = ROOT.TH1D("h","h",100,0,1000)
	dummy.Draw("AXIS")
	dummy.GetXaxis().SetTitle("M_{T} [GeV]")
	dummy.GetYaxis().SetTitle("Expected Events")

	max = -1

	for m in hOut:
		hOut[m].SetLineColor( getColor() )
		#hOut[m].Scale(lumi)
		hOut[m].Draw("HIST SAME" )
	for m in hIn:
		hIn[m].SetMarkerSize( 0.8 )
		hIn[m].SetMarkerStyle( 20 )
		hIn[m].SetMarkerColor(ROOT.kBlack)
		#hIn[m].Scale(lumi)
		hIn[m].Draw("P SAME")
		if hIn[m].GetMaximum() >max : max = hIn[m].GetMaximum()

	dummy.GetYaxis().SetRangeUser(0, max*1.3) 
	l = ROOT.TLatex()
	l.SetNDC()
	l.SetTextSize(0.04)
	l.SetTextAlign(13)
	l.SetTextFont(42)
	l.DrawLatex(0.12,.89,"#bf{CMS}, #scale[0.75]{#it{Simulation}}")
	l.SetTextSize(0.03)
	l.SetTextAlign(31)
	lumi = opts.lumi/1000. ## lumi in fb
	l.DrawLatex(0.89,.91,"%.1f fb^{-1} (13 TeV)"%lumi)

	c.SaveAs(opts.plotDir + "/interpolation.pdf")
	c.SaveAs(opts.plotDir + "/interpolation.png")
	fOutput.cd()
	c.Write()

fInput.Close()
fOutput.Close()
print "--- DONE ---"



