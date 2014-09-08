/** 
* @file
* Definition of a literal that contains a relation between a string & a number.
*
* This tools are used for read the fsin.conf file & the command line parameters(arguments)
* of the simulation & print them in the final brief.
* @see get_conf.c
*/

#include "misc.h"

#ifndef _literal
#define _literal

/**
* Structure to contain a literal.
* This is a #### between a string & a number.
* @see get_conf.c
* @see print_results.c
*/
typedef struct literal_t {
	int value;
	char * name;
} literal_t;

#define LITERAL_END { 0, NULL } ///< This must be the last literal in an array.

// Some declarations
bool_t literal_value(literal_t * literal, char *  name, int * value);
bool_t literal_name (literal_t * literal, char ** name, int   value);

#endif /* _literal */
