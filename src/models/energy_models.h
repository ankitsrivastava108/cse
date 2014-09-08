/*
 * =====================================================================================
 *
 *       Filename:  request_time.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/21/14 06:53:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

float node_energy_model(int total_cores, int num_cores_used, long time);

float switch_energy_model(int total_ports, int num_ports_used, long time);
