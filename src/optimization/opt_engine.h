/*========================================================================
  PISA  (www.tik.ee.ethz.ch/pisa/)
  ========================================================================
  Computer Engineering (TIK)
  ETH Zurich
  ========================================================================
  NSGA2

  Implementation in C for the selector side.
  
  Header file.
  
  file: nsga2.h
  author: Marco Laumanns, laumanns@tik.ee.ethz.ch

  revision by: Stefan Bleuler, bleuler@tik.ee.ethz.ch
  last change: $date$
  ========================================================================
*/

#ifndef _opt_engine 
#define _opt_engine 


#include "../infrastructure/provider.h"
#include "../infrastructure/network.h"
#include "../workloads/application.h" 

#define PISA_MAXDOUBLE 1E99
#define PISA_MINDOUBLE 1E-99

typedef struct opt_params_st{

    int alg;
    int alpha;
    int mu;
    int lambda;
    int dim;
    int tournament;
    int ngen;
    int mtype;
    int cotype;
    float pmut;
    float pmuttopo;
    float pco;
    float pcotopo;
    network *nw;
    int **bw;
    int *maxbw;
} opt_params_st;

float optimize(int alg, int alpha, int mu, int lambda, int dim, int tournament, int ngen, int mtype, int cotype, float pmut, float pmuttopo, float pco, float pcotopo, int size, int *vector, provider *pv, int ndatacenter, application *app);
extern opt_params_st opt_params;
#endif 
