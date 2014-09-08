/**
* @file
* Some miscellaneus tools & definitions.
*
* Tools for memory allocation & error printing.
* Definition of some useful macros.
* Definition of some enumerations.
*/
#ifndef _misc
#define _misc

#include "constants.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// #define zto()  ((double)rand()/(RAND_MAX+1.0))
// Currently defined as panic --> #define critical(e) if((e)) { perror(""); exit(errno); } 

/**
* Choose a random number in [ 0, m ).
* @param m maximum.
* @return a random number.
*/
#define ztm(m) (long) (m * ( (1.0*rand() ) / (RAND_MAX+1.0)))
#define ABS(m) 				((m<0) ? (-m) : (m))
#define P_NULL (-1) ///< Definition of a NULL value. 

/**
* Definition of the maximum chooser
*/
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef mod
#define mod(a,b)            ((((a)%(b)) <  0 ) ? ((a)%(b)) + (b) : ((a)%(b)) )
#endif

/**
* Definition of boolean values.
*/
typedef enum bool_t {
	FALSE = 0, TRUE = 1
} bool_t;

// Some declarations.
void * alloc(long);
void panic(char *mes);

typedef enum network_t{
    FATTREE	
} network_t;

typedef enum scaler_add_policy_t{
    SFF	
} scaler_add_policy_t;

typedef enum scaler_rem_policy_t{
    SLAST
} scaler_rem_policy_t;

typedef enum allocation_policy_t{
    FF, RR
} allocation_policy_t;

typedef enum optimization_alg_t{
    NONE,NSGA2,SPEA2,HYPE,SHV 
} optimization_alg_t;

typedef enum mutation_type_t{
    MRND, MDIST 
} mutation_type_t;

typedef enum crossover_type_t{
    CORND, COTA 
} crossover_type_t;

#endif /* _misc */

