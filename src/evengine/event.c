/**
* @file
* Event management functions used in trace-driven simulation.
*
* Event: A task that must be done (not occurred yet).
* Occurred: A task that has been done (total or partially completed).
*/

/*
FSIN Functional Simulator of Interconnection Networks
Copyright (2003-2005) 
J. Miguel-Alonso, A. Gonzalez, J. Navaridas

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "../utils/globals.h"
#include "../evengine/event.h"

#include <string.h>
//#include <math.h>

/**
* Initializes an event queue.
* @param q a pointer to the queue to be initialized.
*/
void init_event (event_q *q) {
	q->head = NULL;
	q->tail = NULL;
}

/**
* Adds an event to a queue.
* @param q a pointer to a queue.
* @param i the event to be added to q.
*/
void ins_event (event_q *q, event i) {
	event_n *e;
	e=malloc(sizeof(event_n));
	e->ev=i;
	e->next = NULL;
	
	if(q->head==NULL) // Empty Queue
	{
		q->head = e;
		q->tail = e;
	}
	else
	{
		q->tail->next = e;
		q->tail = e;
	}
}

/**
* Looks at the first event in a queue.
* @param q A pointer to the queue.
* @return The first event in the queue (without using nor modifying it).
*/
event *head_event (event_q *q) {
	if (q->head==NULL) {
		panic("Getting event from an empty queue");
	}
	return &q->head->ev;
}

/**
* Deletes the first event in a queue.
* @param q A pointer to the queue.
*/
void rem_head_event (event_q *q) {
	event_n *e;
	if (q->head==NULL) {
		panic("Deleting event from an empty queue");
	}
	e = q->head;
	q->head=q->head->next;
	free (e);
	if (q->head==NULL) q->tail=NULL;
}

/**
* Is a queue empty?.
* @param q A pointer to the queue.
* @return TRUE if the queue is empty FALSE in other case.
*/
bool_t event_empty (event_q *q){
	return (q->head==NULL);
}

