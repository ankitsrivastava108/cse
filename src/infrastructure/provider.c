/*
 * =====================================================================================
 *
 *       Filename:  provider.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/14 09:55:19
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
#include "balancer.h"

#include <stdlib.h>
#include "../utils/globals.h"

provider *providerSet;

int create_providers(){

	
	
}
int create_provider(int ndc){
    
    providerSet=malloc(sizeof(provider)*nproviders);
    providerSet[0].ndc=ndatacenters;
    create_broker(&providerSet[0]);
    create_loadbalancer(&providerSet[0]);
    create_datacenter(&providerSet[0],0,512);
    return(1);    
}

void delete_provider(){
    
    remove_datacenter(providerSet);
    remove_loadbalancer(providerSet);
    remove_broker(providerSet);
    free(providerSet);
}
