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

#define MAX_THREADS 50
int r;
pthread_mutex_t Screen;
pthread_mutex_t RandomNumber;
int Max_Run;
int cars[50];
size_t Status[MAX_THREADS]; /* vehicle status           */
pthread_cond_t cv;
static int Arg[MAX_THREADS];
static int entrada;
void menu();
/* ---------------------------------------------------------------- */
/* FUNCTION  Filler():                                              */
/*    This function fills a char array with spaces.                 */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* FUNCTION  OneVehicle():                                          */
/*   This function simulates a vehicle crossing the bridge.         */
/* ---------------------------------------------------------------- */

void *OneVehicle(void *voidPTR)
{
  while (1)
  {
    int *intPTR = (int *)voidPTR;
    int ID = *intPTR;
    char *Dir[2] = {"<--", "-->"};
    int i;
    int D;
    /* rest for a while         */
    D = cars[ID];        /* generate a random number */
                         /* which direction?    */
    ArriveBridge(D, ID); /* arrive at the bridge     */
    pthread_mutex_lock(&Screen);
    sleep(1);
    pthread_mutex_unlock(&Screen);
    menu();
    /* crossing the bridge    */

    int aux = middle(Dir[D]);
    sleep(1);
    if (aux == 1)
    {
      menu();
    }
    ExitBridge(D, ID); /* exit the bridge          */
    pthread_exit(0);
  }
}

/* ----------------------------------------------------------------- */
/*                        The main program                           */
/* ----------------------------------------------------------------- */

void main()
{
  pthread_t ID[MAX_THREADS]; /* vehicle ID               */
                             /* vehicle argument         */
                             /* # of vehicles            */
  int i;

  pthread_mutex_init(&Screen, (void *)NULL);
  BridgeInit();
  entrada = 0;

  menu();
  printf("Termina ...\n");
}

void menu()
{
  static int Threads;
  char op[10];
  int i;
  strcpy(op, "no");
  int ant = entrada;
  while (strcmp(op, "start") != 0)
  {

    fflush(stdin);
    gets(op);
    if (strcmp(op, "car der") == 0)
      cars[entrada++] = 1;
    if (strcmp(op, "car izq") == 0)
      cars[entrada++] = 0;
    if (strcmp(op, "status") == 0)
      status();
  }
  Threads = entrada;
  pthread_t th[Threads];

  for (i = ant; i < Threads; i++)
  { /* start vehicles          */
    Arg[i] = i;
    pthread_create(th + i, NULL, OneVehicle, (void *)&(Arg[i]));
  }
  for (i = ant; i < Threads; i++) /* wait for vehicles       */
    pthread_join(th[i], (void *)&(Status[i]));
}