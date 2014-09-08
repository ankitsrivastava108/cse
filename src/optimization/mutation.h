/*
 * =====================================================================================
 *
 *       Filename:  mutation.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/14 11:36:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _mutation
#define _mutation

#include "opt_variator.h"

void rnd_mutate_ind (individual *ind);

void topoaware_mutate_ind (individual *ind);
#endif
