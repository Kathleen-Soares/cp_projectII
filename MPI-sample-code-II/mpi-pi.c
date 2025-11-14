#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0
#define SERVER (n_procs - 1)
#define REQ_POINTS 1000
#define TAG_REQUEST 0
#define TAG_REPLY 1

int my_rank, n_procs, total_in, total_out, total_points;
MPI_Comm workers_comm;

void server(void) {
  int i, req_points, rands[REQ_POINTS];
  MPI_Status status;

  do {
    MPI_Recv(&req_points, 1, MPI_INT, MPI_ANY_SOURCE, TAG_REQUEST, MPI_COMM_WORLD, &status);
    if (req_points) {
      for (i = 0; i < req_points; i++)
	rands[i] = random();
      MPI_Send(rands, req_points, MPI_INT, status.MPI_SOURCE, TAG_REPLY, MPI_COMM_WORLD);
    }
  } while (req_points);
}

void client(void) {
  int in, out, more, i, req_points, rands[REQ_POINTS];
  double x, y;

  in = out = 0;
  req_points = REQ_POINTS;
  do {
    MPI_Send(&req_points, 1, MPI_INT, SERVER, TAG_REQUEST, MPI_COMM_WORLD);
    MPI_Recv(rands, req_points, MPI_INT, SERVER, TAG_REPLY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (i = 0; i < req_points; i += 2) {
      x = (((double) rands[i]) / RAND_MAX) * 2 - 1;
      y = (((double) rands[i+1]) / RAND_MAX) * 2 - 1;
      (x * x + y * y < 1.0) ? in++ : out++;
    }
    MPI_Allreduce(&in, &total_in, 1, MPI_INT, MPI_SUM, workers_comm);
    MPI_Allreduce(&out, &total_out, 1, MPI_INT, MPI_SUM, workers_comm);
    more = (total_in + total_out < total_points);
    if (more == 0 && my_rank == ROOT)
      MPI_Send(&more, 1, MPI_INT, SERVER, TAG_REQUEST, MPI_COMM_WORLD);
  } while (more);
}

int main(int argc, char **argv) {
  int ranks[1];
  double start, finish;
  MPI_Group world_group, worker_group;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
  if (n_procs < 2) {
    MPI_Finalize();
    if (my_rank == 0)
      printf("Run the program with at least 2 processes!\n");
    exit(0);
  }

  // define a new communicator for the clients
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);
  ranks[0] = SERVER;
  MPI_Group_excl(world_group, 1, ranks, &worker_group);
  MPI_Comm_create(MPI_COMM_WORLD, worker_group, &workers_comm);
  MPI_Group_free(&worker_group);
  MPI_Group_free(&world_group);

  // read the number of points to process and broadcast it
  if (my_rank == ROOT) {
    printf("Enter number of points:\n");
    scanf("%d", &total_points);
  }
  MPI_Bcast(&total_points, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  // initialize the time counter
  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();
  // compute the approximate value of PI
  if (my_rank == SERVER)
    server();
  else
    client();
  // end time counting and output result
  MPI_Barrier(MPI_COMM_WORLD);
  finish = MPI_Wtime();
  if (my_rank == ROOT) {
    printf("PI = %.20f\n", (4.0 * total_in) / (total_in + total_out));
    printf("Execution time = %f seconds\n", finish - start);
  }

  if (my_rank != SERVER)
    MPI_Comm_free(&workers_comm);
  MPI_Finalize();
}
