/*
 * =====================================================================================
 *
 *       Filename:  fattree.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/14 11:43:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _fattree
#define _fattree

#include "vm.h"

#define NLEVELS 3
#define PORTS_UP 8 
#define PORTS_DOWN 8 
#define BW 1000
#define NUMPORTS 12 


typedef struct network network;
typedef struct switch_ft switch_ft;

typedef struct transmission{

     network_phase *n_p;
     struct transmission *next;
     struct transmission *prev;

}transmission;

typedef struct transmissions{
    
    int num_transmissions;
    struct transmission *first;
    struct transmission *last;
    
}transmissions;

typedef struct port_switch_ft{

    int connected_tier;
    int connected_switch_id;
    int connected_switch_port;
    int tier;
    int switch_id;
    int switch_port;
    int direction;
    transmissions *trans;
    int total_bw;
    float used_bw;
    float free_bw;
    switch_ft *sw;

}port_switch_ft;

typedef struct switch_ft{
    
    int speed;
    int num_ports;
    int ports_up;
    int ports_down;
    int used_ports;
    long energy_event;
    port_switch_ft **ports;
    
}switch_ft;

typedef struct str_ft{

    int nlevels;
    int num_switches;
    int ports_up;
    int ports_down;
    int num_ports;
    int request_path_policy;
    switch_ft **str;

}str_ft;

typedef struct fattree_nw_path{

    int length;
    float maxbw;
    port_switch_ft **path;

}fattree_nw_path;

int create_network_fattree(str_ft **str);

void remove_network_fattree(str_ft *str);

int distance_fattree_cores(str_ft *str, int src, int dst, int cores_node);

int distance_fattree_nodes(str_ft *str, int src, int dst);

int get_node(int core, int cores_node);

int get_switch(int node, int ports_down);

int get_switch_port(int core, int ports_down, int ports_up);

void print_network_str(str_ft *str);

float switch_ft_bw(port_switch_ft *p_s_ft);

fattree_nw_path* network_path_fattree(str_ft *str, int vm_src, int vm_dst, int cores_node);

void reserve_network_path_fattree(fattree_nw_path* path, network_phase *n_p);

transmissions* init_transmissions();

void remove_transmissions(transmissions* trans);

void add_transmission(port_switch_ft *path, network_phase *n_p);

void remove_transmission(port_switch_ft *path, network_phase *n_p);

void remove_path_transmissions(network_phase *n_p);

void update_path_ft_bw(fattree_nw_path* path, float old_bw, float new_bw);
#endif


