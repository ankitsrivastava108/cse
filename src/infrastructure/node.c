/*
 * =====================================================================================
 *
 *       Filename:  node.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/12/14 13:22:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "node.h"
#include "../utils/globals.h"

int init_node(node *n, int id){
	
    int i;
    
    n->id = id;
    n->ncores = ncores;
    n->nvcores = nvcores;
    n->freevcores = nvcores;
    n->busyvcores = 0;
    n->idlecores = 0;
    n->freecores = ncores;
    n->bwtotal = bwtotal;
    n->usedbw = 0.0;
    n->freebw = (float)bwtotal;
    n->memtotal = memtotal;
    n->energy_event = 0;
    n->vcores=malloc(sizeof(int)*nvcores);
    for(i = 0;i < nvcores;i++){
	n->vcores[i] = -1;
    }
    return(1);
}

void remove_node(node *n){
    
    free(n->vcores);
}
