/*========================================================================
  PISA  (www.tik.ee.ethz.ch/pisa/)
  ========================================================================
  Computer Engineering (TIK)
  ETH Zurich
  ========================================================================
  SPEA2 - Strength Pareto EA 2
  
  Implements most functions.
  
  file: spea2_functions.c
  author: Marco Laumanns, laumanns@tik.ee.ethz.ch

  revision by: Stefan Bleuler, bleuler@tik.ee.ethz.ch
  last change: $date$
  ========================================================================
*/

#include "spea2.h"
#include "opt_engine.h"
#include "opt_selector.h"
#include "../utils/rand.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "spea2.h"

/* SPEA2 internal global variables */
int *fitness_bucket;
int *fitness_bucket_mod;
int *copies;
int *old_index;
double **distspea2;
int **NN;

void selectionSPEA2()
{
    int i;
    int size;

    /* Join offspring individuals from variator to population */
    mergeOffspringSPEA2();
    
    size = pp_all->size;

    /* Create internal data structures for selection process */
    /* Vectors */
    fitness_bucket = (int*) malloc(size * size * sizeof(int));
    fitness_bucket_mod = (int*) malloc(size * sizeof(int));
    copies = (int*) malloc(size * sizeof(int));
    old_index = (int*) malloc(size * sizeof(int));

    /* Matrices */
    distspea2 = (double**) malloc(size * sizeof(double*));
    NN = (int**) malloc(size * sizeof(int*));
    for (i = 0; i < size; i++)
    {
	distspea2[i] = (double*) malloc(size * sizeof(double));	  
	NN[i] = (int*) malloc(size * sizeof(int));
    }

    /* Calculates SPEA2 fitness values for all individuals */
    calcFitnessesSPEA2();

    /* Calculates distspea2ance matrix distspea2[][] */
    calcDistancesSPEA2();

    /* Performs environmental selection
       (truncates 'pp_all' to size 'alpha') */
    environmentalSelectionSPEA2();

    /* Performs mating selection
       (fills mating pool / offspring population pp_sel */
    matingSelectionSPEA2();
    
    /* Frees memory of internal data structures */    
    free(fitness_bucket);
    fitness_bucket = NULL;
    free(fitness_bucket_mod);
    fitness_bucket_mod = NULL;
    free(copies);
    copies = NULL;
    free(old_index);
    old_index = NULL;
    for (i = 0; i < size; i++)
    {
       if (NULL != distspea2)
          free(distspea2[i]);
       if (NULL != NN)
          free(NN[i]);
    }
    free(distspea2);
    distspea2 = NULL;
    free(NN);
    NN = NULL;
    
    return;
}


void mergeOffspringSPEA2()
{
    int i;
    
    assert(pp_all->size + pp_new->size <= pp_all->maxsize);
    
    for (i = 0; i < pp_new->size; i++)
    {
	pp_all->ind_array[pp_all->size + i] = pp_new->ind_array[i];
    }
    
    pp_all->size += pp_new->size;
    
    free_pop(pp_new);
    pp_new = NULL;
}


void calcFitnessesSPEA2()
{
    int i, j;
    int size;
    int *strength;
    
    size = pp_all->size;
    strength = (int*) malloc(size * sizeof(int));
    
    /* initialize fitness and strength values */
    for (i = 0; i < size; i++)
    {
        pp_all->ind_array[i]->fitness = 0;
	strength[i] = 0;
	fitness_bucket[i] = 0;
	fitness_bucket_mod[i] = 0;	
	for (j = 0; j < size; j++)
	{
	    fitness_bucket[i * size + j] = 0;
	}
    }
    
    /* calculate strength values */
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
	{
            if (dominates(pp_all->ind_array[i], pp_all->ind_array[j]))
	    {
                strength[i]++;
	    }
	}
    }
    
    /* Fitness values =  sum of strength values of dominators */
    for (i = 0; i < size; i++)
    {
        int sum = 0;
        for (j = 0; j < size; j++)
	{
            if (dominates(pp_all->ind_array[j], pp_all->ind_array[i]))
	    {
		sum += strength[j];
	    }
	}
	pp_all->ind_array[i]->fitness = sum;
	fitness_bucket[sum]++;
	fitness_bucket_mod[(sum / size)]++;
    }
    
    free(strength);
    strength = NULL;
    
    return;
}


void calcDistancesSPEA2()
{
    int i, j;
    int size = pp_all->size;
    
    /* initialize copies[] vector and NN[][] matrix */
    for (i = 0; i < size; i++)
    {
	copies[i] = 1;
	for (j = 0; j < size; j++)
	{
	    NN[i][j] = -1;
	}
	
    }
    
    /* calculate distspea2ances */
    for (i = 0; i < size; i++)
    {
	NN[i][0] = i;
	for (j = i + 1; j < size; j++)
	{
	    distspea2[i][j] = calcDistance(pp_all->ind_array[i], pp_all->ind_array[j]);
	    assert(distspea2[i][j] < PISA_MAXDOUBLE);
	    distspea2[j][i] = distspea2[i][j];
	    if (distspea2[i][j] == 0)
	    {
		NN[i][copies[i]] = j;
		NN[j][copies[j]] = i;
		copies[i]++;
		copies[j]++;
	    }
	}
	distspea2[i][i] = 0;
    }
}


int getNN(int index, int k)
/* lazy evaluation of the k-th nearest neighbor
   pre-condition: (k-1)-th nearest neigbor is known already */
{
    assert(index >= 0);
    assert(k >= 0);
    assert(copies[index] > 0);
    
    if (NN[index][k] < 0)
    {
	int i;
	double min_distspea2 = PISA_MAXDOUBLE;
	int min_index = -1;
	int prev_min_index = NN[index][k-1];
	double prev_min_distspea2 = distspea2[index][prev_min_index];
	assert(prev_min_distspea2 >= 0);
	
	for (i = 0; i < pp_all->size; i++)
	{
	    double my_distspea2 = distspea2[index][i];
	    
	    if (my_distspea2 < min_distspea2 && index != i)
	    {
		if (my_distspea2 > prev_min_distspea2 ||
		    (my_distspea2 == prev_min_distspea2 && i > prev_min_index))
		{
		    min_distspea2 = my_distspea2;
		    min_index = i;
		}
	    }
	}
	
	NN[index][k] = min_index;
    }

    return (NN[index][k]);
}


double getNNd(int index, int k)
/* Returns the distspea2ance to the k-th nearest neigbor
   if this individual is still in the population.
   For for already deleted individuals, returns -1 */
{
    int neighbor_index = getNN(index, k);
    
    if (copies[neighbor_index] == 0)
	return (-1);
    else
	return (distspea2[index][neighbor_index]);
}


void environmentalSelectionSPEA2()
{
    int i;
    int new_size = 0;
    
    if (fitness_bucket[0] > opt_params.alpha)
    {
	truncate_nondominatedSPEA2();
    }
    else if (pp_all->size > opt_params.alpha)
    {
	truncate_dominatedSPEA2();
    }

    /* Move remaining individuals to top of array in 'pp_all' */
    for (i = 0; i < pp_all->size; i++)
    {
	ind* temp_ind = pp_all->ind_array[i];
	if (temp_ind != NULL)
	{
	    assert(copies[i] > 0);	    
	    pp_all->ind_array[i] = NULL;
	    pp_all->ind_array[new_size] = temp_ind;
	    old_index[new_size] = i;
	    new_size++;    
	}
    }
    assert(new_size <= opt_params.alpha);
    pp_all->size = new_size;
    
    return;
}


void truncate_nondominatedSPEA2()
/* truncate from nondominated individuals (if too many) */
{
    int i;	

    /* delete all dominated individuals */
    for (i = 0; i < pp_all->size; i++)
    {
	if (pp_all->ind_array[i]->fitness > 0)
	{
	    free_ind(pp_all->ind_array[i]);
	    pp_all->ind_array[i] = NULL;
	    copies[i] = 0;
	}
    }
    
    /* truncate from non-dominated individuals */
    while (fitness_bucket[0] > opt_params.alpha)
    {
	int *marked;
	int max_copies = 0;
	int count = 0;
	int delete_index;

	marked = (int*) malloc(pp_all->size * sizeof(int));

	/* compute inds with maximal copies */
	for (i = 0; i < pp_all->size; i++)
	{
	    if (copies[i] > max_copies)
	    {
		count = 0;
		max_copies = copies[i];
	    }
	    if (copies[i] == max_copies)
	    {
		marked[count] = i;
		count++;
	    }
	}
	
	//assert(count >= max_copies);
	
	if (count > max_copies)
	{    
	    int *neighbor;
	    neighbor = (int*) malloc(count * sizeof(int));
	    for (i = 0; i < count; i++)
	    {
		neighbor[i] = 1;  /* pointers to next neighbor */
	    }
	    
	    while (count > max_copies)
	    {
		double min_distspea2 = PISA_MAXDOUBLE;
		int count2 = 0;
		
		for (i = 0; i < count; i++)
		{
		    double my_distspea2 = -1;
		    while (my_distspea2 == -1 && neighbor[i] < pp_all->size)
		    {
			my_distspea2 = getNNd(marked[i],neighbor[i]);
			neighbor[i]++;
		    }
		    
		    if (my_distspea2 < min_distspea2)
		    {
			count2 = 0;
			min_distspea2 = my_distspea2;
		    }
		    if (my_distspea2 == min_distspea2)
		    {
			marked[count2] = marked[i];
			neighbor[count2] = neighbor[i];
			count2++;
		    }
		}
		count = count2;
		if (min_distspea2 == -1) /* all have equal distspea2ances */
		{
		    break;
		}
	    }   
	    free(neighbor);
            neighbor = NULL;
	}
	
	/* remove individual from population */
	delete_index = marked[rnd(0,count - 1)];
	free_ind(pp_all->ind_array[delete_index]);
	pp_all->ind_array[delete_index] = NULL;
	for (i = 0; i < count; i++)
	{
	    if (distspea2[delete_index][marked[i]] == 0)
	    {
		copies[marked[i]]--;
	    }
	}
	copies[delete_index] = 0; /* Indicates that this index is empty */
	fitness_bucket[0]--;
	fitness_bucket_mod[0]--;
	free(marked);
        marked = NULL;
    }
    
    return;
}


void truncate_dominatedSPEA2()
/* truncate from dominated individuals */
{
    int i, j;
    int size;
    int num = 0;
    
    size = pp_all->size;
    
    i = -1;
    while (num < opt_params.alpha)
    {
	i++;
	num += fitness_bucket_mod[i];
    }
    
    j = i * size;
    num = num - fitness_bucket_mod[i] + fitness_bucket[j];
    while (num < opt_params.alpha)
    {
	j++;
	num += fitness_bucket[j];
    }
    
    if (num == opt_params.alpha)
    {
	for (i = 0; i < size; i++)
	{
	    if (pp_all->ind_array[i]->fitness > j)
	    {
		free_ind(pp_all->ind_array[i]);
		pp_all->ind_array[i] = NULL;
	    }
	}
    }
    else /* if not all fit into the next generation */
    {
	int k;
	int free_spaces;
	int fill_level = 0;
	int *best = NULL;

	free_spaces = opt_params.alpha - (num - fitness_bucket[j]);
	best = (int*) malloc(free_spaces * sizeof(int));
	for (i = 0; i < size; i++)
	{
	    if (pp_all->ind_array[i]->fitness > j)
	    {
		free_ind(pp_all->ind_array[i]);
		pp_all->ind_array[i] = NULL;
	    }
	    else if (pp_all->ind_array[i]->fitness == j)
	    {
		if (fill_level < free_spaces)
		{
		    best[fill_level] = i;
		    fill_level++;
		    for (k = fill_level - 1; k > 0; k--)
		    {
			int temp;
			if (getNNd(best[k], 1) <= getNNd(best[k - 1], 1))
			{
			    break;
			}
			temp = best[k];
			best[k] = best[k-1];
			best[k-1] = temp;
		    }
		}
		else
		{
		    if (getNNd(i, 1) <= getNNd(best[free_spaces - 1], 1))
		    {
			free_ind(pp_all->ind_array[i]);
			pp_all->ind_array[i] = NULL;
		    }
		    else
		    {
			free_ind(pp_all->ind_array[best[free_spaces - 1]]);
			pp_all->ind_array[best[free_spaces - 1]] = NULL;
			best[free_spaces - 1] = i;
			for (k = fill_level - 1; k > 0; k--)
			{
			    int temp;
			    if (getNNd(best[k], 1) <= getNNd(best[k - 1], 1))
			    {
				break;
			    }
			    temp = best[k];
			    best[k] = best[k-1];
			    best[k-1] = temp;
			}
		    }
		}
	    }
	}        
        free(best);
        best = NULL;
        return;
    }
}


void matingSelectionSPEA2()
/* Fills mating pool 'pp_sel' */
{
    int i, j;

    for (i = 0; i < opt_params.mu; i++)
    {
	int winner = rnd(0, pp_all->size-1);
	
	for (j = 1; j < opt_params.tournament; j++)
	{
	    int opponent = rnd(0, pp_all->size -1);
	    if (pp_all->ind_array[opponent]->fitness
		< pp_all->ind_array[winner]->fitness || winner == opponent)
	    {
		winner = opponent;
	    }
	    else if (pp_all->ind_array[opponent]->fitness
		     == pp_all->ind_array[winner]->fitness)
	    {
		if (distspea2[old_index[opponent]][getNN(old_index[opponent], 1)] >
		    distspea2[old_index[winner]][getNN(old_index[winner], 1)])
		{
		    winner = opponent;
		}
	    }
	}  
	pp_sel->ind_array[i] = pp_all->ind_array[winner];
    }
    pp_sel->size = opt_params.mu;
}

