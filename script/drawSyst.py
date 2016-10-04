
from optparse import OptionParser
import ROOT
parser= OptionParser()
parser.add_option("-i","--input",type='string',help="Input ROOT file. [Default=%default]", default="DY.root")
parser.add_option("","--base",type='string',help="Base [Default=%default]", default="ChargedHiggsTauNu/Vars/Mt_WJets")
parser.add_option("","--syst",type='string',help="Syst [Default=%default]", default="PU")
parser.add_option("","--rebin",type='int',help="Rebin [Default=%default]", default=20)
opts, args = parser.parse_args()


fIn=ROOT.TFile.Open(opts.input)

h=fIn.Get(opts.base)
hUp=fIn.Get(opts.base+"_"+opts.syst+"Up")
hDown=fIn.Get(opts.base+"_"+opts.syst+"Down")

h.Rebin(opts.rebin)
hUp.Rebin(opts.rebin)
hDown.Rebin(opts.rebin)

h.SetLineColor(ROOT.kBlack)
hUp.SetLineColor(ROOT.kBlue)
hDown.SetLineColor(ROOT.kRed)

h.SetLineWidth(2)
hUp.SetLineWidth(2)
hDown.SetLineWidth(2)

hUp.SetLineStyle(7)
hDown.SetLineStyle(7)


h.Draw("HIST")
hUp.Draw("HIST SAME")
hDown.Draw("HIST SAME")

raw_input("ok?")


