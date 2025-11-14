#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int i, my_rank, n_procs, ranks[100], even_rank;
  MPI_Group world_group, even_group;
  MPI_Comm even_comm;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

  for (i = 0; i < n_procs; i += 2)
    ranks[i/2] = i;
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);
  MPI_Group_incl(world_group, (n_procs + 1)/2, ranks, &even_group);
  MPI_Comm_create(MPI_COMM_WORLD, even_group, &even_comm);
  MPI_Group_free(&world_group);
  MPI_Group_free(&even_group);
  if (my_rank % 2 == 0) {
    MPI_Comm_rank(even_comm, &even_rank);
    printf("World %d Even %d\n", my_rank, even_rank);
    MPI_Comm_free(&even_comm);
  }

  MPI_Finalize();
}
