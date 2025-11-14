run-mpi-alive-multiple-machines:
	mpicc mpi-alive.c && mpirun -np 48  --use-hwthread-cpus --hostfile hostfile.txt --display-map a.out

run-mpi-alive-one-machine:
	mpicc mpi-alive.c && mpirun -np 48 --oversubscribe  --display-map a.out
