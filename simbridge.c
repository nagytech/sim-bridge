#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef THREADCOUNT
#define THREADCOUNT 2
#endif

#ifndef CROSSINGTIME
#define CROSSINGTIME 4
#endif

#ifndef TRUCKPERCENTAGE
#define TRUCKPERCENTAGE 0.9
#endif

#ifndef LEFTPERCENTAGE
#define LEFTPERCENTAGE 0.5
#endif

#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__

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

pthread_barrier_t b;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t w = PTHREAD_COND_INITIALIZER;

void *vehicle_init(void *arg) {

  vehicle_t vtype = *(vehicle_t *)arg;
  printf("Vehicle #%d created\n", vtype.id);

  while (1 == 1) {
    pthread_mutex_lock(&m);
    printf("Vehicle #%d waiting\n", vtype.id);
    if (nid == vtype.id) {
      sleep(1);
      nid++;
      // TODO: Check last car type, etc. do logic
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

float rand_f() {
  return rand()/RAND_MAX;
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
