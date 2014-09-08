/*
 * =====================================================================================
 *
 *       Filename:  functions.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/14 09:23:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "../infrastructure/network.h"
#include "../infrastructure/fattree.h"
#include "../workloads/application.h"
#include "../models/energy_models.h"
#include "../utils/globals.h"

#include <math.h>
long eval_cores_f1(int *ind, int length, network *nw, int **bw){

    int i,j;
    long obj=0;

    for (i=0;i<length;i++){
	for (j=0;j<length;j++){
	    obj += distance_network_cores(nw, ind[i], ind[j]) * bw[i][j];    
	}
    }
    //printf("Bw: %ld\n",obj);
    return(obj);
}

long eval_cores_f2(int *ind, int length, network *nw){

    long obj;

    obj =on_servers_allocation(nw, ind, length);
    //printf("Servers: %ld\n",obj);
    return(obj);
}


long eval_pmin_constrained(int length, int *maxbw){

int i,bw,j;
long cores;

bw=0;
cores=0;
j=0;
for(i=0;i<length;i++){
if(bw + maxbw[i] < 1000){
j++;
cores++;
bw+=maxbw[i];
}
else{
cores+=(8-j);
j=1;
bw=maxbw[i];
}
if(j==8){
bw=0;
j=0;
}
}
return(cores/8);
}

void update_metric_energy_nodes(long simcycle){

    int i;
    int total_vcores, idle;
    long time;
    float energy;
    provider *pv = &providerSet[0];
    network *nw = pv->datacenters[0].nw;
    
    if(sim_convergence.sim_state == 0){
    for(i=0;i<nw->nnodes;i++){
	nw->n[i].energy_event = simtime;
    }
    }
    else{
	for(i=0;i<nw->nnodes;i++){
	    //if(nw->n[i].energy_event == simcycle){
		total_vcores = nw->cores_node;
		time =  nw->n[i].energy_event; 
		idle = nw->n[i].idlecores; 
		energy = node_energy_model(total_vcores, idle, simtime - time);
//		printf("(%d %f %ld)", idle, energy,time);
		nodes_energy_metric(pv, 0, energy);
		nw->n[i].energy_event = simtime;
	    //}
	}
    }
//    printf("\n");
  //  printf("SIM: %ld\n",simcycle);
    //for(i=0;i<nw->nnodes;i++){
//	printf("%ld ",nw->n[i].energy_event);
  //  }
    //printf("\n");
} 

void update_metric_energy_switches(long simcycle){
    
    int i, k, n_s;
    float energy;
    provider *pv = &providerSet[0];
    str_ft *str= pv->datacenters[0].nw->str;
    n_s =  pow(PORTS_UP,(NLEVELS - 1));

    if(sim_convergence.sim_state == 0){
	for(i = 0; i < NLEVELS; i++){
	    for(k = 0; k < n_s; k++){
		str->str[i][k].energy_event= simtime;
		//printf("%ld ",str->str[i][k].energy_event); 	
	    }
	    //printf("\n");
	}
    }
    else{
	for(i = 0; i < NLEVELS; i++){
	    for(k = 0; k < n_s; k++){
		energy = switch_energy_model(str->str[i][k].num_ports, str->str[i][k].used_ports, simtime - str->str[i][k].energy_event);
		switches_energy_metric(pv, 0, energy);
		str->str[i][k].energy_event= simtime;
		//printf("%ld ",str->str[i][k].energy_event);
	    }
	    //printf("\n");
	}
    }
}












