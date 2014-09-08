/** 
* @file
* Definition of event, event queue & event list, used for trace driven simulation. 
*/

#ifndef _event
#define _event

#include "../utils/misc.h"

/** 
* Types of event.
* 
* It should be 'r' for a reception or 's' for a sent
*/
typedef enum event_t {
	
	ARRIVAL = 'a',	///< Reception
} event_t;

/**
* Structure that defines an event.
*
* It contains all the needed atributes for distinguish it:
* type (S/R), the second node id (destination/source), a task id,
* the length in packets, and the packets sent/arrived.
*/
typedef struct event {
	event_t type;		///< Type of the event.
	long id;
	long atime;			// Arrival time.
	long rtime;			// Running time.
	int sizel1;
	int sizel2;
	int sizel3;
	int maxreq;
	int app_type;
	int requests;
	int vm_request_policy;
} event;
   
/**
* Structure that defines a node for event queue & lists.
* @see event_q
* @see event_l
*/
typedef struct event_n {
	event ev;				///< The event in this position.
	struct event_n * next;	///< The next node in the list/queue.
} event_n;

/**
* Structure that defines an event queue.
*/
typedef struct event_q {
	event_n *head;	///< A pointer to the first event node (for removing).
	event_n *tail;	///< A pointer to the last event node (for enqueuing).
} event_q;

/**
* Structure that defines a list of events.
*/
typedef struct event_l {
	event_n * first;	///< A pointer to the first element of the list.
} event_l;


/**
* Initializes an event queue.
* @param q a pointer to the queue to be initialized.
*/
void init_event (event_q *q);

/**
* Adds an event to a queue.
* @param q a pointer to a queue.
* @param i the event to be added to q.
*/
void ins_event (event_q *q, event i);

/**
* Uses the first event in the queue.
*
* Takes an event an increases its packet count.
* When it reaches the length of the event, this is erased from the queue.
* @param q A pointer to a queue.
* @param i A pointer to the event to do.
*/
void do_event (event_q *q, event *i);
/**
* Looks at the first event in a queue.
* @param q A pointer to the queue.
* @return The first event in the queue (without using nor modifying it).
*/
event *head_event (event_q *q);
/**
* Deletes the first event in a queue.
* @param q A pointer to the queue.
*/
void rem_head_event (event_q *q);

/**
* Is a queue empty?.
* @param q A pointer to the queue.
* @return TRUE if the queue is empty FALSE in other case.
*/
bool_t event_empty (event_q *q);

/** 
* Initializes all ocurred events lists.
*
* There is a list in each router for each posible source of messages.
* @param l A pointer to the list to be initialized.
*/
void init_occur (event_l **l);

/**
* Inserts an event's occurrence in an event list.
*
* If the event is in the list, then its count is increased. Otherwise a new event is created
* in the occurred event list.
* @param l A pointer to a list.
* @param i The event to be added.
*/
void ins_occur (event_l **l, event i, long ts);

/**
* Has an event completely occurred?.
*
* If it has totally occurred, this is, the event is in the list and its count is equal to
* its length, then it is deleted from the list.
* @param l a pointer to a list.
* @param i the event we are seeking for.
* @return TRUE if the event has been occurred, elseway FALSE
*/
bool_t occurred (event_l **l, event i);
#endif /* _event */

