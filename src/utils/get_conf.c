#include "misc.h"
#include "get_conf.h"
#include "globals.h"
#include "literal.h"

#include <stdio.h>
#include <string.h>

/** The Configuration file. */
#define DEFAULT_CONF_FILE "cse.conf"

/** 
* Default values for options are specified here.
* @see literal.c
*/
static literal_t options_l[] = {
	{ 1,  "provider" },		/* Number of providers*/
	{ 2,  "datacenters" },	/* Number od datacenters/provider*/
	{ 3,  "pracks" },		/* Number of racks*/
	{ 4,  "pnodes" },		/* Number of nodes/rack*/
	{ 5,  "cores"},			/* Number of cores/node */
	{ 6,  "vcores"},			/* Number of cores/node */
	{ 7,  "bw"},			/* Number of cores/node */
	{ 8,  "memory"},			/* Number of cores/node */
	{ 9,  "workload"},		/* File containing the workload*/
	{ 10, "logname"},		/* File containing the workload*/
	{ 11, "seed"},		/* File containing the workload*/
	{ 12, "allocation"},		/* File containing the workload*/
	{ 13, "optalg"},		/* File containing the workload*/
	{ 14, "alpha"},		/* File containing the workload*/
	{ 15, "mu"},		/* File containing the workload*/
	{ 16, "lambda"},		/* File containing the workload*/
	{ 17, "dim"},		/* File containing the workload*/
	{ 18, "tournament"},		/* File containing the workload*/
	{ 19, "ngen"},		/* File containing the workload*/
	{ 20, "mtype"},		/* File containing the workload*/
	{ 21, "cotype"},		/* File containing the workload*/
	{ 22, "pmut"},		/* File containing the workload*/
	{ 23, "topomut"},		/* File containing the workload*/
	{ 24, "pco"},		/* File containing the workload*/
	{ 25, "topoco"},		/* File containing the workload*/
	{ 26, "occupancy"},		/* File containing the workload*/
	{ 27, "reqgen"},		/* File containing the workload*/
	LITERAL_END
};

literal_t allocation_policy_l[] = {
    { FF,       "ff"},
    { RR,       "rr"},
    LITERAL_END
};

literal_t optimization_alg_l[] = {
    { NONE,       "none"},
    { NSGA2,       "nsga2"},
    { SPEA2,       "spea2"},
    { HYPE,       "hype"},
    { SHV,       "shv"},
    LITERAL_END
};

literal_t mutation_type_l[] = {
    { MRND,       "rnd"},
    { MDIST,      "dist"},
    LITERAL_END
};

literal_t crossover_type_l[] = {
    { CORND,       "rnd"},
    { COTA,       "ta"},
    LITERAL_END
};

/**
* Gets the configuration defined into a file.
* @param fname The name of the file containing the configuration.
*/
void get_conf_file(char * fname) {
	FILE * fp;
	char buffer[1024];

	if((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "WARNING: config file %s not found in current directory\n", fname);
		return;
	}

	while(fgets(buffer, 1024, fp) != NULL)
		if(buffer[0] != '\n' && buffer[0] != '#') {
			if(buffer[strlen(buffer) - 1] == '\n')
				buffer[strlen(buffer) - 1] = '\0';
			get_option(buffer);
		}
		fclose(fp);
}

/**
* Get the configuration for the simulation.
*
* First take default values for the simulation.
* Second take the values from the file 'cse.conf'.
* Finally take the command line arguments.
* @param argn The number of arguments.
* @param args The arguments taken from the command line.
*/
void get_conf(long argn, char ** args) {
	long i;

	set_default_conf();
	get_conf_file(DEFAULT_CONF_FILE);
	for(i = 0; i < argn; ++i)
		get_option(args[i]);
	verify_conf();
}

/**
* Gets an option & its value.
*
* Put the value to the CSE variable.
* @param option The string which contains an option=value
*/
void get_option(char * option) {
	int opt;
	char * name;
	//char * param;
	char * value;
	char message[100];

	name = strtok(option, "=");
	if(!literal_value(options_l, name, &opt)) {
		sprintf(message, "get_option: Unknown option %s", name);
		panic(message);
	}
	value = strtok(NULL, "=");
	switch(opt) {
	case 1:
		sscanf(value, "%d", &nproviders);
		break;
	case 2:
		sscanf(value, "%d", &ndatacenters);
		break;
	case 3:
		sscanf(value, "%d", &nracks);
		break;
	case 4:
		sscanf(value, "%d", &nhosts);
		break;
	case 5:
		sscanf(value, "%d", &ncores);
		break;
	case 6:
		sscanf(value, "%d", &nvcores);
		break;
	case 7:
		sscanf(value, "%d", &bwtotal);
		break;
	case 8:
		sscanf(value, "%d", &memtotal);
		break;
	case 9:
		sscanf(value, "%s", (char*)&trcfile);
		break;
	case 10:
		sscanf(value, "%s", (char*)&logname);
		break;
	case 11:
		sscanf(value, "%llu", &seed);
		break;
	case 12:
		if(!literal_value(allocation_policy_l, value, (int *) &allocation_policy))
		    panic("get_conf: Allocation policy");
		break;
	case 13:
	    if(!literal_value(optimization_alg_l, value, (int *) &optimization_alg))
		    panic("get_conf: Optimization algorithm");
		break;
	case 14:
		sscanf(value, "%d", &alpha);
		break;
	case 15:
		sscanf(value, "%d", &mu);
		break;
	case 16:
		sscanf(value, "%d", &lambda);
		break;
	case 17:
		sscanf(value, "%d", &dim);
		break;
	case 18:
		sscanf(value, "%d", &tournament);
		break;
	case 19:
		sscanf(value, "%d", &ngen);
		break;
	case 20:
	    if(!literal_value(mutation_type_l, value, (int *) &mtype))
		    panic("get_conf: Mutation type");
		break;
  	case 21:
	    if(!literal_value(crossover_type_l, value, (int *) &cotype))
		    panic("get_conf: Crossover type");
		break;
	case 22:
		sscanf(value, "%f", &pmut);
		break;
	case 23:
		sscanf(value, "%f", &pmuttopo);
		break;
	case 24:
		sscanf(value, "%f", &pco);
		break;
	case 25:
		sscanf(value, "%f", &pcotopo);
		break;
	case 26:
		sscanf(value, "%f", &occupancy);
		break;
	case 27:
		sscanf(value, "%d", &reqgen);
		break;
	default:
		sprintf(message, "get_option: Unknown option %s", name);
		panic(message);
	}
}

/**
* Verifies the simulation configuration.
*
* Looks for illegal values of the variables or not allowed combinations.
*/
void verify_conf(void) {

}

/**
* The default configuration is set here.
*/
void set_default_conf (void) {
    
    seed = 10;
    nproviders=16;	
    ndatacenters=4;	
    nracks=1;
    nhosts=4;	
    ncores=4;
    nvcores=4;
    bwtotal=1000;
    memtotal=1024;
}

