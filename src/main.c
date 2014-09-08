#include "workloads/workload.h"
#include "utils/get_conf.h"
#include "utils/globals.h"
#include "evengine/calendar.h"
#include "evengine/eventmgr.h"
#include "stats/log.h"
#include "stats/batch.h"
#include "stats/metrics.h"
#include "utils/rand.h"

#include <stdio.h>
#include <time.h>


provider *providerSet;
/**
 * Global variables
 * */

unsigned long long seed;
int nproviders;  
int ndatacenters; 
int nracks;
int nhosts;   
int ncores;
int nvcores;
int bwtotal;
int memtotal;
int allocation_policy;
int optimization_alg;
int alpha, mu, lambda, dim, tournament, ngen;
int mtype, cotype;
float pmut;
float pmuttopo;
float pco;
float pcotopo;
int reqgen;
float occupancy;

char trcfile[128];

static time_t start_time,   ///< Simulation start time / date.
	    	   end_time;   ///< Simulation finish time / date.
/**
 * * MAIN
 * *
 * * The main function of the simulator.
 * */
int main(int argc, char *argv[])
{
    get_conf((long)(argc - 1), argv + 1); 
    randomize(seed);
    init_structures();
    //read_trace();
    gen_workload(0); 
    time(&start_time);
    open_log();
    init_calendar();
    init_warm_up();
    init_batch_metrics();
    start_simulation();
    execute();
    time(&end_time);
    write_log_final(start_time, end_time);
    end_batch_metrics(); 
    close_log();
    remove_structures();
    return(1);
}

