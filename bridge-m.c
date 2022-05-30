#include <pthread.h>

#include "bridge-m.h"
#include <stdio.h>
#define MAX_VEHICLE 3 /* max vehicle on bridge    */
#define TRUE 1
#define FALSE 0
#define EAST 0 /* east bound               */
#define WEST 1 /* west bound               */

static int CurrentDirection; /* current direction of cars*/
static int VehicleCount;     /* # of vehicle on bridge   */
static int Waiting[2];       /* # east/west bound waiting*/
static int stat[20];
static pthread_mutex_t MonitorLock; /* monitor lock             */
static pthread_cond_t EastWest[2];  /* blocking east/west cars  */
static int bridge[50];
static int print[50];
char *Dir[2] = {"<--", "-->"};

void BridgeInit(void)
{
  VehicleCount = 0;            /* no vehicle on bridge     */
  Waiting[0] = Waiting[1] = 0; /* no vehicle waiting       */
  for (int i = 0; i < 50; i++)
  {
    stat[i] = -1;
    bridge[i] = -1;
    print[i] = -1;
  }
  pthread_mutex_init(&MonitorLock, (void *)NULL);
  pthread_cond_init(&EastWest[0], (void *)NULL);
  pthread_cond_init(&EastWest[1], (void *)NULL);
}

static int isSafe(int Direction, int id)
{
  if (id == 0 && Direction == 0)
    return FALSE;
  if (VehicleCount == 0) /* if no vehicle on bridge  */
    return TRUE;         /* safe to cross            */
  else if ((VehicleCount < MAX_VEHICLE) && (CurrentDirection == Direction))
    return TRUE; /* if < 3 in same direction */
  else
  {
    return FALSE; /* otherwise, do not procee */
  }
}

void ArriveBridge(int Direction, int id)
{

  pthread_mutex_lock(&MonitorLock); /* lock the monitor         */
  if (!isSafe(Direction, id))
  {
    Waiting[Direction]++; /* no, wait at the bridge   */
    stat[id] = Direction;
    status();
    while (!isSafe(Direction, id)) /* safe to cross?      */
      pthread_cond_wait(&EastWest[Direction], &MonitorLock);
    Waiting[Direction]--; /* go back to test again    */
    stat[id] = -1;
  }
  VehicleCount++;
  bridge[id] = Direction;
  for (int i = 0; i < 50; i++)
  {
    if (bridge[i] >= 0 && print[i] == -1)
    {
      printf("auto0%d (1) ingresa al puente %s\n", i + 1, Dir[bridge[i]]);
      print[i] = 0;
    }
  }
  CurrentDirection = Direction;       /* set direction
                                       */
  pthread_mutex_unlock(&MonitorLock); /* release monitor          */
}
int middle(char *s)
{
  int aux = 0;
  for (int i = 0; i < 50; i++)
  {
    if (bridge[i] >= 0 && print[i] == 0)
    {
      aux = 1;
      printf("auto0%d (2) en medio del puente %s\n", i + 1, Dir[bridge[i]]);
      print[i] = 1;
    }
  }
  status();
  return aux;
}
void ExitBridge(int Direction, int id)
{
  bridge[id] = Direction - 3;
  pthread_mutex_lock(&MonitorLock);            /* lock the monitor         */
  VehicleCount--;                              /* one vehicle exits        */
  if (VehicleCount > 0)                        /* have vehicles on bridge? */
    pthread_cond_signal(&EastWest[Direction]); /* yes,same dir*/
  else
  {                                                  /* bridge is empty          */
    if (Waiting[1 - Direction] != 0)                 /* any opposite wait?*/
      pthread_cond_signal(&EastWest[1 - Direction]); /* yes */
    else                                             /* no, release the same dir */
      pthread_cond_signal(&EastWest[Direction]);
  }
  pthread_mutex_unlock(&MonitorLock); /* release the monitor      */
  for (int i = 0; i < 50; i++)
  {
    if (bridge[i] < 0 && print[i] == 1)
    {
      printf("auto0%d %s se va\n", i + 1, Dir[bridge[i] + 3]);
      print[i] = 2;
    }
  }
}

void status()
{
  int aux = 0;
  char *Dir[2] = {"<--", "-->"};
  for (int i = 0; i < 20; i++)
  {
    if (stat[i] != -1)
    {
      printf("auto0%d en espera en direccion %s\n\n", i + 1, Dir[stat[i]]);
      aux = 1;
    }
  }
  if (aux == 0)
    printf("No hay autos aun en espera\n");
}
