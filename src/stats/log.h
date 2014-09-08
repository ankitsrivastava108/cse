#ifndef _stats
#define _stats

#include <time.h>

char logname[30];

void open_log();

void write_log_init();

void write_log_partial_infrastructure(int batch);

void write_log_partial_applications(int batch);

void write_log_final(time_t start_time, time_t end_time);	

void close_log();	

void write_opt(long f1, long f2, long ptotal);
#endif
