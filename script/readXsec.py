import ROOT 


f = open("aux/xsec_13TeV_tHp_08Apr2015.txt")

tb=5

g=ROOT.TGraph()

for line in f:
	if line[0] == '#': continue
	if line.startswith("mhp"): continue
	_mh_= float(line.split()[0])
	_tb_= float(line.split()[1])
	_xsec_= float(line.split()[2])
	_uncm_= float(line.split()[3])
	_uncp_= float(line.split()[4])

	if _tb_ != tb :continue;

	g.SetPoint(g.GetN(), _mh_,_xsec_)

s=ROOT.TSpline3("spline",g)
print "G=",s.Eval(350)
