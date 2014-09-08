/*
 * =====================================================================================
 *
 *       Filename:  batch.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/25/14 10:08:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "../evengine/calendar.h"
#include "../stats/metrics.h"
#include "../utils/globals.h"
#include "../stats/log.h"

void init_warm_up(){

    sim_convergence.sim_state=0;
    sim_convergence.num=0;
    sim_convergence.num_batches=3;
    sim_convergence.threshold=0.4;
    sim_convergence.average=0.0;
    sim_convergence.variance=0.0;    
    sim_convergence.cycles_convergence=50000000;
    sim_convergence.cycles_batch= 50000000;

    insert_event(sim_convergence.cycles_convergence,WARMUP_CALENDAR,0,NULL);

}
void check_warm_up(){
    
    // Preliminary code. 
    //if (){
    int i;
    
    //printf("Init bach %ld \n",simtime);
    update_metrics(0);
    sim_convergence.sim_state=1;
    
    for(i=2;i<=sim_convergence.num_batches;i++){
	zeroed_batch_metrics(i-1); 
    }
    for(i=1;i<=sim_convergence.num_batches;i++){
	insert_event(simtime+(i*sim_convergence.cycles_batch),BATCH_CALENDAR,0,NULL);
    }
}

void check_batches(){
    if(sim_convergence.num<sim_convergence.num_batches){
	update_metrics(sim_convergence.num);
	write_log_partial_infrastructure(sim_convergence.num);
	write_log_partial_applications(sim_convergence.num);
	//printf("End batch %d\n",sim_convergence.num);
	//printf("batch %d %f \n",sim_convergence.num,metrics_infrastructure[sim_convergence.num].utilization.value); 
	sim_convergence.num++;
    }
    if(sim_convergence.num == sim_convergence.num_batches){
	//printf("*********FIN**************\n");
	insert_event(simtime,FINISH_CALENDAR,0,NULL);
    }
}


