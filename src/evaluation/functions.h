/*
 * =====================================================================================
 *
 *       Filename:  functions.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/14 09:23:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _functions
#define _functions

long eval_cores_f1(int *ind, int length, network *nw, int **bw);

long eval_cores_f2(int *ind, int length, network *nw);

long eval_pmin_constrained(int length, int *maxbw);

void update_metric_energy_nodes(long simcycle);

void update_metric_energy_switches(long simcycle);
#endif


