/****************************************************************************/
/*                           the diy toolsuite                              */
/*                                                                          */
/* Jade Alglave, University College London, UK.                             */
/* Luc Maranget, INRIA Paris-Rocquencourt, France.                          */
/*                                                                          */
/* This C source is a product of litmus7 and includes source that is        */
/* governed by the CeCILL-B license.                                        */
/****************************************************************************/

/* Parameters */
#define SIZE_OF_TEST 100000
#define NUMBER_OF_RUN 10
#define AVAIL 1
#define STRIDE (-1)
#define MAX_LOOP 0
#define N 2
#define AFF_INCR (-1)
/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include "utils.h"
#include "outs.h"

/* params */
typedef struct {
  int verbose;
  int size_of_test,max_run;
  int do_change;
} param_t;


/* Full memory barrier */

inline static void mbar(void) {
  asm __volatile__ ("fence rw,rw" ::: "memory");
}

/* Barriers macros */
inline static void barrier_wait(unsigned int id, unsigned int k, int volatile *b) {
  if ((k % N) == id) {
    *b = 1 ;
  } else {
    while (*b == 0) ;
  }
}

/**********************/
/* Context definition */
/**********************/


typedef struct {
/* Shared variables */
  int *z;
  int *y;
  uint64_t *x;
/* Final content of observed  registers */
  int *out_0_x5;
  int *out_0_x8;
  int *out_1_x5;
  int *out_1_x8;
/* Check data */
  pb_t *fst_barrier;
  po_t *s_or;
  uint64_t* cpy_x[N] ;
  int* cpy_y[N] ;
  int* cpy_z[N] ;
/* Barrier for litmus loop */
  int volatile *barrier;
/* Instance seed */
  st_t seed;
/* Parameters */
  param_t *_p;
} ctx_t;

inline static int final_cond(int _out_0_x5,int _out_0_x8,int _out_1_x5,int _out_1_x8,uint64_t _x,int _y,int _z) {
  switch (_out_0_x5) {
  case 0:
    switch (_out_0_x8) {
    case 0:
      switch (_out_1_x5) {
      case 0:
        switch (_out_1_x8) {
        case 0:
          switch (_x) {
          case 4294967297:
            switch (_y) {
            case 1:
              switch (_z) {
              case 1:
                return 1;
              default:
                return 0;
              }
            default:
              return 0;
            }
          default:
            return 0;
          }
        default:
          return 0;
        }
      default:
        return 0;
      }
    default:
      return 0;
    }
  default:
    return 0;
  }
}

inline static int final_ok(int cond) {
  return cond;
}

/**********************/
/* Outcome collection */
/**********************/
#define NOUTS 7
typedef intmax_t outcome_t[NOUTS];

static const int out_0_x5_f = 0 ;
static const int out_0_x8_f = 1 ;
static const int out_1_x5_f = 2 ;
static const int out_1_x8_f = 3 ;
static const int x_f = 4 ;
static const int y_f = 5 ;
static const int z_f = 6 ;


typedef struct hist_t {
  outs_t *outcomes ;
  count_t n_pos,n_neg ;
} hist_t ;

static hist_t *alloc_hist(void) {
  hist_t *p = malloc_check(sizeof(*p)) ;
  p->outcomes = NULL ;
  p->n_pos = p->n_neg = 0 ;
  return p ;
}

static void free_hist(hist_t *h) {
  free_outs(h->outcomes) ;
  free(h) ;
}

static void add_outcome(hist_t *h, count_t v, outcome_t o, int show) {
  h->outcomes = add_outcome_outs(h->outcomes,o,NOUTS,v,show) ;
}

static void merge_hists(hist_t *h0, hist_t *h1) {
  h0->n_pos += h1->n_pos ;
  h0->n_neg += h1->n_neg ;
  h0->outcomes = merge_outs(h0->outcomes,h1->outcomes,NOUTS) ;
}


static void do_dump_outcome(FILE *fhist, intmax_t *o, count_t c, int show) {
  fprintf(fhist,"%-6"PCTR"%c>0:x5=%i; 0:x8=%i; 1:x5=%i; 1:x8=%i; x=%"PRIu64"; y=%i; z=%i;\n",c,show ? '*' : ':',(int)o[out_0_x5_f],(int)o[out_0_x8_f],(int)o[out_1_x5_f],(int)o[out_1_x8_f],(uint64_t)o[x_f],(int)o[y_f],(int)o[z_f]);
}

static void just_dump_outcomes(FILE *fhist, hist_t *h) {
  outcome_t buff ;
  dump_outs(fhist,do_dump_outcome,h->outcomes,buff,NOUTS) ;
}

/*******************************************************/
/* Context allocation, freeing and reinitialization    */
/*******************************************************/

static void init(ctx_t *_a) {
  int size_of_test = _a->_p->size_of_test;

  _a->seed = rand();
  _a->out_0_x5 = malloc_check(size_of_test*sizeof(*(_a->out_0_x5)));
  _a->out_0_x8 = malloc_check(size_of_test*sizeof(*(_a->out_0_x8)));
  _a->out_1_x5 = malloc_check(size_of_test*sizeof(*(_a->out_1_x5)));
  _a->out_1_x8 = malloc_check(size_of_test*sizeof(*(_a->out_1_x8)));
  _a->z = malloc_check(size_of_test*sizeof(*(_a->z)));
  _a->y = malloc_check(size_of_test*sizeof(*(_a->y)));
  _a->x = malloc_check(size_of_test*sizeof(*(_a->x)));
  _a->fst_barrier = pb_create(N);
  _a->s_or = po_create(N);
  for (int _p = N-1 ; _p >= 0 ; _p--) {
    _a->cpy_x[_p] = malloc_check(size_of_test*sizeof(*(_a->cpy_x[_p])));
    _a->cpy_y[_p] = malloc_check(size_of_test*sizeof(*(_a->cpy_y[_p])));
    _a->cpy_z[_p] = malloc_check(size_of_test*sizeof(*(_a->cpy_z[_p])));
  }
  _a->barrier = malloc_check(size_of_test*sizeof(*(_a->barrier)));
}

static void finalize(ctx_t *_a) {
  free((void *)_a->z);
  free((void *)_a->y);
  free((void *)_a->x);
  free((void *)_a->out_0_x5);
  free((void *)_a->out_0_x8);
  free((void *)_a->out_1_x5);
  free((void *)_a->out_1_x8);
  pb_free(_a->fst_barrier);
  po_free(_a->s_or);
  for (int _p = N-1 ; _p >= 0 ; _p--) {
    free((void *)_a->cpy_x[_p]);
    free((void *)_a->cpy_y[_p]);
    free((void *)_a->cpy_z[_p]);
  }
  free((void *)_a->barrier);
}

static void reinit(ctx_t *_a) {
  for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
    _a->z[_i] = 0;
    _a->y[_i] = 0;
    _a->x[_i] = 0;
    _a->out_0_x5[_i] = -239487;
    _a->out_0_x8[_i] = -239487;
    _a->out_1_x5[_i] = -239487;
    _a->out_1_x8[_i] = -239487;
    _a->barrier[_i] = 0;
  }
}

/**************************************/
/* Prefetch (and check) global values */
/**************************************/

static void check_globals(ctx_t *_a) {
  int *z = _a->z;
  int *y = _a->y;
  uint64_t *x = _a->x;
  for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
    if (rand_bit(&(_a->seed)) && z[_i] != 0) fatal("LR-SC-mixed2, check_globals failed");
    if (rand_bit(&(_a->seed)) && y[_i] != 0) fatal("LR-SC-mixed2, check_globals failed");
    if (rand_bit(&(_a->seed)) && x[_i] != 0) fatal("LR-SC-mixed2, check_globals failed");
  }
  pb_wait(_a->fst_barrier);
}


static void stabilize_globals(int _id, ctx_t *_a) {
  int size_of_test = _a->_p->size_of_test;

  uint64_t *x = _a->x;
  uint64_t **cpy_x = _a->cpy_x;
  int *y = _a->y;
  int **cpy_y = _a->cpy_y;
  int *z = _a->z;
  int **cpy_z = _a->cpy_z;

  pb_wait(_a->fst_barrier); 
  for ( ; ; ) {
    for (int _i = size_of_test-1 ; _i >= 0 ; _i--) {
      cpy_x[_id][_i] = x[_i];
      cpy_y[_id][_i] = y[_i];
      cpy_z[_id][_i] = z[_i];
    }
    po_reinit(_a->s_or);
    int _found;
    int _nxt_id = (_id+1) % N;
    _found = 0;
    for (int _i = size_of_test-1 ; _i >= 0 && !_found ; _i--) {
      if (cpy_x[_id][_i] != cpy_x[_nxt_id][_i]) { _found = 1; }
      if (cpy_y[_id][_i] != cpy_y[_nxt_id][_i]) { _found = 1; }
      if (cpy_z[_id][_i] != cpy_z[_nxt_id][_i]) { _found = 1; }
    }
    if (_found) { log_error("%i: Stabilizing final state!\n",_id); }
    if (!po_wait(_a->s_or,_found)) return ;
  }
}

/***************/
/* Litmus code */
/***************/

typedef struct {
  int th_id; /* I am running on this thread */
  ctx_t *_a;   /* In this context */
} parg_t;





static void *P0(void *_vb) {
  mbar();
  parg_t *_b = (parg_t *)_vb;
  ctx_t *_a = _b->_a;
  check_globals(_a);
  int _th_id = _b->th_id;
  int volatile *barrier = _a->barrier;
  int _size_of_test = _a->_p->size_of_test;
  int *out_0_x5 = _a->out_0_x5;
  int *out_0_x8 = _a->out_0_x8;
  for (int _i = _size_of_test-1 ; _i >= 0 ; _i--) {
    barrier_wait(_th_id,_i,&barrier[_i]);
    int trashed_x7;
asm __volatile__ (
"\n"
"#START _litmus_P0\n"
"#_litmus_P0_0\n\t"
"lr.d %[x5],0(%[x6])\n"
"#_litmus_P0_1\n\t"
"ori %[x7],x0,1\n"
"#_litmus_P0_2\n\t"
"sw %[x7],0(%[x6])\n"
"#_litmus_P0_3\n\t"
"sc.w.rl %[x8],%[x7],0(%[x9])\n"
"#END _litmus_P0\n\t"
:[x8] "=&r" (out_0_x8[_i]),[x5] "=&r" (out_0_x5[_i]),[x7] "=&r" (trashed_x7)
:[x6] "r" (&_a->x[_i]),[x9] "r" (&_a->y[_i])
:"cc","memory"
);
  }
  stabilize_globals(0,_a);
  mbar();
  return NULL;
}

static void *P1(void *_vb) {
  mbar();
  parg_t *_b = (parg_t *)_vb;
  ctx_t *_a = _b->_a;
  check_globals(_a);
  int _th_id = _b->th_id;
  int volatile *barrier = _a->barrier;
  int _size_of_test = _a->_p->size_of_test;
  int *out_1_x5 = _a->out_1_x5;
  int *out_1_x8 = _a->out_1_x8;
  for (int _i = _size_of_test-1 ; _i >= 0 ; _i--) {
    barrier_wait(_th_id,_i,&barrier[_i]);
    int trashed_x7;
asm __volatile__ (
"\n"
"#START _litmus_P1\n"
"#_litmus_P1_0\n\t"
"lr.d %[x5],0(%[x6])\n"
"#_litmus_P1_1\n\t"
"ori %[x7],x0,1\n"
"#_litmus_P1_2\n\t"
"sw %[x7],4(%[x6])\n"
"#_litmus_P1_3\n\t"
"sc.w.rl %[x8],%[x7],0(%[x9])\n"
"#END _litmus_P1\n\t"
:[x8] "=&r" (out_1_x8[_i]),[x5] "=&r" (out_1_x5[_i]),[x7] "=&r" (trashed_x7)
:[x6] "r" (&_a->x[_i]),[x9] "r" (&_a->z[_i])
:"cc","memory"
);
  }
  stabilize_globals(1,_a);
  mbar();
  return NULL;
}

typedef struct {
  pm_t *p_mutex;
  pb_t *p_barrier;
  param_t *_p;
} zyva_t;

#define NT N

static void *zyva(void *_va) {
  zyva_t *_a = (zyva_t *) _va;
  param_t *_b = _a->_p;
  pb_wait(_a->p_barrier);
  pthread_t thread[NT];
  parg_t parg[N];
  f_t *fun[] = {&P0,&P1};
  hist_t *hist = alloc_hist();
  ctx_t ctx;
  ctx._p = _b;

  init(&ctx);
  for (int _p = N-1 ; _p >= 0 ; _p--) {
    parg[_p].th_id = _p; parg[_p]._a = &ctx;
  }

  for (int n_run = 0 ; n_run < _b->max_run ; n_run++) {
    if (_b->verbose>1) fprintf(stderr,"Run %i of %i\r", n_run, _b->max_run);
    reinit(&ctx);
    if (_b->do_change) perm_funs(&ctx.seed,fun,N);
    for (int _p = NT-1 ; _p >= 0 ; _p--) {
      launch(&thread[_p],fun[_p],&parg[_p]);
    }
    if (_b->do_change) perm_threads(&ctx.seed,thread,NT);
    for (int _p = NT-1 ; _p >= 0 ; _p--) {
      join(&thread[_p]);
    }
    /* Log final states */
    for (int _i = _b->size_of_test-1 ; _i >= 0 ; _i--) {
      int _out_0_x5_i = ctx.out_0_x5[_i];
      int _out_0_x8_i = ctx.out_0_x8[_i];
      int _out_1_x5_i = ctx.out_1_x5[_i];
      int _out_1_x8_i = ctx.out_1_x8[_i];
      uint64_t _x_i = ctx.x[_i];
      int _y_i = ctx.y[_i];
      int _z_i = ctx.z[_i];
      outcome_t o;
      int cond;

      for (int _p = N-1 ; _p >= 0 ; _p--) {
        if (_x_i != ctx.cpy_x[_p][_i]) fatal("LR-SC-mixed2, global x unstabilized") ;
      }
      for (int _p = N-1 ; _p >= 0 ; _p--) {
        if (_y_i != ctx.cpy_y[_p][_i]) fatal("LR-SC-mixed2, global y unstabilized") ;
      }
      for (int _p = N-1 ; _p >= 0 ; _p--) {
        if (_z_i != ctx.cpy_z[_p][_i]) fatal("LR-SC-mixed2, global z unstabilized") ;
      }
      cond = final_ok(final_cond(_out_0_x5_i,_out_0_x8_i,_out_1_x5_i,_out_1_x8_i,_x_i,_y_i,_z_i));
      o[out_0_x5_f] = _out_0_x5_i;
      o[out_0_x8_f] = _out_0_x8_i;
      o[out_1_x5_f] = _out_1_x5_i;
      o[out_1_x8_f] = _out_1_x8_i;
      o[x_f] = _x_i;
      o[y_f] = _y_i;
      o[z_f] = _z_i;
      add_outcome(hist,1,o,cond);
      if (cond) { hist->n_pos++; } else { hist->n_neg++; }
    }
  }

  finalize(&ctx);
  return hist;
}

#define ENOUGH 10

static void postlude(FILE *out,cmd_t *cmd,hist_t *hist,count_t p_true,count_t p_false,tsc_t total) {
  fprintf(out,"Test LR-SC-mixed2 Forbidden\n");
  fprintf(out,"Histogram (%i states)\n",finals_outs(hist->outcomes));
  just_dump_outcomes(out,hist);
  int cond = p_true == 0;
  fprintf(out,"%s\n",cond?"Ok":"No");
  fprintf(out,"\nWitnesses\n");
  fprintf(out,"Positive: %" PCTR ", Negative: %" PCTR "\n",p_false,p_true);
  fprintf(out,"Condition ~exists (x=4294967297 /\\ y=1 /\\ z=1 /\\ 0:x5=0 /\\ 0:x8=0 /\\ 1:x5=0 /\\ 1:x8=0) is %svalidated\n",cond ? "" : "NOT ");
  fprintf(out,"Hash=a1b719f2ab3fce9f3820bb9b101fd69e\n");
  count_t cond_true = p_true;
  count_t cond_false = p_false;
  fprintf(out,"Observation LR-SC-mixed2 %s %" PCTR " %" PCTR "\n",!cond_true ? "Never" : !cond_false ? "Always" : "Sometimes",cond_true,cond_false);
  if (p_true > 0) {
  }
  fprintf(out,"Time LR-SC-mixed2 %.2f\n",total / 1000000.0);
  fflush(out);
}

static void run(cmd_t *cmd,cpus_t *def_all_cpus,FILE *out) {
  tsc_t start = timeofday();
  param_t prm ;
/* Set some parameters */
  prm.verbose = cmd->verbose;
  prm.size_of_test = cmd->size_of_test;
  prm.max_run = cmd->max_run;
  prm.do_change = 1;
  if (cmd->fix) prm.do_change = 0;
/* Computes number of test concurrent instances */
  int n_avail = cmd->avail;
  int n_exe;
  if (cmd->n_exe > 0) {
    n_exe = cmd->n_exe;
  } else {
    n_exe = n_avail < N ? 1 : n_avail / N;
  }
/* Show parameters to user */
  if (prm.verbose) {
    log_error( "LR-SC-mixed2: n=%i, r=%i, s=%i",n_exe,prm.max_run,prm.size_of_test);
    log_error("\n");
  }
  hist_t *hist = NULL;
  int n_th = n_exe-1;
  pthread_t th[n_th];
  zyva_t zarg[n_exe];
  pm_t *p_mutex = pm_create();
  pb_t *p_barrier = pb_create(n_exe);
  for (int k=0 ; k < n_exe ; k++) {
    zyva_t *p = &zarg[k];
    p->_p = &prm;
    p->p_mutex = p_mutex; p->p_barrier = p_barrier; 
    if (k < n_th) {
      launch(&th[k],zyva,p);
    } else {
      hist = (hist_t *)zyva(p);
    }
  }

  count_t n_outs = prm.size_of_test; n_outs *= prm.max_run;
  for (int k=0 ; k < n_th ; k++) {
    hist_t *hk = (hist_t *)join(&th[k]);
    if (sum_outs(hk->outcomes) != n_outs || hk->n_pos + hk->n_neg != n_outs) {
      fatal("LR-SC-mixed2, sum_hist");
    }
    merge_hists(hist,hk);
    free_hist(hk);
  }
  tsc_t total = timeofday() - start;
  pm_free(p_mutex);
  pb_free(p_barrier);

  n_outs *= n_exe ;
  if (sum_outs(hist->outcomes) != n_outs || hist->n_pos + hist->n_neg != n_outs) {
    fatal("LR-SC-mixed2, sum_hist") ;
  }
  count_t p_true = hist->n_pos, p_false = hist->n_neg;
  postlude(out,cmd,hist,p_true,p_false,total);
  free_hist(hist);
}


int main(int argc, char **argv) {
  cpus_t *def_all_cpus = NULL;
  cmd_t def = { 0, NUMBER_OF_RUN, SIZE_OF_TEST, STRIDE, AVAIL, 0, 0, aff_none, 0, 0, AFF_INCR, def_all_cpus, NULL, -1, MAX_LOOP, NULL, NULL, -1, -1, -1, 0, 0};
  cmd_t cmd = def;
  parse_cmd(argc,argv,&def,&cmd);
  run(&cmd,def_all_cpus,stdout);
  return EXIT_SUCCESS;
}
