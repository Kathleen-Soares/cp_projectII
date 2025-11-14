#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>

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
char code[N_PLAYERS][N_BOARD];
int shmid;

void new_code(int);
void check_code(int);
void exit_signal(int);


main() {
  int i, pid = -1;

  // catch signal SIGINT (send by terminal when you press CTRL-C)
  signal(SIGINT, exit_signal);
  // allocate a new shared segment
  if ((shmid = shmget(IPC_KEY, sizeof(struct_player) * N_PLAYERS + sizeof(sem_t) * (N_PLAYERS + 2),
                      IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
    printf("shmget error!\n");
    exit(0);
  }
  // init players
  if ((players = (struct_player *) shmat(shmid, NULL, 0)) == (void *) -1) {
    printf("shmat error!\n");
    exit(0);
  }
  for (i = 0; i < N_PLAYERS; i ++)
    players[i].state = PLAYER_OUT;
  // init semaphores
  sem = (sem_t *) ((void *) players + sizeof(struct_player) * N_PLAYERS);
  for (i = 0; i < N_PLAYERS; i ++)
    sem_init(SEM_PLAYER(i), 1, 0);
  sem_init(SEM_SERVER, 1, 0);
  sem_init(SEM_ENTER, 1, 1);
  printf("Server is running...\n");
  // main loop
  while(1) {
    ...
  }
  return 0;
}

void new_code(int pid) {
  int i;
  for (i = 0; i < N_BOARD; i++)
    code[pid][i] = 'A' + (((double) random()) / RAND_MAX) * N_LETTERS;
  printf("New code sequence for player %d: %.*s\n", pid, N_BOARD, code[pid]);
}

void check_code(int pid) {
  int black, white, i, j;
  black = 0;
  white = 0;
  for (i = 0; i < N_BOARD; i++)
    if (code[pid][i] == players[pid].move[i])
      black++;
  for (j = 0; j < N_BOARD; j++)
    for (i = 0; i < N_BOARD; i++)
      if (code[pid][j] == players[pid].move[i]) {
	players[pid].move[i] = 0;
	white++;
	break;
      }
  players[pid].move[0] = '0' + black;
  players[pid].move[1] = 'B';
  players[pid].move[2] = '0' + white - black;
  players[pid].move[3] = 'W';
}

void exit_signal(int sig) {
  int i;
  // free and remove shared segment
  shmdt(players);
  shmctl(shmid, IPC_RMID, NULL);
  // destroy semaphores
  for (i = 0; i < N_PLAYERS; i ++)
    sem_destroy(SEM_PLAYER(i));
  sem_destroy(SEM_SERVER);
  sem_destroy(SEM_ENTER);
  // terminate the process
  exit(0);
}