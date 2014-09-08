/*
 * =====================================================================================
 *
 *       Filename:  crossover.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/16/14 07:57:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _crossover
#define _crossover

void rndcross (individual *parent1, individual *parent2);

void topoawarecross (individual *parent1, individual *parent2);
#endif
