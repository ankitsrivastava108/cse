/*========================================================================
  PISA  (www.tik.ee.ethz.ch/pisa/)
  ========================================================================
  Computer Engineering (TIK)
  ETH Zurich
  ========================================================================
  NSGA2
  
  Implements most functions.
  
  file: nsga2_functions.c
  author: Marco Laumanns, laumanns@tik.ee.ethz.ch

  revision by: Stefan Bleuler, bleuler@tik.ee.ethz.ch
  last change: $date$
  ========================================================================
*/

#include "opt_selector.h"
#include "opt_variator.h"
#include "opt_engine.h"
#include "nsga2.h"
#include "spea2.h"
#include "hype.h"
#include "shv.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/*-----------------------| initialization |------------------------------*/
pop *pp_all = NULL;
pop *pp_new = NULL;
pop *pp_sel = NULL;
fpart front_part;

pop* create_pop(int maxsize, int dim)
/* Allocates memory for a population. */
{
    int i;
    pop *pp;
    
    pp = (pop*) malloc(sizeof(pop));
    pp->size = 0;
    pp->maxsize = maxsize;
    pp->ind_array = (ind**) malloc(maxsize * sizeof(ind*));
    
    for (i = 0; i < maxsize; i++)
	pp->ind_array[i] = NULL;
    
    return (pp);
}

void initialize()
/* Performs the necessary initialization to start in state 0. */
{
    /* create individual and archive pop */
    pp_all = create_pop(opt_params.alpha + opt_params.lambda, opt_params.dim);
    pp_sel = create_pop(opt_params.mu, opt_params.dim);    
    switch(opt_params.alg){
	case SHV:
	case HYPE:
	      create_front_part(&front_part, opt_params.alpha + opt_params.lambda);
		break;
	default:
		break;
    }
}


ind* create_ind(int dim)
/* Allocates memory for one individual. */
{
    ind *p_ind;
    
    p_ind = (ind*) malloc(sizeof(ind));
    
    p_ind->index = -1;
    p_ind->valid = 0;
    p_ind->fitness = -1;
    p_ind->f = (long*) malloc(dim * sizeof(long));
    return (p_ind);
}


void free_memory()
/* Frees all memory. */
{
    switch(opt_params.alg){
	case HYPE:
	     free_front_part(&front_part);
		break;
	default:
		break;
    }

   free_pop(pp_sel);
   complete_free_pop(pp_all);
   free_pop(pp_new);
   pp_sel = NULL;
   pp_all = NULL;
   pp_new = NULL;
}


void free_pop(pop *pp)
/* Frees memory for given population. */
{
   if(pp != NULL)
   {
      free(pp->ind_array);
      free(pp);
   }
}


void complete_free_pop(pop *pp)
/* Frees memory for given population and for all individuals in the
   population. */
{
   int i = 0;
   if (pp != NULL)
   {
      if(pp->ind_array != NULL)
      {
         for (i = 0; i < pp->size; i++)
         {
            if (pp->ind_array[i] != NULL)
            {
               free_ind(pp->ind_array[i]);
               pp->ind_array[i] = NULL;
            }
         }
         
         free(pp->ind_array);
      }
   
      free(pp);
   }
}


void free_ind(ind *p_ind)
/* Frees memory for given individual. */
{
    assert(p_ind != NULL);
     
    free(p_ind->f);
    free(p_ind);
}

void selectopt()
/* Performs initial selection. */
{
    switch(opt_params.alg){
    
	case NSGA2:
	    selectionNSGA2();
	    break;
	case SPEA2:
	    selectionSPEA2();
	    break;
	case HYPE:
	    selectionHYPE();
	    break;
	case SHV:
	    selectionSHV();
	    break;
	default:
	    panic("opt_selector: No optimization algorithm");

    
    }
}

int dominates(ind *p_ind_a, ind *p_ind_b)
/* Determines if one individual dominates another.
   Minimizing fitness values. */
{
    int i;
    int a_is_worse = 0;
    int equal = 1;
    
    if(p_ind_a->valid && p_ind_b->valid){
	for (i = 0; i < opt_params.dim && !a_is_worse; i++){
	    a_is_worse = p_ind_a->f[i] > p_ind_b->f[i];
	    equal = (p_ind_a->f[i] == p_ind_b->f[i]) && equal;
	}
    }
    else{
	if(p_ind_a->valid){
	    a_is_worse = 0;
	    equal = 0;
	}
	else if(p_ind_b->valid){
	     a_is_worse = 1;
	     equal = 0;
	}
	else{
	    equal = 1;
	}
    }
     
     return (!equal && !a_is_worse);
}


int is_equal(ind *p_ind_a, ind *p_ind_b)
/* Determines if two individuals are equal in all objective values.*/
{
     int i;
     int equal = 1;
     
     for (i = 0; i < opt_params.dim; i++)
	 equal = (p_ind_a->f[i] == p_ind_b->f[i]) && equal;
     
     return (equal);
}

void read_ini(int *identity)
{
    int i,j;
    pp_new = create_pop(opt_params.alpha, opt_params.dim);
    
    for (i = 0; i < opt_params.alpha; i++)
	pp_new->ind_array[i] = create_ind(opt_params.dim);
    pp_new->size = opt_params.alpha;

    for (j = 0; j < opt_params.alpha; j++)
    {
	pp_new->ind_array[j]->index = identity[j];
	pp_new->ind_array[j]->valid = get_valid_value(identity[j]);	
	for (i = 0; i < opt_params.dim; i++)
	{
	    pp_new->ind_array[j]->f[i] = get_objective_value(identity[j], i);
	}
    }
}


void read_var(int *identity)
{
    int i,j;
    pp_new = create_pop(opt_params.lambda, opt_params.dim);
    
    for (i = 0; i < opt_params.lambda; i++){
	pp_new->ind_array[i] = create_ind(opt_params.dim);
    }
    pp_new->size = opt_params.lambda;
    for (j = 0; j < opt_params.lambda; j++)
    {
	pp_new->ind_array[j]->index = identity[j];
	pp_new->ind_array[j]->valid = get_valid_value(identity[j]);	
	for (i = 0; i < opt_params.dim; i++)
	{
	    pp_new->ind_array[j]->f[i] = get_objective_value(identity[j], i);
	}

    }

}


void write_sel(int *id_array)
{
     int i;

     for(i = 0; i < opt_params.mu; i++)
     {
          id_array[i] = pp_sel->ind_array[i]->index;
     }
}

int cmp_int(const void *p_i1, const void *p_i2)
/* Compares the two integers '*p_i1' and '*p_i2'.
   Returns 0 if *p_i1 == *p_i2.
   Returns 1 if *p_i1 > *p_i2.
   Returns -1 if *p_i1 < *p_i2. */
{
     int i1 = *((int *)p_i1);
     int i2 = *((int *)p_i2);
    
     if(i1 == i2) 
          return (0);

     if(i1 > i2) 
          return (1);
     else
          return (-1);
}

double calcDistance(ind *p_ind_a, ind *p_ind_b)
{
    int i;
    double distance = 0;
    
    if (is_equal(p_ind_a, p_ind_b) == 1)
    {   
        return (0);
    }   
    
    for (i = 0; i < opt_params.dim; i++)
        distance += pow(p_ind_a->f[i]-p_ind_b->f[i],2);
    
    if (0.0 == distance)
        distance = PISA_MINDOUBLE;

    return (sqrt(distance));
}

void write_arc()
{

    int *keep;                 
    int i, current;

     keep = (int *) malloc(sizeof(int) * pp_all->size); 

     for(i=0; i < pp_all->size; i++)                                                             
     {                                                                                               
          keep[i] = pp_all->ind_array[i]->index;                            
     }                                                                                             
     qsort(keep, (size_t) pp_all->size, sizeof(int), cmp_int);      
     current = get_first();
     for(i = 0; i < pp_all->size; i++)                                
     {   
          while(current < keep[i])                                                                    
          {                                                                                          
               remove_individual(current);                                                        
               current = get_next(current);                                                       
          } 
          if (current == keep[i])                                                                
          {     
               current = get_next(current);                                                    
          }                                                                            
     }                                                                                                         

     /* delete the last individuals at end of list */
     while(current != -1)
     {
          remove_individual(current); 
          current = get_next(current);                                                         
     }
     free(keep);
}

void print_pop(pop *pp){

    int j;
    
    printf("Size: %d ", pp->size);
    for (j = 0; j < pp->size; j++)
    {
	printf("%d ",pp->ind_array[j]->index);
    }
	printf("\n");
   

}

void chk_free( void* handle )
{
	if( handle != NULL ) {
		free( handle );
	}
}

void cleanUpArchive(fpart* partp)
/** Removes all individuals from pp_all that are not referenced in partp
 *
 * @param[in] partp partition of the population
 */
{
	pop* new_pop;
	int i, j;

	new_pop = create_pop(opt_params.alpha + opt_params.lambda, opt_params.dim);
	new_pop->size = 0;
	for (i = partp->fronts - 1; i >= 0; i--)
	{
		for (j = partp->front_array[i].size - 1; j >= 0; j--)
		{
			int index = partp->front_array[i].members[j];
			new_pop->ind_array[new_pop->size] = pp_all->ind_array[index];
			pp_all->ind_array[index] = NULL;
			partp->front_array[i].members[j] = new_pop->size;
			new_pop->size++;
		}
	}
	complete_free_pop(pp_all);
	pp_all = new_pop;
}

void generateFrontPartition( fpart* front_part )
/**
 * Partition the population pp_all into fronts
 *
 * @param[out] front_part partition of the front
 * @pre the population pp_all needs to be valid
 */
{
	int i,j;
	int actFront = 0;
	int notDominated;
	int checked[ pp_all->size ];
	int actchecked[ pp_all->size ];
	int added;

	cleanPart( front_part );

	for( i = 0; i < pp_all->size; i++ )
		checked[i] = 0;

	added = 0;
	actFront = 0;
	while( added < pp_all->size )
	{
		for( i = 0; i < pp_all->size; i++ )
		{
			if( checked[i] == 1 )
				continue;
			actchecked[i] = 0;
			notDominated = 1;
			j = 0;
			while( notDominated == 1 && j < pp_all->size )
			{
				if( i != j && checked[j] == 0 && dominates(pp_all->ind_array[j], pp_all->ind_array[i]) )
					notDominated = 0;
				j++;
			}
			if( notDominated )
			{
				actchecked[i] = 1;
				insertInPart(front_part, actFront, i );
				added++;
			}
		}
		for( j = 0; j < pp_all->size; j++ )
			if( actchecked[j] == 1 )
				checked[j] = 1;
		actFront++;
	}
}

void cleanPart(fpart* partp)
{
	int i;

	for (i = 0; i < partp->max_fronts; i++)
		partp->front_array[i].size = 0;
	partp->size = 0;
	partp->fronts = 0;
}

void insertInPart( fpart* partp, int nr, int index )
{
	if( partp->fronts < nr + 1 )
		partp->fronts = nr + 1;
	partp->size++;
	partp->front_array[ nr ].members[ partp->front_array[ nr ].size ] = index;
	partp->front_array[ nr ].size++;
}

void removeIndividual( int sel, fpart* partp, front *fp )
{
	assert( sel >= 0 );
	assert( sel < fp->size );
	fp->size--;
	fp->members[sel] = fp->members[fp->size];
	partp->size--;
}

void getObjectiveArray( int* A, int sizea, double* pointArray )
/* Returns an array of all objectives referenced by A */
{
	int i,k;
	for( i = 0; i < sizea; i++ )
	{
		for( k = 0; k < opt_params.dim; k++ )
		{
			pointArray[ i*opt_params.dim + k ] = pp_all->ind_array[ A[i] ]->f[k];
		}
	}
}

void addToSelection(int i, int c )
/**
 * adds the cth individual of pp_all to the ith place of pp_sel
 *
 * @param[in] c index of pp_all to be added
 * @param[in] i index of pp_sel to be replaced
 * @pre 0 <= i < pp_sel->size
 * @pre 0 <= c <= pp_all->size
 */
{
	assert( 0 <= i );
	assert( i < pp_sel->size );
	assert( 0 <= c );
	assert( c < pp_all->size );
	pp_sel->ind_array[i] = pp_all->ind_array[c];
}

void free_front_part(fpart* partp)
{
	int i;

	if (partp->front_array != NULL)
	{
		for (i = 0; i < partp->max_fronts; i++)
			if (partp->front_array[i].members != NULL)
				chk_free(partp->front_array[i].members);
		chk_free(partp->front_array);
		partp->front_array = NULL;
	}
	partp->fronts = 0;
	partp->max_fronts = 0;
	partp->size = 0;
}


