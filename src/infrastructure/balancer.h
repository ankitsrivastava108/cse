/*
 * =====================================================================================
 *
 *       Filename:  balancer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/19/14 08:53:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _balancer
#define _balancer

#include "../workloads/application.h"
#include "../workloads/requests.h"

typedef struct virtual_machine virtual_machine; 

typedef struct load_balancer{

    int app_num;
    struct app_frontend *first;    
    struct app_frontend *last;    

}load_balancer;

typedef struct app_frontend{

    int active;
    provider *pv;
    int id;
    int num_nodes_front_end;
    int rr;
    virtual_machine **vm;
    struct application *app;
    struct app_frontend *next;
    struct app_frontend *prev;

}app_frontend;

void create_loadbalancer(provider *pv);

void remove_loadbalancer(provider *pv);

app_frontend* add_app_lb(provider *pv, struct application *app);

void rem_app_lb(provider *pv, app_frontend* app_fe);

int select_vm_request(app_frontend *app_fe, int nlayer);
#endif 
