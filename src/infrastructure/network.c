/*
 * =====================================================================================
 *
 *       Filename:  network.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/11/14 11:53:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "datacenter.h"
#include "network.h"
#include "fattree.h"
#include "../utils/misc.h"
#include "../utils/globals.h"

int create_network(datacenter *dc, int type, int nnodes){
    
    int i;
    void **str; 

    dc->nw=malloc(sizeof(network));
    dc->nw->nnodes=nnodes;
    dc->nw->cores_node=nvcores;
    dc->nw->free_vcores=nvcores*nnodes;
    dc->nw->total_vcores=nvcores*nnodes;
    dc->nw->n_rr = 0;
    dc->nw->c_rr = 0;
    dc->nw->utilization=0.0;
    dc->nw->type=type;
    dc->nw->n=malloc(sizeof(node)*nnodes);
  
    for(i = 0;i < nnodes;i++){
	init_node(&dc->nw->n[i],i);
    }
    
    str = &dc->nw->str;
    switch(type){
	case FATTREE:
	   // str_ft* str = (str_ft*)dc->nw->str;
	    create_network_fattree((str_ft**)str);
	    break;
	default:
	    panic("Unknown network type");
    }
   //print_network_str(dc->nw->str);
    return(1);
}

void remove_network(datacenter *dc){
    
    int i;

    switch(dc->nw->type){
	case FATTREE:
	    remove_network_fattree(dc->nw->str);
	    free(dc->nw->str);
	    break;
	default:
	    panic("Unknown network type");
    }


    for(i = 0;i < dc->nw->nnodes;i++){
	remove_node(&dc->nw->n[i]);
    }
    free(dc->nw->n);
    free(dc->nw);
}

int request_vcores_network_ff(provider *pv,int ndatacenter,int size, int *assigned_vcores, int *maxbw){

    int i=0;
    int	j;
    int k=0;
    int free_vcores=0;
    int res=0;
    int vcores_aux;
    float bw_aux;
    int END=0;
    while(!END && i<pv->datacenters[ndatacenter].nnodes){
	vcores_aux=pv->datacenters[ndatacenter].nw->n[i].nvcores;
	bw_aux = pv->datacenters[ndatacenter].nw->n[i].freebw;
	for(j=0;j<vcores_aux;j++){
	    if(pv->datacenters[ndatacenter].nw->n[i].vcores[j]==-1 && bw_aux >= maxbw[k]){
		free_vcores++;
		assigned_vcores[k]=(vcores_aux*i)+j;
		bw_aux -= maxbw[k];
		//printf("%d-",assigned_vcores[k]);
		k++;
		if(k==size){
		    res=1;
		    END=1;
		    break;
		}
	    }
	}
	i++;
    }
    //printf("\n");
    return(res);        
}

int request_vcores_network_rr(provider *pv,int ndatacenter,int size, int *assigned_vcores, int *maxbw){

    int i = 0;
    int	j;
    int k = 0;
    int free_vcores = 0;
    int res = 0;
    int vcores_aux,core_rr, node_rr;
    float bw_aux;
    int END = 0;
    int nnodes = pv->datacenters[ndatacenter].nnodes;

    node_rr = pv->datacenters[ndatacenter].nw->n_rr;
    while(!END && i < nnodes){
	core_rr = pv->datacenters[ndatacenter].nw->c_rr;
	pv->datacenters[ndatacenter].nw->c_rr = 0;
	vcores_aux = pv->datacenters[ndatacenter].nw->n[node_rr].nvcores;
	bw_aux = pv->datacenters[ndatacenter].nw->n[i].freebw;
	for(j = core_rr;j < vcores_aux;j++){
	    if(pv->datacenters[ndatacenter].nw->n[node_rr].vcores[j]==-1 && bw_aux >= maxbw[k]){
		free_vcores++;
		assigned_vcores[k]=(vcores_aux * node_rr)+j;
		bw_aux -= maxbw[k];
		k++;
		if(k==size){
		    pv->datacenters[ndatacenter].nw->c_rr = j + 1; 
		    pv->datacenters[ndatacenter].nw->n_rr = node_rr; 
		    res=1;
		    END=1;
		    break;
		}
	    }
	}
	node_rr = ((node_rr + 1) % nnodes);
	i++;
    }
    //printf("\n");
    return(res);        
}


int allocate_vcores_network(provider *pv,int ndatacenter,int size, int *assigned_vcores, int id, int *maxbw){

    int i,j,k;
    int res=0;
    int vcores_aux=pv->datacenters[ndatacenter].nw->n[0].nvcores;
    
    pv->datacenters[ndatacenter].nw->free_vcores -= size;
    for(i=0;i<size;i++){
	j=assigned_vcores[i]/vcores_aux;
	k=assigned_vcores[i]%vcores_aux;
	if(pv->datacenters[ndatacenter].nw->n[j].busyvcores == 0){
	    num_servers_on_metric(pv, ndatacenter, 1);
	}
	pv->datacenters[ndatacenter].nw->n[j].vcores[k] = id;
	pv->datacenters[ndatacenter].nw->n[j].freevcores--;
	pv->datacenters[ndatacenter].nw->n[j].busyvcores++;
	pv->datacenters[ndatacenter].nw->n[j].energy_event = simtime;;
	pv->datacenters[ndatacenter].nw->n[j].freebw -= maxbw[i];
	pv->datacenters[ndatacenter].nw->n[j].usedbw += maxbw[i];
    }
    num_busy_vcores_metric(pv, ndatacenter, size);	
    return(res);        
}

int release_vcores_network(provider *pv,int ndatacenter,int size, int *assigned_vcores, int *maxbw){

    int i,j,k;
    int res=0;
    int vcores_aux=pv->datacenters[ndatacenter].nw->n[0].nvcores;

    pv->datacenters[ndatacenter].nw->free_vcores += size;
    for(i=0;i<size;i++){
	j=assigned_vcores[i]/vcores_aux;
	k=assigned_vcores[i]%vcores_aux;
	pv->datacenters[ndatacenter].nw->n[j].vcores[k]=-1;
	pv->datacenters[ndatacenter].nw->n[j].freevcores++;
	pv->datacenters[ndatacenter].nw->n[j].busyvcores--;
	pv->datacenters[ndatacenter].nw->n[j].energy_event = simtime;;
	pv->datacenters[ndatacenter].nw->n[j].freebw += maxbw[i];
	pv->datacenters[ndatacenter].nw->n[j].usedbw -= maxbw[i];

	if(pv->datacenters[ndatacenter].nw->n[j].busyvcores == 0){
	    num_servers_on_metric(pv, ndatacenter, -1);
	}
    }
    num_busy_vcores_metric(pv, ndatacenter, -size);	
    return(res);        
}

void print_network_status(provider *pv,int node_a, int node_b){
    
    int i,j;

    for(i=0;i<pv->datacenters[0].nw->nnodes;i++){
	for(j=0;j<pv->datacenters[0].nw[0].n[i].nvcores;j++){

	    printf("%d ", pv->datacenters[0].nw[0].n[i].vcores[j]);

	}
	printf(" | ");
    }
    printf("\n");
}

int distance_network_cores(network *nw, int src,int dst){
    
    
    int dist=0;

     str_ft *str = nw->str;
     switch(nw->type){
	case FATTREE:
	    dist = distance_fattree_cores(str, src, dst, nw->cores_node);
	    break;
	default:
	    panic("Unknown network type (distance)");
    }
       
    return(dist);

}

int distance_network_nodes(network *nw, int src,int dst){

    int dist=0;

    str_ft *str = nw->str;
    switch(nw->type){
	case FATTREE:
	    dist = distance_fattree_nodes(str, src, dst);
	    break;
	default:
	    panic("Unknown network type (distance)");
    }
       
    return(dist);

}

void* network_path(provider *pv,int ndatacenter, int vm_src, int vm_dst){

    //str_ft *str;
    void *str;
    void *path;

    switch(pv->datacenters[ndatacenter].nw->type){
	case FATTREE:
	    str = (str_ft*)pv->datacenters[ndatacenter].nw->str;
	    path = (fattree_nw_path*)network_path_fattree(str, vm_src, vm_dst, pv->datacenters[ndatacenter].nw->cores_node);
	    break;
	default:
	     panic("Unknown network type (network path)");
    }

    return( (fattree_nw_path*)path);
}

void reserve_network_path(provider *pv,int ndatacenter, void *path, network_phase *n_p){

    switch(pv->datacenters[ndatacenter].nw->type){
	case FATTREE:
	   reserve_network_path_fattree((fattree_nw_path*)path, n_p);
	    break;
	default:
	     panic("Unknown network type (network path)");
    }
}

void print_network_path(void *path, provider *pv, int ndatacenter){

    int i;

    switch(pv->datacenters[ndatacenter].nw->type){
	case FATTREE:
	    for(i = 0; i < ((fattree_nw_path*)(path))->length; i++){
		printf("(%d,%d,%d,%d) *%f t%d\n", ((fattree_nw_path*)path)->path[i]->tier, ((fattree_nw_path*)path)->path[i]->switch_id, ((fattree_nw_path*)path)->path[i]->direction, ((fattree_nw_path*)path)->path[i]->switch_port, ((fattree_nw_path*)path)->path[i]->used_bw, ((fattree_nw_path*)path)->path[i]->trans->num_transmissions);
    }
	    break;
	default:
	    panic("Unknown network type (print network path)");
    }
}

long on_servers_allocation(network *nw, int *core_set, int size){

    int i, node_core;
    long nodes = 0;
    int *nodes_aux = malloc(sizeof(int) * nw->nnodes); 

    for(i = 0; i < nw->nnodes; i++){
	nodes_aux[i] = 0;
    }
    for(i = 0; i < size; i++){
	node_core = core_set[i] / nw->cores_node; 	
	if(nodes_aux[node_core] == 0){
	    nodes_aux[node_core] = 1;
	    nodes++;
	}
    }
    free(nodes_aux);
    return(nodes);
}

long on_servers_total(network *nw, int *core_set, int size){

    int i, node_core;
    long nodes = 0;
    int *nodes_aux = malloc(sizeof(int) * nw->nnodes); 

    for(i = 0; i < nw->nnodes; i++){
	if(nw->n[i].busyvcores == 0){
	    nodes_aux[i] = 0;
	}
	else{
	    nodes_aux[i] = 1;
	    nodes++;
	}
    }
    for(i = 0; i < size; i++){
	node_core = core_set[i] / nw->cores_node; 	
	if(nodes_aux[node_core] == 0){
	    nodes_aux[node_core] = 1;
	    nodes++;
	}
    }
    free(nodes_aux);
    return(nodes);
}








