#include "../utils/misc.h"

#include "../infrastructure/broker.h"
#include "../infrastructure/network.h"
#include "../infrastructure/balancer.h"
#include "../infrastructure/request_mgr.h"
#include "eventmgr.h"
#include "../utils/globals.h"
#include "../stats/metrics.h"
#include "../workloads/workload.h"

/**
 * * Schedules the next event in the cpu tasks list.
 * *
 * *@param n the cpu id.
 * *@param ts the timestamp when it has to be scheduled.
 * */
void schedule_next_event( long n, long ts) {
    cal_event_type t;
    
    if (!event_empty(&providerSet[0].events))
    {   
            event *ev=head_event(&providerSet[0].events);
            switch(ev->type){
                    case ARRIVAL:
                        t=ARRIVAL_CALENDAR;
                        //printf("Insert ARRIVAL event in providerSet %ld at ts %ld\n",n,ev->atime);
                        break;
                    default:
                        panic("Unexpected type of event in schedule_next_event\n");
                    }
            insert_event(ev->atime,t,ev->id,NULL); 
    }   
    else
    {   
	//insert_event(ts,FINISH_CALENDAR,n,0);
    	//providerSet[0].finished=TRUE;
    }   
}

void init_structures(){
	
	create_provider(ndatacenters);	
	init_event(&providerSet[0].events);

}

void remove_structures(){
	
    delete_provider();
}

void start_simulation(){

    schedule_next_event(0,0);

}


void run_arrival_event(int v){

    event *ev;           // trace event.
    unsigned long wt;   // waiting time.

    if(!event_empty(&providerSet[0].events)){
        ev = head_event(&providerSet[0].events);
        wt = ev->rtime + 600000 + 300000 + simtime;
	if(request_vcores(&providerSet[0], ev)==1){
    //printf("App %d starts %ld\n",ev->id,simtime);
	    insert_event(wt,DEPARTURE_CALENDAR,ev->id,NULL);
	    rem_head_event(&providerSet[0].events);
	    schedule_next_event(0,0);
	}
	else{
	    panic("Not enough free nodes");
	}
	
    }
}

void run_departure_event(int id){
  //  if(providerSet[0].finished==TRUE){
//	insert_event(simtime+1,FINISH_CALENDAR,0,0);
    //}
    int ndatacenter=0;
    //event *ev;
    //unsigned long wt;   // waiting time.
    release_vcores(&providerSet[0],ndatacenter,id);
    gen_workload(1);
    schedule_next_event(0,0);
    //printf("App %d finishes %ld\n",id,simtime);
    //if(!event_empty(&providerSet[0].events)){
        //  ev = head_event(&providerSet[0].events);
       // wt = (ev->rtime * 1000) + 600000 + 300000 + simtime;
       // insert_event(wt,DEPARTURE_CALENDAR,ev->id,NULL);
//	if(request_vcores(&providerSet[0], ev)==1){
//	    rem_head_event(&providerSet[0].events);
//	    schedule_next_event(0,0);
//	}
//	else{
//	    panic("Not enough free nodes");
//	}
	
  //  }
}

void run_warmup_event(){

    check_warm_up();

}

void run_batch_event(){

    check_batches();
}

void run_request_event(void *vp){

    if(((request*)vp)->app_fe->active == 1){
     //rem_request((request*)vp); 
	process_request((request*)vp); 
    }
else{
    rem_request((request*)vp);
}
}

void run_request_generation_event(int basereq, void *vp){
    
    app_frontend *app_fe = (app_frontend*)vp;
    application *app = app_fe->app;
    gen_requests(app_fe->pv, app->id, app->type, app->etime, app_fe, app->rtime, 0, app->maxreq, app->req_chunk, basereq, app->last_time_req);
}
void run_transmission_event(int id, void *vp){

    request *req = (request*)vp;
    network_phase *n_p;

    
    n_p =  req->vm_req[0]->net_phase;

    if(id == req->num_events){
	time_network_metric(req->pv, req->ndatacenter, simtime - n_p->time_arrival);
	//printf("D Simtime: %ld Time arrival: %ld Data remaining: %f Bw: %f Time remaining: %ld\n", simtime, n_p->time_arrival, n_p->data_remaining, n_p->bw, n_p->time_remaining);
	remove_path_transmissions(n_p);
	req->remaining_events--;
	req->phase++;
	process_request_phases(req);
	//print_network_path(n_p->path,req->pv, req->ndatacenter);

    }
    else{
	req->remaining_events--;
	check_remove_request(req);
    }

}

void  run_transmission_node_event(int v, void *vp){

    request *req = (request*)vp;
    req->remaining_events--;
    req->phase++;
    process_request_phases(req);
} 

void run_computation_event(int id, void *vp){
    
    int nlayer;
    request *req = (request*)vp;
    if(req->phase == 3 && req->reqType == 3){
	nlayer = 1;
    }
    else if(req->phase == 3 && req->reqType == 4 && req->app_fe->app->type == 2){
	nlayer = 1;
    }

    else if(req->phase == 3 && req->reqType == 4){
	nlayer = 2;
    }
    else{
	nlayer = 0;
    }
    computational_phase *c_p = req->vm_req[nlayer]->comp_phase;
    virtual_machine *vm = &req->app_fe->vm[nlayer][req->assigned_vm[nlayer]];   
    if(id == req->num_events){

	//printf("F Simtime: %ld Time event: %ld Time arrival: %ld Total instructions: %d Remaining instructions: %d Mips: %f Layer: %d Vm: %d Phase: %d\n",simtime, c_p->time_event, c_p->time_arrival, c_p->instructions, c_p->remaining_instructions, c_p->mips_assigned, nlayer, req->assigned_vm[nlayer],req->phase);
	time_proc_metric(req->pv, req->ndatacenter, simtime - c_p->time_arrival);
	req->remaining_events--;
	req->phase++;
	req->vm_req[nlayer]->active = 0;
	vm->num_active--;
	//printf("F Number of actives on layer %d: %d\n", nlayer, vm->num_active);
	modify_vm_comp_requests_depart(req, nlayer);
	process_request_phases(req);
    }
    else{
	//printf("F2 Simtime: %ld Time event: %ld Time arrival: %ld Total instructions: %d Remaining instructions: %d Mips: %f Layer: %d Vm: %d APP: %d Req: %d Id: %d Ne: %d\n",simtime, c_p->time_event, c_p->time_arrival, c_p->instructions, c_p->remaining_instructions, c_p->mips_assigned, nlayer, req->assigned_vm[c_p->nlayer],req->app_fe->app->type, req->phase, id, req->num_events);
	req->remaining_events--;
    }
}



