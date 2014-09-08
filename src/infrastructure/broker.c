/*
 * =====================================================================================
 *
 *       Filename:  broker.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/25/14 11:18:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "provider.h"
#include "datacenter.h"
#include "broker.h"
#include "../evengine/calendar.h"

int request_vcores(provider *pv, event *ev){

    int resp;
    int ndatacenter=0;
    resp = request_vcores_datacenter(pv, ndatacenter, ev);

    return(resp);
}

void release_vcores(provider *pv,int ndatacenter,int id){

    release_application_datacenter(pv, ndatacenter, id);
}

void create_broker(provider *pv) {

    pv->brk = malloc(sizeof(struct broker));
    pv->brk->list_requests = malloc(sizeof(requests));
    pv->brk->list_requests->num_requests = 0;

}

void remove_broker(provider *pv){

    free(pv->brk->list_requests);
    free(pv->brk);

}
void add_request(provider *pv, request *req) {

    if(pv->brk->list_requests->num_requests == 0){
	req->next = NULL;
	req->prev = NULL;
	pv->brk->list_requests->first_request = req;
	pv->brk->list_requests->last_request = req;
    }
    else{
	req->prev = pv->brk->list_requests->last_request;
	pv->brk->list_requests->last_request->next = req;			    
	pv->brk->list_requests->last_request = req;
    }
    pv->brk->list_requests->num_requests++;
    insert_event(req->time_sec,REQUEST_CALENDAR,0,req);
}

void rem_request(request *req){ 
    
    provider *pv = req->pv;

    if(pv->brk->list_requests->num_requests == 1){
        pv->brk->list_requests->first_request = NULL;
        pv->brk->list_requests->last_request = NULL;
    }
    else if(req->prev == NULL){
	req->next->prev = NULL;
	pv->brk->list_requests->first_request = req->next;
    }
    else if(req->next == NULL){
	req->prev->next = NULL;
	pv->brk->list_requests->last_request =req->prev;
    }
    else{
	req->prev->next = req->next;
	req->next->prev = req->prev;
    }
    //free(req->data);
    //free(req->assigned_vm);
    //free(req->vm_req);
    free(req);
    pv->brk->list_requests->num_requests--;
}

