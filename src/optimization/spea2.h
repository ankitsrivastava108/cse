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
#ifndef _spea2
#define _spea2

void selectionSPEA2();
void mergeOffspringSPEA2();
void calcFitnessesSPEA2();
void calcDistancesSPEA2();
void environmentalSelectionSPEA2();
void truncate_nondominatedSPEA2();
void truncate_dominatedSPEA2();
void matingSelectionSPEA2();

int getNN(int index, int k);
double getNNd(int index, int k);
#endif
