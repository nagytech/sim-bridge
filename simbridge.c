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
int main(int argc, char **argv) {

  struct bs_t *bs = bs_create(THREADCOUNT);

  pthread_t *pts = pts_create(THREADCOUNT, bs);
  pts_join(THREADCOUNT, pts);
  pts_destroy(pts);

  bs_destroy(bs);

  return(0);

}

// Thread Initializer
void *vthread_init(void *arg) {

  vehicle_t *v = (vehicle_t *)arg;
  struct bs_t *bs = v->bs;

  for (int i = 0; i < bs->tc; i++) {

    pthread_mutex_lock(bs->m);

    if (bs->nid != v->id) {
      pthread_cond_wait(bs->w, bs->m);
      continue;
    }

    struct timeval tv;
    int r = can_i_go((vehicle_t *)bs->la, v);
    if (r < 1) {
      gettimeofday(&tv, NULL);
      printf("[%03ld.%03ds] #%03.3d << WAIT! %s.\n",
        tv.tv_sec - bs->start, tv.tv_usec / 1000, v->id, why_not(r));
      pthread_mutex_lock(bs->h);
      printf("[%03ld.%03ds] #%03.3d << OK\n",
        tv.tv_sec - bs->start, tv.tv_usec / 1000, v->id);
      pthread_mutex_unlock(bs->h);
    }

    if (bs->nid > 1) free(bs->la);
    ustate(arg, bs);

    pthread_mutex_lock(bs->b);
    bs->bc = bs->bc + 1;
    if (bs->bc == 1)
      pthread_mutex_lock(bs->h);
    pthread_mutex_unlock(bs->b);

    pthread_cond_signal(bs->w);
    pthread_mutex_unlock(bs->m);

    gettimeofday(&tv, NULL);
    printf("[%03ld.%03ds] #%03.3d >> (%s) going %s\n",
      tv.tv_sec - bs->start, tv.tv_usec / 1000, v->id,
      char_c(v->c), char_d(v->d));
    usleep(CROSSINGTIME);
    gettimeofday(&tv, NULL);
    printf("[%03ld.%03ds] #%03.3d >> (%s) done\n",
      tv.tv_sec - bs->start, tv.tv_usec / 1000, v->id, char_c(v->c));

    pthread_mutex_lock(bs->b);
    (bs->bc)--;
    if (bs->bc == 0)
      pthread_mutex_unlock(bs->h);
    pthread_mutex_unlock(bs->b);

    break;
  }

  return(0);
}

// Create bridge state
struct bs_t *bs_create(int tc) {
 struct bs_t *bs = malloc(sizeof(bstate_t));
 mutex_init(bs);
 sw_init(bs);
 bs->nid = 1;
 bs->bc = 0;
 bs->tc = tc;
 return bs;
}

// Destroy bridge state
int bs_destroy(struct bs_t *bs) {
  mutex_destroy(bs);
  free(bs);
  return 1;
}

// Compare last and current vehicle to see if current can cross
int can_i_go(vehicle_t *c, vehicle_t *m) {
  if (m->id == 1) return 1;
  if (c->c == TRUCK) return -1;
  if (m->c == TRUCK) return -2;
  if (c->d == m->d) return 1;
  return 0;
}

// Text value of class type
char *char_c(class_t c) {
  return c == CAR ? " CAR " : "TRUCK";
}

// Text value of direction type
char* char_d(direction_t d) {
  return d == LEFT ? "LEFT": "RIGHT";
}

// Destroy mutexes
int mutex_destroy(struct bs_t *bs) {
  free(bs->b);
  free(bs->m);
  free(bs->w);
  return 1;
}

// Initialize mutexes
int mutex_init(struct bs_t *bs) {
  pthread_mutex_t *b = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_t *h = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_t *m = malloc(sizeof(pthread_mutex_t));
  pthread_cond_t *w = malloc(sizeof(pthread_cond_t));
  pthread_mutex_init(b, NULL);
  pthread_mutex_init(h, NULL);
  pthread_mutex_init(m, NULL);
  pthread_cond_init(w, NULL);
  // TODO: error codes
  bs->b = b;
  bs->h = h;
  bs->w = w;
  bs->m = m;
  return 1;
}

// Create a new vehicle_t object
vehicle_t *new_v(int i, struct bs_t *bs, pthread_t pt) {
  vehicle_t *vtype = malloc(sizeof(vehicle_t));
  vtype->id = i + 1;
  vtype->bs = bs;
  vtype->c = rand_c();
  vtype->d = rand_d();
  return vtype;
}

// Create new pool of threads
pthread_t *pts_create(int tc, bstate_t *bs) {
  pthread_t *pts = malloc(sizeof(pthread_t) * tc);
  for (int i = 0; i < tc; i++) {
   pthread_create(&pts[i], NULL, vthread_init, (void *)new_v(i, bs, pts[i]));
  }
  return pts;
}

// Cleanup a thread
int pts_destroy(pthread_t *pts) {
  free(pts);
  return 1;
}

// Join all threads to the main
int pts_join(int tc, pthread_t *pts) {
  for (int i = 0; i < tc; i++) {
    pthread_join(pts[i], NULL);
  }
  return 1;
}

// Random class of vehicle
class_t rand_c() {
  return rand_f() <= TRUCKPERCENTAGE ? TRUCK : CAR;
}

// Random direction
direction_t rand_d() {
  return rand_f() <= LEFTPERCENTAGE ? LEFT : RIGHT;
}

// Random float
double rand_f() {
  return (double)rand() / (double)RAND_MAX;
}

// Initialize the stopwatch
int sw_init(struct bs_t *bs) {
  srand(time(NULL));
  struct timeval tv;
  gettimeofday(&tv, NULL);
  bs->start = tv.tv_sec;
  return 1;
}

// Update the bridge state
void ustate(void *arg, struct bs_t *bs) {
  bs->la = arg;
  bs->nid = bs->nid + 1;
}

// Convert can_i_go result status to text
char *why_not(int b) {
  if (b == 0) return "ONCOMING TRAFFIC MUST FINISH";
  if (b == -1) return "TRUCK ON THE BRIDGE";
  if (b == -2) return "IM A TRUCK";
  return "UNKNOWN";
}
