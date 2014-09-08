/*========================================================================
  PISA  (http://www.tik.ee.ethz.ch/sop/pisa/)
  ========================================================================
  Computer Engineering (TIK)
  ETH Zurich
  ========================================================================
  SHV - Sampling-based HyperVolume-oriented Algorithm

  authors: Johannes Bader, johannes.bader@tik.ee.ethz.ch

  last change: 20.04.2009
  ========================================================================
 */

#include "opt_engine.h"
#include "opt_selector.h"
#include "../utils/rand.h"
#include "shv.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

/* local parameters from paramfile*/
double bound;
int nrOfJunks;
int nrOfSamplesPerJunk;


void selectionSHV()
{
	mergeOffspringSHV();
	generateFrontPartition( &front_part );
	environmentalSelectionSHV();
	cleanUpArchive(&(front_part));
	matingSelectionSHV();
}

void mergeOffspringSHV()
/**
 * Merge the offspring individuals (pp_new) with the archive population pp_new
 *
 * @pre pp_all->maxsize >= pp_all->size + pp_new->size
 * @remark the population pp_new is added to pp_all, the population pp_new
 * 	is emptied.
 */
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

void  swap(double  *front, int  i, int  j)
{
	int  k;
	double  temp;

	for (k = 0; k < opt_params.dim; k++) {
		temp = front[i * opt_params.dim + k];
		front[i * opt_params.dim + k] = front[j * opt_params.dim + k];
		front[j * opt_params.dim + k] = temp;
	}
}

int  dominatesPoints(double  *point1, double  *point2, int  no_objectives)
/* returns true if 'point1' dominates 'points2' with respect to the
       to the first 'no_objectives' objectives
 */
{
	int  i;
	int  better_in_any_objective, worse_in_any_objective;

	better_in_any_objective = 0;
	worse_in_any_objective = 0;
	for (i = 0; i < no_objectives && !worse_in_any_objective; i++)
		if (point1[i] > point2[i])
			better_in_any_objective = 1;
		else if (point1[i] < point2[i])
			worse_in_any_objective = 1;
	return (!worse_in_any_objective && better_in_any_objective);
}

int  filter_nondominated_set(double  *front, int  no_points,
		int  no_objectives)
/* all nondominated points regarding the first 'no_objectives' opt_params.dimensions
       are collected; the points 0..no_points-1 in 'front' are
       considered; the points in 'front' are resorted, such that points
       [0..n-1] represent the nondominated points; n is returned
 */
{
	int  i, j;
	int  n;

	n = no_points;
	i = 0;
	while (i < n) {
		j = i + 1;
		while (j < n) {
			if (dominatesPoints(&(front[i * opt_params.dim]), &(front[j * opt_params.dim]),
					no_objectives)) {
				/* remove point 'j' */
				n--;
				swap(front, j, n);
			}
			else if (dominatesPoints(&(front[j * opt_params.dim]), &(front[i * opt_params.dim]),
					no_objectives)) {
				/* remove point 'i'; ensure that the point copied to index 'i'
		   is considered in the next outer loop (thus, decrement i) */
				n--;
				swap(front, i, n);
				i--;
				break;
			}
			else
				j++;
		}
		i++;
	}
	return n;
}

double  surface_unchanged_to(double  *front, int  no_points, int  objective)
/* calculate next value regarding opt_params.dimension 'objective'; consider
	points 0..no_points-1 in 'front'
 */
{
	int     i;
	double  min, value;

	min = front[objective];
	for (i = 1; i < no_points; i++) {
		value = front[i * opt_params.dim + objective];
		if (value < min)  min = value;
	}

	return min;
}

int  reduce_nondominated_set(double  *front, int  no_points, int  objective,
		double  threshold)
/* remove all points which have a value <= 'threshold' regarding the
       opt_params.dimension 'objective'; the points [0..no_points-1] in 'front' are
       considered; 'front' is resorted, such that points [0..n-1] represent
       the remaining points; 'n' is returned
 */
{
	int  n;
	int  i;

	n = no_points;
	for (i = 0; i < n; i++)
		if (front[i * opt_params.dim + objective] <= threshold) {
			n--;
			swap(front, i, n);
		}

	return n;
}

double  calc_hypervolume(double  *front, int  no_points, int  no_objectives)
{
	int     n;
	double  volume, distance;

	volume = 0;
	distance = 0;
	n = no_points;
	while (n > 0) {
		int     no_nondominated_points;
		double  temp_vol, temp_dist;

		no_nondominated_points = filter_nondominated_set(front, n,
				no_objectives - 1);
		temp_vol = 0;
		if (no_objectives < 3) {
			temp_vol = front[0];
		}
		else
			temp_vol = calc_hypervolume(front, no_nondominated_points,
					no_objectives - 1);
		temp_dist = surface_unchanged_to(front, n, no_objectives - 1);
		volume += temp_vol * (temp_dist - distance);
		distance = temp_dist;
		n = reduce_nondominated_set(front, n, no_objectives - 1, distance);
	}

	return volume;
}

double unaryHypervolumeIndicator(int *A, int sizea, double bound  )
{
	double pointArray[ sizea*opt_params.dim ];
	int i, k;
	for (i = 0; i < sizea; i++)
		for (k = 0; k < opt_params.dim; k++)
			pointArray[i * opt_params.dim + k] = bound - pp_all->ind_array[ A[i] ]->f[k];

	return calc_hypervolume(pointArray, sizea, opt_params.dim );
}

void assignFitness(front* fp )
/* fitness is to be maximized */
{
	int i;
	front* fpTemp1;
	fpart dum1_part;  /* temporary storage */
	create_front_part(&dum1_part, opt_params.alpha + opt_params.lambda);

	double indicatorValue;

	fpTemp1 = &(dum1_part.front_array[0]);
	for (i = 0; i < fp->size; i++)
		fpTemp1->members[i] = fp->members[i];
	dum1_part.size = fp->size;
	dum1_part.fronts = 1;
	dum1_part.front_array[0].size = fp->size;


	fpTemp1->size--;
	for (i = fpTemp1->size; i >= 0; i--)
	{
		int temp = fpTemp1->members[i];
		fpTemp1->members[i] = fpTemp1->members[fpTemp1->size];
		indicatorValue = unaryHypervolumeIndicator( fpTemp1->members, fpTemp1->size,
				bound );
		pp_all->ind_array[temp]->fitness = -indicatorValue;
		fpTemp1->members[fpTemp1->size] = temp;
	}


	free_front_part(&dum1_part);
}

void removeTheWorstIndividual(front* fp, fpart* partp )
{
	int j;
	int sel = 0;
	double min = -1;

	for (j = fp->size - 1; j >= 0; j--)
	{
		if ( j == fp->size - 1 || min >= pp_all->ind_array[fp->members[j]]->fitness)
		{
			min = pp_all->ind_array[fp->members[j]]->fitness;
			sel = j;
		}
	}
	removeIndividual( sel, partp, fp );
}
void getObjectiveVector( int* A, int sizea, int i, double* objVect  )
{
	assert( i >= 0);
	assert( i < sizea );
	int k;
	for( k = 0; k < opt_params.dim; k++ ) {
		objVect[ k ] = pp_all->ind_array[ A[i] ]->f[k];
	}
}

int getNextSamplingIndex( int* h, int* N, double* V, int sizea, int* save,
		int decreaseDependent )
/* Get the next sampling index. All save points ignored, all points with 0
 * sample sizes prioritized */
{
	double sel = -1;
	double maxDecrease = -1;

	int i;
	double pt, actDecrease;
	for( i = 0; i < sizea; i++ )
	{
		if( save[i] > 0 )
			continue;
		if( N[i] == 0 )
		{
			sel = i;
			break;
		}
		if( decreaseDependent == 0 )
			actDecrease = rand();
		else {
			pt = ( h[i] + 1 ) / ( (double)N[i] + 2.0 );
			actDecrease = V[i]*V[i]*( pt*(1-pt) / ( (double)N[i] + 3.0 ) -
					pt*(1-pt) / ( (double)N[i] + 4.0 ) );
		}
		if( sel == -1 || actDecrease > maxDecrease )
		{
			sel = i;
			maxDecrease = actDecrease;
		}
	}
	return sel;
}

void quick_sort_indices(double values[], int indices[], int left, int right )
{
	double pivot;
	int pivot_ind;
	int l_hold, r_hold;

	l_hold = left;
	r_hold = right;
	pivot = values[left];
	pivot_ind = indices[left];
	while (left < right)
	{
		while ((values[right] >= pivot) && (left < right))
			right--;
		if (left != right)
		{
			values[left] = values[right];
			indices[left] = indices[right];
			left++;
		}
		while ((values[left] <= pivot) && (left < right))
			left++;
		if (left != right)
		{
			values[right] = values[left];
			indices[right] = indices[left];
			right--;
		}
	}
	values[left] = pivot;
	indices[left] = pivot_ind;
	pivot = left;
	left = l_hold;
	right = r_hold;
	if (left < pivot)
		quick_sort_indices(values, indices, left, pivot-1 );
	if (right > pivot)
		quick_sort_indices(values, indices, pivot+1, right );
}

void calcSamplingHypercubes( int *A, int sizea, double* SHC_lower,	double* SHC_upper )
/* Calculate the Sampling hypercubes for all individuals in A
 * Assumptions: Minimizing */
{
	getObjectiveArray(A, sizea, SHC_lower);

	int i,k,j;
	int Ind[ sizea ];
	double objVect[ sizea ];

	/* All opt_params.dimensions */
	for( k = 0; k < opt_params.dim; k++ )
	{
		for( i = 0; i < sizea; i++ )
			Ind[i] = i;
		getObjectiveVector( A, sizea, k, objVect  );
		quick_sort_indices( objVect, Ind, 0, sizea-1 );
		/* All individuals */
		for( j = 0; j < sizea; j++ )
		{
			int found = 0;
			int l = j;
			/* If there are opt_params.multiple individuals with the same kth coordinate,
			 * check all */
			while( l > 0 && objVect[l-1] == objVect[l] )
				l--;

			do {
				if( l == j )
					continue;
				int o = 0;
				found = 1;
				while( found == 1 && o < opt_params.dim)
				{
					if( o != k )
						found = (pp_all->ind_array[A[Ind[j]]]->f[o] -
								pp_all->ind_array[A[Ind[l]]]->f[o] ) >= 0;
					o++;
				}
			} while( found == 0 && ++l < sizea );
			assert( (Ind[j]*opt_params.dim + k) >= 0 );
			assert( (Ind[j]*opt_params.dim + k) < opt_params.dim*sizea );
			if( found )
				SHC_upper[ Ind[j]*opt_params.dim + k ] = pp_all->ind_array[ A[ Ind[l] ] ]->f[k];
			else
				SHC_upper[ Ind[j]*opt_params.dim + k ] = bound;

			/* Debug Checks */
			if( SHC_upper[ Ind[j]*opt_params.dim + k ] < SHC_lower[ Ind[j]*opt_params.dim + k ] )
			{
				fprintf(stderr,"Error: Upper bound (%lf) smaller than lower "
						"bound (%lf)\n", SHC_upper[ Ind[j]*opt_params.dim + k ],
						SHC_lower[ Ind[j]*opt_params.dim + k ] ); fflush(stderr);
			}
		}
	}
}

void resetSamplingCounts( int* h, int* N, int size )
{
	while( --size >= 0 )
	{
		h[ size ] = 0;
		N[ size ] = 0;
	}
}


void calcSamplingHypercubesVolume( double* SHCVol, double* SHC_lower, double* SHC_upper,
		int size )
{
	int i,k;

	for( i = 0; i < size; i++ )
	{
		SHCVol[i] = 1.0;
		for( k = 0; k < opt_params.dim; k++ )
			SHCVol[i] *= SHC_upper[ i*opt_params.dim + k ] - SHC_lower[ i*opt_params.dim + k ];
	}
}

int weaklyDominatesMin( double *point1, double *point2, int no_objectives )
{
	int better;
	better = 1;
	int i = 0;

	while( i < no_objectives && better )
	{
		better = point1[i] <= point2[i];
		i++;
	}
	return better;
}

void monteCarloSampling( int* h, int* N, int* A, int sizea, double* SHC_lower,
		double* SHC_upper, int nrOfJunks, int nrOfSamplesPerJunk, double* SHCVol )
{
	double sample[ opt_params.dim ];
	int save[ sizea ];
	int i,c,s,k,j;
	for( i = 0; i < sizea; i++ )
		save[i] = ( SHCVol[i] == 0);

	for( c = 0; c < nrOfJunks; c++ )
	{
		i = getNextSamplingIndex( h, N, SHCVol, sizea, save, 1 );

		assert( i >= 0 );
		assert( i < sizea );

		/* In this case, sampling makes no sense */
		if( SHCVol[i] == 0 )
			continue;

		N[i] += nrOfSamplesPerJunk;

		for( s = 0; s < nrOfSamplesPerJunk; s++ )
		{
			for( k = 0; k < opt_params.dim; k++ )
				sample[ k ] = randf(SHC_lower[ i*opt_params.dim + k ],
						SHC_upper[ i*opt_params.dim + k ] );

			int single = 1;
			j = 0;
			while( single == 1 && j < sizea )
			{
				if( j != i )
					single = !weaklyDominatesMin( SHC_lower + (j*opt_params.dim), sample, opt_params.dim);
				j++;
			}
			if( single )
				h[i]++;
		}
	}
}

void monteCarloSampleNewAfterRemoval( fpart* partp, front* fp, int nrOfJunks,
		int nrOfSamplesPerJunk )
{
	int totalNrOfSamples = 0;
	double* SHCVol = NULL;
	double* SHC_upper = NULL;
	double* SHC_lower = NULL;
	int h[ fp->size];
	int N[ fp->size];
	double minVol = DBL_MAX;
	double checkVol;
	int sel = -1;
	int i;

	SHC_lower = (double*)malloc( sizeof(double)*(fp->size)*opt_params.dim );
	SHC_upper  = (double*)malloc( sizeof(double)*(fp->size)*opt_params.dim );
	SHCVol = (double*)malloc( sizeof(double)*fp->size );

	while( partp->size > opt_params.alpha )
	{

		calcSamplingHypercubes( fp->members, fp->size, SHC_lower, SHC_upper);
		calcSamplingHypercubesVolume( SHCVol, SHC_lower, SHC_upper, fp->size );
		resetSamplingCounts( h, N, fp->size);
		monteCarloSampling( h, N, fp->members, fp->size, SHC_lower, SHC_upper,
				nrOfJunks, nrOfSamplesPerJunk, SHCVol );
		totalNrOfSamples += nrOfJunks*nrOfSamplesPerJunk;

		sel = -1;
		for( i = 0; i < fp->size; i++ )
		{
			if( N[i] > 0 )
				checkVol = (h[i]/(double)N[i] )*SHCVol[i];
			else
				checkVol = 0;
			if( sel == -1  || checkVol < minVol ) {
				minVol = checkVol;
				sel = i;
			}
		}
		assert( sel >= 0 );
		assert( sel < fp->size );
		removeIndividual( sel, partp, fp );
	}

	chk_free(SHCVol); SHCVol = NULL;
	chk_free(SHC_lower); SHC_lower = NULL;
	chk_free(SHC_upper); SHC_upper = NULL;
}

void environmentalSelectionSHV()
/**
 * Selects opt_params.alpha individuals of the population pp_all
 *
 * @pre pp_all and front_part set
 * @post front_part.size == opt_params.alpha
 * @remark operates only on the front partition front_part. No individuals
 * 		are actually removed.
 */
{
	int i;
	/** Start with front wise reduction */
	for( i = front_part.fronts - 1; i >= 0; i-- )
	{
		if( front_part.size - front_part.front_array[i].size >= opt_params.alpha )
		{
			front_part.size -= front_part.front_array[i].size;
			front_part.front_array[i].size = 0;
			front_part.fronts--;
		}
		else
			break;
	}
	/** Then remove from worst front */
	front* fp = &( front_part.front_array[ front_part.fronts-1 ] );
	while( front_part.size > opt_params.alpha )
	{
		monteCarloSampleNewAfterRemoval(&(front_part), fp,	nrOfJunks, nrOfSamplesPerJunk );
	}

	assert( front_part.size == opt_params.alpha );
}

void matingSelectionSHV()
/**
 * Select parents individuals uniformly from pp_all and add them to pp_sel
 */
{
	int i;

	pp_sel->size = opt_params.mu;
	for (i = 0; i < opt_params.mu; i++)
	{
		addToSelection( i, rnd(0, pp_all->size ) );
	}
}

