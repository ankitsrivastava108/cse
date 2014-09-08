#include "workload.h"
#include "../infrastructure/network.h"
#include "../utils/globals.h"
#include "../utils/rand.h"

#include <string.h>

int app_id_aux = 0;

/**
* The trace reader dispatcher selects the format type and calls to the correct trace read.
*
* The selection reads the first character in the file. This could be: '#' for dimemas, 
* 'c', 's' or 'r' for fsin trc, and '-' for alog (in complete trace the header is "-1",
* or in filtered trace could be "-101" / "-102"). This is a very naive decision, so we
* probably have to change this, but for the moment it works.
*@see read_dimemas
*@see read_fsin_trc
*@see read_alog
*/
void read_trace(){

	FILE *ftrc;
	char buffer[512];
	char *tok;
	char sep[]=" \t";
	event ev;
	long id=0;

	if((ftrc = fopen(trcfile, "r")) == NULL){
		printf("%s\t\t",trcfile);
		panic("Trace file not found in current directory\n");
	}

	while(fgets(buffer, 512, ftrc) != NULL) {
	   	if(buffer[0] != '\n' && buffer[0] != '#') {
			if(buffer[strlen(buffer) - 1] == '\n')	
				buffer[strlen(buffer) - 1] = '\0';
		        	ev.type = ARRIVAL;
				ev.id = id++;
		    		tok = strtok( buffer, sep);
		        	ev.atime = atoi(tok);
				tok=strtok(NULL, sep);  // from
				ev.rtime = atoi(tok);
				tok=strtok(NULL, sep);  // from
				ev.sizel1 = atoi(tok);
				tok=strtok(NULL, sep);  // from
				ev.sizel2 = atoi(tok);
				tok=strtok(NULL, sep);  // from
				ev.sizel3 = atoi(tok);
				tok=strtok(NULL, sep);  // from
				ev.app_type = atoi(tok);
				tok=strtok(NULL, sep);  // from
				ev.maxreq = atoi(tok);
				tok=strtok(NULL, sep);  // from
				ev.vm_request_policy = atoi(tok);
				ins_event(&providerSet[0].events,ev);
				}
			}
	fclose(ftrc);
}

void gen_workload(int phase){

    float current_occupancy = 0.0;
    int cores_busy = 0;
    //int cores_totales = 13824;
    int cores_totales = 4096;
    long time=0;
    event ev;
    provider *pv;
    network *nw;
    if(phase == 0){
	while(current_occupancy < occupancy){
	    gen_application(&ev, 0, app_id_aux, time);
	    ins_event(&providerSet[0].events,ev);
	    cores_busy += (ev.sizel1 + ev.sizel2 +  ev.sizel3); 
	    current_occupancy = (float)cores_busy / (float)cores_totales;
	    time += 1000;
	    app_id_aux++;
	}
    }
    else{
	pv = &providerSet[0];
	nw = pv->datacenters[0].nw;
	current_occupancy = (( (float)cores_totales - (float)nw->free_vcores) / (float)cores_totales);
	if(current_occupancy < occupancy){
	time = simtime + 10000;
	    gen_application(&ev, reqgen, app_id_aux, time);
	    ins_event(&providerSet[0].events,ev);
	    cores_busy += (ev.sizel1 + ev.sizel2 +  ev.sizel3); 
	    current_occupancy = (float)cores_busy / (float)cores_totales;
	    app_id_aux++;
	}
    }
}

void gen_application(event *ev, int requests, int id, long atime){

    ev->type = ARRIVAL;
    ev->id = id;
    ev->atime = atime; 
    ev->rtime = rnd(1000, 25000) * 1000;
    if(randomperc() <= 0.5){
	ev->app_type = 2;
	ev->sizel1 = rnd(10, 100);
	ev->sizel2 = 1;
	ev->sizel3 = 0;
    }
    else{
	ev->app_type = 3;
	ev->sizel1 = rnd(10, 250);
	ev->sizel2 = rnd(1, (ev->sizel1 / 10));
	ev->sizel3 = 1;
    }
    ev->maxreq = rnd(20, 30);
    ev->vm_request_policy = 3;
    ev->requests = requests;
}








