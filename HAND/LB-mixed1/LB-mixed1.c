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
  uint64_t *z;
  int *y;
  int *x;
/* Final content of observed  registers */
  int *out_0_x5;
  int *out_1_x1;
  int *out_1_x5;
  int *out_1_x9;
  int *out_1_x10;
/* Check data */
  pb_t *fst_barrier;
/* Barrier for litmus loop */
  int volatile *barrier;
/* Instance seed */
  st_t seed;
/* Parameters */
  param_t *_p;
} ctx_t;

inline static int final_cond(int _out_0_x5,int _out_1_x1,int _out_1_x5,int _out_1_x9,int _out_1_x10) {
  switch (_out_0_x5) {
  case 1:
    switch (_out_1_x1) {
    case 0:
      switch (_out_1_x5) {
      case 1:
        switch (_out_1_x9) {
        case 1:
          switch (_out_1_x10) {
          case 0:
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
}

inline static int final_ok(int cond) {
  return cond;
}

/**********************/
/* Outcome collection */
/**********************/
#define NOUTS 5
typedef intmax_t outcome_t[NOUTS];

static const int out_0_x5_f = 0 ;
static const int out_1_x1_f = 1 ;
static const int out_1_x5_f = 2 ;
static const int out_1_x9_f = 3 ;
static const int out_1_x10_f = 4 ;


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
  fprintf(fhist,"%-6"PCTR"%c>0:x5=%i; 1:x1=%i; 1:x5=%i; 1:x9=%i; 1:x10=%i;\n",c,show ? '*' : ':',(int)o[out_0_x5_f],(int)o[out_1_x1_f],(int)o[out_1_x5_f],(int)o[out_1_x9_f],(int)o[out_1_x10_f]);
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
  _a->out_1_x1 = malloc_check(size_of_test*sizeof(*(_a->out_1_x1)));
  _a->out_1_x5 = malloc_check(size_of_test*sizeof(*(_a->out_1_x5)));
  _a->out_1_x9 = malloc_check(size_of_test*sizeof(*(_a->out_1_x9)));
  _a->out_1_x10 = malloc_check(size_of_test*sizeof(*(_a->out_1_x10)));
  _a->z = malloc_check(size_of_test*sizeof(*(_a->z)));
  _a->y = malloc_check(size_of_test*sizeof(*(_a->y)));
  _a->x = malloc_check(size_of_test*sizeof(*(_a->x)));
  _a->fst_barrier = pb_create(N);
  _a->barrier = malloc_check(size_of_test*sizeof(*(_a->barrier)));
}

static void finalize(ctx_t *_a) {
  free((void *)_a->z);
  free((void *)_a->y);
  free((void *)_a->x);
  free((void *)_a->out_0_x5);
  free((void *)_a->out_1_x1);
  free((void *)_a->out_1_x5);
  free((void *)_a->out_1_x9);
  free((void *)_a->out_1_x10);
  pb_free(_a->fst_barrier);
  free((void *)_a->barrier);
}

static void reinit(ctx_t *_a) {
  for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
    _a->z[_i] = 0;
    _a->y[_i] = 0;
    _a->x[_i] = 0;
    _a->out_0_x5[_i] = -239487;
    _a->out_1_x1[_i] = -239487;
    _a->out_1_x5[_i] = -239487;
    _a->out_1_x9[_i] = -239487;
    _a->out_1_x10[_i] = -239487;
    _a->barrier[_i] = 0;
  }
}

/**************************************/
/* Prefetch (and check) global values */
/**************************************/

static void check_globals(ctx_t *_a) {
  uint64_t *z = _a->z;
  int *y = _a->y;
  int *x = _a->x;
  for (int _i = _a->_p->size_of_test-1 ; _i >= 0 ; _i--) {
    if (rand_bit(&(_a->seed)) && z[_i] != 0) fatal("LB+mixed1, check_globals failed");
    if (rand_bit(&(_a->seed)) && y[_i] != 0) fatal("LB+mixed1, check_globals failed");
    if (rand_bit(&(_a->seed)) && x[_i] != 0) fatal("LB+mixed1, check_globals failed");
  }
  pb_wait(_a->fst_barrier);
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
  for (int _i = _size_of_test-1 ; _i >= 0 ; _i--) {
    barrier_wait(_th_id,_i,&barrier[_i]);
    int trashed_x7;
asm __volatile__ (
"\n"
"#START _litmus_P0\n"
"#_litmus_P0_0\n\t"
"lw %[x5],0(%[x6])\n"
"#_litmus_P0_1\n\t"
"fence rw,rw\n"
"#_litmus_P0_2\n\t"
"ori %[x7],x0,1\n"
"#_litmus_P0_3\n\t"
"sw %[x7],0(%[x8])\n"
"#END _litmus_P0\n\t"
:[x5] "=&r" (out_0_x5[_i]),[x7] "=&r" (trashed_x7)
:[x6] "r" (&_a->x[_i]),[x8] "r" (&_a->y[_i])
:"cc","memory"
);
  }
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
  int *out_1_x1 = _a->out_1_x1;
  int *out_1_x5 = _a->out_1_x5;
  int *out_1_x9 = _a->out_1_x9;
  int *out_1_x10 = _a->out_1_x10;
  for (int _i = _size_of_test-1 ; _i >= 0 ; _i--) {
    barrier_wait(_th_id,_i,&barrier[_i]);
    int trashed_x7;
    int trashed_x11;
asm __volatile__ (
"\n"
"#START _litmus_P1\n"
"#_litmus_P1_0\n\t"
"lw %[x5],0(%[x6])\n"
"#_litmus_P1_1\n\t"
"ori %[x7],x0,1\n"
"#_litmus_P1_2\n\t"
"amoadd.w.rl %[x1],%[x7],(%[x8])\n"
"#_litmus_P1_3\n\t"
"ld %[x9],0(%[x8])\n"
"#_litmus_P1_4\n\t"
"lw %[x10],4(%[x8])\n"
"#_litmus_P1_5\n\t"
"xor %[x11],%[x10],%[x10]\n"
"#_litmus_P1_6\n\t"
"ori %[x11],%[x11],1\n"
"#_litmus_P1_7\n\t"
"sw %[x11],0(%[x13])\n"
"#END _litmus_P1\n\t"
:[x10] "=&r" (out_1_x10[_i]),[x9] "=&r" (out_1_x9[_i]),[x5] "=&r" (out_1_x5[_i]),[x1] "=&r" (out_1_x1[_i]),[x11] "=&r" (trashed_x11),[x7] "=&r" (trashed_x7)
:[x6] "r" (&_a->y[_i]),[x8] "r" (&_a->z[_i]),[x13] "r" (&_a->x[_i])
:"cc","memory"
);
  }
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
      int _out_1_x1_i = ctx.out_1_x1[_i];
      int _out_1_x5_i = ctx.out_1_x5[_i];
      int _out_1_x9_i = ctx.out_1_x9[_i];
      int _out_1_x10_i = ctx.out_1_x10[_i];
      outcome_t o;
      int cond;

      cond = final_ok(final_cond(_out_0_x5_i,_out_1_x1_i,_out_1_x5_i,_out_1_x9_i,_out_1_x10_i));
      o[out_0_x5_f] = _out_0_x5_i;
      o[out_1_x1_f] = _out_1_x1_i;
      o[out_1_x5_f] = _out_1_x5_i;
      o[out_1_x9_f] = _out_1_x9_i;
      o[out_1_x10_f] = _out_1_x10_i;
      add_outcome(hist,1,o,cond);
      if (cond) { hist->n_pos++; } else { hist->n_neg++; }
    }
  }

  finalize(&ctx);
  return hist;
}

#define ENOUGH 10

static void postlude(FILE *out,cmd_t *cmd,hist_t *hist,count_t p_true,count_t p_false,tsc_t total) {
  fprintf(out,"Test LB+mixed1 Allowed\n");
  fprintf(out,"Histogram (%i states)\n",finals_outs(hist->outcomes));
  just_dump_outcomes(out,hist);
  int cond = p_true > 0;
  fprintf(out,"%s\n",cond?"Ok":"No");
  fprintf(out,"\nWitnesses\n");
  fprintf(out,"Positive: %" PCTR ", Negative: %" PCTR "\n",p_true,p_false);
  fprintf(out,"Condition exists (0:x5=1 /\\ 1:x1=0 /\\ 1:x5=1 /\\ 1:x9=1 /\\ 1:x10=0) is %svalidated\n",cond ? "" : "NOT ");
  fprintf(out,"Hash=79a8c230a2b40f02ef6ab9d1dd206a3c\n");
  count_t cond_true = p_true;
  count_t cond_false = p_false;
  fprintf(out,"Observation LB+mixed1 %s %" PCTR " %" PCTR "\n",!cond_true ? "Never" : !cond_false ? "Always" : "Sometimes",cond_true,cond_false);
  if (p_true > 0) {
  }
  fprintf(out,"Time LB+mixed1 %.2f\n",total / 1000000.0);
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
    log_error( "LB+mixed1: n=%i, r=%i, s=%i",n_exe,prm.max_run,prm.size_of_test);
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
      fatal("LB+mixed1, sum_hist");
    }
    merge_hists(hist,hk);
    free_hist(hk);
  }
  tsc_t total = timeofday() - start;
  pm_free(p_mutex);
  pb_free(p_barrier);

  n_outs *= n_exe ;
  if (sum_outs(hist->outcomes) != n_outs || hist->n_pos + hist->n_neg != n_outs) {
    fatal("LB+mixed1, sum_hist") ;
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
