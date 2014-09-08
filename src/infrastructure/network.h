/*
 * =====================================================================================
 *
 *       Filename:  network.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/14 11:42:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _network
#define _network

#include "fattree.h"
#include "datacenter.h"
#include "provider.h"
#include "node.h"

typedef struct datacenter datacenter;
typedef struct network{

    int nnodes;
    struct node *n;
    int cores_node;
    long total_vcores;
    long free_vcores;
    float utilization;
    int type;
    void *str; 
    int c_rr;
    int n_rr;
} network;

int create_network(datacenter *dc, int type, int nnodes);

void remove_network(datacenter *dc);

int request_vcores_network_ff(provider *pv,int ndatacenter,int size, int *assigned_vcores, int *maxbw);

int request_vcores_network_rr(provider *pv,int ndatacenter,int size, int *assigned_vcores, int *maxbw);

int allocate_vcores_network(provider *pv,int ndatacenter,int size, int *assigned_vcores, int id, int *maxbw);

int release_vcores_network(provider *pv,int ndatacenter,int size, int *assigned_vcores, int *maxbw);

void print_network_status(provider *pv,int node_a, int node_b);

int distance_network_cores(network *n,int src,int dst);

int distance_network_nodes(network *n,int src,int dst);

void* network_path(provider *pv,int ndatacenter, int vm_src, int vm_dst);

void reserve_network_path(provider *pv,int ndatacenter, void *path, network_phase *n_p);

void print_network_path(void *path, provider *pv, int ndatacenter);

long on_servers_allocation(network *nw, int *core_set, int size);

long on_servers_total(network *nw, int *core_set, int size);
#endif
