#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>


#define N 10

int A[N], *R;

void compute_rank(int);


int main() {
  int k, fd;

  // init vector A[]
  for (k = 0; k < N; k++)
    A[k] = N - k - 1;
  printf("Vector A[]:\n");
  for (k = 0; k < N; k++)
    printf("%d ", A[k]);
  printf("\n");

  // map a file into a shared memory region for R[]
  fd = open("mapfile", O_RDWR | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR); 
  lseek(fd, N * sizeof(int), SEEK_SET);
  write(fd, "", 1);
  R = (int *) mmap(NULL, N * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

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

  // unmap shared memory region
  munmap(R, N * sizeof(int));
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
