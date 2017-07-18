#!/bin/bash

# Run me from the root directory of the repository

echo "COUNTING NUMBER OF TRAINING EVENTS FOR ALL SETS\n\n"

for i in aux/tb_*.dump.txt
do
    echo "${i} ---------->  $(cut -d' ' -f1  ${i} | grep -v '[0-9][0-9][0-9]'  | grep -v mc | wc -l) "
done


for i in aux/tb_train_GB/tb_*.dump.txt
do
    echo "${i} ---------->  $(cut -d' ' -f1  ${i} | grep -v '[0-9][0-9][0-9]'  | grep -v mc | wc -l) "
done

for i in aux/tb_train_Greduced/tb_*.dump.txt
do
    echo "${i} ---------->  $(cut -d' ' -f1  ${i} | grep -v '[0-9][0-9][0-9]'  | grep -v mc | wc -l) "
done

echo "\n\nCOUNTED!"
exit 0
