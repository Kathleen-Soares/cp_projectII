#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int n_procs, my_rank, grid_rank, up, down, right, left, reorder, dims[2], periods[2], coords[2];
  MPI_Comm grid_comm;

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
  MPI_Cart_coords(grid_comm, grid_rank, 2, coords);
  // up
  coords[0] -= 1;
  MPI_Cart_rank(grid_comm, coords, &up);
  // down
  coords[0] += 2;
  MPI_Cart_rank(grid_comm, coords, &down);
  // left
  coords[0] -= 1;
  coords[1] -= 1;
  MPI_Cart_rank(grid_comm, coords, &left);
  // right
  coords[1] += 2;
  MPI_Cart_rank(grid_comm, coords, &right);
  coords[1] -= 1;
  printf("World %d Grid %d (%d,%d) --> up %d down %d left %d right %d\n", 
         my_rank, grid_rank, coords[0], coords[1], up, down, left, right);

  MPI_Comm_free(&grid_comm);
  MPI_Finalize();
}
