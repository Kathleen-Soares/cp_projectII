#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MY_TAG 0
#define ROWS 10
#define COLS 5

int main(int argc, char **argv) {
  int i, j, my_rank, n_procs, my_matrix[ROWS][COLS], my_vector[ROWS];
  MPI_Datatype colMatrix;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
  if (n_procs != 2) {
    MPI_Finalize();
    if (my_rank == 0)
      printf("Run the program with 2 processes!\n");
    exit(0);
  }

  // initialize my_matrix
  for (i = 0; i < ROWS; i++)
    for (j = 0; j < COLS; j++)
      my_matrix[i][j] = i + j;

  // build a derived data type with ROWS blocks of 1 element separated by COLS elements
  MPI_Type_vector(ROWS, 1, COLS, MPI_INT, &colMatrix);
  MPI_Type_commit(&colMatrix);

  if (my_rank == 0) {
    // send column 1 of my_matrix
    MPI_Send(&my_matrix[0][1], 1, colMatrix, 1, MY_TAG, MPI_COMM_WORLD);
    printf("Proc 0 sending column 1: ");
    for(i = 0; i < ROWS; i++)
      printf("%d ", my_matrix[i][1]);
    printf("\n");
    // send column 2 of my_matrix
    MPI_Send(&my_matrix[0][2], 1, colMatrix, 1, MY_TAG, MPI_COMM_WORLD);
    printf("Proc 0 sending column 2: ");
    for(i = 0; i < ROWS; i++)
      printf("%d ", my_matrix[i][2]);
    printf("\n");
  } else {
    // receive in column 3 of my_matrix
    MPI_Recv(&my_matrix[0][3], 1, colMatrix, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Proc 1 receiving in column 3: ");
    for(i = 0; i < ROWS; i++)
      printf("%d ", my_matrix[i][3]);
    printf("\n");
    // receive in my_vector
    MPI_Recv(&my_vector, ROWS, MPI_INT, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Proc 1 receiving in vector: ");
    for(i = 0; i < ROWS; i++)
      printf("%d ", my_vector[i]);
    printf("\n");
  }

  // free the derived data type
  MPI_Type_free(&colMatrix);
  MPI_Finalize();
}
