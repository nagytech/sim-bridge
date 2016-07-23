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

struct bs_t;

typedef struct v_t {
  int id;
  class_t c;
  direction_t d;
  struct bs_t *bs;
} vehicle_t;

typedef struct bs_t {
  int nid;
  vehicle_t *lv;
  void *la;
  pthread_mutex_t *m;
  pthread_cond_t *w;
  long int start;
} bstate_t;


int can_i_go(vehicle_t *c, vehicle_t *m);
char *char_c(class_t c);
char *char_d(direction_t d);
vehicle_t *new_v(int i, struct bs_t *bs, pthread_t pt);
class_t rand_c();
direction_t rand_d();
double rand_f();
void *vthread_init(void *arg);
char *why_not(int b);

#endif /* SIMBRIDGE_HEADER */
