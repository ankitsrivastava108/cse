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

#include "nsga2.h"
#include "opt_engine.h"
#include "opt_selector.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "../utils/rand.h"

/* NSGA2 internal global variables */
int *copies;
int **frontnsga2;
double *distnsga2;



void selectionNSGA2()
{
    int i;
    int size;
    
    /* Join offspring individuals from variator to population */
    mergeOffspringNSGA2();
    size = pp_all->size;
    /* Create internal data structures for selection process */
    /* Vectors */
    copies = (int*) malloc(size * sizeof(int));
    distnsga2 = (double*) malloc(size * sizeof(double));

    /* Matrices */
    frontnsga2 = (int**) malloc(size * sizeof(int*));
    for (i = 0; i < size; i++)
    {
	frontnsga2[i] = (int*) malloc(size * sizeof(int));
    }
    
    /* Calculates NSGA2 fitness values for all individuals */
    calcFitnessesNSGA2();

    /* Calculates distnsga2ance cuboids */
    calcDistancesNSGA2();

    /* Performs environmental selection
       (truncates 'pp_all' to size 'alpha') */
    environmentalSelectionNSGA2();

    /* Performs mating selection
       (fills mating pool / offspring population pp_sel */
    matingSelectionNSGA2();
    
    /* Frees memory of internal data structures */    
    free(copies);
    copies = NULL;
    free(distnsga2);
    distnsga2 = NULL;
    for (i = 0; i < size; i++)
    {
	free(frontnsga2[i]);
    }
    free(frontnsga2);
    frontnsga2 = NULL;
    return;
}


void mergeOffspringNSGA2()
{
    int i,j;

    assert(pp_all->size + pp_new->size <= pp_all->maxsize);

    for (i = 0; i < pp_new->size; i++)
    {
	pp_all->ind_array[pp_all->size + i] = create_ind(opt_params.dim); 
	pp_all->ind_array[pp_all->size + i]->index = pp_new->ind_array[i]->index;
	pp_all->ind_array[pp_all->size + i]->valid = pp_new->ind_array[i]->valid;
	for(j = 0; j< opt_params.dim; j++){
	    pp_all->ind_array[pp_all->size + i]->f[j] = pp_new->ind_array[i]->f[j];
	}
    }
    pp_all->size += pp_new->size;
    complete_free_pop(pp_new);
    pp_new = NULL;
}


void calcFitnessesNSGA2()
{
    int i, j, l;
    int size;
    int num;
    int *d;
    int *f;
    
    size = pp_all->size;
    d = (int*) malloc(size * sizeof(int));
    f = (int*) malloc(size * sizeof(int));
    
    /* initialize fitness and strength values */
    for (i = 0; i < size; i++)
    {
        pp_all->ind_array[i]->fitness = 0;
	d[i] = 1;
	f[i] = 1;
	copies[i] = 0;
    }

    /* calculate strength values */
    num = size;
    for (l = 0; l < size; l++)
    {
	/* find next frontnsga2 */
	for (i = 0; i < size; i++)
	{
	    d[i] = 0;
	    if (f[i] != NULL)
            {
		for (j = 0; j < i && d[i] == 0; j++)
		    if (f[j] != NULL)
			if (dominates(pp_all->ind_array[j], pp_all->ind_array[i]))
			    d[i] = 1;
		for(j = i+1; j < size && d[i] == 0; j++)
                    if (f[j] != NULL)
                        if (dominates(pp_all->ind_array[j], pp_all->ind_array[i]))
                            d[i] = 1;
	    }
	}

	/* extract frontnsga2 */
	for (i = 0; i < size; i++)
	{
	    if (f[i] != NULL && d[i] == 0)
            {
                pp_all->ind_array[i]->fitness = l;
                f[i] = NULL;
                num--;
		frontnsga2[l][copies[l]] = i;
		copies[l] += 1;
            }
        }
	
	if (num == 0)
	    break;
    }
    
    free(d);
    d = NULL;
    free(f);
    f = NULL;
    return;
}


void calcDistancesNSGA2()
{
    int i, j, l, d;
    int size = pp_all->size;
    double dmax = PISA_MAXDOUBLE / (opt_params.dim + 1);

    for (i = 0; i < size; i++)
    {
	distnsga2[i] = 1;
    }
    
    for (l = 0; l < size; l++)
    {
	for (d = 0; d < opt_params.dim; d++)
	{
	    /* sort accorting to d-th objective */
	    for (i = 0; i < copies[l]; i++)
	    {
		int min_index = -1;
		int min = i;
		for (j = i + 1; j < copies[l]; j++)
		{
		    if (pp_all->ind_array[frontnsga2[l][j]]->f[d] <
			pp_all->ind_array[frontnsga2[l][min]]->f[d])
			min = j;
		}
		min_index = frontnsga2[l][min];
		frontnsga2[l][min] = frontnsga2[l][i];
		frontnsga2[l][i] = min_index;
	    }

	    /* add distnsga2ances */
	    for (i = 0; i < copies[l]; i++)
	    {
		if (i == 0 || i == copies[l] - 1)
		    distnsga2[frontnsga2[l][i]] += dmax;
		else
		{
		    distnsga2[frontnsga2[l][i]] +=
			pp_all->ind_array[frontnsga2[l][i+1]]->f[d] -
			pp_all->ind_array[frontnsga2[l][i-1]]->f[d];
		}
	    }
	}
    }
}


void environmentalSelectionNSGA2()
{
    int i, j;
    int size = pp_all->size;

    
    for (i = 0; i < size; i++)
    {
	pp_all->ind_array[i]->fitness += 1.0 / distnsga2[i];
    }

    for (i = 0; i < opt_params.alpha; i++)
    {
	ind *p_min;
	int min = i;
	for (j = i + 1; j < size; j++)
	{
	    if (pp_all->ind_array[j]->fitness <
		pp_all->ind_array[min]->fitness)
		min = j;
	}
	p_min = pp_all->ind_array[min];
	pp_all->ind_array[min] = pp_all->ind_array[i];
	pp_all->ind_array[i] = p_min;
    }
    
    for (i = opt_params.alpha; i < size; i++)
    {
       free_ind(pp_all->ind_array[i]);
       pp_all->ind_array[i] = NULL;
    }

    pp_all->size = opt_params.alpha;
    
    return;
}


void matingSelectionNSGA2()
/* Fills mating pool 'pp_sel' */
{
    int i, j;

    for (i = 0; i < opt_params.mu; i++)
    {
	int winner = rnd(0, pp_all->size-1);
	
	for (j = 1; j < opt_params.tournament; j++)
	{
	    int opponent = rnd(0,pp_all->size-1);
	    if (pp_all->ind_array[opponent]->fitness
		< pp_all->ind_array[winner]->fitness || winner == opponent)
	    {
		winner = opponent;
	    }
	}  
	pp_sel->ind_array[i] = pp_all->ind_array[winner];
    }
    pp_sel->size = opt_params.mu;
}


