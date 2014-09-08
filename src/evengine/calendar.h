#ifndef _calendar
#define _calendar

// Undefine to perform faster simulations.
#define COUNT_EVENTS

#ifndef LMAX
#define LMAX 4294967295
#endif

/** 
* @file
* Definition of calendar_event, calendar_metaevent & event_type, used event-driven simulation of INs. 
*/

/**
* Type of the events inside our modified calendar queue.
* 
* Important because metaevents have a pointer for each type of events, so it should be as reduced as posible.
* Event_types should be arranged in the order they are used in simulation.
*/
typedef enum cal_event_type
{
	ARRIVAL_CALENDAR,			// CPU Burst.
	DEPARTURE_CALENDAR,			// CPU Burst.
	REQUEST_CALENDAR,
	REQUEST_GENERATION_CALENDAR,
	WARMUP_CALENDAR,
	BATCH_CALENDAR,
	TRANSMISSION_CALENDAR,
	TRANSMISSION_NODE_CALENDAR,
	///TRANSMISSION_NODE_BD_CALENDAR,
	//TRANSMISSION_BD_CALENDAR,
	COMPUTATION_CALENDAR,
	FINISH_CALENDAR,		// A nodes finishes all its tasks.
	TYPES_OF_EVENT
}cal_event_type;

/**
* Events in the calendar.
*
* The minimum chunk of information in use by the simulator. 
*/
typedef struct cal_event
{
	int v;
	void *vp;
	struct cal_event *next;
}cal_event;

/**
* Meta-events contains all the events to perform in a given timestamp
*
* A meta event is a set of events and the timestamp when they have to be performed. 
*/
typedef struct cal_metaevent
{
	unsigned long timestamp;
	cal_event *eventSet[TYPES_OF_EVENT];
	struct cal_metaevent *next;
}cal_metaevent;

/**
* Amount of days in our calendar.
* 
* The number of days is variable during the simulation and tries to adapt to the number of metaevents
* in such a way that calendar is balanced and so, the acces to the calendar is almost constant.
*/
long days;

/**
* The current number of events in our calendar
*/
long num_metaevents;

#ifdef COUNT_EVENTS
/**
* The total number of executed events.
*/
long total_events;
#endif

/**
* Calendar Queue.
*
* The calendar queue that contains the event set and which is the heart of the event-driven simulation.
*/
cal_metaevent **cq;

/**
* Current day.
*
* The day (queue) that has the lowest day within the calendar.
*/
unsigned long cur_day;

/**
* Current date
*
* The lowest day within the calendar.
*/
unsigned long simtime;

/**
* Dispatcher function.
*
* Calls the appropiate function to execute an event.
*/
void dispatcher(cal_event_type t, int v, void *vp);

/**
* Initializes the calendar.
*
* Creates the needed structures and insert the starting events in the event-set.
*/
void init_calendar();


/**
* Finishes the calendar.
*
* Destroy all the structures and show results.
*/
void finish_calendar();

/**
* Writes the current calendar queue in the standard output
*/
void print_cq();

/**
* Insert an event in the calendar queue
*@param ts 		The timestamp when the event should be executed.
*@param type 	The type of the event.
*@param node	The node where the event should be executed.
*@param port	The port where the event should be executed.
*@return 		0 if the event has been inserted correctly,
*				-1 if there is any error.
*/
long insert_event(long ts, cal_event_type type, int v,void *vp);

/**
* Rebuilds a Calendar.
*
* Duplicates the number of days in the calendar, useful to mantain reduced the number of events per day.
*/
void rebuild(long new);

/**
* Executes the event-set in the calendar queue.
*@return 		0 if the event-set has been executed correctly,
*				-1 if there is any error.
*/
void execute();

//void run_arrival_event(int node, int port);

//void run_departure_event(int node, int port); 
#endif
