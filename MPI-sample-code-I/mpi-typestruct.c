#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MY_TAG 0

struct {
  int a; 
  char b[4];
  double c[2];
} my_struct;

int main(int argc, char **argv) {
  int my_rank, n_procs, blocklengths[3];
  MPI_Datatype strType, oldtypes[3];
  MPI_Aint lb, int_length, char_length, offsets[3];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
  if (n_procs != 2) {
    MPI_Finalize();
    if (my_rank == 0)
      printf("Run the program with 2 processes!\n");
    exit(0);
  }

  // build a derived data type representing my_struct

  MPI_Type_get_extent(MPI_INT, &lb, &int_length);
  MPI_Type_get_extent(MPI_CHAR, &lb, &char_length);
  
  blocklengths[0] = 1;
  blocklengths[1] = 4;
  blocklengths[2] = 2;
  oldtypes[0] = MPI_INT;
  oldtypes[1] = MPI_CHAR;
  oldtypes[2] = MPI_DOUBLE;
  offsets[0] = 0;
  offsets[1] = int_length;
  offsets[2] = int_length + 4 * char_length;
  // alternative using MPI_Address to compute the offsets[] 
  /*
  MPI_Address(&(my_struct.a), &offsets[0]);
  MPI_Address(&(my_struct.b), &offsets[1]);
  MPI_Address(&(my_struct.c), &offsets[2]);
  offsets[2] -= offsets[0];
  offsets[1] -= offsets[0];
  offsets[0] = 0;
  */
  //MPI_Type_struct(3, blocklengths, offsets, oldtypes, &strType);
  MPI_Type_create_struct(3, blocklengths, offsets, oldtypes, &strType);
  
  MPI_Type_commit(&strType);

  if (my_rank == 0) {
    // initialize my_struct
    my_struct.a = 10;
    sprintf(my_struct.b, "mpi");
    my_struct.c[0] = 5.5;
    my_struct.c[1] = 12.4567;
    // send my_struct
    MPI_Send(&my_struct, 1, strType, 1, MY_TAG, MPI_COMM_WORLD);
    printf("Proc 0 sending: %d - %s - %f - %f\n", my_struct.a, my_struct.b, my_struct.c[0], my_struct.c[1]);
  } else {
    // receive in my_struct
    MPI_Recv(&my_struct, 1, strType, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Proc 1 receiving: %d - %s - %f - %f\n", my_struct.a, my_struct.b, my_struct.c[0], my_struct.c[1]);
  }

  // free the derived data type
  MPI_Type_free(&strType);
  MPI_Finalize();
}
