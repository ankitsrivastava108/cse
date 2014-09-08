/*
 * =====================================================================================
 *
 *       Filename:  metrics.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/25/14 09:13:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "../infrastructure/provider.h"
#include "../infrastructure/network.h"
#include "../infrastructure/datacenter.h"
#include "../utils/globals.h"
#include "../evaluation/functions.h"

#include <stdlib.h>
#include <math.h>

long pmin_aux = 0;

void init_batch_metrics(){

    metrics_infrastructure = malloc(sizeof(metrics_infrastructure_s)*sim_convergence.num_batches);
    metrics_application = malloc(sizeof(metrics_application_s)*sim_convergence.num_batches);
    zeroed_batch_metrics(0);

}

void zeroed_batch_metrics(int i){

    metrics_infrastructure[i].utilization.value = 0.0;
    metrics_infrastructure[i].utilization.time_p = 0.0;
    metrics_infrastructure[sim_convergence.num].nodes_energy = 0.0;
    metrics_infrastructure[sim_convergence.num].switches_energy = 0.0;
    metrics_application[i].num_applications = 0;
    metrics_application[i].num_requests = 0;
    metrics_application[i].time_request = 0;
    metrics_application[i].time_proc = 0;
    metrics_application[i].time_network = 0;
    metrics_application[i].f1 = 0;
    metrics_application[i].f2 = 0;
    metrics_application[i].ptotal = 0;
    metrics_application[i].pmin = 0;
    metrics_application[i].hv = 0.0;
    metrics_infrastructure[i].num_servers_on = 0;
    metrics_infrastructure[i].num_busy_vcores = 0;
}

void update_metrics(int i){

    update_metric_energy_nodes( sim_convergence.cycles_convergence+(i*sim_convergence.cycles_batch));
    update_metric_energy_switches(simtime);
     metrics_infrastructure[i].utilization.value/=(float)sim_convergence.cycles_batch;
     if(i != sim_convergence.num_batches - 1){
	metrics_infrastructure[i+1].num_servers_on = metrics_infrastructure[i].num_servers_on;
	metrics_infrastructure[i+1].num_busy_vcores = metrics_infrastructure[i].num_busy_vcores;

     }

}

void end_batch_metrics(){

    free(metrics_infrastructure);
    free(metrics_application);
}

void utilization_metric(provider *pv, int ndatacenter){
    metrics_infrastructure[sim_convergence.num].utilization.value+=(((simtime-metrics_infrastructure[sim_convergence.num].utilization.time_p))*(pv->datacenters[ndatacenter].nw->total_vcores-pv->datacenters[ndatacenter].nw->free_vcores))/pv->datacenters[ndatacenter].nw->total_vcores;
    metrics_infrastructure[sim_convergence.num].utilization.time_p=simtime;
}

void num_applications_metric(provider *pv, int ndatacenter){

    if(sim_convergence.sim_state ==1){
	 metrics_application[sim_convergence.num].num_applications++;
    }
}

void num_requests_metric(provider *pv, int ndatacenter){

    if(sim_convergence.sim_state ==1){
     metrics_application[sim_convergence.num].num_requests++;
    }
}

void num_servers_on_metric(provider *pv, int ndatacenter, int v){
    
    //if(sim_convergence.sim_state ==1){
    metrics_infrastructure[sim_convergence.num].num_servers_on += v;
    //}
}

void num_busy_vcores_metric(provider *pv, int ndatacenter, int v){

    //if(sim_convergence.sim_state ==1){
    metrics_infrastructure[sim_convergence.num].num_busy_vcores += v;
    //}
}

void nodes_energy_metric(provider *pv, int ndatacenter, float energy){

    
    if(sim_convergence.sim_state ==1){
    metrics_infrastructure[sim_convergence.num].nodes_energy += (energy / 1000.0);
    }
}

void switches_energy_metric(provider *pv, int ndatacenter, float energy){


    if(sim_convergence.sim_state ==1){
	metrics_infrastructure[sim_convergence.num].switches_energy += (energy / 1000.0);
    }
}

void time_request_metric(provider *pv, int ndatacenter, long time){

    if(sim_convergence.sim_state ==1){
    metrics_application[sim_convergence.num].time_request += time;
    }
}

void time_network_metric(provider *pv, int ndatacenter, long time){

    if(sim_convergence.sim_state ==1){
    metrics_application[sim_convergence.num].time_network += time;
    }
}

void time_proc_metric(provider *pv, int ndatacenter, long time){

    if(sim_convergence.sim_state ==1){
    metrics_application[sim_convergence.num].time_proc += time;
    }
}

void f1_metric(provider *pv, int ndatacenter, long v){

    if(sim_convergence.sim_state ==1){
	 metrics_application[sim_convergence.num].f1 += v;
    }
}

void f2_metric(provider *pv, int ndatacenter, long v){
    
    if(sim_convergence.sim_state ==1){
	metrics_application[sim_convergence.num].f2 += v;
    }
}

void ptotal_metric(provider *pv, int ndatacenter, long v){

    if(sim_convergence.sim_state ==1){
	metrics_application[sim_convergence.num].ptotal += v;
    }
}

void pmin_metric(provider *pv, int ndatacenter, long v){

	pmin_aux = pmin_aux + v;    
	if(sim_convergence.sim_state ==1 && v > 0){
	    metrics_application[sim_convergence.num].pmin += pmin_aux;
	}
//	
}

void hv_metric(provider *pv, int ndatacenter, float v){
    
    if(sim_convergence.sim_state ==1){
     metrics_application[sim_convergence.num].hv += v;
    }
}

float mean_f1(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
	mean += (metrics_application[i].f1 / (float)metrics_application[i].num_applications);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_f1(){
    int i;
    float std_dev = 0.0;
    float mean = mean_f1();

    for(i = 0; i < sim_convergence.num_batches;i++){
	std_dev += pow((((float)metrics_application[i].f1 / (float)metrics_application[i].num_applications) - mean),2);	
    }	
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_f2(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
	mean += (metrics_application[i].f2 / (float)metrics_application[i].num_applications);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_f2(){
    int i;
    float std_dev = 0.0;
    float mean = mean_f2();

    for(i = 0; i < sim_convergence.num_batches;i++){
	std_dev += pow((((float)metrics_application[i].f2  / (float)metrics_application[i].num_applications)  - mean),2);	
    }	
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_ptotal(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
	mean += (metrics_application[i].ptotal / (float)metrics_application[i].num_applications);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_ptotal(){
    int i;
    float std_dev = 0.0;
    float mean = mean_ptotal();

    for(i = 0; i < sim_convergence.num_batches;i++){
	std_dev += pow((((float)metrics_application[i].ptotal  / (float)metrics_application[i].num_applications)  - mean),2);	
    }	
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_pmin(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
        mean += (metrics_application[i].pmin / (float)metrics_application[i].num_applications);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_pmin(){
    int i;
    float std_dev = 0.0;
    float mean = mean_pmin();

    for(i = 0; i < sim_convergence.num_batches;i++){
        std_dev += pow((((float)metrics_application[i].pmin  / (float)metrics_application[i].num_applications)  - mean),2);
    }
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_time_request(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
        mean += (metrics_application[i].time_request / (float)metrics_application[i].num_requests);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_time_request(){
    int i;
    float std_dev = 0.0;
    float mean = mean_time_request();

    for(i = 0; i < sim_convergence.num_batches;i++){
        std_dev += pow((((float)metrics_application[i].time_request  / (float)metrics_application[i].num_requests)  - mean),2);
    }
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_time_network(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
        mean += (metrics_application[i].time_network / (float)metrics_application[i].num_requests);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_time_network(){
    int i;
    float std_dev = 0.0;
    float mean = mean_time_network();

    for(i = 0; i < sim_convergence.num_batches;i++){
        std_dev += pow((((float)metrics_application[i].time_network  / (float)metrics_application[i].num_requests)  - mean),2);
    }
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_time_proc(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
        mean += (metrics_application[i].time_proc / (float)metrics_application[i].num_requests);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_time_proc(){
    int i;
    float std_dev = 0.0;
    float mean = mean_time_proc();

    for(i = 0; i < sim_convergence.num_batches;i++){
        std_dev += pow((((float)metrics_application[i].time_proc  / (float)metrics_application[i].num_requests)  - mean),2);
    }
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_nodes_energy(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
	mean += (metrics_infrastructure[i].nodes_energy / 3600000.0);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_nodes_energy(){
    int i;
    float std_dev = 0.0;
    float mean = mean_nodes_energy();

    for(i = 0; i < sim_convergence.num_batches;i++){
	std_dev += pow((((float)metrics_infrastructure[i].nodes_energy / 3600000.0) - mean),2);	
    }	
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_switches_energy(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
	mean += (metrics_infrastructure[i].switches_energy / 3600000.0);
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_switches_energy(){
    int i;
    float std_dev = 0.0;
    float mean = mean_switches_energy();

    for(i = 0; i < sim_convergence.num_batches;i++){
	std_dev += pow((((float)metrics_infrastructure[i].switches_energy / 3600000.0) - mean),2);	
    }	
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}

float mean_total_energy(){

    int i;
    float mean = 0.0;

    for(i = 0; i < sim_convergence.num_batches;i++){
	mean += ((metrics_infrastructure[i].switches_energy / 3600000.0) + (metrics_infrastructure[i].nodes_energy / 3600000.0));
    }
    mean /= (float)sim_convergence.num_batches;
    return(mean);
}

float std_dev_total_energy(){
    int i;
    float energy_aux;
    float std_dev = 0.0;
    float mean = mean_total_energy();

    for(i = 0; i < sim_convergence.num_batches;i++){
	energy_aux =  ((metrics_infrastructure[i].switches_energy / 3600000.0) + (metrics_infrastructure[i].nodes_energy / 3600000.0));
	std_dev += pow((energy_aux - mean),2);	
    }	
    std_dev /= (float)sim_convergence.num_batches;
    return(sqrt(std_dev));
}














