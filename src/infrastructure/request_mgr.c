/*
 * =====================================================================================
 *
 *       Filename:  request_processor.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/28/14 08:27:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "../workloads/requests.h"
#include "../infrastructure/balancer.h"
#include "../infrastructure/network.h"
#include "../infrastructure/vm.h"
#include "../infrastructure/broker.h"
#include "../stats/metrics.h"
#include "../infrastructure/request_mgr.h"
#include "../utils/globals.h"

void process_request(request *req){

    int vm_id;
    app_frontend *app_fe = req->app_fe; 
    vm_id = select_vm_request(app_fe, 0);
    //vm_id_r = select_vm_request(app_fe, 1);
    req->assigned_vm[0] = vm_id;
    assign_request_vm(req, 0, vm_id);
    //assign_request_vm(req, 1, vm_id_r);
    //assign_network_resources(req, 6, 0, vm_id, -1, -1);

    assign_network_resources(req, 0, vm_id, -1, -1);
    //delete_request_vm(req);
    //network_path(req->pv, req->ndatacenter, 0, 16);
}

void process_request_phases(request *req){

    int vm_id_r, vm_id_w;
    app_frontend *app_fe = req->app_fe; 

    if(req->phase == 1){
	assign_computational_resources(req,0);
    }
      
    else if(req->phase == 2){
	switch(req->reqType){
	    case 1:
		req->phase = 6;
		assign_network_resources(req, 0, req->assigned_vm[0], -1, -1);
		break;
	    case 3:
		vm_id_r = select_vm_request(app_fe, 1);
		req->assigned_vm[1] = vm_id_r;
		assign_network_resources(req, 0, req->assigned_vm[0], 1, req->assigned_vm[1]);
		break;
	    case 4:
		if(req->app_fe->app->type == 2){
		    vm_id_w = select_vm_request(app_fe, 1);
		    req->assigned_vm[1] = vm_id_w;
		    assign_network_resources(req, 0, req->assigned_vm[0], 1, req->assigned_vm[1]);
		}
		else{
		    vm_id_w = select_vm_request(app_fe, 2);
		    req->assigned_vm[2] = vm_id_w;
		    assign_network_resources(req, 0, req->assigned_vm[0], 2, req->assigned_vm[2]);
		}
		break;
	    default:
		panic("Unknown request type!\n");
		break;
	}
    }
    else if(req->phase == 3){
	switch(req->reqType){
	    case 1:
		assign_computational_resources(req,0);	
		break;
	    case 3:
		assign_request_vm(req, 1, req->assigned_vm[1]);
		assign_computational_resources(req, 1);	
		break;
	    case 4:
		if(req->app_fe->app->type == 2){
		    assign_request_vm(req, 1, req->assigned_vm[1]);
		    assign_computational_resources(req, 1);	
		
		}
		else{
		    assign_request_vm(req, 2, req->assigned_vm[2]);
		    assign_computational_resources(req, 2);	
		}
		break;
	    default:
		panic("Unknown request type!\n");
		break;
	}
    }
    else if(req->phase == 4){
	//assign_network_resources(req);
	switch(req->reqType){
	    case 3:
		assign_network_resources(req, 1, req->assigned_vm[1], 0, req->assigned_vm[0]);
		break;
	    case 4:
		if(req->app_fe->app->type == 2){
		    assign_network_resources(req, 1, req->assigned_vm[1], 0, req->assigned_vm[0]);
		}
		else{
		    assign_network_resources(req, 2, req->assigned_vm[2], 0, req->assigned_vm[0]);
		}
		break;
	    default:
		panic("Unknown request type!\n");
		break;
	}
    } 
    else if(req->phase == 5){
	assign_computational_resources(req, 0);
    }
        
    else if(req->phase == 6){
	assign_network_resources(req, 0, req->assigned_vm[0], -1, -1);
    }
    else{
	time_request_metric(req->pv, req->ndatacenter, simtime - req->time_sec);
	switch(req->reqType){
	    case 1:
		delete_request_vm(req, 0, req->assigned_vm[0]);
		break;
	    case 3:
		delete_request_vm(req, 0, req->assigned_vm[0]);
		delete_request_vm(req, 1, req->assigned_vm[1]);
		break;
	    case 4:
		if(req->app_fe->app->type == 2){
		     delete_request_vm(req, 0, req->assigned_vm[0]);
		     delete_request_vm(req, 1, req->assigned_vm[1]);
		}
		else{
		     delete_request_vm(req, 0, req->assigned_vm[0]);
		}
		break;
	    default:
		panic("Unknown request type!\n");
		break;
	}
	//if(app_fe->app->gen_requests == 100){
	    
//	printf("ENTRAMOS: %ld %d %d\n",simtime,app_fe->app->gen_requests, req->app_fe->app->last_req);
	    //req->app_fe->app->last_req = gen_requests(req->pv, req->app_fe->app->id, req->app_fe->app->type, req->app_fe->app->etime, req->app_fe, req->app_fe->app->rtime, 1, req->app_fe->app->maxreq, req->app_fe->app->sizel2, req->app_fe->app->last_req);
	//}
	num_requests_metric(req->pv,req->ndatacenter);
	check_remove_request(req);
    }
}

void check_remove_request(request *req){
    
    if(req->remaining_events == 0){
	 if(req->reqType == 4  && req->app_fe->app->type == 3){ 
	    delete_request_vm(req, 2, req->assigned_vm[2]);
	 }
	rem_request(req);
    }

}
