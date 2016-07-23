#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "simbridge.h"

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

int mutex_init(struct bs_t *bs) {
  pthread_mutex_t *m = malloc(sizeof(pthread_mutex_t));
  pthread_cond_t *w = malloc(sizeof(pthread_cond_t));
  pthread_mutex_init(m, NULL);
  pthread_cond_init(w, NULL);
  // TODO: error codes
  bs->w = w;
  bs->m = m;
  return 1;
}

int sw_init(struct bs_t *bs) {
  srand(time(NULL));
  struct timeval tv;
  gettimeofday(&tv, NULL);
  bs->start = tv.tv_sec;
  return 1;
}

struct bs_t *bs_init() {
 struct bs_t *bs = malloc(sizeof(bstate_t));
 bs->nid = 1;
 return bs;
}

int *bs_clean() {
    // TODO: 
}

 int main(int argc, char **argv) {

  struct bs_t *bs = bs_init();
  mutex_init(bs);
  sw_init(bs);

  // Create a series of vehicles as threads
  pthread_t pts[THREADCOUNT];
  for (int i = 0; i < THREADCOUNT; i++) {
   vehicle_t *vtype = new_v(i, bs, pts[i]);
   pthread_create(&pts[i], NULL, vthread_init, (void *)vtype);
  }

  // Re-join the terminating threads
  for (int i = 0; i < THREADCOUNT; i++)
   pthread_join(pts[i], NULL);

  // Exit
  return(0);

}

void ustate(void *arg, struct bs_t *bs) {
  bs->lv = (vehicle_t *)arg;
  bs->la = arg;
  (bs->nid)++;
}

void *vthread_init(void *arg) {

  vehicle_t *vtype = (vehicle_t *)arg;
  struct bs_t *bs = vtype->bs;
  bs->nid = 1;

  for (int i = 0; i < THREADCOUNT; i++) {

    pthread_mutex_lock(bs->m);

    // Apply turn order to the thread pool
    if (bs->nid != vtype->id) {
      pthread_cond_wait(bs->w, bs->m);
      continue;
    }

    struct timeval tv;
    int r = can_i_go(bs->lv, vtype);
    if (r < 1) {
      gettimeofday(&tv, NULL);
      printf("[%03ld.%03ds] #%03.3d << WAIT! %s.\n", tv.tv_sec - bs->start, tv.tv_usec / 1000, vtype->id, why_not(r));
      usleep(CROSSINGTIME + DELAYTIME);
      // TODO: Lock up, lock down
      // TODO: Need a signaler here rather than waiting
      // TODO: Maybe do it by counting the number of cars on teh bridge inc, decr
      // TODO: Check if the last one finished..
      printf("[%03ld.%03ds] #%03.3d << OK\n", tv.tv_sec - bs->start, tv.tv_usec / 1000, vtype->id);
    }

    usleep(DELAYTIME);

    if (bs->nid > 1) free(bs->la);
    ustate(arg, bs);

    pthread_cond_signal(bs->w);
    pthread_mutex_unlock(bs->m);

    gettimeofday(&tv, NULL);
    printf("[%03ld.%03ds] #%03.3d >> (%s) going %s\n", tv.tv_sec - bs->start, tv.tv_usec / 1000, vtype->id, char_c(vtype->c), char_d(vtype->d));

    usleep(CROSSINGTIME);

    gettimeofday(&tv, NULL);
    printf("[%03ld.%03ds] #%03.3d >> (%s) done\n", tv.tv_sec - bs->start, tv.tv_usec / 1000, vtype->id, char_c(vtype->c));

    break;
  }

  // TODO: output the number of cars, trucks percentace, crossing time, etc.

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

vehicle_t *new_v(int i, struct bs_t *bs, pthread_t pt) {
  vehicle_t *vtype = malloc(sizeof(vehicle_t));
  vtype->id = i + 1;
  vtype->bs = bs;
  vtype->c = rand_c();
  vtype->d = rand_d();
  return vtype;
}
