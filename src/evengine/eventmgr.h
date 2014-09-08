/*
 * =====================================================================================
 *
 *       Filename:  eventmgr.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/14 11:42:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _eventmgr
#define _eventmgr


void schedule_next_event( long n, long ts);

void init_structures();

void remove_structures();

void start_simulation();

void run_arrival_event(int v);

void run_departure_event(int v);

void run_warmup_event();

void run_batch_event();    

void run_request_event(void *vp);

void run_request_generation_event(int v, void *vp);

void run_transmission_event(int id, void *vp);

void  run_transmission_node_event(int v, void *vp);

void run_transmission_node_bd_event(int v, void *vp);

void run_transmission_bd_event(int v, void *vp);

void run_computation_event(int id, void *vp);
#endif
