/*========================================================================
  PISA  (http://www.tik.ee.ethz.ch/sop/pisa/)
  ========================================================================
  Computer Engineering (TIK)
  ETH Zurich
  ========================================================================
  HypE - Hypervolume Estimation Algorithm for Multiobjective Optimization

  author: Johannes Bader, johannes.bader@tik.ee.ethz.ch

  last change: 31.10.2008
  ========================================================================
 */

#include "hype.h"
#include "opt_engine.h"
#include "opt_selector.h"
#include "../utils/rand.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/**
 * mating type
 * 0 use uniform mating selection
 * 1 use tournament selection with hype fitness
*/
int mating = 1;
double bound = 0;
int nrOfSamples= -1;

void create_front_part(fpart*  partp, int  max_pop_size)
{
	int i;

	partp->max_fronts = (max_pop_size > 0 ? max_pop_size : 0);
	partp->fronts = 0;
	partp->size = 0;
	partp->front_array = (front*) malloc(max_pop_size * sizeof(front));
	for (i = 0; i < max_pop_size; i++)
	{
		partp->front_array[i].size = 0;
		partp->front_array[i].members = malloc(max_pop_size * sizeof(int));
	}
}



/*-----------------------| selection functions|--------------------------*/

void selectionHYPE()
/**
 *  Do environmental and mating selection
 */
{
	mergeOffspringHYPE();
	generateFrontPartition( &front_part );
	environmentalSelectionHYPE();
	cleanUpArchive(&(front_part));
	matingSelectionHYPE();
}

void mergeOffspringHYPE()
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



void environmentalSelectionHYPE()
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
	if( front_part.size > opt_params.alpha )
	{
		hypeReduction( &(front_part),
				&(front_part.front_array[front_part.fronts-1]), opt_params.alpha, bound,
				nrOfSamples );
	}
	assert( front_part.size == opt_params.alpha );
}


void rearrangeIndicesByColumn(double *mat, int rows, int columns, int col,
		int *ind )
/**
 * Internal function used by hypeExact
 */
{
#define  MAX_LEVELS  300
	int  beg[MAX_LEVELS], end[MAX_LEVELS], i = 0, L, R, swap;
	double pref, pind;
	double ref[rows];
	for( i = 0; i < rows; i++ ) {
		ref[i] = mat[ col + ind[i]*columns ];
	}
	i = 0;

	beg[0] = 0; end[0] = rows;
	while ( i >= 0 ) {
		L = beg[i]; R = end[i]-1;
		if( L < R ) {
			pref = ref[ L ];
			pind = ind[ L ];
			while( L < R ) {
				while( ref[ R ] >= pref && L < R )
					R--;
				if( L < R ) {
					ref[ L ] = ref[ R ];
					ind[ L++] = ind[R];
				}
				while( ref[L] <= pref && L < R )
					L++;
				if( L < R) {
					ref[ R ] = ref[ L ];
					ind[ R--] = ind[L];
				}
			}
			ref[ L ] = pref; ind[L] = pind;
			beg[i+1] = L+1; end[i+1] = end[i];
			end[i++] = L;
			if( end[i] - beg[i] > end[i-1] - beg[i-1] ) {
				swap = beg[i]; beg[i] = beg[i-1]; beg[i-1] = swap;
				swap = end[i]; end[i] = end[i-1]; end[i-1] = swap;
			}
		}
		else {
			i--;
		}
	}
}

void hypeExactRecursive( double* input_p, int pnts, int dim, int nrOfPnts, int actDim, double* bounds, int* input_pvec, double* fitness,double* rho, int param_k){

	int i, j;
	double extrusion;
	int pvec[pnts];
	double p[pnts*opt_params.dim];
	for( i = 0; i < pnts; i++ ) {
		fitness[i] = 0;
		pvec[i] = input_pvec[i];
	}
	for( i = 0; i < pnts*opt_params.dim; i++ )
		p[i] = input_p[i];

	rearrangeIndicesByColumn( p, nrOfPnts, opt_params.dim, actDim, pvec );

	for( i = 0; i < nrOfPnts; i++ )
	{
		if( i < nrOfPnts - 1 )
			extrusion = p[ (pvec[i+1])*opt_params.dim + actDim ] - p[ pvec[i]*opt_params.dim + actDim ];
		else
			extrusion = bounds[actDim] - p[ pvec[i]*opt_params.dim + actDim ];

		if( actDim == 0 ) {
			if( i+1 <= param_k )
				for( j = 0; j <= i; j++ ) {
					fitness[ pvec[j] ] = fitness[ pvec[j] ]
					                              + extrusion*rho[ i+1 ];
				}
		}
		else if( extrusion > 0 ) {
			double tmpfit[ pnts ];
			hypeExactRecursive( p, pnts, opt_params.dim, i+1, actDim-1, bounds, pvec,
					tmpfit, rho, param_k );
			for( j = 0; j < pnts; j++ )
				fitness[j] += extrusion*tmpfit[j];
		}
	}
}

void hypeExact( double* val, int popsize, double lowerbound, double upperbound,
		int param_k, double* points, double* rho  )
/**
 * Calculating the hypeIndicator
 * \f[ \sum_{i=1}^k \left( \prod_{j=1}^{i-1} \frac{k-j}{|P|-j} \right) \frac{ Leb( H_i(a) ) }{ i } \f]
 */
{
	int i;;
	double boundsVec[ opt_params.dim ];
	int indices[ popsize ];
	for( i = 0; i < opt_params.dim; i++ )
		boundsVec[i] = bound;
	for( i = 0; i < popsize; i++  )
		indices[i] = i;

	/** Recursively calculate the indicator values */
	hypeExactRecursive( points, popsize, opt_params.dim, popsize, opt_params.dim-1, boundsVec,
			indices, val, rho, param_k );
}

void hypeSampling( double* val, int popsize, double lowerbound,
		double upperbound, int nrOfSamples, int param_k, double* points,
		double* rho )
/**
 * Sampling the hypeIndicator
 * \f[ \sum_{i=1}^k \left( \prod_{j=1}^{i-1} \frac{k-j}{|P|-j} \right) \frac{ Leb( H_i(a) ) }{ i } \f]
 *
 * @param[out] val vector of all indicators
 * @param[in] popsize size of the population \f$ |P| \f$
 * @param[in] lowerbound scalar denoting the lower vertex of the sampling box
 * @param[in] upperbound scalar denoting the upper vertex of the sampling box
 * @param[in] nrOfSamples the total number of samples
 * @param[in] param_k the variable \f$ k \f$
 * @param[in] points matrix of all objective values opt_params.dim*popsize entries
 * @param[in] rho weight coefficients
 * @pre popsize >= 0 && lowerbound <= upperbound && param_k >= 1 &&
 * 		param_k <= popsize
 */
{
	assert(popsize >= 0 );
	assert( lowerbound <= upperbound );
	assert( param_k >= 1 );
	assert( param_k <= popsize );

	int i, s, k;
	int hitstat[ popsize ];
	int domCount;

	double sample[ opt_params.dim ];
	for( s = 0; s < nrOfSamples; s++ )
	{
		for( k = 0; k < opt_params.dim; k++ )
			sample[ k ] = randf( lowerbound, upperbound );

		domCount = 0;
		for( i = 0; i < popsize; i++ )
		{
			if( weaklyDominates( points + (i*opt_params.dim), sample, opt_params.dim) )
			{
				domCount++;
				if( domCount > param_k )
					break;
				hitstat[i] = 1;
			}
			else
				hitstat[i] = 0;
		}
		if( domCount > 0 && domCount <= param_k )
		{
			for( i = 0; i < popsize; i++ )
				if( hitstat[i] == 1 )
					val[i] += rho[domCount];
		}
	}
	for( i = 0; i < popsize; i++ )
	{
		val[i] = val[i] * pow( (upperbound-lowerbound), opt_params.dim ) / (double)nrOfSamples;
	}
}

void hypeIndicator( double* val, int popsize, double lowerbound,
		double upperbound, int nrOfSamples, int param_k, double* points )
/**
 * Determine the hypeIndicator
 * \f[ \sum_{i=1}^k \left( \prod_{j=1}^{i-1} \frac{k-j}{|P|-j} \right) \frac{ Leb( H_i(a) ) }{ i } \f]
 *
 * if nrOfSamples < 0, then do exact calculation, else sample the indicator
 *
 * @param[out] val vector of all indicator values
 * @param[in] popsize size of the population \f$ |P| \f$
 * @param[in] lowerbound scalar denoting the lower vertex of the sampling box
 * @param[in] upperbound scalar denoting the upper vertex of the sampling box
 * @param[in] nrOfSamples the total number of samples or, if negative, flag
 * 		that exact calculation should be used.
 * @param[in] param_k the variable \f$ k \f$
 * @param[in] points matrix of all objective values opt_params.dim*popsize entries
 * @param[in] rho weight coefficients
 */
{
	int i,j;
	double rho[param_k+1];
	/** Set opt_params.alpha */
	rho[0] = 0;
	for( i = 1; i <= param_k; i++ )
	{
		rho[i] = 1.0 / (double)i;
		for( j = 1; j <= i-1; j++ )
			rho[i] *= (double)(param_k - j ) / (double)( popsize - j );
	}
	for( i = 0; i < popsize; i++ )
		val[i] = 0.0;

	if( nrOfSamples < 0 )
		hypeExact( val, popsize, lowerbound, upperbound, param_k, points,
				rho );
	else
		hypeSampling( val, popsize, lowerbound, upperbound, nrOfSamples,
				param_k, points, rho );
}

void determineIndexAndFront(fpart* partp, int n, int* index, int* front,
		int sort)
/** Determine the front and the referencing index of the
 * nth individual
 *
 * @param[in] partp front paritioning
 * @param[in] n index of the individual to be retrieved
 * @param[out] index index the individuals is referencing
 * @param[out] front front the individual is in.
 * @param[out] sort if sorting = 0, don't use front partition
 * @pre n >= 0 && n < partp->size - 1
 * @post front >= 0 && front < partp->fronts, index >= 0 &&
 * 	index <= partp->size
 */
{
	assert( n >= 0 );
	assert( n <  partp->size );
	if( sort == 0 )
		*index = n;

	int i;

	i = partp->fronts - 1;
	while (i >= 0 && (n - partp->front_array[i].size) >= 0)
	{
		n -= partp->front_array[i].size;
		i--;
	}
	assert(i >= 0 );
	assert(n >= 0 );
	assert(n < partp->front_array[i].size);
	*index = partp->front_array[i].members[n];
	*front = i;
}

void hypeReduction( fpart* partp, front* fp, int alpha, double bound, int nrOfSamples )
/**
 * Iteratively remove individuals from front based on sampled hypeIndicator
 * value
 *
 * @param[in] partp partition of the population pp_all
 * @param[in] fp the current front to operate on
 * @param[in] opt_params.alpha the number of individuals in partp after removal
 * @param[in] bound scaler denoting the upper bound
 * @param[in] nrOfSamples the number of Samples per iteration. If negative, use
 * 		exact hypeIndicator calculation
 * @pre fp->size > 0
 * @pre partp->size >= opt_params.alpha
 * @post partp->size == opt_params.alpha
 */
{
	double val[ fp->size];
	double points[ fp->size*opt_params.dim ];
	double checkRate;
	double minRate = -1;
	int sel = -1;
	int i;

	assert( fp->size > 0 );
	getObjectiveArray( fp->members, fp->size, points );

	while( partp->size > opt_params.alpha )
	{
		hypeIndicator( val, fp->size, 0, bound, nrOfSamples,
					partp->size - opt_params.alpha, points );

		sel = -1;
		for( i = 0; i < fp->size; i++ )
		{
			checkRate = val[i];
			if( sel == -1  || checkRate < minRate ) {
				minRate = checkRate;
				sel = i;
			}
		}
		assert( sel >= 0 );
		assert( sel < fp->size );
		memcpy( points + opt_params.dim*sel, points + opt_params.dim*(fp->size - 1),
					sizeof(double)*opt_params.dim );
		/** Removing Individual <sel> */
		fp->size--;
		fp->members[sel] = fp->members[fp->size];
		partp->size--;
	}
}

void hypeFitnessMating( double bound, int nrOfSamples )
/**
 * Calculates the fitness of all individuals in pp_all based on the hypE
 * indicator
 *
 * @param[in] bound used by hypeIndicator
 * @param[in] nrOfSamples used by hypeIndicator
 */
{
	int i;
	double val[ pp_all->size ];
	double points[ pp_all->size*opt_params.dim ];
	int indices[ pp_all->size ];
	for( i = 0; i < pp_all->size; i++ )
		indices[i] = i;
	getObjectiveArray( indices, pp_all->size, points );
	hypeIndicator( val, pp_all->size, 0, bound, nrOfSamples, pp_all->size,
			points );
	for( i = 0; i < pp_all->size; i++ )
		pp_all->ind_array[ i ]->fitness = val[i];
}

void matingSelectionHYPE()
/**
 * Select parents individuals from pp_all and add them to pp_sel
 */
{
	int winner, winnerFront;
	int opponent, opponentFront;
	int i,j;

	if( mating == 1  )
		hypeFitnessMating( bound, nrOfSamples );
	else
		for( i = 0; i < pp_all->size; i++ )
			pp_all->ind_array[i]->fitness = 0.0;

	pp_sel->size = opt_params.mu;
	for (i = 0; i < opt_params.mu; i++)
	{
		determineIndexAndFront(&front_part, rnd( 0, pp_all->size - 1 ),
				&winner, &winnerFront, !( mating == 2) );
		assert( winner < pp_all->size );
		assert( winner >= 0 );
		for (j = 1; j < opt_params.tournament; j++)
		{
			determineIndexAndFront(&front_part, rnd(0, front_part.size-1 ),
					&opponent, &opponentFront, !( mating == 2) );
			assert( opponent < pp_all->size );
			assert( opponent >= 0 );
			if (opponentFront < winnerFront || (opponentFront == winnerFront &&
					pp_all->ind_array[opponent]->fitness >
			pp_all->ind_array[winner]->fitness ))
			{
				winner = opponent;
				winnerFront = opponentFront;
			}
		}
		addToSelection( i, winner );
	}
}

int weaklyDominates( double *point1, double *point2, int no_objectives )
{
	int better;
	int i = 0;
	better = 1;


	while( i < no_objectives && better )
	{
		better = point1[i] <= point2[i];
		i++;
	}
	return better;
}


comp dominanceCheckInd(int a, int b)
/* objectives are to be minimized */
{
	int i;
	int aWorse = 0;
	int bWorse = 0;

	for (i = 0; i < opt_params.dim && !(aWorse && bWorse); i++)
	{
		aWorse = (aWorse || (pp_all->ind_array[a]->f[i] >
		pp_all->ind_array[b]->f[i] ) );
		bWorse = (bWorse || (pp_all->ind_array[a]->f[i] <
				pp_all->ind_array[b]->f[i] ) );
	}
	if (aWorse && bWorse)
		return incomparable;
	else if (aWorse)
		return b_better_a;
	else if (bWorse)
		return a_better_b;
	else
		return indifferent;
}

