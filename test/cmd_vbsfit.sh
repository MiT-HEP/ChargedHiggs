#!/bin/bash

OUTPUT=VBSFit
WWW=~/www/cms-private/VBSHadronic/2021_02_15_AntiRegion/
#doHighLow=true
doHighLow=false

rm -r $OUTPUT $WWW
mkdir $OUTPUT

#################################################
##                 SIGNAL MODEL                ##
#################################################

#for target in met had bhad; do
/bin/true && {
for target in had bhad; do
  for year in 2016 2017 2018 ; do
    CONFIG=vbshadr_${target}_${year}
    [ $doHighLow = true ] && CONFIG=vbshadr_highlow_${target}_${year}
    
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
        bhad) 
            BASEDIR="/eos/user/d/dalfonso/VBS-CH/FEB7" ;
            FILE="BHAD_sig_res.root" ;;
        had) 
            BASEDIR="/eos/user/d/dalfonso/VBS-CH/FEB12" ;
            FILE="HAD_sig_res.root" ;;
        met) 
            BASEDIR="XXX" ;
            FILE="MET_sig_res.root" ;;
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
#for target in had bhad; do
for target in had bhad ; do
for year in 2016 2017 2018 ; do
    CONFIG=vbshadr_${target}_${year}
    [ $doHighLow = true ] && CONFIG=vbshadr_highlow_${target}_${year}
    
    # construct input
    ULTAG=""
    HADTF=1.0
    LUMI=1.0
    if [ "$target" == "had" ]; then
    case $year in
        2016) ULTAG="UL16" ; HADTF=0.000555; LUMI=35920 ;;
        2017) ULTAG="UL17" ; HADTF=0.000184; LUMI=41530 ;;
        2018) ULTAG="UL18" ; HADTF=0.000351; LUMI=59740 ;;
        *) ULTAG="XXX" ; echo "year not found '${year}'" ;;
    esac
    fi

    if [ "$target" == "bhad" ] ; then
    case $year in
        2016) ULTAG="UL16" ; HADTF=0.01875; LUMI=35920 ;;
        2017) ULTAG="UL17" ; HADTF=0.0482; LUMI=41530 ;;
        2018) ULTAG="UL18" ; HADTF=0.0221; LUMI=59740 ;;
        *) ULTAG="XXX" ; echo "year not found '${year}'" ;;
    esac
    fi

    FILE=""
    case $target in
        bhad) 
            BASEDIR="/eos/user/d/dalfonso/VBS-CH/FEB7" ;
            FILE="BHAD_sig_res.root" 
            INPUT=${BASEDIR}/${ULTAG}/BHADanti_qcd_res.root
            CAT="BBtag"
            ;;
        had) 
            BASEDIR="/eos/user/d/dalfonso/VBS-CH/FEB12" ;
            FILE="HAD_sig_res.root" ;
            INPUT=${BASEDIR}/${ULTAG}/HADanti_qcd_res.root
            CAT="BB"
            ;;
        met) 
            BASEDIR="XXX" ;
            FILE="MET_sig_res.root" 
            INPUT=${BASEDIR}/${ULTAG}/METanti_qcd_res.root
            CAT="XXX"
            ;;
        *) FILE="XXX.root" ; echo "target not found '${target}'" ;;
    esac

    #INPUT=${BASEDIR}/${ULTAG}/${FILE}
    [ $doHighLow = false ] && {
        python script/preprocessmc.py -l $(echo "${HADTF}*${LUMI}" | bc -l) -i $INPUT -o $OUTPUT/Asimov_${target}_${year}.root -n MVV_${CAT}_AsimovB  VBShadAnalysis/MVV_${CAT}_QCD_HT # VBShadAnalysis/MVV_BB_TT_TuneCUETP8M2T4
    }

    [ $doHighLow = true ] && {    
        for x in high low ; do 
            python script/preprocessmc.py -l $(echo "${HADTF}*${LUMI}" | bc -l) -i $INPUT -o $OUTPUT/Asimov_${target}_${year}_tmp_${x}.root -n MVV_${CAT}_${x}_AsimovB  VBShadAnalysis/MVV_${CAT}_${x}_QCD_HT # VBShadAnalysis/MVV_${CAT}_${x}_TT_TuneCUETP8M2T4
        done
        hadd $OUTPUT/Asimov_${target}_${year}.root $OUTPUT/Asimov_${target}_${year}_tmp_*.root
        rm $OUTPUT/Asimov_${target}_${year}_tmp_*.root
    }

    python python/fitter.py -c BackgroundFitter -f $OUTPUT/Asimov_${target}_${year}.root -o ${OUTPUT}/BackgroundModel_${target}_${year}.root -p ${OUTPUT}/bkgfit_${target}_${year} --hmm=${CONFIG}  2>&1 | tee /tmp/amarini/log_bkg.txt 
done
done
    } || true

mkdir $WWW
rsync -avP $OUTPUT/ $WWW/


