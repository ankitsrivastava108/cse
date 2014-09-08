/*
 * =====================================================================================
 *
 *       Filename:  application.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/24/14 11:23:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "application.h"
#include "../utils/misc.h"
#include "../utils/rand.h"
#include "requests.h"
#include "../utils/globals.h"
#include <math.h>
#include <stdlib.h>

void init_application(provider *pv, application **app, event *ev){


    *app = (application*) malloc(sizeof(application));

    (*app)->id = ev->id;
    (*app)->type = ev->app_type;
    (*app)->vm_request_policy = ev->vm_request_policy;
    (*app)->size = ev->sizel1 + ev->sizel2 + ev->sizel3;
    (*app)->sizel1 = ev->sizel1;
    (*app)->sizel2 = ev->sizel2;
    (*app)->sizel3 = ev->sizel3;
    (*app)->atime = ev->atime;
    (*app)->rtime = ev->rtime;
    (*app)->maxreq = ev->maxreq;
    (*app)->last_time_req = 0;
    (*app)->req_chunk = 10000;
    //app->etime = simtime + t_deploy;
    (*app)->etime = simtime + 600000; 
    (*app)->assigned_vcores = NULL;
    (*app)->app_fe = NULL;
    (*app)->next = NULL;
    (*app)->prev = NULL;
    init_bw(*app);
    //gen_requests(app->type, app->etime, app->rtime, pattern, maxreq);
}

void rem_application(application *app){

    int i;

    free(app->assigned_vcores);
    free(app->str.maxbw);
    free(app->str.nodeslayer);
    for(i = 0; i < app->size; i++){
	free(app->str.bw[i]);
    }
    free(app->str.bw);
    free(app);
}

void init_bw(application *app){
    
    int i, j, d, dr1, dr2, dw1, dw2, max;

    switch(app->type){
	case 1:
	    app->str.nlayers = 1;
	    app->str.nodeslayer = malloc(app->str.nlayers*sizeof(int));
	    app->str.nodeslayer[0] = app->sizel1;
	    app->str.maxbw = malloc(app->size * sizeof(int));
	    app->str.bw = malloc(app->size * sizeof(int*));
	    for(i = 0; i < app->size; i++){
		app->str.bw[i] = malloc(app->size*sizeof(int));
	    }
	    d = rnd(100,500);
	    for(i = 0; i < app->size; i++){
		max=0;
		for(j = 0; j < app->size; j++){
		    if(i != j){ 
			app->str.bw[i][j] = d;
			if(d > max)
			    max=d;
		    }
		    else{   
			app->str.bw[i][j] = 0;
		    }	
		}
		app->str.maxbw[i]=max;
		//printf("%d ",app->str.maxbw[i]);
	    }	    
	    break;
	case 2:
	    app->str.nlayers = 2;
	    app->str.nodeslayer = malloc(app->str.nlayers*sizeof(int));
	    app->str.nodeslayer[0] = app->sizel1;
	    app->str.nodeslayer[1] = app->sizel2;
	    app->str.maxbw = malloc(app->size * sizeof(int));
	    app->str.bw = malloc(app->size * sizeof(int*));
	    for(i = 0; i < app->size; i++){
		app->str.bw[i] = malloc(app->size*sizeof(int));
	    }
	    dr1 = rnd(30,100);
	    dr2 = rnd(100,200);
	    dw1 = rnd(50,150);
	    dw2 = rnd(50,100);
	    for(i = 0; i < app->size; i++){
	    max=0;
		for(j = 0; j < app->size; j++){
		    if(i < app->str.nodeslayer[0] && j == app->size - 1){
			app->str.bw[i][j] = dr1 + dw1;
			if(dr1 + dw1 > max)
			    max = dr1 + dw1;
		    }
		    else if(j < app->str.nodeslayer[0] && i == app->size - 1){
			app->str.bw[i][j] = dr2 + dw2;
			if( (dr2 + dw2) > max)
			    max = dr2 + dw2;
		    } 
		    else{
			app->str.bw[i][j] = 0;
		    }
		}
		app->str.maxbw[i] = max;
		//printf("%d ",app->str.maxbw[i]);
	    }
	    break;
	case 3:
	    app->str.nlayers = 3;
	    app->str.nodeslayer = malloc(app->str.nlayers*sizeof(int));
	    app->str.nodeslayer[0] = app->sizel1;
	    app->str.nodeslayer[1] = app->sizel2;
	    app->str.nodeslayer[2] = app->sizel3;
	    app->str.maxbw = malloc(app->size * sizeof(int));
	    app->str.bw = malloc(app->size * sizeof(int*));
	    for(i = 0; i < app->size; i++){
		app->str.bw[i] = malloc(app->size*sizeof(int));
	    }
	    dr1 = rnd(30,100);
	    dr2 = rnd(100,200);
	    dw1 = rnd(50,150);
	    dw2 = rnd(50,100);
	    for(i = 0; i < app->size; i++){
		max=0;
		for(j = 0; j < app->size; j++){
		    if(i < app->str.nodeslayer[0] && j == app->size - 1){
			app->str.bw[i][j] = dw1;
			if(dw1 > max)
			    max = dw1;
		    }
		    else if(j < app->str.nodeslayer[0] && i == app->size - 1){
			app->str.bw[i][j] = dw2;
			if(dw2 > max)
			    max = dw2;
		    }
		    else if(i < app->str.nodeslayer[0] && (app->str.nodeslayer[0] <= j) && (j < app->size - 1)){
			app->str.bw[i][j] = dr1;
			if(dr1 > max)
			    max = dr1;
		    }
		    else if((app->str.nodeslayer[0] <= i) && (i < app->size - 1) && j < app->str.nodeslayer[0] ){
			app->str.bw[i][j] = dr2;
			if(dr2 > max)
			    max=dr2;
		    }
		    else if(i == app->size - 1 && (app->str.nodeslayer[0] <= j ) && (j < app->size - 1)){
			app->str.bw[i][j] = dw1;
			if(dw1 > max)
			    max = dw1;
		    }
		    else{
			app->str.bw[i][j] = 0;
		    }
		}
		app->str.maxbw[i]=max;
		//printf("%d ",app->str.maxbw[i]);
	    }
	    break;

	default:
	    panic("No application type!!!\n");
	    break;
    }
}	

void change_app_fe(application *app, app_frontend *app_fe){
   
    app->app_fe = app_fe;
}

void change_assigned_cores(application *app, int *assigned_vcores){

    app->assigned_vcores = assigned_vcores;
}
void printApplication(application *app){
        
    int i,j;

    printf("Type: %d\n",app->type);
    printf("Num nodes: %d\n",app->size);
    printf("Num layers: %d -- ",app->str.nlayers);
    for(i=0; i<app->str.nlayers;i++){
	printf("%d --",app->str.nodeslayer[i]);
    }
    printf("\n");
    for(i = 0; i < app->size; i++){  
	for(j = 0; j < app->size; j++){
	    printf("%d ",app->str.bw[i][j]);
	}
	printf("\n");
    }
    printf("\n");
}
