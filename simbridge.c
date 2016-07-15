#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef THREADCOUNT
#define THREADCOUNT 100
#endif

#ifndef CROSSINGTIME
#define CROSSINGTIME 500 / 0.001
#endif

#ifndef DELAYTIME
#define DELAYTIME 50 / 0.001
#endif

#ifndef TRUCKPERCENTAGE
#define TRUCKPERCENTAGE 0.1
#endif

#ifndef LEFTPERCENTAGE
#define LEFTPERCENTAGE 0.9
#endif

typedef enum vehicle_class {
  CAR = 0,
  TRUCK = 1
} class_t;

typedef enum vehicle_direction {
  LEFT = 0,
  RIGHT = 1
} direction_t;

typedef struct vehicle_type {
  int id;
  pthread_t pt;
  class_t c;
  direction_t d;
} vehicle_t;

int nid = 1;
vehicle_t lv, cv;
void *la;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t w = PTHREAD_COND_INITIALIZER;

int can_i_go(vehicle_t *c, vehicle_t *m) {
  if (c->id == 1) return 1;
  if (c->c == TRUCK || m->c == TRUCK) return 0;
  if (c->d == m->d) return 1;
  return 0;
}

double rand_f() {
  return (double)rand() / (double)RAND_MAX;
}

class_t rand_c() {
  return rand_f() <= TRUCKPERCENTAGE ? TRUCK : CAR;
}

direction_t rand_d() {
  return rand_f() <= LEFTPERCENTAGE ? LEFT : RIGHT;
}

char* char_c(class_t c) {
  return c == CAR ? " CAR " : "TRUCK";
}

char* char_d(direction_t d) {
  return d == LEFT ? "LEFT": "RIGHT";
}

void *vehicle_init(void *arg) {

  vehicle_t *vtype = (vehicle_t *)arg;
  while (1 == 1) {
    pthread_mutex_lock(&m);
    if (nid != vtype->id) {
      pthread_cond_wait(&w, &m);
      continue;
    }

    cv = *vtype;

    if (can_i_go(&lv, vtype) == 0) {
      printf(">> WAIT\n");
      usleep(CROSSINGTIME + DELAYTIME);
      // TODO: Check if the last one finished..
      printf(">> OK\n");
    }

    usleep(DELAYTIME);

    //if (la != NULL) free(la);
    la = arg;
    lv = *vtype;
    nid++;

    pthread_cond_signal(&w);
    pthread_mutex_unlock(&m);

    printf("#%03.3d (%s) going %s\n", vtype->id, char_c(vtype->c), char_d(vtype->d));
    usleep(CROSSINGTIME);

    printf("#%03.3d (%s) done\n", vtype->id, char_c(vtype->c));

    break;
  }

  return(0);
}

vehicle_t *new_v(int i, pthread_t pt) {
  vehicle_t *vtype = malloc(sizeof(vehicle_t));
  vtype->id = i + 1;
  vtype->pt = pt;
  vtype->c = rand_c();
  vtype->d = rand_d();
  return vtype;
}

int main(int argc, char **argv) {

  srand(time(NULL));

  pthread_t pts[THREADCOUNT];
  vehicle_t *vts[THREADCOUNT];
  for (int i = 0; i < THREADCOUNT; i++) {
    vehicle_t *vtype = new_v(i, pts[i]);
    vts[i] = vtype;
    pthread_create(&pts[i], NULL, vehicle_init, (void *)vtype);
  }

  for (int i = 0; i < THREADCOUNT; i++)
    pthread_join(pts[i], NULL);

  return(0);

}
