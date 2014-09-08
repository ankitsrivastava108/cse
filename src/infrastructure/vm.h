/*
 * =====================================================================================
 *
 *       Filename:  vm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/19/14 08:53:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _vm
#define _vm

#define MAX_REQUESTS 100000
#define NUM_PHASES_NETWORK 4
#define NUM_PHASES_COMP 3 


typedef struct application application; 
typedef struct request request;

typedef struct computational_phase{

    int instructions;
    int remaining_instructions;
    long time_init;
    float mips_assigned;
    long time_total;
    long time_arrival;
    long time_event;
    int nlayer;

}computational_phase;

typedef struct network_phase{

    void* path;
    int nlayer;
    long time_init;
    float data_remaining;
    float bw;
    long time_arrival; 
    long time_remaining; 
    long time_event;
    struct vm_requests *vm_req;

}network_phase;

typedef struct vm_requests{
    
    int active;
    request *req;
    computational_phase *comp_phase;
    network_phase *net_phase;
    struct vm_requests *next;
    struct vm_requests *prev;

}vm_requests;

typedef struct virtual_machine {

    int vcore;
    int mips;
    int num_requests;
    int num_active;
    vm_requests *vm_req_first;
    vm_requests *vm_req_last;
} virtual_machine;

virtual_machine **init_vm(application *app);


void assign_request_vm(request *req, int vm_layer, int vm_id);

computational_phase *init_comp_phase();

network_phase *init_net_phase(int nlayer);

void delete_request_vm(request *req, int vm_layer, int vm_id);

void assign_network_resources(request *req, int vm_layer_1, int vm_id_1, int vm_layer_2, int vm_id_2);

void assign_computational_resources(request *req, int nlayer);

float get_instruction_seconds(virtual_machine *vm);

void init_computation(request *req, int num_instructions, int nlayer);

float modify_vm_comp_requests_arrival(request *req, int nlayer);

void modify_vm_comp_requests_depart(request *req, int nlayer);

void init_transfer(request *req, float bw, float data);

void init_transfer_node(request *req, float data);

void modify_transfer(vm_requests *vm_req, float bw_new);
#endif
