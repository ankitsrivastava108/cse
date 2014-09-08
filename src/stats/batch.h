/*
 * =====================================================================================
 *
 *       Filename:  batch.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/25/14 10:08:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _batch
#define _batch

typedef struct convergence{
    
    int sim_state;
    int cycles_convergence;
    int cycles_batch;
    float average;
    float variance;
    float threshold;
    int num; 
    int num_batches; 
} convergence;

convergence sim_convergence;

void init_warm_up();

void check_warm_up();

void check_batches();

#endif
