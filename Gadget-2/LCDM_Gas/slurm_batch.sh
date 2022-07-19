#!/bin/bash -l
#
#SBATCH --nodes=1
#SBATCH --tasks-per-node=8
#SBATCH -J Gadget2
#SBATCH -o ./gadget2.out
#SBATCH -p cosma7
#SBATCH -A dp004
#SBATCH --exclusive
#SBATCH -t 4:00:00
#

module purge
module load intel_comp/2022.1.2 compiler openmpi/4.1.4
module load parallel_hdf5/1.12.0 fftw/2.1.5 gsl

mpirun /cosma/home/jch/Codes/DiRAC/Gadget-2.0.7/Gadget2/Gadget2 \
    ./parameterfiles/lcdm_gas.param
