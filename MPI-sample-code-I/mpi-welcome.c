#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MY_TAG 0
#define BSIZE 1000

int main(int argc, char **argv) {
  int i, my_rank, n_procs, buf_size;
  char msg[100], *local_buf;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

  buf_size = BSIZE;
  local_buf = (char *) malloc(buf_size);
  MPI_Buffer_attach(local_buf, buf_size);

  sprintf(msg, "Welcome!");
  for (i = 0; i < n_procs; i++)
    if (my_rank != i)
      MPI_Bsend(msg, strlen(msg) + 1, MPI_CHAR, i, MY_TAG, MPI_COMM_WORLD);
  for (i = 0; i < n_procs; i++)
    if (my_rank != i) {
      sprintf(msg, "Argh!");
      MPI_Recv(msg, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      printf("%d->%d: %s\n", status.MPI_SOURCE, my_rank, msg);
    }

  MPI_Buffer_detach(&local_buf, &buf_size);
  free(local_buf);
  MPI_Finalize();
}
