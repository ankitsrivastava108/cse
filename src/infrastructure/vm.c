/*
 * =====================================================================================
 *
 *       Filename:  vm.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/19/14 08:53:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "../infrastructure/balancer.h"
#include "../infrastructure/network.h"
#include "../infrastructure/request_mgr.h"
#include "../workloads/requests.h"
#include "vm.h"
#include "../utils/globals.h"
#include "../utils/rand.h"
#include"../models/energy_models.h"
#include "../stats/metrics.h"

#include <math.h>

#define MIPS 1000

virtual_machine **init_vm(application *app){

    int i, j, k;
    virtual_machine **vm = malloc(sizeof(virtual_machine*) * app->str.nlayers);
    
    k = 0; 
    for(i = 0;i < app->str.nlayers; i++){
	vm[i] = malloc(sizeof(virtual_machine) * app->str.nodeslayer[i]); 
	for(j = 0; j < app->str.nodeslayer[i]; j++){
	    vm[i][j].vcore = app->assigned_vcores[k];
	    vm[i][j].mips = MIPS; 
	    vm[i][j].num_requests = 0; 
	    vm[i][j].num_active = 0; 
	    vm[i][j].vm_req_first = NULL; 
	    vm[i][j].vm_req_last = NULL; 
	    //printf("%d ",app->assigned_vcores[k]);
	    k++;
	}
	//printf("\n");
    }
   
    return(vm);
}

void assign_request_vm(request *req, int vm_layer, int vm_id){

    virtual_machine *vm = &req->app_fe->vm[vm_layer][vm_id];
    req->vm_req[vm_layer] = malloc(sizeof(vm_requests));
    req->vm_req[vm_layer]->req = req;
    req->vm_req[vm_layer]->active = 0;
    req->vm_req[vm_layer]->comp_phase = init_comp_phase();
    req->vm_req[vm_layer]->net_phase = init_net_phase(vm_layer);
    req->vm_req[vm_layer]->net_phase->vm_req = req->vm_req[vm_layer];
    req->vm_req[vm_layer]->next = NULL;
    req->vm_req[vm_layer]->prev = NULL;
    if(vm->num_requests == 0){
    	vm->vm_req_first = req->vm_req[vm_layer];	
	vm->vm_req_last = req->vm_req[vm_layer];	
    }
    else{
	vm->vm_req_last->next = req->vm_req[vm_layer];
	req->vm_req[vm_layer]->prev = vm->vm_req_last;
	vm->vm_req_last = req->vm_req[vm_layer];
    }
    vm->num_requests++; 
}

computational_phase *init_comp_phase(){

     computational_phase *c_p = malloc(sizeof(computational_phase));

     c_p->instructions = 0;
     c_p->remaining_instructions = 0;
     c_p->mips_assigned = 0.0;
     c_p->time_init = 0;
     c_p->time_total = 0;
     c_p->time_arrival = 0;
     c_p->time_event = 0;
     return(c_p);

}

network_phase *init_net_phase(int nlayer){

    network_phase *n_p = malloc(sizeof(network_phase));

    n_p->path = NULL;
    n_p->time_init = 0;
    n_p->data_remaining = 0.0;
    n_p->bw = 0.0;
    n_p->time_arrival = 0;
    n_p->time_remaining = 0;
    n_p->nlayer = nlayer;
    return(n_p);

}

void delete_request_vm(request *req, int vm_layer, int vm_id){

    virtual_machine *vm = &req->app_fe->vm[vm_layer][vm_id];
    vm_requests *vm_req = req->vm_req[vm_layer];

    if(vm->num_requests == 1){
	vm->vm_req_first = NULL;
	vm->vm_req_last = NULL;
    }
    else if(vm_req->prev == NULL){
	vm_req->next->prev = NULL;
	 vm->vm_req_first = vm_req->next;

    }	
    else if(vm_req->next == NULL){
	vm_req->prev->next=NULL;
	vm->vm_req_last = vm_req->prev;
    }
    else{
	 vm_req->prev->next = vm_req->next;
	 vm_req->next->prev = vm_req->prev;
    }
    free(vm_req->net_phase);
    free(vm_req->comp_phase);
    free(vm_req);
    vm->num_requests--;
    //printf("R: %d -- \n", vm->num_requests);
}

void assign_network_resources(request *req, int vm_layer_1, int vm_id_1, int vm_layer_2, int vm_id_2){

    int core_src, core_dst;
    void *path;
    int phase = req->phase;
    virtual_machine *vm_1 = &req->app_fe->vm[vm_layer_1][vm_id_1];
    virtual_machine *vm_2;
    if(req->phase == 0){
	core_src = -1;
	core_dst = vm_1->vcore;
    }
    else if(req->phase == 6){
	core_dst = -1;
	core_src = vm_1->vcore;
    }
    else{
	vm_2 = &req->app_fe->vm[vm_layer_2][vm_id_2];
	core_src = vm_1->vcore;
	core_dst = vm_2->vcore;
    }
  //  printf("Core1: %d Core2: %d\n",core_src,core_dst);
    switch(req->pv->datacenters[req->ndatacenter].nw->type){
	case FATTREE:
	    path = (fattree_nw_path*)network_path(req->pv, req->ndatacenter, core_src, core_dst);
	    if(path == NULL){
		init_transfer_node(req, req->data[phase/2]);
	    }
	    else{
//		printf("%d -> %d\n",core_src,core_dst);
		//print_network_path(path,req->pv, req->ndatacenter);
//		printf("%d\n",core_dst);
		reserve_network_path(req->pv, req->ndatacenter, path, req->vm_req[0]->net_phase);
		init_transfer(req, ((fattree_nw_path*)path)->maxbw, req->data[phase/2]);
	    }
	    break;	
	default:
	    panic("Unknown network type (ssign_network_resources)");
	break;
    }

}

void assign_computational_resources(request *req, int nlayer){

    int num_instructions = rnd(50, 150);           

    init_computation(req, num_instructions, nlayer);

}

void init_computation(request *req, int num_instructions, int nlayer){
   
    computational_phase *c_p = req->vm_req[nlayer]->comp_phase;

    c_p->mips_assigned = modify_vm_comp_requests_arrival(req, nlayer);
    c_p->instructions = num_instructions;
    c_p->time_arrival = simtime;
    c_p->time_init = simtime;
    c_p->remaining_instructions = num_instructions;
    //c_p->mips_assigned = get_instruction_seconds(&req->app_fe->vm[nlayer][req->assigned_vm[nlayer]]); 
    c_p->time_total = (long)ceilf((c_p->remaining_instructions / c_p->mips_assigned));
    c_p->remaining_instructions = num_instructions; 
    c_p->time_event = simtime + c_p->time_total;
    c_p->nlayer = nlayer;
    req->num_events++;
    req->remaining_events++;
    req->vm_req[nlayer]->active = 1;
    //printf("C Simtime: %ld Time event: %ld Time arrival: %ld Total instructions: %d Remaining instructions: %d Mips: %f Vm: %d Layer: %d Num events: %d APP: %d Req: %d\n",simtime, c_p->time_event, c_p->time_arrival, c_p->instructions, c_p->remaining_instructions, c_p->mips_assigned, req->assigned_vm[nlayer],nlayer,req->num_events,req->app_fe->app->type, req->reqType);
    insert_event(c_p->time_event, COMPUTATION_CALENDAR, req->num_events, req);
}

float modify_vm_comp_requests_arrival(request *req, int nlayer){
    
    int i, total_vcores, idle;
    long time;
    float speed, processed_instructions, energy;
    virtual_machine *vm = &req->app_fe->vm[nlayer][req->assigned_vm[nlayer]]; 
    computational_phase *c_p;
    vm_requests *vm_req = vm->vm_req_first;
    speed = ((float)((vm->mips / 1000.0) / (float)(vm->num_active + 1)));

    if(vm->num_active == 0){
	total_vcores = req->pv->datacenters[req->ndatacenter].nw->cores_node;
	time =  req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].energy_event; 
	idle = req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].idlecores; 
	energy = node_energy_model(total_vcores, idle, simtime - time); 
	nodes_energy_metric(req->pv, req->ndatacenter, energy);
	req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].idlecores++; 
	req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].energy_event = simtime;
    }
    //printf("Number of actives on layer %d: %d Speed: %f Vm mips: %d\n", nlayer, vm->num_active,speed, vm->mips);
    for(i=0; i < vm->num_requests;i++){
	if(vm_req->active){ 
	    c_p = vm_req->comp_phase;
	    ///printf("M1 Simtime: %ld Time event: %ld Time arrival: %ld Total instructions: %d Remaining instructions: %d Mips: %f Layer: %d, APP: %d Req: %d\n",simtime, c_p->time_event, c_p->time_arrival, c_p->instructions, c_p->remaining_instructions, c_p->mips_assigned, c_p->nlayer, vm_req->req->app_fe->app->type, vm_req->req->reqType);
	    processed_instructions = ((c_p->mips_assigned * ((float)(simtime - c_p->time_init))));
	    c_p->remaining_instructions -= (int)processed_instructions;
	    c_p->time_total = (long)ceilf((c_p->remaining_instructions / speed));
	    c_p->time_event = simtime + c_p->time_total;
	    c_p->mips_assigned = speed;
	    c_p->time_init = simtime;
	    vm_req->req->num_events++;
	    vm_req->req->remaining_events++;
	    //printf("M2 Simtime: %ld Time event: %ld Time arrival: %ld Total instructions: %d Remaining instructions: %d Mips: %f Layer: %d, APP: %d Req: %d Num events: %d Processed instructions: %f\n",simtime, c_p->time_event, c_p->time_arrival, c_p->instructions, c_p->remaining_instructions, c_p->mips_assigned, nlayer, req->app_fe->app->type, req->reqType, vm_req->req->num_events, processed_instructions);
	    insert_event(c_p->time_event, COMPUTATION_CALENDAR, vm_req->req->num_events, vm_req->req);
	}	
	vm_req = vm_req->next;
    }
    vm->num_active++;
    return(speed);
}

void modify_vm_comp_requests_depart(request *req, int nlayer){
    
    int i, total_vcores,idle;
    long time;
    float speed, processed_instructions, energy;
    virtual_machine *vm = &req->app_fe->vm[nlayer][req->assigned_vm[nlayer]]; 
    computational_phase *c_p;
    vm_requests *vm_req = vm->vm_req_first;
    speed = ((float)((vm->mips / 1000.0) / (float)(vm->num_active)));

    if(vm->num_active == 0){
	total_vcores = req->pv->datacenters[req->ndatacenter].nw->cores_node;
	time =  req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].energy_event; 
	idle = req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].idlecores; 
	energy = node_energy_model(total_vcores, idle, simtime - time); 
	nodes_energy_metric(req->pv, req->ndatacenter, energy);
	req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].idlecores--; 
	req->pv->datacenters[req->ndatacenter].nw->n[vm->vcore/total_vcores].energy_event = simtime;
    }

    for(i=0; i < vm->num_requests;i++){
	if(vm_req->active){ 
	    c_p = vm_req->comp_phase;
	    if(c_p->time_event != simtime){
	    //printf("M3 Simtime: %ld Time event: %ld Time arrival: %ld Total instructions: %d Remaining instructions: %d Mips: %f\n",simtime, c_p->time_event, c_p->time_arrival, c_p->instructions, c_p->remaining_instructions, c_p->mips_assigned);
	    processed_instructions = ((c_p->mips_assigned * ((float)(simtime - c_p->time_init))));
	    c_p->remaining_instructions -= (int)processed_instructions;
	    c_p->time_total = (long)ceilf((c_p->remaining_instructions / speed));
	    c_p->time_event = simtime + c_p->time_total;
	    c_p->mips_assigned = speed;
	    c_p->time_init = simtime;
	    vm_req->req->num_events++;
	    vm_req->req->remaining_events++;
	    insert_event(c_p->time_event, COMPUTATION_CALENDAR, vm_req->req->num_events, vm_req->req);
	    //printf("M4 Simtime: %ld Time event: %ld Time arrival: %ld Total instructions: %d Remaining instructions: %d Mips: %f Processed instructions: %f\n",simtime, c_p->time_event, c_p->time_arrival, c_p->instructions, c_p->remaining_instructions, c_p->mips_assigned, processed_instructions);
	}	
	}
	vm_req = vm_req->next;
    }
}

void init_transfer(request *req, float bw, float data){

    network_phase *n_p =  req->vm_req[0]->net_phase;

    n_p->time_arrival = simtime;
    n_p->time_init = simtime;
    n_p->data_remaining = data;
    n_p->time_remaining =(long)ceilf((((n_p->data_remaining * 8.0) / 1000) / bw ) * 1000);
    n_p->bw = bw; 
    n_p->vm_req = req->vm_req[0];
    n_p->time_event = simtime + n_p->time_remaining;
    req->num_events++;
    req->remaining_events++;
    //printf("I Simtime: %ld Time event: %ld Time arrival: %ld Data remaining: %f Bw: %f Time remaining: %ld Data: %f\n", simtime, n_p->time_event, n_p->time_arrival, n_p->data_remaining, n_p->bw, n_p->time_remaining, data);
    //print_network_path(n_p->path,req->pv, req->ndatacenter);
    insert_event(n_p->time_event, TRANSMISSION_CALENDAR, req->num_events, req);

}

void init_transfer_node(request *req, float data){

    network_phase *n_p =  req->vm_req[0]->net_phase;

    req->num_events++;
    req->remaining_events++;
    n_p->time_remaining = 1;
    n_p->time_event = simtime + n_p->time_remaining;
    insert_event(n_p->time_event, TRANSMISSION_NODE_CALENDAR, req->num_events, req);
}

void modify_transfer(vm_requests  *vm_req, float bw_new){

    float data_transfered;
    long time;
    network_phase *n_p;

    n_p =  vm_req->net_phase;
    if(n_p->time_event != simtime /*  && n_p->nlayer == 0*/){
	//printf("M0 Simtime: %ld Time event: %ld Time arrival: %ld Data remaining: %f Bw: %f Time remaining: %ld\n", simtime, n_p->time_event, n_p->time_arrival, n_p->data_remaining, n_p->bw, n_p->time_remaining);
	time = simtime - n_p->time_init;
	n_p->time_init = simtime;
	data_transfered = time * (((n_p->bw / 8) / 1000) * 1000);
	if(data_transfered < n_p->data_remaining){
	    n_p->data_remaining -= data_transfered;
	    n_p->time_remaining =(long)ceilf((((n_p->data_remaining * 8.0) / 1000) / bw_new ) * 1000);
	    n_p->time_event = simtime + n_p->time_remaining;
	    n_p->bw = bw_new;
	    vm_req->req->num_events++;
	    vm_req->req->remaining_events++;
	//printf("M Simtime: %ld Time event: %ld Time arrival: %ld Data remaining: %f Bw: %f Time remaining: %ld\n", simtime, n_p->time_event, n_p->time_arrival, n_p->data_remaining, n_p->bw, n_p->time_remaining);
	 //print_network_path(n_p->path,vm_req->req->pv, vm_req->req->ndatacenter);
	
	insert_event(n_p->time_event, TRANSMISSION_CALENDAR, vm_req->req->num_events, vm_req->req);
	}
    }/*  
    else if(n_p->time_event != simtime && n_p->nlayer == 2){
	time = simtime - n_p->time_init;
	n_p->time_init = simtime;
	data_transfered = time * (((n_p->bw / 8) / 1000) * 1000);
	n_p->data_remaining -= data_transfered;
	n_p->time_remaining =(long)ceilf((((n_p->data_remaining * 8.0) / 1000) / bw_new ) * 1000);
	n_p->time_event = simtime + n_p->time_remaining;
	n_p->bw = bw_new;
	vm_req->req->num_events_bd++;
	vm_req->req->remaining_events_bd++;
	//printf("MBD Simtime: %ld Time event: %ld Time arrival: %ld Data remaining: %f Bw: %f Time remaining: %ld\n", simtime, n_p->time_event, n_p->time_arrival, n_p->data_remaining, n_p->bw, n_p->time_remaining);
	 //print_network_path(n_p->path, vm_req->req->pv, vm_req->req->ndatacenter);
	insert_event(n_p->time_event, TRANSMISSION_BD_CALENDAR, vm_req->req->num_events_bd, vm_req->req);
    }
*/
}






