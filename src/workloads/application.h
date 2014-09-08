/*
 * =====================================================================================
 *
 *       Filename:  application.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/24/14 11:23:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _application
#define _application

#include "../infrastructure/provider.h"
#include "../infrastructure/balancer.h"

typedef struct structure{

    int nlayers;
    int *nodeslayer;
    int *maxbw;
    int **bw;

} structure;

typedef struct application{

    int id;
    int type;
    int size;
    int sizel1;
    int sizel2;
    int sizel3;
    long atime;
    long rtime;
    long etime;
    long last_time_req;
    int maxreq;
    int last_req;
    int processed_requests;
    int gen_requests;
    int basereq;
    int req_chunk;
    int vm_request_policy;
    int *assigned_vcores;
    struct structure str;
    struct app_frontend *app_fe;
    struct application *next;
    struct application *prev;

} application;

typedef struct allocated_applications{

    int num_alloc_apps;
    struct application *first_app;
    struct application *last_app;
   
} allocated_applications;

void init_application(provider *pv, application **app, event *ev);

void rem_application(application *app);

void init_bw(application *app);

void change_app_fe(application *app, struct app_frontend *app_fe);

void change_assigned_cores(application *app, int *assigned_vcores);

void printApplication(application *app);
#endif
