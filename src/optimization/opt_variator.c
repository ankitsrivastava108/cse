/*
 * =====================================================================================
 *
 *       Filename:  aux_function.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/14 07:33:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "opt_variator.h"
#include "opt_engine.h"
#include "problem.h"
#include "mutation.h"
#include "crossover.h"
#include "../utils/rand.h"

#include <stdio.h>
#include <string.h>

#define STANDARD_SIZE 32200
 
int current_max_size; 

population global_population;

individual *new_individual(int length, int *vector, int i){

    individual *return_ind;

    return_ind = (individual *) malloc(sizeof(individual));
    return_ind->bit_string = (int *) malloc(sizeof(int) * length);
    if(i == 0){
	memcpy(return_ind->bit_string, vector, sizeof(int) * length);
    }
    else{
	initialize_ind(return_ind->bit_string, vector, length);
    }

    return_ind->length = length;

    /* evaluating two objective functions */
    return_ind->f1 = eval_f1(return_ind);
    return_ind->f2 = eval_f2(return_ind);  
    return_ind->valid = check_constrainsts(return_ind);
    //printf("f1: %ld ",return_ind->f1);
    //printf("f2: %ld ",return_ind->f2);
    //printf("Valid: %d \n",return_ind->valid);
    return (return_ind);
}

void initialize_ind(int *ind,int *core_set, int length){

    int j, k, l;
    j = 0;
    
     
    while ( j < length )
    {   
	k = rnd(0, length - 1);
	for (l=0; l<j; l++){
	    if (core_set[k] == ind[l] )
		break;
	}
	if(l == j){ 
	    ind[l] = core_set[k];    
	    //printf("%d ",ind[l]);
	    j++;
	}
    }
    //printf("\n");
}

int add_individual(individual *ind) 
{     
    int i;                                                                    
     int identity = -1;                                                                         
     individual **tmp; /* in case we need to double array size */                                                                                                              
     if(global_population.size == 0)                                                           
     {                                                                            
          current_max_size = STANDARD_SIZE;                                                     
          global_population.individual_array =  
               (individual **) malloc(current_max_size * sizeof(int)); 
          global_population.last_identity = -1;                                                 
     }                                                                                          
     identity = popf(&global_population.free_ids_stack);                                               
     if (identity == -1)                                                                         
     {                                                                                          
          identity = global_population.last_identity + 1;                                      
          global_population.last_identity++;                                                    
     } 
     global_population.size++;                                                                                                                                                 
     if (global_population.last_identity < current_max_size)  
     {                                                                                          
          global_population.individual_array[identity] = ind;                                   
     }                                                                                         
     else /* enlargement of individual array (size doubling) */                                  
     {                                                         
          tmp = (individual **) malloc(sizeof(int) * current_max_size * 2);   
          for (i = 0; i < current_max_size; i++) 
               tmp[i] = global_population.individual_array[i]; 
          current_max_size = current_max_size * 2;
          free(global_population.individual_array);
          global_population.individual_array = tmp;
     }
     return (identity);
}                                           

void free_individual(individual *ind) 
{
     free(ind->bit_string);
     free(ind);
}

long get_objective_value(int identity, int i)
{
     individual *temp_ind;
     long objective_value = -1;
         
     temp_ind = get_individual(identity);
     if(i == 0)
          objective_value = temp_ind->f1;
     else
          objective_value = temp_ind->f2;
     return (objective_value);
}

int get_valid_value(int identity)
{
     individual *temp_ind;
     int valid_value = -1;
         
     temp_ind = get_individual(identity);
     valid_value = temp_ind->valid;
     return (valid_value);
}

individual *get_individual(int identity) 
/* Returns a pointer to the individual corresponding to 'identity'. */
{
     if((identity > global_population.last_identity) || (identity < 0)){ 
          return (NULL);
     }
     return (global_population.individual_array[identity]);
}

int get_first()
/* Returns the identity of first individual in the global population. */
{
     return (get_next(-1));
}


int get_next(int identity) 
/* Takes an identity an returns the identity of the next following
   individual in the global population. */
{ 
     int next_id;

     if((identity < -1) || (identity > global_population.last_identity))
          return (-1);
  
     next_id = identity + 1;
     while(next_id <= global_population.last_identity) {
          if(global_population.individual_array[next_id] != NULL)
               return (next_id);
          next_id++;
     }   

     return(-1);
}

int remove_individual(int identity)
/* Removes the individual with ID 'identity' from the global population. */
{
     individual *temp;
     if((identity > global_population.last_identity) || (identity < 0)) 
          return (1);
     temp = get_individual(identity);
     if(temp == NULL)
          return (1);

     global_population.individual_array[identity] = NULL;

     if(identity == global_population.last_identity)
     {   
          global_population.last_identity--;
     }   
     else
     {   
          pushf(&global_population.free_ids_stack, identity);
     }   

     global_population.size--;

     free_individual(temp);
    
     return (0);
}


int clean_population()
/* Frees memory for all individuals in population and for the global
   population itself. */
{
     int current_id;

     if (NULL != global_population.individual_array)
     {   
        current_id = get_first();
        while(current_id != -1) 
        {
           remove_individual(current_id);
           current_id = get_next(current_id);
        }
        
        free_stack(&global_population.free_ids_stack);
        free(global_population.individual_array);

        global_population.individual_array = NULL;
        global_population.size = 0;
        global_population.last_identity = -1; 
        
     }   
        
        
     return (0);
}

int free_stack(stack *st)
{
     while(popf(st) != -1) 
     {   
          /* don't do anything, pop() does it already. */
     }   
     return (0);
}

int pushf(stack *st,int id) 
{
     stack_node *new_el = (stack_node *) malloc(sizeof(stack_node));
     if (new_el == NULL)
          return(1);
     new_el->next = st->top;
     new_el->identity = id; 
     st->top = new_el;
     st->size++;
     return(0);
}

int popf(stack *st)
{
     int identity;
     stack_node *next;
     if(st->size == 0)
          return(-1);
     identity = st->top->identity;
     next = st->top->next;
     free(st->top);
     st->top = next;
     st->size--;
     return(identity);
}

int variate(int *parents, int *offspring, int length)
/* Performs variation (= recombination and mutation) according to
   settings in the parameter file.
   Returns 0 if successful and 1 otherwise.*/
{    
     int i,  k;

     /* copying all individuals from parents */
     for(i = 0; i < opt_params.mu; i++)
     {    
          offspring[i] = 
               add_individual(copy_individual(get_individual(parents[i]), length));
     }
     if((((double)opt_params.mu/2) - (int)(opt_params.mu/2)) != 0)
          k = opt_params.mu - 1; 
     else
          k = opt_params.mu;

     /* do recombination */
     for(i = 0; i < k; i+= 2)
     {  
	switch(opt_params.cotype){
	    case CORND:
		rndcross(get_individual(offspring[i]),get_individual(offspring[i + 1]));
		break;
	    case COTA:
		topoawarecross(get_individual(offspring[i]),get_individual(offspring[i + 1]));
		break;
	    default:
		 panic("variate: No crossover type!");
	}

     }
     /* do mutation */
    for(i = 0; i < opt_params.mu; i++){
	switch(opt_params.mtype){
	    case MRND:
		rnd_mutate_ind(get_individual(offspring[i]));
		break;
	    case MDIST:
		topoaware_mutate_ind(get_individual(offspring[i]));
		break;
	    default:
		panic("variate: No mutation type!");
	}
    }

     return (0);
}

individual *copy_individual(individual *ind, int length)
/* Allocates memory for a new individual and copies 'ind' to this place.
   Returns the pointer to new individual.
   Returns NULL if allocation failed. */
{
     individual *return_ind;
     int i;

     return_ind = (individual *) malloc(sizeof(individual));
     return_ind->bit_string = (int *) malloc(sizeof(int) * length);

     for (i = 0; i < length; i++)
          return_ind->bit_string[i] = ind->bit_string[i];

     return_ind->f1 = ind->f1;
     return_ind->f2 = ind->f2;  

     return_ind->length = ind->length;

     return (return_ind);
}

void write_output_file()
{
     int j, current_id;
     FILE *fp_out;
     individual *temp;
    
     fp_out = fopen("res", "w");
     current_id = get_first();
     while (current_id != -1) 
     {    
          temp = get_individual(current_id);
          fprintf(fp_out, "%d ", current_id); 
          fprintf(fp_out, "f1: %ld ", temp->length - 
                  get_objective_value(current_id, 0));
    
          fprintf(fp_out, "f2: %ld ", temp->length -
                  get_objective_value(current_id, 1));
    
          for (j = 0; j < temp->length; j++)
          {
               fprintf(fp_out, "%d ", temp->bit_string[j]);
          }
          fprintf(fp_out, "\n");
          current_id = get_next(current_id);
     }   

     fclose(fp_out);
}
void write_output()
{
     int j, current_id;
     individual *temp;
    
     current_id = get_first();
     while (current_id != -1) 
     {    
          temp = get_individual(current_id);
          printf("%d ", current_id); 
          printf("f1: %ld ", get_objective_value(current_id, 0));
    
          printf("f2: %ld ", get_objective_value(current_id, 1)); 
    
          for (j = 0; j < temp->length; j++)
          {
               printf("%d ", temp->bit_string[j]);
          }
          printf("\n");
          current_id = get_next(current_id);
     }   

}

int *eval_pareto_set()
{
     int current_id;
     int aux_id = -1;
     individual *temp;
     long nodes_aux;
     long nodes = opt_params.nw->nnodes+1;
    
     current_id = get_first();
     while (current_id != -1) 
     {    
          temp = get_individual(current_id);
	  nodes_aux = on_servers_total(opt_params.nw, temp->bit_string,  temp->length); 
	  if(temp->valid && nodes_aux < nodes){
	    aux_id = current_id;
	    nodes = nodes_aux;
	  }
          current_id = get_next(current_id);
     }   
     if(aux_id == -1){
	return(NULL);
     }
     else{
	temp = get_individual(aux_id);
	return(temp->bit_string);
     }

}


int contains(int n,int size, int *ind){

    int i;
	
    for (i=0;i<size;i++){
	if(n==ind[i]){
	    return(i);
	}
    }
    return(-1);
}

int repeated(int size, int *ind,int * check){

    int i,j,num,aux;

    num=0;
    for (i=0;i<size;i++){
	aux=ind[i];
	for (j=0;j<i;j++){
	    if(i!=j && aux==ind[j]){
		check[j]=1;
		num++;
		break;
	    }
	}
    }
    return(num);
}

