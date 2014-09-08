/*
 * =====================================================================================
 *
 *       Filename:  problem.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/14 15:53:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "opt_engine.h"
#include "opt_variator.h"
#include "../utils/rand.h"
#include "../infrastructure/network.h"

long eval_f1(individual *ind){

    int i,j;
    long obj=0;

    for (i=0;i<ind->length;i++){
	for (j=0;j<ind->length;j++){
	    obj += distance_network_cores(opt_params.nw, ind->bit_string[i], ind->bit_string[j]) * opt_params.bw[i][j];    
	}
    }
    return(obj);
}

long eval_f2(individual *ind){

    long obj;

    obj =on_servers_allocation(opt_params.nw, ind->bit_string, ind->length);
    return(obj);
}

int check_constrainsts(individual *ind){
    
    int i, node, valid;
    valid = 1;
    float *sw_aux = malloc(sizeof(float) * opt_params.nw->nnodes); 
    for(i = 0; i < opt_params.nw->nnodes; i++){
	sw_aux[i] = opt_params.nw->n[i].freebw;
    }

    for(i = 0; i < ind->length; i++){
	node =  ind->bit_string[i] / opt_params.nw->cores_node;
	if(sw_aux[node] >= opt_params.maxbw[i]){
	    sw_aux[node] -= opt_params.maxbw[i];
	}
	else{
	    valid = 0;
	    break;
	}	    
    }	
    free(sw_aux);
    return(valid);
}

