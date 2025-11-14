#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MY_TAG 0
#define BSIZE 1000

int main(int argc, char **argv) {
  int i, j, my_rank, n_procs, pos, *my_matrix, ROWS, COLS;
  char pack_buf[BSIZE];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
  if (n_procs != 2) {
    MPI_Finalize();
    if (my_rank == 0)
      printf("Run the program with 2 processes!\n");
    exit(0);
  }

  if (my_rank == 0) {
    // initialize ROWS, COLS and my_matrix
    ROWS = 3;
    COLS = 5;
    my_matrix = (int *) malloc(ROWS * COLS * sizeof(int));
    for (i = 0; i < ROWS; i++)
      for (j = 0; j < COLS; j++)
	my_matrix[i * COLS + j] = i + j;
    // pack and send ROWS, COLS and my_matrix
    pos = 0;
    MPI_Pack(&ROWS, 1, MPI_INT, pack_buf, BSIZE, &pos, MPI_COMM_WORLD);
    MPI_Pack(&COLS, 1, MPI_INT, pack_buf, BSIZE, &pos, MPI_COMM_WORLD);
    MPI_Pack(my_matrix, ROWS * COLS, MPI_INT, pack_buf, BSIZE, &pos, MPI_COMM_WORLD);
    MPI_Send(pack_buf, pos, MPI_PACKED, 1, MY_TAG, MPI_COMM_WORLD);
  } else {
    // receive and unpack ROWS, COLS and my_matrix
    pos = 0;
    MPI_Recv(&pack_buf, BSIZE, MPI_PACKED, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Unpack(&pack_buf, BSIZE, &pos, &ROWS, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(&pack_buf, BSIZE, &pos, &COLS, 1, MPI_INT, MPI_COMM_WORLD);
    // allocate space to represent my_matrix
    my_matrix = (int *) malloc(ROWS * COLS * sizeof(int));
    MPI_Unpack(&pack_buf, BSIZE, &pos, my_matrix, ROWS * COLS, MPI_INT, MPI_COMM_WORLD);
    // printf results
    printf("ROWS: %d\nCOLS: %d\nMatrix:\n", ROWS, COLS);
    for (i = 0; i < ROWS; i++) {
      for (j = 0; j < COLS; j++)
	printf("%d ", my_matrix[i * COLS + j]);
      printf("\n");
    }
  }

  MPI_Finalize();
}
