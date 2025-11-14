#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define MY_TAG 0

int main(int argc, char **argv) {
  int i, my_rank, n_procs;
  char msg[100], recv_msg[100];
  MPI_Status status;
  MPI_Request req[100];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

  for (i = 0; i < n_procs; i++)
    if (my_rank != i) {
      sprintf(msg, "Hello proc %d!", i);
      MPI_Irecv(recv_msg, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &(req[i]));
      MPI_Isend(msg, strlen(msg) + 1, MPI_CHAR, i, MY_TAG, MPI_COMM_WORLD, &(req[i + n_procs]));
    }
  for (i = 0; i < n_procs; i++)
    if (my_rank != i) {
      sprintf(recv_msg, "Argh!");
      MPI_Wait(&(req[i]), &status);
      printf("%d->%d: %s\n", status.MPI_SOURCE, my_rank, recv_msg);
      MPI_Wait(&(req[i + n_procs]), &status);
    }

  MPI_Finalize();
}
