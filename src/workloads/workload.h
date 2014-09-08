#ifndef _workload
#define _workload

#include "../evengine/event.h"

void read_trace();

void gen_workload(int phase);

void gen_application(event *ev, int requests, int id, long atime);

#endif
