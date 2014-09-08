#include "log.h"
#include "../utils/globals.h"
#include "../utils/misc.h"
#include <time.h>
#include <unistd.h>


FILE *fp_infrastructure,*fp_apps, *fp_opt;

void open_log(){

	char log[100];
	
	sprintf(log, "%s.infr.log", logname);
	if((fp_infrastructure = fopen(log, "w")) == NULL){
		printf("WARNING: cannot create log output file");
	}
	
	sprintf(log, "%s.apps.log", logname);
	if((fp_apps = fopen(log, "w")) == NULL){
		printf("WARNING: cannot create log output file");
	}
	sprintf(log, "%s.opt.log", logname);
	if((fp_opt = fopen(log, "w")) == NULL){
		printf("WARNING: cannot create log output file");
	}

	
}

void write_log_init() {

	fprintf(fp_infrastructure,"\nCloud Simulation Environment Log\n\n");
	fprintf(fp_infrastructure,"Simulation information:\n\n");
}

void write_opt(long f1, long f2, long ptotal){
     fprintf(fp_opt,"%ld,%ld,%ld\n",f1, f2, ptotal);

}
void write_log_partial_infrastructure(int batch) {

    fprintf(fp_infrastructure,"Batch: %d\n",batch);
    fprintf(fp_infrastructure,"Utilization:          %f\n",metrics_infrastructure[batch].utilization.value);
    fprintf(fp_infrastructure,"Number of servers on: %d\n",metrics_infrastructure[batch].num_servers_on);
    fprintf(fp_infrastructure,"Number of busy cores: %d\n",metrics_infrastructure[batch].num_busy_vcores);
    fprintf(fp_infrastructure,"Nodes energy:         %f\n",(metrics_infrastructure[batch].nodes_energy / 3600.0));
    fprintf(fp_infrastructure,"Switches energy:      %f\n",(metrics_infrastructure[batch].switches_energy / 3600.0));
    fprintf(fp_infrastructure,"Total energy:         %f\n",(metrics_infrastructure[batch].switches_energy / 3600.0) + (metrics_infrastructure[batch].nodes_energy / 3600.0));
    

}

void write_log_partial_applications(int batch) {

    fprintf(fp_apps,"Batch: %d\n",batch);
    fprintf(fp_apps,"Num applications:        %d\n",metrics_application[batch].num_applications);
    fprintf(fp_apps,"Num requests:            %d\n",metrics_application[batch].num_requests);
    fprintf(fp_apps,"Time request:            %f\n",metrics_application[batch].time_request / (float)metrics_application[batch].num_requests);
    fprintf(fp_apps,"Time request(proc):      %f\n",metrics_application[batch].time_proc / (float)metrics_application[batch].num_requests);
    fprintf(fp_apps,"Time request(network):   %f\n",metrics_application[batch].time_network / (float)metrics_application[batch].num_requests);
    fprintf(fp_apps,"f1:                      %f\n",metrics_application[batch].f1 / (float)metrics_application[batch].num_applications);
    fprintf(fp_apps,"f2:                      %f\n",metrics_application[batch].f2 / (float)metrics_application[batch].num_applications);
    fprintf(fp_apps,"ptotal:                  %f\n",metrics_application[batch].ptotal / (float)metrics_application[batch].num_applications);
    fprintf(fp_apps,"pmin:                     %f\n",metrics_application[batch].pmin / (float)metrics_application[batch].num_applications);
    fprintf(fp_apps,"Hypervolume:             %f\n",metrics_application[batch].hv / (float)metrics_application[batch].num_applications);


}

void write_log_final(time_t start_time, time_t end_time) {

     fprintf(fp_infrastructure,"\n___________________________________\n");
    fprintf(fp_infrastructure,"Mean nodes energy:         %f\n",mean_nodes_energy());
    fprintf(fp_infrastructure,"Std Dev nodes energy:         %f\n",std_dev_nodes_energy());
    fprintf(fp_infrastructure,"Mean switches energy:      %f\n",mean_switches_energy());
    fprintf(fp_infrastructure,"Std Dev switches energy:      %f\n",std_dev_switches_energy());
    fprintf(fp_infrastructure,"Mean total energy:         %f\n",mean_total_energy());
    fprintf(fp_infrastructure,"Std Dev total energy:         %f\n",std_dev_total_energy());
    
     fprintf(fp_apps,"\n___________________________________\n");
    fprintf(fp_apps,"Mean time request:      %f\n",mean_time_request());
     fprintf(fp_apps,"Std Dev time request:  %f\n",std_dev_time_request());
    fprintf(fp_apps,"Mean request proc:      %f\n",mean_time_proc());
     fprintf(fp_apps,"Std Dev request proc:  %f\n",std_dev_time_proc());
    fprintf(fp_apps,"Mean time network:      %f\n",mean_time_network());
     fprintf(fp_apps,"Std Dev time network:  %f\n",std_dev_time_network());

     fprintf(fp_apps,"Mean f1:               %f\n",mean_f1());
     fprintf(fp_apps,"Std Dev f1:            %f\n",std_dev_f1());
     fprintf(fp_apps,"Mean f2:               %f\n",mean_f2());
     fprintf(fp_apps,"Std Dev f2:            %f\n",std_dev_f2());
     fprintf(fp_apps,"Mean ptotal:           %f\n",mean_ptotal());
     fprintf(fp_apps,"Std Dev ptotal:        %f\n",std_dev_ptotal());
    fprintf(fp_apps,"Mean pmin:             %f\n",mean_pmin());
     fprintf(fp_apps,"Std Dev pmin:          %f\n",std_dev_pmin());



}

void close_log(){
	
    fclose(fp_opt);
    fclose(fp_infrastructure);
    fclose(fp_apps);
}
	
