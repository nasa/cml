#!/bin/bash
for run in SET_*/RUN*;
do

   echo "Running $run"
  ./S_main* $run/input.py
  retval=$?

  if [ ${retval} != 0 ] ; then
      echo "*********************************************************************" 
      echo "Run ${run} FAILED with Error! Stopping execution of subsequent runs." 
      echo "*********************************************************************" 
      exit 1
  fi

done

