#include "opt_engine.h"
#include "opt_variator.h"
#include "opt_selector.h"
#include "hype.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>

opt_params_st opt_params;

float optimize(int alg, int alpha, int mu, int lambda, int dim, int tournament, int ngen, int mtype, int cotype, float pmut, float pmuttopo, float pco, float pcotopo, int size, int *vector, provider *pv, int ndatacenter, application *app){
     
    int i, gen;
    int *initial_population, *result;
    int *parent_identities, *offspring_identities;
    float hv = 0;

    global_population.individual_array = NULL; 
    global_population.size = 0;
    global_population.last_identity = -1;
    opt_params.alg = alg;
    opt_params.alpha = alpha;
    opt_params.mu = mu;
    opt_params.lambda = lambda;
    opt_params.dim = dim;
    opt_params.tournament = tournament;
    opt_params.ngen = ngen;
    opt_params.mtype = mtype;
    opt_params.cotype = cotype;
    opt_params.pmut = pmut;
    opt_params.pmuttopo = pmuttopo;
    opt_params.pco = pco;
    opt_params.pcotopo = pcotopo;
    opt_params.nw = pv->datacenters[ndatacenter].nw;
    opt_params.bw = app->str.bw;
    opt_params.maxbw = app->str.maxbw;

    initial_population = (int *) malloc(alpha * sizeof(int));
    parent_identities = (int *) malloc(mu * sizeof(int));
    offspring_identities = (int *) malloc(lambda * sizeof(int));

    for(i = 0; i < alpha; i++){
	 initial_population[i] = add_individual(new_individual(size, vector,i));
    }
    initialize();
    read_ini(initial_population);   
    selectopt(); /* do selection */
    write_arc();      
    write_sel(parent_identities);      /* write sel file */
    for(gen = 0; gen < opt_params.ngen; gen++){
	variate(parent_identities, offspring_identities, size);
	read_var(offspring_identities);    
	selectopt(); /* do selection */
	write_arc();      
    	write_sel(parent_identities);      /* write sel file */
    } 
    free(initial_population);
    free(parent_identities);
    free(offspring_identities);
    result = eval_pareto_set();
    //hv = unaryHypervolumeIndicator( 	 
    if(result != NULL){
	memcpy(vector, result, sizeof(int) * size);
    }
    else{
	//printf("***** NO VALID ***** \n");
    }
    free_memory();
    clean_population();
    return (hv);
}
