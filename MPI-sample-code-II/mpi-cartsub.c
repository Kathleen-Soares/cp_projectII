#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int n_procs, my_rank, grid_rank, row_rank, reorder, dims[2], periods[2];
  MPI_Comm grid_comm, col_comm;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  dims[0] = 4;
  dims[1] = 2;
  periods[0] = 1;
  periods[1] = 1;
  reorder = 0;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &grid_comm);
  MPI_Comm_rank(grid_comm, &grid_rank);
  dims[0] = 1;  // dimension 0 is part of the new communicators
  dims[1] = 0;  // dimension 1 is not part of the new communicators
  MPI_Cart_sub(grid_comm, dims, &col_comm);
  MPI_Comm_rank(col_comm, &row_rank);
  printf("World %d Grid %d Row %d\n", my_rank, grid_rank, row_rank);

  MPI_Comm_free(&grid_comm);
  MPI_Finalize();
}
