#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 10

int A[N], *R;

void compute_rank(int);


int main() {
  int k, shmid;

  // init vector A[]
  for (k = 0; k < N; k++)
    A[k] = N - k - 1;
  printf("Vector A[]:\n");
  for (k = 0; k < N; k++)
    printf("%d ", A[k]);
  printf("\n");

  // allocate and map a shared segment for R[]
  shmid = shmget(IPC_PRIVATE, N * sizeof(int), S_IRUSR | S_IWUSR);
  R = (int *) shmat(shmid, NULL, 0);

  // each child executes one task
  for (k = 0; k < N; k++) 
    if (fork() == 0) {
      compute_rank(A[k]);
      exit(0);
    }

  // parent waits for all children to complete
  for (k = 0; k < N; k++)
    wait(NULL);

  // parent shows result
  printf("Vector R[]:\n");
  for (k = 0; k < N; k++)
    printf("%d ", R[k]);
  printf("\n");

  // free and remove shared segment
  shmdt(R);
  shmctl(shmid, IPC_RMID, NULL);
}


void compute_rank(int elem) {
  int i, rank;

  rank = 0;
  for (i = 0; i < N; i++)  
    if (elem > A[i])
      rank++;
  R[rank]= elem;
  return;
}
