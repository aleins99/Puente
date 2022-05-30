/* ---------------------------------------------------------------- */
/* FILE  bridge.c                                                   */
/*    This program solves the bridge-crossing problem using monitor */
/* ---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include "bridge-m.h"

#define MAX_CROSSING 20
#define BIG ((double)0x7FFF)

#define MAX_THREADS 20
int r;
pthread_mutex_t Screen;
pthread_mutex_t RandomNumber;
int tests = 1;
int Max_Run;

/* ---------------------------------------------------------------- */
/* FUNCTION  Filler():                                              */
/*    This function fills a char array with spaces.                 */
/* ---------------------------------------------------------------- */

void Filler(char x[], int n)
{
  int i;

  for (i = 0; i < n * 2; i++)
    x[i] = ' ';
  x[i] = '\0';
}

/* ---------------------------------------------------------------- */
/* FUNCTION  OneVehicle():                                          */
/*   This function simulates a vehicle crossing the bridge.         */
/* ---------------------------------------------------------------- */

void *OneVehicle(void *voidPTR)
{
  int *intPTR = (int *)voidPTR;
  int ID = *intPTR;
  int Direction;
  char *Dir[2] = {"<--", "-->"};
  char space[200];
  int i;
  double D;

  Filler(space, ID);
  pthread_mutex_lock(&Screen);
  printf("%sVehicle %d started ...\n", space, ID);
  pthread_mutex_unlock(&Screen);
  for (i = 1; i <= Max_Run; i++)
  {                                      /* for each crossing   */
    sched_yield();                       /* rest for a while         */
    pthread_mutex_lock(&RandomNumber);   /* lock random # generator  */
    D = rand() % 2;                      /* generate a random number */
    pthread_mutex_unlock(&RandomNumber); /* release random # gen.    */
    Direction = D;
    /* which direction?    */
    pthread_mutex_lock(&Screen);
    printf("%s Vehiculo %d (%d) llega al puente en la  "
           "direccion %s\n",
           space, ID, i, Dir[Direction]);
    pthread_mutex_unlock(&Screen);
    ArriveBridge(Direction); /* arrive at the bridge     */
    pthread_mutex_lock(&Screen);
    printf("%sVehiculo %d (2) cruza el puente\n", space, ID);
    pthread_mutex_unlock(&Screen);
    sched_yield();         /* crossing the bridge      */
    ExitBridge(Direction); /* exit the bridge          */
    pthread_mutex_lock(&Screen);
    printf("%sVehicle %d (3) termina\n", space, ID);
    pthread_mutex_unlock(&Screen);
  }
  pthread_mutex_lock(&Screen);
  printf("%sVehicle %d se va para siempre ...\n", space, ID);
  pthread_mutex_unlock(&Screen);
  pthread_exit(0);
}

/* ----------------------------------------------------------------- */
/*                        The main program                           */
/* ----------------------------------------------------------------- */

void main(int argc, char *argv[])
{
  pthread_t ID[MAX_THREADS];  /* vehicle ID               */
  size_t Status[MAX_THREADS]; /* vehicle status           */
  int Arg[MAX_THREADS];       /* vehicle argument         */
  int Threads;                /* # of vehicles            */
  int i;

  if (argc != 2)
  {
    printf("Coloque la cantidad de vehiculos que tienen que pasar en %s", argv[0]);
    exit(0);
  }
  Max_Run = 1;
  Threads = abs(atoi(argv[1]));
  if (Threads > MAX_THREADS)
  {
    printf("Hay demasiada cantidad de vehiculos %d\n",
           MAX_THREADS);
    Threads = MAX_THREADS;
  }

  printf("Empiezan ...\n");

  pthread_mutex_init(&Screen, (void *)NULL);
  BridgeInit();
  srand((unsigned)time(NULL));
  pthread_t th[Threads];
  for (i = 0; i < Threads; i++)
  { /* start vehicles          */
    Arg[i] = i + 1;
    pthread_create(th + i, NULL, OneVehicle, (void *)&(Arg[i]));
  }
  for (i = 0; i < Threads; i++) /* wait for vehicles       */
    pthread_join(th[i], (void *)&(Status[i]));

  printf("Termina ...\n");
}
