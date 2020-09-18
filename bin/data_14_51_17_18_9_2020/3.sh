#PBS -S /bin/bash
#PBS -q main
#PBS -l walltime=00:00:10
#PBS -l select=1:ncpus=1:mem=1GB

./program a=1.003 b=31.66 c=4 d=y > ./data_14_51_17_18_9_2020/3.data

qstat -f $PBS_JOBID | grep resources_used.vmem > ./data_14_51_17_18_9_2020/3.data
qstat -f $PBS_JOBID | grep resources_used.walltime > ./data_14_51_17_18_9_2020/3.data
