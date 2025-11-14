#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int my_rank, n_procs, split_rank;
  MPI_Comm split_comm;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

  MPI_Comm_split(MPI_COMM_WORLD, my_rank % 2, my_rank, &split_comm);
  MPI_Comm_rank(split_comm, &split_rank);
  printf("World %d Split %d\n", my_rank, split_rank);
  MPI_Comm_free(&split_comm);

  MPI_Finalize();
}
