#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef THREADCOUNT
#define THREADCOUNT 10
#endif

#ifndef CROSSINGTIME
#define CROSSINGTIME 4
#endif

#ifndef DELAYTIME
#define DELAYTIME 1
#endif

#ifndef TRUCKPERCENTAGE
#define TRUCKPERCENTAGE 0.9
#endif

#ifndef LEFTPERCENTAGE
#define LEFTPERCENTAGE 0.5
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

int nid = 0;
vehicle_t lv;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t w = PTHREAD_COND_INITIALIZER;

int can_i_go(vehicle_t *c, vehicle_t *m) {
  if (c == NULL) return 1;
  if (c->c == TRUCK || m->c == TRUCK) return 0;
  if (c->d == c->d) return 1;
  return 0;
}

void *vehicle_init(void *arg) {

  vehicle_t vtype = *(vehicle_t *)arg;
  printf("Vehicle #%d (%u:%u) created\n", vtype.id, vtype.c, vtype.d);

  while (1 == 1) {
    pthread_mutex_lock(&m);
    printf("Vehicle #%d waiting\n", vtype.id);
    if (nid == vtype.id) {
      if (can_i_go(&lv, &vtype) == 1) {
        sleep(DELAYTIME);
      } else {
        sleep(CROSSINGTIME);
      }
      printf("Vehicle #%d (%u:%u) crossing\n", vtype.id, vtype.c, vtype.d);
      nid++;
      lv = vtype;
      pthread_cond_signal(&w);
      pthread_mutex_unlock(&m);
      break;
    } else {
      pthread_cond_wait(&w, &m);
      if (nid == vtype.id) {
        printf("Vehicle #%d signalled\n", vtype.id);
        pthread_mutex_unlock(&m);
        break;
      } else {
        printf("Vehicle #%d skipped\n", vtype.id);
      }
      pthread_mutex_unlock(&m);
    }
  }

  printf("Vehicle #%d completed signal\n", vtype.id);

  free(arg);
  return(0);
}

double rand_f() {
  return (double)rand() / (double)RAND_MAX;
}

class_t rand_c() {
  return rand_f() >= TRUCKPERCENTAGE ? TRUCK : CAR;
}

direction_t rand_d() {
  return rand_f() >= LEFTPERCENTAGE ? LEFT : RIGHT;
}

int main(int argc, char **argv) {

  srand(time(NULL));

  pthread_t pts[THREADCOUNT];
  for (int i = 0; i < THREADCOUNT; i++) {
    vehicle_t *vtype = malloc(sizeof(vehicle_t));
    (*vtype).id = i;
    (*vtype).pt = pts[i];
    (*vtype).c = rand_c();
    (*vtype).d = rand_d();
    pthread_create(&pts[i], NULL, vehicle_init, (void *)vtype);
  }

  // check the left side
  // check for a truck
    // move truck, or;
  // move a few cars through one at a time until
    // either
      // time elapses
      // truck is encountered
  // wait for last vehicle to cross
  // change sides

  for (int i = 0; i < THREADCOUNT; i++)
    pthread_join(pts[i], NULL);

  //sleep(1);

  return(0);

}
