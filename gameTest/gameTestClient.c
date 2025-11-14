#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define N_BOARD 4  // do not change
#define N_PLAYERS 10
#define N_LETTERS 6

#define IPC_KEY 0x1234
#define PLAYER_OUT 0
#define PLAYER_IN 1
#define SERVER_MOVE 0
#define PLAYER_MOVE 1
#define SEM_PLAYER(PID) (&sem[PID])     // semaphore to notify player
#define SEM_SERVER (&sem[N_PLAYERS])    // semaphore to notify server
#define SEM_ENTER  (&sem[N_PLAYERS+1])  // semaphore to enter the game

typedef struct {
  char state;          // PLAYER_OUT: no player; PLAYER_IN: player in the game
  char who;            // SERVER_MOVE: server has responded; PLAYER_MOVE: player has made new attempt
  char move[N_BOARD];  // last attempt of player or last response of server
  int attempts;        // number of attempts
} struct_player;

struct_player *players;
sem_t *sem;
int shmid, pid;

void exit_signal(int sig);


int main() {
  // catch signal SIGINT (send by terminal when you press CTRL-C)
  signal(SIGINT, exit_signal);
  // allocate shared segment
  if ((shmid = shmget(IPC_KEY, sizeof(struct_player) * N_PLAYERS + sizeof(sem_t) * (N_PLAYERS + 2),
                      S_IRUSR | S_IWUSR)) == -1) {
    printf("shmget error!\n");
    exit(0);
  }
  // init players
  if ((players = (struct_player *) shmat(shmid, NULL, 0)) == (void *) -1) {
    printf("shmat error!\n");
    exit(0);
  }
  // init semaphores
  sem = (sem_t *) ((void *) players + sizeof(struct_player) * N_PLAYERS);
  // try enter the game
  // wait for entering the game
  sem_wait(SEM_ENTER);
  // find a player slot
  for (pid = 0; pid < N_PLAYERS; pid++)
    if (players[pid].state == PLAYER_IN) {
      break;
    }
  // release entering the game
  sem_post(SEM_ENTER);
  // check if found a slot
  if (pid == N_PLAYERS) {
    printf("No free player slots. Exiting...\n");
    // free shared segment
    shmdt(players);
    // terminate the process
    exit(0);
  }
  printf("Player %d is playing...\n", pid);
  // main loop
  while(1) {
    // make a move (here we just generate a random move)
    // wait for server response
    sem_wait(SEM_SERVER);
    // process server response
    if (players[pid].who == SERVER_MOVE) {
      printf("Player %d received server move: %.*s\n", pid, N_BOARD, players[pid].move);
    }
    // notify server
    sem_post(SEM_SERVER);
  }
  return 0;
}

void exit_signal(int sig) {
  // exit the game
  sem_wait(SEM_ENTER);
  players[pid].state = PLAYER_OUT;
  sem_post(SEM_ENTER);
  printf("\n\nPlayer %d exiting...\n", pid);
  // free shared segment
  shmdt(players);
  // terminate the process
  exit(0);
}