#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0

int scalar_product(int x[], int y[], int n) {
  int i, sp = 0;
  for (i = 0; i < n; i++)
    sp = sp + x[i] * y[i];
  return sp;
}

int main(int argc, char **argv) {
  int i, j, my_rank, n_procs, ROWS, COLS, *mat, *vec, *prod, sp, *row;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

  // initialize ROWS, COLS and the vector
  ROWS = n_procs;
  COLS = 10;
  row = (int *) malloc(COLS * sizeof(int));
  vec = (int *) malloc(COLS * sizeof(int));
  for (i = 0; i < COLS; i++)
    vec[i] = i;
  // initialize the matrix
  if (my_rank == ROOT) {
    prod = (int *) malloc(ROWS * sizeof(int));
    mat = (int *) malloc(ROWS * COLS * sizeof(int));
    for (i = 0; i < ROWS; i++)
      for (j = 0; j < COLS; j++)
	mat[i * COLS + j] = i + j;
  }

  // distribute the matrix
  MPI_Scatter(mat, COLS, MPI_INT, row, COLS, MPI_INT, ROOT, MPI_COMM_WORLD); 
  // calculate the matrix-vector product and print the result
  sp = scalar_product(row, vec, COLS);
  MPI_Gather(&sp, 1, MPI_INT, prod, 1, MPI_INT, ROOT, MPI_COMM_WORLD); 
  if (my_rank == ROOT) {
    printf("Matrix-vector product: ");
    for (i = 0; i < ROWS; i++)
      printf("%d ", prod[i]);
    printf("\n");
  }

  MPI_Finalize();
}

