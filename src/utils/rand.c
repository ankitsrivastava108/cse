# include <stdio.h>
# include <stdlib.h>
# include <math.h>

void randomize(unsigned long long seed){
	srandom(seed);
}
/* Fetch a single random number between 0.0 and 1.0 */
double randomperc()
{
    return((float)random()/(float)RAND_MAX);
}

/* Fetch a single random integer between low and high including the bounds */
int rnd (int low, int high)
{
    int res;
    if (low >= high)
    {
        res = low;
    }
    else
    {
        res = low + (randomperc()*(high-low+1));
        if (res > high)
        {
            res = high;
        }
    }
    return (res);
}

double randf( double from, double to )
{
	double j;
	j = from + (double)( (to-from)*rand() / ( RAND_MAX + 1.0) );
	return (j);
}

