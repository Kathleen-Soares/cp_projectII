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
  int i, my_rank, n_procs, K, sp, my_sp, *vecX, *vecY, *locX, *locY;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

  // initialize vecX, vecY and K
  if (my_rank == ROOT) {
    K = 100;
    vecX = (int *) malloc(K * n_procs * sizeof(int));
    vecY = (int *) malloc(K * n_procs * sizeof(int));
    for (i = 0; i < K * n_procs; i++) {
      vecX[i] = i;
      vecY[i] = K * n_procs - 1 - i;
    }
  }

  // send K to all processes
  MPI_Bcast(&K, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
  // allocate space for local components of vecX and vecY
  locX = (int *) malloc(K * sizeof(int));
  locY = (int *) malloc(K * sizeof(int));
  // distribute the components of vecX and vecY
  MPI_Scatter(vecX, K, MPI_INT, locX, K, MPI_INT, ROOT, MPI_COMM_WORLD); 
  MPI_Scatter(vecY, K, MPI_INT, locY, K, MPI_INT, ROOT, MPI_COMM_WORLD); 
  // calculate the scalar product and print the result
  my_sp = scalar_product(locX, locY, K);
  MPI_Reduce(&my_sp, &sp, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
  if (my_rank == ROOT) {
    printf("Scalar product (all) = %d\n", sp);
    // confirm that the result is OK!
    sp = scalar_product(vecX, vecY, K * n_procs);
    printf("Scalar product (root) = %d\n", sp);
  }

  MPI_Finalize();
}

