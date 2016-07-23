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
  struct timeval tv;
} vehicle_t;

typedef struct bs_t {
  int tc;
  int nid;
  vehicle_t *lv;
  void *la;
  int bc;
  pthread_mutex_t *m;
  pthread_mutex_t *b;
  pthread_mutex_t *h;
  pthread_cond_t *w;
  long int start;
} bstate_t;


struct bs_t *bs_create(int tc);
int bs_destroy(struct bs_t *bs);
int can_i_go(vehicle_t *c, vehicle_t *m);
char *char_c(class_t c);
char *char_d(direction_t d);
int mutex_destroy(struct bs_t *bs);
int mutex_init(struct bs_t *bs);
vehicle_t *new_v(int i, struct bs_t *bs, pthread_t pt);
pthread_t *pts_create(int tc, bstate_t *bs);
int pts_destroy(pthread_t *pts);
int pts_join(int tc, pthread_t *pts);
class_t rand_c();
direction_t rand_d();
double rand_f();
void ustate(void *arg, struct bs_t *bs);
int sw_init(struct bs_t *bs);
void *vthread_init(void *arg);
char *why_not(int b);

#endif /* SIMBRIDGE_HEADER */
