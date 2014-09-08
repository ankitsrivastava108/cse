/*
 * =====================================================================================
 *
 *       Filename:  balancer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/19/14 08:53:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "balancer.h"
#include "network.h"
#include "vm.h"
#include "../stats/metrics.h"
#include "../utils/rand.h"

void create_loadbalancer(provider *pv){

    pv->lb = malloc(sizeof(load_balancer));    
    pv->lb->app_num = 0;
    pv->lb->first = NULL;
    pv->lb->last = NULL;

}

void remove_loadbalancer(provider *pv){

    free(pv->lb);
}
app_frontend* add_app_lb(provider *pv, struct application *app){

    app_frontend *app_fe = malloc(sizeof(app_frontend));
    app_fe->active = 1;
    app_fe->id= app->id;
    app_fe->pv = pv;
    //app_fe->num_nodes_front_end = app->str.nodeslayer[0];
    //app_fe->num_nodes_read = app->str.nodeslayer[1];
    //app_fe->num_nodes_write = app->str.nodeslayer[2];
    app_fe->app = app;  
    app_fe->rr = 0; 
    app_fe->vm = init_vm(app);
    app_fe->next = NULL;
    app_fe->prev = NULL;
    
    if( pv->lb->app_num == 0){
	pv->lb->first = app_fe;
	pv->lb->last = app_fe;
    }
    else{
	pv->lb->last->next = app_fe;
	app_fe->prev =  pv->lb->last;
	pv->lb->last = app_fe;
    }
    pv->lb->app_num++;
    return(app_fe);
}

void rem_app_lb(provider *pv, app_frontend* app_fe){

    int i;

    for(i = 0;i < app_fe->app->str.nlayers; i++){
	free(app_fe->vm[i]);
    }
   free(app_fe->vm); 
   app_fe->active = 0;
         
   if(pv->lb->app_num == 1){
        pv->lb->first = NULL;
        pv->lb->last = NULL;
   }
   else if(app_fe->prev == NULL){
	app_fe->next->prev = NULL;
	 pv->lb->first = app_fe->next;
   }
    else if(app_fe->next == NULL){
	 app_fe->prev->next = NULL;
	 pv->lb->last =app_fe->prev;
    }
    else{
	app_fe->prev->next = app_fe->next;
	app_fe->next->prev = app_fe->prev;
    }
   free(app_fe);
      
   pv->lb->app_num--;

}

int select_vm_request(app_frontend *app_fe, int nlayer){

    int id = 0;
    int id_aux = 0;
    int min_req = MAX_REQUESTS + 1;

    switch(app_fe->app->vm_request_policy){
	case 0:
	    id = rnd(0, app_fe->app->str.nodeslayer[nlayer] - 1);
	    while( app_fe->vm[nlayer][id].num_requests == MAX_REQUESTS){
		id = rnd(0, app_fe->app->str.nodeslayer[nlayer] - 1);
		printf("%d \n",app_fe->vm[nlayer][id].num_requests);
	    }
	    break;
	case 1:
	    while(id < app_fe->app->str.nodeslayer[nlayer] && app_fe->vm[nlayer][id].num_requests == MAX_REQUESTS){
		id++;
	    }
	    if(id== app_fe->app->str.nodeslayer[nlayer]){
		panic("All VMs saturated!\n");
	    }
	    break;
	case 2:
	    id_aux = 0;
	    id = app_fe->rr;
	    while(id_aux < app_fe->app->str.nodeslayer[nlayer] && app_fe->vm[nlayer][id].num_requests == MAX_REQUESTS){
		id++;
		id %= app_fe->app->str.nodeslayer[nlayer];
		id_aux++;
	    }
	    if(id_aux == app_fe->app->str.nodeslayer[nlayer]){
		panic("All VMs saturated!\n");
	    }
	    app_fe->rr = ((id+1)%app_fe->app->str.nodeslayer[nlayer]);
	    break;
	case 3:
	    id = -1;
	    for(id_aux=0; id_aux < app_fe->app->str.nodeslayer[nlayer]; id_aux++){
		if(app_fe->vm[nlayer][id_aux].num_requests != MAX_REQUESTS && app_fe->vm[nlayer][id_aux].num_requests < min_req){
		    min_req = app_fe->vm[nlayer][id_aux].num_requests;
		    id = id_aux;
		}
	    }
	    if(id == -1){
		panic("All VMs saturated!\n");
	    }
	    break;
	default:
	    panic("No vm request policy\n");
    }

    return(id);
}






