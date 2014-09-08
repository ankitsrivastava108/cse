#ifndef _globals
#define _globals

#include "../evengine/calendar.h"
#include "../evengine/event.h"
#include "../infrastructure/provider.h"
#include "../utils/misc.h"
#include "../utils/constants.h"
#include "../stats/batch.h"
#include "../stats/metrics.h"

extern provider *providerSet;

extern unsigned long long seed;
extern int nproviders;  
extern int ndatacenters; 
extern int nracks;
extern int nhosts;   
extern int ncores;
extern int nvcores;
extern int bwtotal;
extern int memtotal;
extern int optimization_alg;
extern int allocation_policy;
extern int alpha, mu, lambda, dim, tournament, ngen;
extern int mtype, cotype;
extern float pmut;
extern float pmuttopo;
extern float pco;
extern float pcotopo;
extern char trcfile[128];
extern char logname[30];
extern float occupancy;
extern int reqgen;
extern metrics_infrastructure_s *metrics_infrastructure;
extern metrics_application_s *metrics_application;
extern convergence sim_convergence;
#endif
