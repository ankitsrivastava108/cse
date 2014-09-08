/*
 * =====================================================================================
 *
 *       Filename:  datacenter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/14 11:43:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _datacenter
#define _datacenter

#include "provider.h"
#include "network.h"
#include "../workloads/application.h"

typedef struct datacenter{

    struct network *nw;
    int type;
    int nnodes;
    int allocation_policy;
    int optimization_alg;
    struct allocated_applications *alloc_apps;         

} datacenter;

int create_datacenter(provider *pv, int type, int nnodes);

void remove_datacenter(provider *pv);

int request_vcores_datacenter(provider *pv, int ndatacenter, event *ev);

void release_application_datacenter(provider *pv, int ndatacenter, int id);

void  allocate_application(provider *pv, int ndatacenter, application *app);

#endif

