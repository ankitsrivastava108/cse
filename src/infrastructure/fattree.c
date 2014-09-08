/*
 * =====================================================================================
 *
 *       Filename:  fattree.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/12/14 14:10:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#include "network.h"
#include "node.h"
#include "../utils/globals.h"
#include "../utils/rand.h"
#include"../models/energy_models.h"

#include <math.h>

int create_network_fattree(str_ft **str){
    
    int i, j, k, l, n_s, aux_l, aux_s, aux_p;
    
    n_s =  pow(PORTS_UP,(NLEVELS - 1));
    (*str) = malloc(sizeof(str_ft));
    (*str)->nlevels = NLEVELS;
    (*str)->ports_up = PORTS_UP;
    (*str)->ports_down = PORTS_DOWN;
    (*str)->num_ports = NUMPORTS;
    (*str)->num_switches= n_s;
    (*str)->request_path_policy = 0;
    (*str)->str = malloc(sizeof(switch_ft*) * NLEVELS);
    for(i = 0; i < NLEVELS; i++){
	(*str)->str[i] = malloc(sizeof(switch_ft) * n_s); 
	for(k = 0; k < n_s; k++){
	    (*str)->str[i][k].speed = BW;
	    (*str)->str[i][k].num_ports = NUMPORTS;
	    (*str)->str[i][k].ports_up = PORTS_UP;
	    (*str)->str[i][k].ports_down = PORTS_DOWN;
	    (*str)->str[i][k].used_ports = 0;
	    (*str)->str[i][k].energy_event = 0;
	    (*str)->str[i][k].ports = malloc(sizeof(port_switch_ft*) * 2);
	    for(l=0; l < 2; l++){
		(*str)->str[i][k].ports[l] = malloc(sizeof(port_switch_ft) * NUMPORTS);
		for(j = 0; j < NUMPORTS; j++){
		    (*str)->str[i][k].ports[l][j].trans = init_transmissions();
		    //printf("%d\n", (*str)->str[i][k].ports[l][j].trans->num_transmissions);
		    (*str)->str[i][k].ports[l][j].trans->num_transmissions = 0;   
		    (*str)->str[i][k].ports[l][j].tier = i;   
		    (*str)->str[i][k].ports[l][j].switch_id = k;   
		    (*str)->str[i][k].ports[l][j].direction = l;   
		    (*str)->str[i][k].ports[l][j].switch_port = j;   
		    (*str)->str[i][k].ports[l][j].total_bw = BW;   
		    (*str)->str[i][k].ports[l][j].free_bw = BW;   
		    (*str)->str[i][k].ports[l][j].used_bw = 0;   
		    (*str)->str[i][k].ports[l][j].sw = &(*str)->str[i][k];
		    if(i == 0 && j >= PORTS_UP){
			(*str)->str[i][k].ports[l][j].connected_tier = -1;   
			(*str)->str[i][k].ports[l][j].connected_switch_id = -1;   
			(*str)->str[i][k].ports[l][j].connected_switch_port = (k * PORTS_DOWN) + (j % PORTS_DOWN);   
		    }
		    else if(i == NLEVELS - 1 && j < PORTS_UP){
			(*str)->str[i][k].ports[l][j].connected_tier = NLEVELS ;  
			(*str)->str[i][k].ports[l][j].connected_switch_id = -1;   
			(*str)->str[i][k].ports[l][j].connected_switch_port = -1;   
		    }  
		    else if(j >= PORTS_UP){
			aux_l = i - 1;
			(*str)->str[i][k].ports[l][j].connected_tier = aux_l;  
			aux_s =  ((k % (int)pow(PORTS_UP, NLEVELS - i )) - ((k % (int)pow(PORTS_UP, NLEVELS - i )) % (int)pow(PORTS_UP, NLEVELS - i - 1)))  + ((j % PORTS_UP) * (int)pow(PORTS_UP, i-1));
			(*str)->str[i][k].ports[l][j].connected_switch_id = aux_s; 
			aux_p =  ((int)(floor(k) / (int)pow(PORTS_UP, i - 1)) % ((int)pow(PORTS_UP, i )));
			(*str)->str[i][k].ports[l][j].connected_switch_port = aux_p; 
			(*str)->str[i][k].ports[l][j].total_bw = BW;   
			(*str)->str[i][k].ports[l][j].used_bw = 0;   
			(*str)->str[aux_l][aux_s].ports[l][aux_p].connected_tier = i;
			(*str)->str[aux_l][aux_s].ports[l][aux_p].connected_switch_id = k;
			(*str)->str[aux_l][aux_s].ports[l][aux_p].connected_switch_port = j;
		    }
		}
	    }
	}
    }	
    return(1);
}

void remove_network_fattree(str_ft *str){

    int i,j,k, l, n_s;
    n_s =  pow(PORTS_UP,(NLEVELS - 1));

     for(i = 0; i < NLEVELS; i++){
	 for(k = 0; k < n_s; k++){ 
	      for(j=0; j < 2; j++){
		  for(l =0; l< NUMPORTS; l++){
		    remove_transmissions(str->str[i][k].ports[j][l].trans);
		  }
		  free(str->str[i][k].ports[j]);
	      }
	      free(str->str[i][k].ports);
	 }
	 free(str->str[i]);
     }
     free(str->str);
}

int distance_fattree_cores(str_ft *str, int src, int dst, int cores_node){
            
    int i,dist;
    int srcaux = src / cores_node;
    int dstaux = dst / cores_node;

    dist = 0;
    if(src == -1 || dst == -1){
	dist = str->nlevels;
    }
    else{
	for(i = 0;i <= str->nlevels;i++){
	    if(srcaux / (int)pow(str->ports_up,i) == dstaux / (int)pow(str->ports_up,i)){
		dist = i*2;
		break;
	    }
	}
    }
    return(dist);
}

int distance_fattree_nodes(str_ft *str, int src, int dst){
 
    int i,dist;
    int srcaux = src;
    int dstaux = dst;

    dist = 0;
    for(i = 0;i <= str->nlevels;i++){
        if(srcaux/(int)pow(str->ports_up,i) == dstaux/(int)pow(str->ports_up,i)){
            dist = i*2;
        }
    }
    return(dist);
}           

int get_node(int core, int cores_node){
    
    return((int)core / cores_node);
}

int get_switch(int node, int ports_down){

    return((int)node / ports_down);
    
}

int get_switch_port(int node, int ports_down, int ports_up){

    return((node % ports_down) + ports_up);
} 

void print_network_str(str_ft *str){
    
    int i, j, k, l, n_s;

    n_s =  pow(PORTS_UP,(NLEVELS - 1));
    for(i = 0; i < NLEVELS; i++){
	for(k = 0; k < n_s; k++){
	    for(l = 0; l < 2; l++){
		for(j = 0; j < NUMPORTS; j++){
		    printf("(%d %d %d %d) - (%d %d %d)\n",i,k,l,j,str->str[i][k].ports[l][j].connected_tier,str->str[i][k].ports[l][j].connected_switch_id,str->str[i][k].ports[l][j].connected_switch_port); 	
	    
		    }
	    }
	    printf("\n");
	}
    printf("\n");
    }	    
}

fattree_nw_path*  network_path_fattree(str_ft *str, int core_src, int core_dst, int cores_node){
    
    int i, i_aux, j;
    float bw_path = BW + 1;
    int sw_aux = 0;
    float bw = 0;
    float max_bw = 0;
    int level_id = 0;
    int switch_id = 0;
    int port_id = 0;
    int level_init = 0;
    int mean_path = 0;
    int dir = 0;
    int dist = distance_fattree_cores(str, core_src, core_dst, cores_node);

    if(dist == 0){
	return(NULL);
    }
    
    fattree_nw_path *path_ft = malloc(sizeof(fattree_nw_path));
    
    if(core_src == -1){
	path_ft->length = dist * 2; 
	level_id = str->nlevels - 1;
	switch_id = rnd(0, str->num_switches-1);
	port_id = rnd(0, str->ports_up-1);
	mean_path = path_ft->length;
	level_init = mean_path;
	dir = 0;
    }
    else if(core_dst == -1){
	path_ft->length = dist * 2;
	mean_path = path_ft->length;
	dir = 1;
    }
    else{
	path_ft->length = (2 * dist) - 2;
	mean_path = path_ft->length / 2;
	dir = 1;
    }
    path_ft->path = malloc(sizeof(port_switch_ft*) * path_ft->length);

    switch(str->request_path_policy){
	case 0:
	    for(i = level_init;i <  (level_init + path_ft->length); i++){
		i_aux = i - level_init;
		if(i == 0){
		    switch_id = get_switch(get_node(core_src, cores_node), str->ports_down);
		    port_id = get_switch_port(get_node(core_src, cores_node), str->ports_down, str->ports_up);
		    //printf("%d %d %d %d\n", i,level_id, switch_id, port_id);
		    path_ft->path[i_aux] = &(str->str[level_id][switch_id].ports[dir][port_id]);
		}
		else if( (i < mean_path)   && (i % 2 == 1) ){
		    max_bw = 0;
		    for(j = 0; j < str->ports_up; j++){
			bw = switch_ft_bw(&str->str[level_id][switch_id].ports[dir][j]);
			if(bw > max_bw){
			    path_ft->path[i_aux] =  &(str->str[level_id][switch_id].ports[dir][j]);
			   max_bw = bw;
			}
		    }
		    //printf("%d %d %d %d\n", i,level_id, switch_id, j_aux);
		    level_id = path_ft->path[i_aux]->connected_tier;
		    switch_id = path_ft->path[i_aux]->connected_switch_id;
		    port_id = path_ft->path[i_aux]->connected_switch_port;
		}
		else if (i <  mean_path){
		    //printf("%d %d %d %d\n", i,level_id, switch_id, port_id);
		    path_ft->path[i_aux] = &(str->str[level_id][switch_id].ports[dir][port_id]);
		}
		else if(i ==  (level_init + path_ft->length - 1) ){
		    dir = 0;
		    port_id = (get_node(core_dst, cores_node) % str->ports_down) + str->ports_down;
		    path_ft->path[i_aux] = &(str->str[level_id][switch_id].ports[dir][port_id]);
		}

		else if(i % 2 == 1){
		    dir = 0;
		    sw_aux = get_switch(get_node(core_dst, cores_node),str->ports_down);    
		    sw_aux = ((sw_aux / str->ports_up) *  str->ports_up) + (( str->nlevels - level_id -1) * (sw_aux % str->ports_up));  
		    //printf("%d %d\n",i,sw_aux);
		    for(j = str->ports_up; j < str->num_ports; j++){
			if(str->str[level_id][switch_id].ports[dir][j].connected_switch_id >= sw_aux && str->str[level_id][switch_id].ports[dir][j].connected_switch_id < (sw_aux +  str->ports_up)){
			    port_id = j;
			    break;
			}
		    }
		    path_ft->path[i_aux] = &(str->str[level_id][switch_id].ports[dir][port_id]);
		    level_id = path_ft->path[i_aux]->connected_tier;
		    switch_id = path_ft->path[i_aux]->connected_switch_id;
		    port_id = path_ft->path[i_aux]->connected_switch_port;
	
		}
		else{
		    		    path_ft->path[i_aux] = &(str->str[level_id][switch_id].ports[dir][port_id]);
		}
	}
//print_network_path(path_ft,&providerSet[0],0 );

	for(i_aux = 0; i_aux < path_ft->length; i_aux++){
	    bw = switch_ft_bw(path_ft->path[i_aux]);
	    //printf("%f ", bw);
	    if(bw < bw_path){
		bw_path = bw;
	    }
	}
	//printf("\n");
	path_ft->maxbw = bw_path;
	    break;
	default:
	    panic("No request path policy!\n");
	    break;
    }
    return(path_ft);
}

float switch_ft_bw(port_switch_ft *p_s_ft){
   
    int i;
    int num_tr = 1;
    float bw;
    float r_bw = 0.0;
    fattree_nw_path* path;
    transmission *tr = p_s_ft->trans->first; 

    bw = p_s_ft->total_bw / (p_s_ft->trans->num_transmissions + 1);
    for(i=0; i < p_s_ft->trans->num_transmissions; i++){
       path = (fattree_nw_path*)(tr->n_p->path);	
	if(path->maxbw < bw){
	    r_bw += (bw - path->maxbw);
	}
	else{
	    num_tr++;
	}
	tr = tr->next;
    } 
    bw = (p_s_ft->total_bw - r_bw) / num_tr; 
    return(bw);
}

void reserve_network_path_fattree(fattree_nw_path* path, network_phase *n_p){
    
    int i,j;
    int num_tr;
    float bw, bw_aux;
    float r_bw = 0.0;
    transmission *tr; // = p_s_ft->trans->first; 
    fattree_nw_path* path_aux;

    for(i = 0; i < path->length; i++){
	num_tr = 0;	
	bw = path->path[i]->total_bw / (path->path[i]->trans->num_transmissions + 1);
	r_bw = 0; 
	tr = path->path[i]->trans->first;
	for(j = 0; j < path->path[i]->trans->num_transmissions; j++){
	    path_aux = (fattree_nw_path*)(tr->n_p->path);	
	    if(path_aux->maxbw < bw){
		r_bw += (bw - path_aux->maxbw);
	    }
	    else{
		num_tr++;
	    }
	    tr = tr->next;
	}
	if(j!=0 && num_tr>0){
	bw_aux = (path->path[i]->total_bw - r_bw - path->maxbw) / num_tr; 
		  
	tr = path->path[i]->trans->first;
	for(j = 0; j < path->path[i]->trans->num_transmissions; j++){
	    path_aux = (fattree_nw_path*)(tr->n_p->path);	
	    //printf("%f %f %f - %d %f\n",path_aux->maxbw,bw_aux,r_bw, path->path[i]->total_bw,path->maxbw);
	    if(path_aux->maxbw > bw_aux){
		update_path_ft_bw(path_aux, path_aux->maxbw, bw_aux);
		modify_transfer(tr->n_p->vm_req, bw_aux);	
		path_aux->maxbw = bw_aux;
	    }
	    tr = tr->next;
	}
    }
    }
    for(i = 0; i < path->length; i++){
	n_p->path = path;
	add_transmission(path->path[i], n_p); 
    }
  //  printf("************************%f\n", path->maxbw);
    update_path_ft_bw(path, 0.0, path->maxbw);
    //printf("########################%f\n", path->maxbw);
}
    
transmissions* init_transmissions(){

    transmissions *trans = malloc(sizeof(transmissions));
    trans->num_transmissions = 0;
    trans->first = NULL;
    trans->last = NULL;
    return(trans);

}

void remove_transmissions(transmissions* trans){

    if( trans->num_transmissions == 0){
	free(trans);
    }	
    else{
	//panic("fattree: transmissions\n");
    }
}
void add_transmission(port_switch_ft *path, network_phase *n_p){

    float energy;
    int port;
    transmissions* trans = path->trans;
    transmission *tr = malloc(sizeof(transmission));
    tr->next = NULL;
    tr->prev= NULL;
    tr->n_p = n_p;
    if(trans->num_transmissions == 0){

	trans->first = tr; 
	trans->last = tr; 
	port=path->switch_port;
	if(path->sw->ports[0][port].trans->num_transmissions == 0 && path->sw->ports[1][port].trans->num_transmissions == 0){
	    energy = switch_energy_model(path->sw->num_ports, path->sw->used_ports, simtime - path->sw->energy_event);
	    switches_energy_metric(NULL, 0, energy);
	    path->sw->used_ports ++;
       //printf(" ON %d %d %d %d %d %d %ld %f\n",path->tier, path->switch_id, path->direction, path->switch_port,path->sw->num_ports,path->sw->used_ports, simtime-path->energy_event,energy);
	    path->sw->energy_event = simtime;
	}
    }	
    else{
	tr->prev =  trans->last;
	trans->last->next = tr;
	trans->last = tr;
    }
    trans->num_transmissions++;
}

void remove_transmission(port_switch_ft *path, network_phase *n_p){

    float energy;
    int port;
    transmissions *trans = path->trans;
    transmission *tr = path->trans->first;

    while(tr->n_p != n_p){
	tr = tr->next;
    }
    if(trans->num_transmissions == 1){
	trans->first = NULL; 
	trans->last = NULL; 
	port=path->switch_port;
	if(path->sw->ports[0][port].trans->num_transmissions == 0 || path->sw->ports[1][port].trans->num_transmissions == 0){
	    energy = switch_energy_model(path->sw->num_ports, path->sw->used_ports, simtime - path->sw->energy_event);
	    switches_energy_metric(NULL, 0, energy);
	    path->sw->used_ports--;
       //printf(" OF %d %d %d %d %d %d %ld %f\n",path->tier, path->switch_id, path->direction, path->switch_port,path->sw->num_ports,path->sw->used_ports, simtime-path->energy_event,energy);
	    path->sw->energy_event = simtime;
	}
    }	
    else if(tr->prev == NULL){
	tr->next->prev = NULL;
	trans->first = tr->next;
    }
    else if(tr->next == NULL){
	tr->prev->next = NULL;
	trans->last = tr->prev;
    }
    else{
	tr->prev->next = tr->next;
	tr->next->prev = tr->prev;
    }
    trans->num_transmissions--;
    free(tr);
}

void remove_path_transmissions(network_phase *n_p){

    int i;
    fattree_nw_path* path = n_p->path;

	//printf("R Simtime: %ld Time arrival: %ld Data remaining: %f Bw: %f Time remaining: %ld Path_bw: %f\n", simtime, n_p->time_arrival, n_p->data_remaining, n_p->bw, n_p->time_remaining, path->maxbw);
    update_path_ft_bw(path, n_p->bw, 0.0);
    for(i = 0; i < path->length; i++){
	remove_transmission(path->path[i], n_p);
	//free(path->path[i]);
	/*  
    	num_tr = 0;	
	bw = path->path[i]->total_bw / (path->path[i]->trans->num_transmissions + 1);
	//r_bw = -path->maxbw; 
	tr = path->path[i]->trans->first;
	for(j = 0; j < path->path[i]->trans->num_transmissions; j++){
	    path_aux = (fattree_nw_path*)(tr->n_p->path);	
	    if(path_aux->maxbw < bw){
		r_bw += (bw - path_aux->maxbw);
	    }
	    else{
	// r_bw += bw;
	    num_tr++;
	    }
	   tr = tr->next;
	}
	if(path->length >= 1){
	    bw_aux = (path->path[i]->total_bw - r_bw - path->maxbw)/ num_tr; 
	}
	tr = path->path[i]->trans->first;
	for(j = 0; j < path->path[i]->trans->num_transmissions; j++){
	    path_aux = (fattree_nw_path*)(tr->n_p->path);	
	    printf("*****BW_AUX*******: %f",bw_aux);









	    if(i == 0){
		path_aux->maxbw = bw_aux;
	    }
	    else{
		if(path_aux->maxbw > bw_aux){
		    path_aux->maxbw = bw_aux;
		}
	    }
	    if(i == path->length-1){
		modify_transfer(tr->n_p, path_aux->maxbw);	
	    }
	    tr = tr->next;
	}
	  */
    }
    free(path->path);
    free(path);
}


void update_path_ft_bw(fattree_nw_path* path, float old_bw, float new_bw){

    int i;
//	    printf("PARAMS: %f %f\n",new_bw,old_bw);
//printf("______________________ %d \n",path->length);
    for(i = 0; i < path->length; i++){

//	printf("%f * ", path->path[i]->used_bw);
	path->path[i]->used_bw -= old_bw;
	path->path[i]->used_bw += new_bw;
	path->path[i]->free_bw = path->path[i]->total_bw - path->path[i]->used_bw;
//	if(path->path[i]->used_bw < 0){
//	    printf("%f %f\n",new_bw,old_bw);
//	    exit(0);
//	}

    }
//    printf("END\n");
}
























