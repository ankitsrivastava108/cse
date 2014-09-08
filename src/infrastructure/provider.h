#ifndef _provider
#define _provider

#include "../evengine/event.h"
//#include "../infrastructure/datacenter.h"

typedef struct provider{

	event_q events;
	bool_t finished;
	struct broker *brk;
	int ndc;
	struct datacenter *datacenters;
	struct load_balancer *lb;

}provider;

int create_provider(int ndc);

void delete_provider();
#endif
