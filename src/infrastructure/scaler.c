/*
 * =====================================================================================
 *
 *       Filename:  scaler.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/14 07:49:35
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
#include "../workloads/application.h"
#include "../utils/misc.h"

int scale_add_ff(application *app, int n){
    return(1);
} 

int scale_add_opt(application *app, int n){
    return(1);
} 

int scale_rem_last(){
    return(1);
} 

int scale_rem_opt(){
    return(1);
} 


int add_vm(provider *pv, int ndatacenter,int id,int n, scaler_add_policy_t policy){
    
    int i;
    int END=0; 
    
    application *app_aux=pv->datacenters[ndatacenter].alloc_apps->first_app;

    while(!END && app_aux!=NULL && i<pv->datacenters[ndatacenter].alloc_apps->num_alloc_apps){
	  if(app_aux->id==id){
	    switch(policy){
		case SFF:
		    scale_add_ff(app_aux,n);
		    break;
		default:
		    panic("No scale policy");
	    }		
	  }
	  else{
	        app_aux=app_aux->next; 
	  }
    i++;
    }
    return(1);
}

int remove_vm(provider *pv, int ndatacenter,int id, int n,scaler_rem_policy_t policy){
    
    int i;
    int END=0; 
    
    application *app_aux=pv->datacenters[ndatacenter].alloc_apps->first_app;

    while(!END && app_aux!=NULL && i<pv->datacenters[ndatacenter].alloc_apps->num_alloc_apps){
	  if(app_aux->id==id){
	    switch(policy){
		case SFF:
		    scale_rem_last();
		    break;
		default:
		    panic("No scale policy");
	    }	
	  }
	  else{
	        app_aux=app_aux->next; 
	  }
    i++;
    }
    return(1);
}
