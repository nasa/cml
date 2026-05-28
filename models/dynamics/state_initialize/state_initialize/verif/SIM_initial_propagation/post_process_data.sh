#!/bin/bash

# See README in this directory
for set in SET_reverse*;
do
  cd $set
  mkdir -p RUN_combined
  cp RUN_verif_0010/log_test_data.header RUN_combined

  rm -f RUN_combined/log_test_data.csv
  head -1 RUN_verif_0010/log_test_data.csv > RUN_combined/log_test_data.csv
  echo '0, 0.0, 6800000, 0.0, 0.0, 8000.0, 0.0, 0.0' >> RUN_combined/log_test_data.csv

  for file in RUN_verif*/log_test_data.csv;
  do
    tail -1 $file >> RUN_combined/log_test_data.csv
  done

  cd ..
done


for set in SET_forward*;
do
  cd $set
  mkdir -p RUN_$set
  cp RUN_verif_0010/log_test_data.header RUN_$set

  rm -f RUN_$set/log_test_data.csv
  head -1 RUN_verif_0010/log_test_data.csv > RUN_$set/log_test_data.csv

  for file in RUN_verif*/log_test_data.csv;
  do
    tail -1 $file | awk '{print $2 $2 $3 $4 $5 $6 $7 $8}' >> RUN_$set/log_test_data.csv
  done

  ../../../../../../../../bin/regression/antaresCompare.py  -r RUN_baseline -t RUN_$set -i 'log.*.csv'
  okular RUN_$set/Docs/*.pdf &
  cd ..
done


