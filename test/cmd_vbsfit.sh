#!/bin/bash

BASEDIR=/eos/user/d/dalfonso/VBS-CH/FEB2
OUTPUT=VBSFit
WWW=~/www/cms-private/VBSHadronic/2021_02_05_AntiRegion/
rm -r $OUTPUT $WWW
mkdir $OUTPUT

#################################################
##                 SIGNAL MODEL                ##
#################################################
#/eos/user/d/dalfonso/VBS-CH/FEB2

#for target in met had bhad; do
/bin/true && {
for target in had; do
  for year in 2016 2017 2018 ; do
    CONFIG=vbshadr_${target}_${year}
    
    # construct input
    ULTAG=""
    case $year in
        2016) ULTAG="UL16" ;;
        2017) ULTAG="UL17" ;;
        2018) ULTAG="UL18" ;;
        *) ULTAG="XXX" ; echo "year not found '${year}'" ;;
    esac
    FILE=""
    case $target in
        bhad) FILE="BHAD_ch.root" ;;
        had) FILE="HAD_sig_res.root" ;;
        met) FILE="MET_ch.root" ;;
        *) FILE="XXX.root" ; echo "target not found '${target}'" ;;
    esac

    INPUT=${BASEDIR}/${ULTAG}/${FILE}

    python python/fitter.py -c Fitter -f $INPUT -o $OUTPUT/SignalModel_${target}_${year}.root --hmm ${CONFIG} -p ${OUTPUT}/sigfit_${target}_${year} 2>&1 | tee /tmp/amarini/log_${target}_${year}.txt
  done
done
}

#################################################
##                 BACKGROUND MODEL            ##
#################################################

/bin/true && {
for target in had; do
for year in 2016 2017 2018 ; do
    CONFIG=vbshadr_${target}_${year}
    
    # construct input
    ULTAG=""
    HADTF=1.0
    LUMI=1.0
    case $year in
        2016) ULTAG="UL16" ; HADTF=0.000555; LUMI=35920 ;;
        2017) ULTAG="UL17" ; HADTF=0.000184; LUMI=41530 ;;
        2018) ULTAG="UL18" ; HADTF=0.000351; LUMI=59740 ;;
        *) ULTAG="XXX" ; echo "year not found '${year}'" ;;
    esac
    FILE=""
    case $target in
        bhad) FILE="BHAD_ch.root" ;;
        had) FILE="HAD_sig_res.root" ;;
        met) FILE="MET_ch.root" ;;
        *) FILE="XXX.root" ; echo "target not found '${target}'" ;;
    esac

    #INPUT=${BASEDIR}/${ULTAG}/${FILE}
    INPUT=${BASEDIR}/${ULTAG}/HADanti_qcd_res.root
    #0.0042 -> Anti region
    #31900
    python script/preprocessmc.py -l $(echo "${HADTF}*${LUMI}" | bc -l) -i $INPUT -o $OUTPUT/Asimov_${target}_${year}.root -n MVV_BB_AsimovB  VBShadAnalysis/MVV_BB_QCD_HT # VBShadAnalysis/MVV_BB_TT_TuneCUETP8M2T4

    python python/fitter.py -c BackgroundFitter -f $OUTPUT/Asimov_${target}_${year}.root -o ${OUTPUT}/BackgroundModel_${target}_${year}.root -p ${OUTPUT}/bkgfit_${target}_${year} --hmm=${CONFIG}  2>&1 | tee /tmp/amarini/log_bkg.txt 
done
done
    } || true

mkdir $WWW
rsync -avP $OUTPUT/ $WWW/


