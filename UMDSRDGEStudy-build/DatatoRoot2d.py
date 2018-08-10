import sys, string, os
from ROOT import TFile, TNtuple
ifn = os.path.expandvars("EffMuon.csv")
ofn = 'EffMuon.root'
print ('opening file'), ifn, '...'
infile = open( ifn, 'r' )
lines  = infile.read().splitlines()
title  = "Weighted Efficiency vs. Muon Energy"
labels = lines[0].split(",")
print ('writing file'), ofn, '...'
outfile = TFile( ofn, 'RECREATE', 'ROOT file with an NTuple' )
ntuple  = TNtuple( 'ntuple', title, string.join( labels, ':') )
for line in lines[1:]:
    words = line.split(",")
    row = map( float, words )
    apply( ntuple.Fill, row )
outfile.Write()
print(labels)
print ('done')
#TH1D* h3 = new TH1D("h3", "Weighted Efficiency vs. Muon Energy",38, 0, 0.6,) use this to define a 2d histogram with 20 bins in x and y axis (range -6 to 6)
#ntuple->Draw("muon>>h3","eff") use this to fill the histogram using the ntuple created in this .py, using 'avg' as n for the histogram, SURF3 is a draw option
#make sure in the fill function above, the arguments (x,y,avg in this case) match the printed 'labels', otherwise the histogram will not fill correctly

