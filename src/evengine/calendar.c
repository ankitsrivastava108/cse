#ifndef _main
#define _main

#include "../utils/globals.h"
#include "../infrastructure/provider.h"
#include "eventmgr.h"
#include "calendar.h"
#include <stdio.h>
#include <time.h>

#endif

/**
* Insert an event in the calendar queue
*@param ts 		The timestamp when the event should be executed.
*@param type 	The type of the event.
*@param node	The node where the event should be executed.
*@param port	The port where the event should be executed.
*@return 		0 if the event has been inserted correctly,
*				-1 if there is any error.
*/
long insert_event(long ts, cal_event_type type, int v, void *vp)
{
    cal_metaevent *aux, *old;	// auxiliar metaevents to check the timestamps in the calendar.
    long is_cq=1;				// do we have to insert in the first place of a day.
    if (ts<simtime){
	printf("%d\n",type);
	panic("Ts lower than simtime\n");
	}
    old=cq[ts%days];
    for (aux=old; aux!=NULL && aux->timestamp<=ts; aux=aux->next){
	is_cq=0;
	if (aux->timestamp==ts){ // Exists a metaevent for this timestamp.
	    cal_event *ev;
	    ev=(cal_event *)malloc(sizeof(cal_event));		
	    ev->v=v;						
	    ev->vp=vp;

	    ev->next=aux->eventSet[type];	// pointer to the next element in the event-set
	    aux->eventSet[type]=ev;			// let the event-set point to this element.
	    return 0;
	}
	old=aux;	// old points to the last metaevent which timestamp is lesser than ts.
    }
	
    {
    cal_metaevent *meta;				// the new metaevent.
    cal_event *ev;
    long i;
    meta=(cal_metaevent *)malloc(sizeof(cal_metaevent));	// alloc memory for the new metaevent.
    meta->timestamp=ts;										// timestamp of the new metaevent.
    ev=(cal_event *)malloc(sizeof(cal_event));
    ev->v=v;
    ev->vp=vp; 
    ev->next=NULL;											// last should be NULL.
    for (i=0;i<TYPES_OF_EVENT;i++)
	meta->eventSet[i]=NULL;
    meta->eventSet[type]=ev;			// let the event-set point to this element.
	
    if(is_cq){
	meta->next=cq[ts%days];
	cq[ts%days]=meta;
    }
    else{
	meta->next=old->next;				// insert meta in the correct position
	old->next=meta;						// between old and old->next
    }

    if (++num_metaevents>days*2){	// Rebuild to mantain queues small.
	rebuild(days*2);
    }
    return 0;
    }
}

/**
* Executes the event-set in the calendar queue.
*@return 		0 if the event-set has been executed correctly,
*				-1 if there is any error.
*/
void execute()
{
	long i;
	cal_event *ev;	//the event to execute.
	cal_event *aux;	//auxiliar event.
	cal_metaevent *meta;	//an auxiliar metaevent
	while(num_metaevents>0){
		// Executes all events within a metaevent.
		//printf("metaevents %d!\n",num_metaevents);
	    if ((meta=cq[cur_day])!=NULL)
		{
			for (i=0; i<TYPES_OF_EVENT; i++){
				ev=meta->eventSet[i];
				while (ev!=NULL)
				{
					dispatcher(i, ev->v, ev->vp);
					aux=ev;
					ev=ev->next;
					free(aux);
				}
			}
		}
		cq[cur_day]=cq[cur_day]->next;
		free(meta);
		num_metaevents--;
		if (num_metaevents*2<days){
			rebuild(days/2);
		}

		// Look for the next metaevent
		if (num_metaevents>0)
		{
			unsigned long min=LMAX,curr;
			unsigned long min_day;
			for (i=1; i<=days; i++){
				if (cq[(cur_day+i)%days]!=NULL){
					if ((curr=cq[(cur_day+i)%days]->timestamp)==simtime+i) // a day during the next year.
					{
						min=curr;
						min_day=(cur_day+i)%days;
						break;
					}
					else if (curr<min)
					{
						min=curr;
						min_day=(cur_day+i)%days;
					}
				}
			}
			cur_day=min_day;
			if (simtime>min)
			{
				print_cq();
				panic("Trying to execute an event from the past\n");
			}
			simtime=min;
		}
	}
	finish_calendar();
}

/**
* Rebuilds a Calendar.
*
* Builds a calendar with the same meta-events but a different number of buckets;
*@param new The new number of days.
*/
void rebuild(long new)
{
	cal_metaevent **cq2;		// A pointer to the old calendar
	cal_metaevent *last[new];	// A pointer to the last element in each bucket of the new calendar
	cal_metaevent 	*aux,		// A meta event in the old calendar
					*aux2,	// To look in the new calendar
					*old;	// A meta event in the new calendar
	long i,n,is_cq;

#ifdef DEBUG
	printf("REBUILDING, new days %d\n",new);
#endif

	if (new<8)
		return;

#ifdef DEBUG
	printf("OLD CALENDAR\n");
	print_cq();
#endif

	cq2=cq;
	cq=(cal_metaevent **)malloc(new*sizeof(cal_metaevent*));
	for (i=0; i<new;i++)
	{
		cq[i]=NULL;
		last[i]=NULL; 
	}

	for (i=0; i<days; i++)
	{
		aux=cq2[i];
		while(aux!=NULL)
		{
			n=aux->timestamp%new;
			aux2=cq[n];
			if(aux2==NULL)
			{
				cq[n]=aux;
				last[n]=aux;
				aux=aux->next;
				cq[n]->next=NULL;
			}
			else
			{
				is_cq=1;
				while(aux2!=NULL && aux2->timestamp<aux->timestamp)
				{
					is_cq=0;
					old=aux2;
					aux2=aux2->next;
				}

				if (is_cq)
				{
					cq[n]=aux;
					aux=aux->next;
					cq[n]->next=aux2;
				}
				else
				{
					old->next=aux;
					
					if (aux2==NULL)
						last[n]=aux;
					
					aux=aux->next;
					old->next->next=aux2;
					
				}
			}
		}
	}
	for (i=0;i<new;i++)
		if (last[i]!=NULL)
			last[i]->next=NULL;
	days=new;
	cur_day=simtime%new; 
	
	free(cq2);
#ifdef DEBUG
	printf("AFTER REBUILD\n");
	print_cq();
#endif
}

void print_cq()
{
	long i;
	long halt=0;
	
	cal_metaevent *aux;	//an auxiliar metaevent
	for (i=0;i<days;i++)
	{
		printf("%ld\t[ ",i);
		aux=cq[i];
		while(aux!=NULL)
		{
			if (aux->timestamp==114)
				halt=1;
			printf("%ld, ",aux->timestamp);
			aux=aux->next;
		}
		printf("end ]\n");
	}
	if (halt)
		panic("...");
}

/**
* Dispatcher function.
*
* Calls the appropiate function to execute an event. Should be an array of functions.
*/
void dispatcher(cal_event_type t, int v, void *vp)
{
	switch(t){
		case ARRIVAL_CALENDAR:
	    	    run_arrival_event(v);
    		    break;
		case DEPARTURE_CALENDAR:
	    	    run_departure_event(v);
    		    break;
       		case WARMUP_CALENDAR:
	    	    run_warmup_event();
    		    break;
		case BATCH_CALENDAR:
	    	    run_batch_event();
    		    break;
		case REQUEST_CALENDAR:
	    	    run_request_event(vp);
    		    break;
		case REQUEST_GENERATION_CALENDAR:
	    	    run_request_generation_event(v,vp);
    		    break;
		case TRANSMISSION_CALENDAR:
		    run_transmission_event(v, vp);
		    break;
		case TRANSMISSION_NODE_CALENDAR:
		    run_transmission_node_event(v, vp);
		    break;
		case COMPUTATION_CALENDAR:
		    run_computation_event(v, vp);
		    break;
		///case TRANSMISSION_BD_CALENDAR:
		  //  run_transmission_bd_event(v, vp);
		    //break;
	//	case TRANSMISSION_NODE_BD_CALENDAR:
	//	    run_transmission_node_bd_event(v, vp);
	//	    break;
		
		case FINISH_CALENDAR:
		    providerSet[0].finished=TRUE;
		    num_metaevents=0;
		    //printf("ProviderSet 0 finishes at time %ld\n",simtime);
	    	    break;
		default:
		    panic("Unexpected type of event in the calendar\nAborting . . .\n");
	}
}


/**
* Initializes the calendar.
*
* Creates the needed structures and insert the starting events in the event-set.
*/
void init_calendar()
{
	long i;
	cur_day=0;
	simtime=0;
	days=8;
	cq=(cal_metaevent **)malloc(days*sizeof(cal_metaevent*));
	for (i=0; i<days; i++)
		cq[i]=NULL;
}

void finish_calendar()
{
    long i;
	long pcq=0; /// print_cq...?
	
	if (pcq)
	   print_cq();
	for (i=0; i<days; i++)
	    free(cq[i]);
	free(cq);
}
