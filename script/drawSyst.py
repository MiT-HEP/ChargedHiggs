
from optparse import OptionParser
import ROOT
from array import array
parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="DY.root")
parser.add_option("","--base",type='string',help="Base [Default=%default]", default="ChargedHiggsTauNu/Vars/Mt_WJets")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)
opts, args = parser.parse_args()

def Rebin(h):
	''' Rebin with un-even bins '''
	mybins=array('d',[0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,350,400,500,600,700,800,900,1000,1500,2000,8000])
	#h1 = ROOT.TH1D(h.GetName()+"_rebin",h.GetTitle(),len(mybins)-1,mybins)
	h1=h.Rebin(len(mybins)-1,h.GetName()+"_rebin",mybins)
	return h1


fIn=ROOT.TFile.Open(opts.input)

if fIn==None:
	print "[ERROR] File", opts.input, "doesn't exist"
	raise IOError

h=fIn.Get(opts.base)

if h==None:
	print "[ERROR] Hist", opts.base, "doesn't exist"
	raise IOError

hL=[]

if opts.syst!='Pdf' and opts.syst != 'Scale':
	hUp=fIn.Get(opts.base+"_"+opts.syst+"Up")
	hDown=fIn.Get(opts.base+"_"+opts.syst+"Down")
	if hUp==None or hDown==None:
		print "[ERROR] Hist", opts.base+"_"+opts.syst+"Up/Down", "doesn't exist"
		raise IOError
	hL = [hUp,hDown]
elif opts.syst=='Scale':
	for w in [ 'R','F','RF']:
		for s in ['Up','Down']:
			hTmp=fIn.Get(opts.base+"_Scale"+w+s)
			if hTmp==None:
				print "[ERROR] Hist", opts.base+"_Scale"+w+s, "doesn't exist"
				raise IOError
			hL.append(hTmp)
elif opts.syst=='Pdf':
	for i in range(0,100):
		hTmp=fIn.Get(opts.base+"_Pdf%d"%i)
		if hTmp==None:
			print "[ERROR] Hist", opts.base+"_Pdf%d"%i, "doesn't exist"
			raise IOError
		hL.append(hTmp)

if opts.rebin> 999:
	h=Rebin(h)
else:
	h.Rebin(opts.rebin)

for idx,hTmp in enumerate(hL):
	if opts.rebin> 999:
		hL[idx]=Rebin(hTmp)
	else:
		hTmp.Rebin(opts.rebin)
	#hUp.Rebin(opts.rebin)
	#hDown.Rebin(opts.rebin)

h.SetLineColor(ROOT.kBlack)

if len(hL)==2:
	colors=[ROOT.kBlue,ROOT.kRed]
elif len(hL)==6:
	colors=[ROOT.kBlue,ROOT.kBlue-4,ROOT.kRed,ROOT.kRed-4,ROOT.kGreen+2,ROOT.kGreen]
else:
	colors=[ 
		 ROOT.kOrange-7,ROOT.kOrange-4,ROOT.kOrange,ROOT.kOrange+2,
		 ROOT.kRed+2,ROOT.kRed,ROOT.kRed-4,ROOT.kRed-7,
		 ROOT.kMagenta-7,ROOT.kMagenta-4,ROOT.kMagenta,ROOT.kMagenta+2,
		 ROOT.kBlue+2,ROOT.kBlue,ROOT.kBlue-4,ROOT.kBlue-7,
		 ROOT.kCyan,
		 ROOT.kGreen-7,ROOT.kGreen-4,ROOT.kGreen,ROOT.kGreen+2
		 ]
for hTmp in hL:
	mycolor=colors[0]
	colors=colors[1:]+ [mycolor]
	hTmp.SetLineColor(mycolor)

h.SetLineWidth(2)


for hTmp in hL:
	hTmp.SetLineWidth(2)
	hTmp.SetLineStyle(7)



h.Draw("HIST")
for hTmp in hL:
	hTmp.Draw("HIST SAME")
	print "Yields variation for ",hTmp.GetName(),":", "%.1f %%"%( (hTmp.Integral()/h.Integral()-1) *100.)

c2=ROOT.TCanvas("c2")
r=h.Clone("r")

rL=[]
for hTmp in hL:
	rTmp=hTmp.Clone("r"+hTmp.GetName())
	rL.append(rTmp)

r.Divide(h)
for rTmp in rL:
	rTmp.Divide(h)

r.Draw("HIST")
for rTmp in rL:
	rTmp.Draw("HIST SAME")

r.GetYaxis().SetRangeUser(0.,2.)
raw_input("ok?")


