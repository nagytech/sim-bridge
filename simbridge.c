#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "simbridge.h"

int nid = 1;
vehicle_t lv, cv;
void *la;
long int start;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t w = PTHREAD_COND_INITIALIZER;

/*
 * SimBridge
 * ---------
 * Simulates the coordination of cars and trucks on a a single lane bridge using
 * threads, mutex, and condition variables.
 *
 * author:    Jonathan Nagy <jnagy@myune.edu.au>
 * studentid: (220103482)
 * date:      15 July 2016
 *
 * arguments:
 *   none
 *
 * returns: writes the coordinated bridge activity and the traffic behaviour to
 * stdout
 */
 int main(int argc, char **argv) {

   // Seed the randomizer
   srand(time(NULL));

   struct timeval tv;
   gettimeofday(&tv, NULL);
   start = tv.tv_sec;

   // Create a series of vehicles as threads
   pthread_t pts[THREADCOUNT];
   vehicle_t *vts[THREADCOUNT];
   for (int i = 0; i < THREADCOUNT; i++) {
     vehicle_t *vtype = new_v(i, pts[i]);
     vts[i] = vtype;
     pthread_create(&pts[i], NULL, vthread_init, (void *)vtype);
   }

   // Re-join the terminating threads
   for (int i = 0; i < THREADCOUNT; i++)
     pthread_join(pts[i], NULL);

   return(0);

}

int can_i_go(vehicle_t *c, vehicle_t *m) {
  if (m->id == 1) return 1;
  if (c->c == TRUCK) return -1;
  if (m->c == TRUCK) return -2;
  if (c->d == m->d) return 1;
  return 0;
}

char *why_not(int b) {
  if (b == 0) return "ONCOMING TRAFFIC MUST FINISH";
  if (b == -1) return "TRUCK ON THE BRIDGE";
  if (b == -2) return "IM A TRUCK";
  return "UNKNOWN";
}

char *char_c(class_t c) {
  return c == CAR ? " CAR " : "TRUCK";
}

char* char_d(direction_t d) {
  return d == LEFT ? "LEFT": "RIGHT";
}

class_t rand_c() {
  return rand_f() <= TRUCKPERCENTAGE ? TRUCK : CAR;
}

direction_t rand_d() {
  return rand_f() <= LEFTPERCENTAGE ? LEFT : RIGHT;
}

double rand_f() {
  return (double)rand() / (double)RAND_MAX;
}

vehicle_t *new_v(int i, pthread_t pt) {
  vehicle_t *vtype = malloc(sizeof(vehicle_t));
  vtype->id = i + 1;
  vtype->pt = pt;
  vtype->c = rand_c();
  vtype->d = rand_d();
  return vtype;
}

void *vthread_init(void *arg) {

  vehicle_t *vtype = (vehicle_t *)arg;
  while (1 == 1) {
    pthread_mutex_lock(&m);
    if (nid != vtype->id) {
      pthread_cond_wait(&w, &m);
      continue;
    }

    cv = *vtype;

    struct timeval tv;
    int r = can_i_go(&lv, vtype);
    if (r < 1) {
      gettimeofday(&tv, NULL);
      printf("[%03ld.%03ds] #%03.3d >> WAIT! %s.\n", tv.tv_sec - start, tv.tv_usec / 1000, vtype->id, why_not(r));
      usleep(CROSSINGTIME + DELAYTIME);
      // TODO: Need a signaler here rather than waiting
      // TODO: Maybe do it by counting the number of cars on teh bridge inc, decr
      // TODO: Check if the last one finished..
      printf("[%03ld.%03ds] #%03.3d >> OK\n", tv.tv_sec - start, tv.tv_usec / 1000, vtype->id);
    }

    usleep(DELAYTIME);

    // TODO: free

    la = arg;
    lv = *vtype;
    nid++;

    pthread_cond_signal(&w);
    pthread_mutex_unlock(&m);

    gettimeofday(&tv, NULL);
    printf("[%03ld.%03ds] #%03.3d >> (%s) going %s\n", tv.tv_sec - start, tv.tv_usec / 1000, vtype->id, char_c(vtype->c), char_d(vtype->d));

    usleep(CROSSINGTIME);

    gettimeofday(&tv, NULL);
    printf("[%03ld.%03ds] #%03.3d >> (%s) done\n", tv.tv_sec - start, tv.tv_usec / 1000, vtype->id, char_c(vtype->c));

    break;
  }

  return(0);
}
