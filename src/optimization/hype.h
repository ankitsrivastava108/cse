/*========================================================================
  PISA  (http://www.tik.ee.ethz.ch/sop/pisa/)
  ========================================================================
  Computer Engineering (TIK)
  ETH Zurich
  ========================================================================
  HypE - Hypervolume Estimation Algorithm for Multiobjective Optimization

  author: Johannes Bader, johannes.bader@tik.ee.ethz.ch

  last change: 14.05.2008
  ========================================================================
 */

#ifndef _hype 
#define _hype 

#include "opt_selector.h"
/*-----------------------| specify Operating System |------------------*/
/* necessary for wait() */

void create_front_part(fpart* partp, int max_pop_size);
void free_front_part(fpart* partp);
void generateFrontPartition( fpart* front_part );
void insertInPart( fpart* partp, int nr, int id );
void cleanPart(fpart* partp);
void removeIndividual( int sel, fpart* partp, front *fp );

void selectionHYPE();
void mergeOffspringHYPE();
void environmentalSelectionHYPE();
void matingSelectionHYPE();

void hypeReduction( fpart* partp, front* fp, int alpha, double bound,
		int nrOfSamples );
void hypeFitnessFrontwise( front* fp, double bound, int nrOfSamples, int type );
int weaklyDominates( double *point1, double *point2, int no_objectives );
void cleanUpArchive(fpart* partp);
void hypeFitnessMating( double bound, int nrOfSamples );

#endif /* HYPE_H */
