/*
 * =====================================================================================
 *
 *       Filename:  nsga2.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/14 07:17:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _nsga2
#define _nsga2

void selectionNSGA2();
void mergeOffspringNSGA2();
void calcFitnessesNSGA2();
void calcDistancesNSGA2();
void environmentalSelectionNSGA2();
void truncate_nondominatedNSGA2();
void truncate_dominatedNSGA2();
void matingSelectionNSGA2();

#endif
