#PBS -S /bin/bash
#PBS -q main
#PBS -l walltime=00:00:10
#PBS -l select=1:ncpus=1:mem=1GB

./program a=1.003 b=31.66 c=4 d=x > ./data_17_9_31_18_9_2020/1.data

qstat -f $PBS_JOBID | grep resources_used.vmem > ./data_17_9_31_18_9_2020/1.data
qstat -f $PBS_JOBID | grep resources_used.walltime > ./data_17_9_31_18_9_2020/1.data
