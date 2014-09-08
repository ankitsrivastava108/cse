/*
 * =====================================================================================
 *
 *       Filename:  broker.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/14 11:42:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _broker
#define _broker

#include "provider.h"
#include "../workloads/requests.h" 

typedef struct requests {

    int num_requests;
    struct request *first_request;
    struct request *last_request; 

} requests;

typedef struct broker{
    
    struct requests *list_requests;
    

}broker;

int request_vcores(provider *pv, event *ev);

void release_vcores(provider *pv,int ndatacenter, int id);

void create_broker(provider *pv);

void remove_broker(provider *pv);

void add_request(provider *pv, request *req); 

void rem_request(request *req); 

#endif

