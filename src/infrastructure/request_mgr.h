/*
 * =====================================================================================
 *
 *       Filename:  request_processor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/28/14 08:27:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jose A. Pascual (), joseantonio.pascual@ehu.es
 *   Organization:  UPV/EHU
 *
 * =====================================================================================
 */

#ifndef _request_mgr
#define _request_mgr

void process_request(request *req);

void process_request_phases(request *req);

void check_remove_request(request *req);
#endif
