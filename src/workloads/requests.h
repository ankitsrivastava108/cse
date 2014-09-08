
#ifndef _requests
#define _requests

//#include "../workloads/application.h"
#include "../infrastructure/provider.h"
#include "../infrastructure/vm.h"

typedef struct app_frontend app_frontend;

typedef struct request {
    
    int id;
    provider *pv;
    int ndatacenter;
    long time_sec;
    int reqType;
    int data[4];
    int num_events;
    int remaining_events;
    int phase;
    int assigned_vm[3];
    vm_requests *vm_req[3];
    app_frontend* app_fe;
    struct request *next;
    struct request *prev;

} request;


int gen_requests(provider *pv, int app_id, int app_type, int t_i, app_frontend *app_fe, int duration, int pattern, int maxreq, int req_chunk, int basereq, long last_time);

int get_reqs(int pattern, int time, int maxreq);

int get_data (int reqType, int* data);

#endif
