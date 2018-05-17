#!/bin/bash

echo "-> Produce Histos and workspaces"
python produce_histo.py
text2workspace.py datacard.txt
text2workspace.py datacard_bin.txt

echo "-> autoMCStats"
combine -M AsymptoticLimits -t -1 --rMin=-2 --rMax=8 datacard.root
echo "-> bin shapes"
combine -M AsymptoticLimits -t -1 --rMin=-2 --rMax=8 datacard_bin.root
