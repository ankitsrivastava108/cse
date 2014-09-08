/*
 * =====================================================================================
 *
 *       Filename:  node.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/14 11:42:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _node
#define _node

typedef struct node{
    int id;
    int ncores;
    int freecores;
    int busyvcores;
    int idlecores;
    int freevcores;
    int nvcores;
    int bwtotal; //Mbits/s
    float usedbw; //Mbits/s
    float freebw; //Mbits/s
    int memtotal; //MB
    int *vcores;
    long energy_event;
}node;

int init_node(node *n, int id);

void remove_node(node *n);
#endif
