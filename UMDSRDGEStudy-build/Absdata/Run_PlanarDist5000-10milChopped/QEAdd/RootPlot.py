import time
import ROOT
from ROOT import TGraph,TCanvas,TMath,TLegend,gPad,gStyle

# ROOT Files, ranges (keep the same ranges)
f = ROOT.TFile("Rod115Unirr.root")
t = f.Get("h2")
t.GetXaxis().SetRangeUser(0,1000)
g = ROOT.TFile("DiscUnirr.root")
u = g.Get("h2")
u.GetXaxis().SetRangeUser(0,1000)
h = ROOT.TFile("Rod145Unirr.root")
v = h.Get("h2")
v.GetXaxis().SetRangeUser(0,1000)
i = ROOT.TFile("MirroredDiscUnirr.root")
w = i.Get("h2")
w.GetXaxis().SetRangeUser(0,1000)
y = ROOT.TFile("MirroredRodUnirr.root")
o = y.Get("h2")
o.GetXaxis().SetRangeUser(0,1000)

#Get Values for bounds on gaussian fits (calculated mean, standard deviation)
meant = t.GetMean()
meanu = u.GetMean()
meanv = v.GetMean()
meanw = w.GetMean()
meano = o.GetMean()
sdt = t.GetStdDev()
sdu = u.GetStdDev()
sdv = v.GetStdDev()
sdw = w.GetStdDev()
sdo = o.GetStdDev()
ubt = meant + 3*sdt
lbt = meant - 3*sdt
ubu = meanu + 3*sdu
lbu = meanu - 3*sdu
ubv = meanv + 3*sdv
lbv = meanv - 3*sdv
ubw = meanw + 3*sdw
lbw = meanw - 3*sdw
ubo = meano + 3*sdo
lbo = meano - 3*sdo

#Create Gauss Fit, use the defined bounds
fit_v = ROOT.TF1("fit_t","gaus",lbt,ubt)
fit_v.SetLineColor(ROOT.kBlue)
fit_v.SetLineWidth(2)
fit_t = ROOT.TF1("fit_u","gaus",lbu,ubu)
fit_t.SetLineColor(ROOT.kRed)
fit_t.SetLineWidth(2)
fit_u = ROOT.TF1("fit_v","gaus",lbv,ubv)
fit_u.SetLineColor(ROOT.kGreen)
fit_u.SetLineWidth(2)
fit_w = ROOT.TF1("fit_w","gaus",lbw,ubw)
fit_w.SetLineColor(ROOT.kViolet)
fit_w.SetLineWidth(2)
fit_o = ROOT.TF1("fit_o","gaus",lbo,ubo)
fit_o.SetLineColor(ROOT.kCyan)
fit_o.SetLineWidth(2)

#Set Attributes
t.SetLineColor(ROOT.kRed)
v.SetLineColor(ROOT.kBlue)
u.SetLineColor(ROOT.kGreen)
w.SetLineColor(ROOT.kViolet)
o.SetLineColor(ROOT.kCyan)
u.SetName("Disc")
t.SetName("Rod 1*1*5")
v.SetName("Rod 1*.4*5")
w.SetName("Mirrored Disc")
o.SetName("Mirrored Rod")

# Draw Function
def fun():
	c1=TCanvas("c1","c1",600,500)
	t.Draw()
	u.Draw("SAMES")
	v.Draw("SAMES")
	w.Draw("SAMES")
	o.Draw("SAMES")
	v.GetXaxis().SetTitle("Photons in Evt")
	v.GetYaxis().SetTitle("Events")
	v.GetYaxis().SetTitleOffset(1.3)
	u.GetXaxis().SetTitle("Photons in Evt")
	u.GetYaxis().SetTitle("Events")
	u.GetYaxis().SetTitleOffset(1.3)
	t.GetXaxis().SetTitle("Photons in Evt")
	t.GetYaxis().SetTitle("Events")	
	t.GetYaxis().SetTitleOffset(1.3)
	w.GetXaxis().SetTitle("Photons in Evt")
	w.GetYaxis().SetTitle("Events")	
	w.GetYaxis().SetTitleOffset(1.3)
	o.GetXaxis().SetTitle("Photons in Evt")
	o.GetYaxis().SetTitle("Events")	
	o.GetYaxis().SetTitleOffset(1.3)

	#Stats/Fit Options, stat box placement (top/bottom)
	gPad.Update()
#	su = u.FindObject("stats")
#	su.SetY1NDC(.1)
#	su.SetY2NDC(.25)
#	su.SetTextColor(ROOT.kGreen)
	u.Fit(fit_u)
#	sv = v.FindObject("stats")
#	sv.SetY1NDC(.25)
#	sv.SetY2NDC(.4)
#	sv.SetTextColor(ROOT.kBlue)
	v.Fit(fit_v)
#	sw = w.FindObject("stats")
#	sw.SetY1NDC(.4)
#	sw.SetY2NDC(.55)
#	sw.SetTextColor(ROOT.kViolet)
	w.Fit(fit_w)
	gStyle.SetOptFit(0)
	gStyle.SetOptStat(0)  #REMOVE FOR STATS
#	st = t.FindObject("stats")
#	st.SetY1NDC(.55)
#	st.SetY2NDC(.7)
#	st.SetTextColor(ROOT.kRed)
	t.Fit(fit_t)
#	so = o.FindObject("stats")
#	so.SetY1NDC(.7)
#	so.SetY2NDC(.85)
#	so.SetTextColor(ROOT.kCyan)
	o.Fit(fit_o)
	return c1
c1 = fun()
c1.Draw()
raw_input()