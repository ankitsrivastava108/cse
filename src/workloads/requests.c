#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "application.h"
#include "requests.h"
#include "../utils/rand.h"
#include "../utils/globals.h"

//#define PERIOD 1440
#define PERIOD 86400 

#include "../infrastructure/broker.h"

int get_data (int reqType, int* data)
{
	switch (reqType)
	{
		case 1: //p1
			data[0] = rnd(5,10);
			data[1] = 0;
			data[2] = 0;
			data[3] = rnd(5,500);
			break;
		case 2: //p2
			data[0] = rnd(20,25);
			data[1] = 0;
			data[2] = 0;
			data[3] = rnd(5,500);
			break;
		case 3: //r
			data[0] = rnd(5,10);
			data[1] = rnd(10,50);
			data[2] = rnd(20,400);
			data[3] = rnd(5,500);
			break;
		case 4: //w
			data[0] = rnd(5,10);
			data[1] = rnd(10,50);
			data[2] = rnd(20,300);
			data[3] = rnd(5,500);
			break;
		case 5: //p3
			data[0] = rnd(1,7);
			data[1] = 0;
			data[2] = 0;
			data[3] = rnd(5,10);
			break;
		case 6: //r
			data[0] = rnd(5,10);
			data[1] = rnd(15,20);
			data[2] = rnd(150,300);
			data[3] = rnd(5,10);
			break;	
		case 7: //w
			data[0] = rnd(10,15);
			data[1] = rnd(30,35);
			data[2] = rnd(10,15);
			data[3] = rnd(5,10);
			break;	
		case 8: //p4
			data[0] = rnd(1,7);
			data[1] = 0;
			data[2] = 0;
			data[3] = rnd(5,10);
			break;							
		default:
			break;
	}


	return (0);
}

int get_reqs(int pattern, int time, int maxreq)
{
	
	int reqs;
	double r;
				
	switch (pattern){
		case 1:
			//Sinusoidal
			//sin = Amplitud * sin (2*pi*time/periodo + desfase)
			reqs = (maxreq/2) * sin((2*M_PI*time)/PERIOD + 4.75) + (maxreq/2);
			//Variacion
			r = randomperc(); 
			if (r < 0.5)
				reqs += rnd(-maxreq*0.30, maxreq*0.30);
			else if (r < 0.9)
				reqs += rnd(-maxreq*0.50, maxreq*0.50);
			else if (r < 0.98)
				reqs += rnd(-maxreq*1.50, maxreq*1.50);
			else
				reqs += rnd(-maxreq*3, maxreq*3);	
			if (reqs < 0)
				reqs = 0;
			break;
		default:
			break;	
	}
	return (reqs);
}


int gen_requests(provider *pv, int app_id, int app_type, int t_i, app_frontend *app_fe, int duration, int pattern, int maxreq, int req_chunk, int basereq, long last_time){

	int i=0, time, numreqs, day;
	double r;
	struct request *req;
	int req_min = 0;
	duration /= 1000;
	int chunk = 0;
	for (time = last_time; time < duration && chunk < req_chunk; time++)
	{
	    chunk++;	
		//Segun dia de la semana, variar maxreq
		if (time % PERIOD == 0)
		{
			day = ((time/PERIOD)%7)+1;
			
			if (day < 6){ //(lunes-viernes)
				basereq = maxreq + rnd((int)-maxreq*0.30, (int)maxreq*0.30);
			}
			else{ //sabado-domingo
			    basereq = rnd((int)maxreq*0.30, (int)maxreq*0.50);
			}
		}
		
		//Obtener numero de peticiones
		numreqs = get_reqs(pattern, time, basereq);
		
		//printf("%d %d %d %d **", numreqs, pattern, time, basereq);
		
		for (i=0; i<numreqs; i++) 
		{
			r = randomperc();
			req = malloc(sizeof(struct request));
			req->ndatacenter=0;
			req->id = app_id;
			req->num_events = 0;
			req->remaining_events = 0;
			req->phase = 0;
			req->pv = pv;
			req->app_fe = app_fe;
			//req->data = malloc(sizeof(int) * 4);
			//req->assigned_vm = malloc(sizeof(int) * 3);
			req->assigned_vm[0] = -1;
			req->assigned_vm[1] = -1;
			req->assigned_vm[2] = -1;
			///req->vm_req = malloc(sizeof(vm_requests*) * 3);
			switch(app_type)
			{
				case 1:  //1 capa
					if (r < 0.5)
						req->reqType = 1; //p1
					else
						req->reqType = 1; //p2	
					break;
				case 2:  //2 capas. R/W BD
					if (r < 0.35)
						req->reqType = 1; //r	
					else if (r < 0.7)
						req->reqType = 4; //w
					else
						req->reqType = 3; //p3		
					break;	
				case 3:  //2 capas. Varios nodos de R y uno de W
					if (r < 0.60)
						 req->reqType = 3; //r	
					else if (r < 0.9)
						 req->reqType = 4; //w
					else
						 req->reqType = 1; //p4		
					break;		
				default:
					break;
			}
			req->time_sec = (time  * 1000) + t_i ;
			get_data (req->reqType, req->data);
			//printf("%d %d %d %d %d %d -%d- %d\n", req->time_sec, req->reqType, req->data[0], req->data[1], req->data[2], req->data[3], duration, t_i);
			add_request(pv,req);
		}	
	    	req_min += numreqs;
		if(time % 60 == 0 && time != 0){
		    //printf("%d\n", req_min);
		    req_min = 0;
		}
	}	
	if(time < duration){
	    app_fe->app->last_time_req = time;
	   insert_event((time  * 1000) + t_i, REQUEST_GENERATION_CALENDAR, basereq, app_fe); 
	}
	//printf("End generating %d requests/minute for application %d\n", req_min, app_id);
	return(0);
}

