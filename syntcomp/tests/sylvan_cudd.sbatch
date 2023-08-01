#!/bin/bash
#parameters for slurm
#SBATCH -J sylvan-cudd                            # job name, don't use spaces, keep it short
#SBATCH -c 32                                     # number of cores, 32
#SBATCH --gres=gpu:1                              # number of gpus 1, some clusters don't have GPUs
#SBATCH --mem=16gb                                # Job memory request
#SBATCH --mail-type=END,FAIL                      # email status changes (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=a.pistek@student.utwente.nl   # Where to send mail to
#SBATCH --time=8:00:00                            # time limit 8h
#SBATCH --output=%j.log                           # Standard output and error log
#SBATCH --error=%j.err                            # if you want the errors logged seperately

# Create a directory for this job on the node
cd /local
mkdir ${SLURM_JOBID}
cd ${SLURM_JOBID}

# Copy input and executable to the node
cp -r ${SLURM_SUBMIT_DIR}/* .

# It's nice to have some information logged for debugging
echo "Date              = $(date)"
echo "Hostname          = $(hostname -s)" # log hostname
echo "Working Directory = $(pwd)"
echo "Number of nodes used        : "$SLURM_NNODES
echo "Number of threads           : "$SLURM_CPUS_PER_TASK
echo "Number of threads per core  : "$SLURM_THREADS_PER_CORE
echo "Name of nodes used          : "$SLURM_JOB_NODELIST
echo ""

caseName=${PWD##*/} # to distinguish several log files
# Run the job -- make sure that it terminates itself before time is up
# Do not submit into the background (i.e. no & at the end of the line).

chmod +x sylvan_cudd.sh
./sylvan_cudd.sh

mkdir ${SLURM_SUBMIT_DIR}/job_results/${SLURM_JOBID}
# Copy output back to the master, comment with # if not used
cp -r ./results/* ${SLURM_SUBMIT_DIR}/job_results/${SLURM_JOBID}/
cp ./* ${SLURM_SUBMIT_DIR}/job_results/${SLURM_JOBID}/

# Clean up on the node ! make sure you are still on the node...
rm *
cd ..
rmdir ${SLURM_JOBID}
# Done.