/*
* =====================================================================================
*
*       Filename:  crossover.c
*
*    Description:  
*
*        Version:  1.0
*        Created:  06/16/14 07:08:47
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
*   Organization:  UPV/EHU
*
* =====================================================================================
*/

#include "opt_engine.h"
#include "opt_variator.h"
#include "problem.h"
#include "../utils/rand.h"

void rndcross (individual *parent1, individual *parent2){

    int i, j, aux, auxj;

    int *check,*check2;
    int *child1, *child2;

    if (randomperc() <= opt_params.pco){ 
	check = malloc(parent1->length * sizeof(int));
	check2 = malloc(parent1->length * sizeof(int));
	child1 = malloc(parent1->length * sizeof(int));
	child2 = malloc(parent1->length * sizeof(int));

	for(i=0;i<parent1->length;i++){
	    check[i]=0;
	    check2[i]=0;
	}
	for(i=0;i<parent1->length;i++){
	    if(randomperc() <= 0.5){
		child1[i] = parent1->bit_string[i];
		child2[i] = parent2->bit_string[i];
	    }      
	    else{
		child2[i] = parent1->bit_string[i];
		child1[i] = parent2->bit_string[i];
	    }
	}
	repeated(parent1->length, child1,check);
	repeated(parent1->length, child2,check2);
	auxj=0;
	for(i=0;i<parent1->length;i++){
	    if(check[i]==1){
		for(j=auxj;j<parent1->length;j++){
		    if(check2[j]==1){
			aux=child1[i];
			child1[i]=child2[j];
			child2[j]=aux;
			auxj=j+1;
			check2[j]=0;
			check[i]=0;
			break;
		    }
		}
	    }
	}
	for(i=0;i<parent1->length;i++){
	    if(check[i]==1){
		for(j=0;j<parent1->length;j++){
		    if((contains(child2[j],parent1->length,child1))==-1){
			aux=child2[j];
			child2[j]=child1[i];
			child1[i]=aux;
			break;
		    }
		}
	    }
	    else if(check2[i]==1){
		for(j=0;j<parent1->length;j++){
		    if((contains(child1[j],parent1->length,child2))==-1){
			aux=child1[j];
			child1[j]=child2[i];
			child2[i]=aux;
			break;
		    }
		}
	    }
	}
	free(check);
	free(check2);
    free(parent1->bit_string);
    free(parent2->bit_string);
    parent1->bit_string = child1;
    parent2->bit_string = child2;
    parent2->f1 = eval_f1(parent2); 
    parent2->f2 = eval_f2(parent2);
    parent1->f1 = eval_f1(parent1); 
    parent1->f2 = eval_f2(parent1);
    parent1->valid = check_constrainsts(parent1);
    parent2->valid = check_constrainsts(parent2);
    }
}


void topoawarecross (individual *parent1, individual *parent2){

    int i,j,aux,auxj;
    int **mat_aux,**mat_aux2;
    int *sum1,*sum2,*diff;
    int *check,*check2;
    int *child1, *child2;

    if (randomperc() <= opt_params.pco){
	sum1 = malloc(parent1->length*sizeof(int));
	sum2 = malloc(parent1->length*sizeof(int));
	diff = malloc(parent1->length*sizeof(int));
	check = malloc(parent1->length*sizeof(int));
	check2 = malloc(parent1->length*sizeof(int));
	child1 = malloc(parent1->length*sizeof(int));
	child2 = malloc(parent1->length*sizeof(int));

	mat_aux=malloc(parent1->length*sizeof(int*));
	mat_aux2=malloc(parent1->length*sizeof(int*));

	for(i=0;i<parent1->length;i++){
	    mat_aux[i] = malloc(parent1->length*sizeof(int));
	    mat_aux2[i] = malloc(parent1->length*sizeof(int));
	    for(j=0;j<parent1->length;j++){
		mat_aux[i][j] = opt_params.bw[i][j] * distance_network_cores(opt_params.nw, parent1->bit_string[i],parent1->bit_string[j]);
		mat_aux2[i][j] = opt_params.bw[i][j] * distance_network_cores(opt_params.nw, parent2->bit_string[i],parent2->bit_string[j]);
	    }
	    sum1[i]=0;
	    sum2[i]=0;
	    check[i]=0;
	    check2[i]=0;
	}
	for(i=0;i<parent1->length;i++){
	    for(j=0;j<parent1->length;j++){
		sum1[i]+=mat_aux[i][j]+mat_aux[j][i];
		sum2[i]+=mat_aux2[i][j]+mat_aux2[j][i];
	    }
	    diff[i]=sum1[i]-sum2[i];
	}
	for(i=0;i<parent1->length;i++){
	    if(diff[i]<=0){
		child1[i] = parent1->bit_string[i];
		child2[i] = parent2->bit_string[i];
	    }
	    else{
		child2[i] = parent1->bit_string[i];
		child1[i] = parent2->bit_string[i];
	    }
	}
	repeated(parent1->length, child1,check);
	repeated(parent1->length, child2,check2);
	auxj=0;
	for(i=0;i<parent1->length;i++){
	    if(check[i]==1){
		for(j=auxj;j<parent1->length;j++){
		    if(check2[j]==1){
			aux=child1[i];
			child1[i]=child2[j];
			child2[j]=aux;
			auxj=j+1;
			check2[j]=0;
			check[i]=0;
			break;
		    }
		}
	    }
	}

	for(i=0;i<parent1->length;i++){
	    if(check[i]==1){
		for(j=0;j<parent1->length;j++){
		    if((contains(child2[j],parent1->length,child1))==-1){
			aux=child2[j];
			child2[j]=child1[i];
			child1[i]=aux;
			break;
		    }
		}
	    }
	    else if(check2[i]==1){
		for(j=0;j<parent1->length;j++){
		    if((contains(child1[j],parent1->length,child2))==-1){
			aux=child1[j];
			child1[j]=child2[i];
			child2[i]=aux;
			break;
		    }
		}
	    }
	}	
	free(check);
	free(check2);
	free(sum1);
	free(sum2);
	free(diff);
	for(i=0;i<parent1->length;i++){
	    free(mat_aux[i]);
	    free(mat_aux2[i]);
	}

	free(mat_aux);
    free(mat_aux2);
    free(parent1->bit_string);
    free(parent2->bit_string);
    parent1->bit_string = child1;
    parent2->bit_string = child2;
    parent2->f1 = eval_f1(parent2); 
    parent2->f2 = eval_f2(parent2);
    parent1->f1 = eval_f1(parent1); 
    parent1->f2 = eval_f2(parent1);
    parent1->valid = check_constrainsts(parent1);
    parent2->valid = check_constrainsts(parent2);
    
    }
}

