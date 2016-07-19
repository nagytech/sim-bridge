#ifndef SIMBRIDGE_HEADER
#define SIMBRIDGE_HEADER


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
  CAR     = 0,
  TRUCK   = 1
} class_t;

typedef enum vehicle_direction {
  LEFT    = 0,
  RIGHT   = 1
} direction_t;

typedef struct vehicle_type {
  int id;
  pthread_t pt;
  class_t c;
  direction_t d;
} vehicle_t;

int can_i_go(vehicle_t *c, vehicle_t *m);
char* char_c(class_t c);
char* char_d(direction_t d);
vehicle_t *new_v(int i, pthread_t pt);
class_t rand_c();
direction_t rand_d();
double rand_f();
void *vthread_init(void *arg);

#endif /* SIMBRIDGE_HEADER */
