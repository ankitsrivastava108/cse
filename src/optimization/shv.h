/*========================================================================
  PISA  (http://www.tik.ee.ethz.ch/sop/pisa/)
  ========================================================================
  Computer Engineering (TIK)
  ETH Zurich
  ========================================================================
  Sibea - Simple Hypervolume Based Evolutionary Algorithm

  authors: Johannes Bader, johannes.bader@tik.ee.ethz.ch
           Dimo Brockhoff, dimo.brockhoff@tik.ee.ethz.ch

  last change: 30.07.2008
  ========================================================================
 */

#ifndef _shv 
#define _shv 

void create_front_part(fpart* partp, int max_pop_size);
void generateFrontPartition( fpart* front_part );
void insertInPart( fpart* partp, int nr, int id );
void cleanPart(fpart* partp);
void removeIndividual( int sel, fpart* partp, front *fp );

void selectionSHV();
void mergeOffspringSHV();
void environmentalSelectionSHV();
void matingSelectionSHV();

void cleanUpArchive(fpart* partp);

#endif /* SIBEA_H */
