/*
 * =====================================================================================
 *
 *       Filename:  mutation.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/14 11:01:52
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
#include "problem.h"
#include "../utils/rand.h"
#include <math.h>

void rnd_mutate_ind (individual *ind)
{
    int pos,pos1,pos2,tmp, node_aux_pos, core_aux_pos;

    if (randomperc() <= opt_params.pmut){
	if(randomperc() <= 0.5 || opt_params.nw->free_vcores == ind->length){
	    pos1 = rnd(0, ind->length - 1); 
	    pos2 = rnd(0, ind->length - 1); 
	    tmp = ind->bit_string[pos1];
	    ind->bit_string[pos1] = ind->bit_string[pos2];
	    ind->bit_string[pos2] = tmp;
	}
	else {
	    pos2 = rnd(0, ind->length - 1);
	    pos = rnd(0, opt_params.nw->total_vcores - 1); 
	    node_aux_pos = pos /  opt_params.nw->cores_node; 
	    core_aux_pos = pos % opt_params.nw->cores_node;
	    while(opt_params.nw->n[node_aux_pos].vcores[core_aux_pos] != -1 || contains(pos, ind->length, ind->bit_string) != -1){
		pos = rnd(0, opt_params.nw->total_vcores - 1); 
		node_aux_pos = pos /  opt_params.nw->cores_node; 
		core_aux_pos = pos % opt_params.nw->cores_node;
	    
	    }
	    ind->bit_string[pos2] = pos; 
	}
    }      
    ind->f1 = eval_f1(ind);
    ind->f2 = eval_f2(ind);
    ind->valid = check_constrainsts(ind);
}

  
void topoaware_mutate_ind (individual *ind){

    int core, dist, pos1, pos2, i, tmp, found, d, node_aux_pos, core_aux_pos;
    double rnddist;

    if (randomperc() <= opt_params.pmut){
	 if(randomperc() <= 0.5 || opt_params.nw->free_vcores == ind->length){
	      pos1 = rnd(0, ind->length - 1);
	      pos2 = rnd(0, ind->length - 1);
		while(pos1==pos2){
		    pos2 = rnd(0, ind->length - 1);
		}
		tmp = ind->bit_string[pos1];
		ind->bit_string[pos1] = ind->bit_string[pos2];
		ind->bit_string[pos2] = tmp;
	}
	else{
	    pos2 = rnd(0, ind->length - 1);
	    core = ind->bit_string[pos2];  
	    rnddist = randomperc();
	    if(rnddist <= 0.5){
		dist=2;
	    }
	    else if(rnddist <= 0.8){
		dist=4;
	    }
	    else{
		dist=6;
	    }
	    found=0;
	    while(found==0){
		for(i = 0;i < opt_params.nw->total_vcores;i++){
		    d = distance_network_cores(opt_params.nw, i, core);
		    if(d == 0){
			continue;
		    }
		    node_aux_pos = i /  opt_params.nw->cores_node;	    
		    core_aux_pos = i % opt_params.nw->cores_node;
		    if(opt_params.nw->n[node_aux_pos].vcores[core_aux_pos] == -1  &&  d == dist && contains(i, ind->length, ind->bit_string) == -1){
			found=1;
			break;
		    }
		}
		pos2 = rnd(0, ind->length - 1);
		core = ind->bit_string[pos2];
		rnddist = randomperc();
		if(rnddist <= 0.5){
		    dist=2;
		}
		else if(rnddist <= 0.8){
		    dist=4;
		}
		else{
		    dist=6;
		}
	    }
	    ind->bit_string[pos2] = i;
	}
    }
    ind->f1 = eval_f1(ind);
    ind->f2 = eval_f2(ind);
    ind->valid = check_constrainsts(ind);
}
