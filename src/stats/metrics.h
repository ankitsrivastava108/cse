/*
 * =====================================================================================
 *
 *       Filename:  metrics.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/25/14 09:14:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */
#ifndef _metrics
#define _metrics

#include "../infrastructure/provider.h"

typedef struct utilization_s{
    
    int time_p;
    float value;

} utilization_s;

typedef struct metrics_infrastructure_s {

    struct utilization_s utilization;
    int num_servers_on;
    int num_busy_vcores;
    float nodes_energy;
    float switches_energy;

} metrics_infrastructure_s;

typedef struct metrics_application_s {

    int num_applications;
    int num_requests;    
    float time_request;
    float time_network;
    float time_proc;
    long f1;
    long f2;
    long ptotal;
    long pmin;
    float hv;

} metrics_application_s;

metrics_infrastructure_s *metrics_infrastructure;

metrics_application_s *metrics_application;

void init_batch_metrics();

void zeroed_batch_metrics(int i);

void update_metrics(int i);

void end_batch_metrics();

void utilization_metric(provider *pv, int ndatacenter);

void num_applications_metric(provider *pv, int ndatacenter);

void num_requests_metric(provider *pv, int ndatacenter);

void num_servers_on_metric(provider *pv, int ndatacenter, int v);

void num_busy_vcores_metric(provider *pv, int ndatacenter, int v);

void nodes_energy_metric(provider *pv, int ndatacenter, float energy);

void switches_energy_metric(provider *pv, int ndatacenter, float energy);

void time_request_metric(provider *pv, int ndatacenter, long time);

void time_network_metric(provider *pv, int ndatacenter, long time);

void time_proc_metric(provider *pv, int ndatacenter, long time);

void f1_metric(provider *pv, int ndatacenter, long v);

void f2_metric(provider *pv, int ndatacenter, long v);

void ptotal_metric(provider *pv, int ndatacenter, long v);

void pmin_metric(provider *pv, int ndatacenter, long v);

void hv_metric(provider *pv, int ndatacenter, float v);

float mean_f1();

float mean_f2();

float mean_ptotal();

float mean_pmin();

float mean_nodes_energy();

float mean_switches_energy();

float mean_total_energy();

float mean_time_request();

float mean_time_proc();

float mean_time_network();

float std_dev_f1();

float std_dev_f2();

float std_dev_ptotal();

float std_dev_pmin();

float std_dev_nodes_energy();

float std_dev_switches_energy();

float std_dev_total_energy();

float std_dev_time_request();

float std_dev_time_proc();

float std_dev_time_network();


#endif


