#!/bin/bash
for cat in {0..12} ; 
do 
    mkdir -p all/cat${cat};  
    for i in  9494/cat${cat}/*root ; 
    do 
        echo "Doing cat$cat ${i##*/} "; 
        hadd all/cat${cat}/${i##*/} $(ls */cat${cat}/${i##*/} | grep -v "all/cat$cat" | tr '\n' ' ' ) ; 
    done ;
done
