#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 10

int A[N], R[N];

void compute_rank(int);


int main() {
  int k;

  // init vector A[]
  for (k = 0; k < N; k++)
    A[k] = N - k - 1;
  printf("Vector A[]:\n");
  for (k = 0; k < N; k++)
    printf("%d ", A[k]);
  printf("\n");

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
  return 0;
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
