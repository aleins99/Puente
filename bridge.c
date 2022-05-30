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
#include <string.h>
#define MAX_CROSSING 20
#define BIG ((double)0x7FFF)

#define MAX_THREADS 20
int r;
pthread_mutex_t Screen;
pthread_mutex_t RandomNumber;
int Max_Run;
int cars[20];

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
  char *Dir[2] = {"<--", "-->"};
  char space[200];
  int i;
  int D;
  Filler(space, ID);
  pthread_mutex_lock(&Screen);
  printf("%sauto0%d started ...\n", space, ID);
  pthread_mutex_unlock(&Screen);
  for (i = 1; i <= Max_Run; i++)
  {               /* for each crossing   */
    sleep(1);     /* rest for a while         */
    D = cars[ID]; /* generate a random number */
    /* which direction?    */
    ArriveBridge(D, ID); /* arrive at the bridge     */
    pthread_mutex_lock(&Screen);
    printf("%sauto0%d (2) cruza el puente %s\n", space, ID, Dir[D]);
    pthread_mutex_unlock(&Screen);
    sleep(1);      /* crossing the bridge      */
    ExitBridge(D); /* exit the bridge          */
  }
  pthread_mutex_lock(&Screen);
  printf("%sauto0%d %s se va\n", space, ID, Dir[D]);
  pthread_mutex_unlock(&Screen);
  pthread_exit(0);
}

/* ----------------------------------------------------------------- */
/*                        The main program                           */
/* ----------------------------------------------------------------- */

void main()
{
  pthread_t ID[MAX_THREADS];  /* vehicle ID               */
  size_t Status[MAX_THREADS]; /* vehicle status           */
  int Arg[MAX_THREADS];       /* vehicle argument         */
  static int Threads;         /* # of vehicles            */
  int i;
  Max_Run = 1;

  pthread_mutex_init(&Screen, (void *)NULL);
  BridgeInit();
  srand((unsigned)time(NULL));
  char op[10];
  int entrada = 0;
  while (1 == 1)
  {
    strcpy(op, "no");
    printf("Ingrese un comando \n");
    int ant = entrada;
    while (strcmp(op, "start") != 0)
    {
      fflush(stdin);
      printf("\n");
      gets(op);
      if (strcmp(op, "car der") == 0)
        cars[entrada++] = 1;
      if (strcmp(op, "car izq") == 0)
        cars[entrada++] = 0;
      if (strcmp(op, "status") == 0)
        status();
    }
    printf("Empiezan ...\n");
    Threads = entrada;
    pthread_t th[Threads];
    for (i = ant; i < Threads; i++)
    { /* start vehicles          */
      Arg[i] = i;
      pthread_create(th + i, NULL, OneVehicle, (void *)&(Arg[i]));
    }
    for (i = ant; i < Threads; i++) /* wait for vehicles       */
      pthread_join(th[i], (void *)&(Status[i]));

    printf("Termina ...\n");
    sleep(1);
  }
}
