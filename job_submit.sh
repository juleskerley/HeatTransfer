#!/bin/bash

#SBATCH --mem=4G
#SBATCH --time=06:00:00

./ompHeat $1 5000 5000 60 1 1000 heaterFile.txt "${SLURM_CPUS_PER_TASK}_${1}_${SLURM_JOB_ID}.csv"
