/*
 * =====================================================================================
 *
 *       Filename:  scaler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/14 07:49:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _scaler
#define _scaler

int add_vm(provider *pv, int ndatacenter,int id,int n, scaler_add_policy_t policy);

int remove_vm(provider *pv, int ndatacenter,int id, int n,scaler_rem_policy_t policy);
#endif
