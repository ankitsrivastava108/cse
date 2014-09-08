/*
 * =====================================================================================
 *
 *       Filename:  datacenter.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/14 09:54:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "provider.h"
#include "network.h"
#include "balancer.h"
#include "../stats/log.h"
#include "../workloads/application.h"
#include "../utils/globals.h"
#include "../optimization/opt_engine.h"
#include "../evaluation/functions.h"

int create_datacenter(provider *pv, int type, int nnodes){
    
    int i;

    pv->datacenters = malloc(sizeof(datacenter)*pv->ndc);
   
    for(i=0;i<pv->ndc;i++){    
      	pv->datacenters[i].type = type;
     	pv->datacenters[i].nnodes = nnodes;
     	pv->datacenters[i].allocation_policy = allocation_policy;
     	pv->datacenters[i].optimization_alg = optimization_alg;
	pv->datacenters[i].alloc_apps = malloc(sizeof(allocated_applications));
	pv->datacenters[i].alloc_apps->num_alloc_apps = 0;
	pv->datacenters[i].alloc_apps->first_app = NULL;
	pv->datacenters[i].alloc_apps->last_app = NULL;
	create_network(&pv->datacenters[i],type,nnodes);
    }
    return(1);
     
}

void remove_datacenter(provider *pv){

    int i;

    for(i=0;i<pv->ndc;i++){
	remove_network(&pv->datacenters[i]);
	if(pv->datacenters[i].alloc_apps->num_alloc_apps == 0){
	    free(pv->datacenters[i].alloc_apps);
	}
	else{
	    //printf("datacenter: apps!!");
	}
    }
    free(pv->datacenters);
}

int request_vcores_datacenter(provider *pv, int ndatacenter, event *ev){
    
    int pattern=1;
    int res, size;
    float hv = 0.0;
    long f1 = 0;
    long f2 = 0;
    long ptotal = 0;
    long pmin = 0;
    application *app = NULL;
    app_frontend *app_fe;
    size = ev->sizel1 + ev->sizel2 + ev->sizel3;
    int *assigned_vcores = malloc(sizeof(int)*size);
    init_application(pv, &app, ev);
    switch(pv->datacenters[ndatacenter].allocation_policy){
	case FF: 
	    res = request_vcores_network_ff(pv,ndatacenter,size,assigned_vcores, app->str.maxbw);
	    break;
	case RR: 
	    res = request_vcores_network_rr(pv,ndatacenter,size,assigned_vcores, app->str.maxbw);
	    break;

	default:
	    panic("No policy selected");
    }
    if(res == 1){
	switch(pv->datacenters[ndatacenter].optimization_alg){
	    case NONE:
		allocate_vcores_network(pv, ndatacenter, size, assigned_vcores, app->id, app->str.maxbw);
		break;
	    case NSGA2:
	    case SPEA2:
	    case HYPE:
	    case SHV:
		    hv = optimize(pv->datacenters[ndatacenter].optimization_alg, alpha, mu, lambda, dim, tournament, ngen, mtype, cotype, pmut, pmuttopo, pco, pcotopo, size, assigned_vcores, pv, ndatacenter, app);
		allocate_vcores_network(pv, ndatacenter, size, assigned_vcores, app->id, app->str.maxbw);
		break;
	    default:
		panic("datacenter: no optimization algorithm");
	}
	f1 = eval_cores_f1(assigned_vcores, size, pv->datacenters[ndatacenter].nw, app->str.bw);
	f2 = eval_cores_f2(assigned_vcores, size, pv->datacenters[ndatacenter].nw);
	ptotal = on_servers_total(pv->datacenters[ndatacenter].nw, assigned_vcores, size);
	pmin = eval_pmin_constrained(size, app->str.maxbw);
	f1_metric(pv, ndatacenter, f1);
	f2_metric(pv, ndatacenter, f2);
	ptotal_metric(pv, ndatacenter, ptotal); 
	pmin_metric(pv, ndatacenter, pmin);
	hv_metric(pv, ndatacenter, hv);
	
	//write_opt(f1,f2,ptotal);
	change_assigned_cores(app, assigned_vcores);	
	//printApplication(app);
	app_fe = add_app_lb(pv, app);
	change_app_fe(app, app_fe);
	allocate_application(pv, ndatacenter, app);
	if(ev->requests){
	    gen_requests(pv, app->id, app->type, app->etime, app_fe, app->rtime, pattern, app->maxreq, app->req_chunk,0,0);
	}
	num_applications_metric(pv, ndatacenter);
    }
    else{

	free(assigned_vcores);
	rem_application(app);
    }
    
    return(res);    
}

void release_application_datacenter(provider *pv, int ndatacenter, int id){

    int i=0;
    int END=0;
    long pmin = 0;
    application *app_aux=pv->datacenters[ndatacenter].alloc_apps->first_app; 

    while(!END && app_aux!=NULL && i<pv->datacenters[ndatacenter].alloc_apps->num_alloc_apps){
	if(app_aux->id==id){
	     pv->datacenters[ndatacenter].alloc_apps->num_alloc_apps--;
	     
	     if(app_aux->prev==NULL && app_aux->next==NULL){
		 pv->datacenters[ndatacenter].alloc_apps->first_app=NULL;
		 pv->datacenters[ndatacenter].alloc_apps->last_app=NULL;
	     }
	     else if(app_aux->prev==NULL){
		pv->datacenters[ndatacenter].alloc_apps->first_app=app_aux->next;	
		app_aux->next->prev=NULL;
	     }
	     else if(app_aux->next==NULL){
		pv->datacenters[ndatacenter].alloc_apps->last_app=app_aux->prev;
		app_aux->prev->next=NULL;
		}
	     else{
		    app_aux->prev->next=app_aux->next;
		    app_aux->next->prev=app_aux->prev;
		}
	    
	     pmin = eval_pmin_constrained(app_aux->size, app_aux->str.maxbw);
	     pmin_metric(pv, ndatacenter, -pmin);
     	     release_vcores_network(pv,ndatacenter,app_aux->size, app_aux->assigned_vcores, app_aux->str.maxbw);
	     rem_app_lb(pv, app_aux->app_fe);
	     rem_application(app_aux);
	     END=1;
	}
	else{
	    app_aux=app_aux->next;
	}
	i++;
    }
    utilization_metric(pv,ndatacenter);
}

void allocate_application(provider *pv, int ndatacenter, application *app){
     
    if(pv->datacenters[ndatacenter].alloc_apps->first_app==NULL){
	
	pv->datacenters[ndatacenter].alloc_apps->num_alloc_apps=1;
	pv->datacenters[ndatacenter].alloc_apps->first_app=app;
	pv->datacenters[ndatacenter].alloc_apps->last_app=app;

    }
    else{

	pv->datacenters[ndatacenter].alloc_apps->num_alloc_apps++;
	app->prev=pv->datacenters[ndatacenter].alloc_apps->last_app;
	pv->datacenters[ndatacenter].alloc_apps->last_app->next=app;
	pv->datacenters[ndatacenter].alloc_apps->last_app=app;

    }
    utilization_metric(pv,ndatacenter);
//    print_network_status(pv,0,0); 
}

