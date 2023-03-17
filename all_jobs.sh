#!/bin/bash

for core_count in 4 4 8 8 16 16; do
    for thread_count in 4 8 16 32; do
        sbatch --cpus-per-task=$core_count ./job_submit.sh $thread_count
    done
done
