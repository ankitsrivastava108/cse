/*
 * =====================================================================================
 *
 *       Filename:  opt_selector.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/14 07:10:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _opt_selector
#define _opt_selector

typedef struct ind_st  /* an individual */
{
    int index;
    int valid;
    long *f; /* objective vector */
    double fitness;
} ind;

typedef struct pop_st  /* a population */
{
    int size;
    int maxsize;
    ind **ind_array;
} pop;

typedef struct front_st /* a single nondominated set */
{
        int     size;
        int*    members;
} front;

typedef struct fp_st  /* a hierarchy of multiple nondominated sets */
{
        int     max_fronts;
        int     fronts;
        int     size;
        front*  front_array;
} fpart;

typedef enum
{
	a_better_b, b_better_a, incomparable, indifferent
} comp;

extern pop *pp_all;
extern pop *pp_new;
extern pop *pp_sel;
extern fpart front_part;


void initialize();

pop* create_pop(int size, int dim);
ind* create_ind(int dim);

void free_memory(void);
void free_pop(pop *pp);
void complete_free_pop(pop *pp);
void free_ind(ind *p_ind);

void selectopt();
int dominates(ind *p_ind_a, ind *p_ind_b);
int is_equal(ind *p_ind_a, ind *p_ind_b);
double calcDistance(ind *p_ind_a, ind *p_ind_b);

void read_ini(int *identity);
void read_var(int *identity);
void print_pop(pop *pp);

void write_sel();
void write_arc();
int check_file(char *filename);
void getObjectiveArray( int* A, int sizea, double* pointArray );
void addToSelection(int i, int c );
void chk_free( void* handle);
void free_front_part(fpart* partp);
#endif
