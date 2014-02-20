#!/bin/bash
#$ -V
#$ -cwd
#$ -q test.q
#$ -pe openmpi_2 2
##$ -l exclusive_job=1

## SETUP STORAGE ##
STORAGE_DIR="/storage/scratch/$USER/$JOB_ID"
mkdir -pv $STORAGE_DIR
cd $STORAGE_DIR
## COPY FILES ##
cp $SGE_CWD_PATH/* $STORAGE_DIR
## EXECUTE CODE ##
time ./Rationals 3 10000 2 out.csv
## COPY OUTPUT BACK ##
cp -a $STORAGE_DIR/* $SGE_CWD_PATH