/*
 * =====================================================================================
 *
 *       Filename:  request_time.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/21/14 06:53:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#include "../utils/misc.h"

float node_energy_model(int total_cores, int num_cores_used, long time){

    int model = 0;
    float energy = 0.0;

    switch(model){
	case 0:
	    if(num_cores_used == 0){
		energy = 10.0;
	    }
	    else if(num_cores_used == 1){
		energy = 160.0;
	    }
	    else{
		energy = 160.0 + (num_cores_used *(40.0 / (float)(total_cores - 1))); 
	    }
	    break;
	default:
	    panic("No energy model for the nodes!\n");
	    break;
    }
    energy *= (float)time;
    //printf("C %f %ld %d %d\n",energy,time,total_cores, num_cores_used);
    return(energy);
}

float switch_energy_model(int total_ports, int num_ports_used, long time){

    int model = 0;
    float energy = 0.0;

    switch(model){
	case 0:
	    if(num_ports_used == 0){
		energy = 10.0;
	    }
	    else if(num_ports_used == 1){
		energy = 31.0;
	    }
	    else{
		energy = 31.0 + (num_ports_used * (69.0 / (float)(total_ports - 1))); 
	    }
	    break;
	default:
	    panic("No energy model for the nodes!\n");
	    break;
    }
//    printf("S %f %ld %d\n",energy,time,num_ports_used);
    energy *= (float)time;
    return(energy);
}
