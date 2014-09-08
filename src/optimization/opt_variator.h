/*
 * =====================================================================================
 *
 *       Filename:  aux_functions.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/14 07:37:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _opt_variator
#define _opt_variator

#include <stdlib.h>

typedef struct stack_node_t
{
    int identity;
    struct stack_node_t *next;
} stack_node;

typedef struct stack_t
{
    stack_node *top;
    int size;
} stack;

struct individual_t
{
         /* *********| added for LOTZ2 |*****************/
int *bit_string; /*  the genes */
int length;      /*  length of the bit_string */
int valid;
long f1;       /*  objective value (length - leading ones) */
long f2;       /*  objective value (length - trailing zeros) */
		        /* *********| addition for LOTZ2 end |**********/
};

typedef struct individual_t individual;

typedef struct population_t
{
int size; /*  size of the population */
individual **individual_array; /*  array of pointers to individuals */
int last_identity; /*  identity of last individual */  
stack free_ids_stack; /*  stack for keeping freed ids with remove */
	
} population;


extern population global_population; /*  defined in variator_internal.c */

individual *new_individual(int length, int*vector, int i);

void initialize_ind(int *ind,int *core_set, int length);

long get_objective_value(int identity, int i);

int get_valid_value(int identity);

individual *get_individual(int identity);

int add_individual(individual *ind);

individual *copy_individual(individual *ind, int length);

void free_individual(individual *ind);

int get_first();

int get_next(int identity); 

int remove_individual(int identity);

int clean_population();

int free_stack(stack *st);

int pushf(stack *st, int id);

int popf(stack *st);

int variate(int *parents, int *offspring, int length);

void write_output_file();

void write_output();

int contains(int n,int size, int *ind);

int repeated(int size, int *ind,int * check);

int *eval_pareto_set();
#endif
