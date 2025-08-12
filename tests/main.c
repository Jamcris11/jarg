#include <stdio.h>
#include <stdlib.h>
#include "../include/jarg.h"

static void	handle_print(const struct jarg* arg, int argc, char** argv);

const struct jarg jarg_args[] = {
/*	  id,		param,	description,								flags,					handle			*/
	{ "-h",		NULL, 	"example of print usage", 					JARGF_USAGE|JARGF_OPT,	handle_print },
	{ "-f",		"file", "example of arg opt with param", 			JARGF_OPT,				handle_print },
	{ "-x",		NULL, 	"example of arg opt without param", 		JARGF_OPT,				handle_print },
	{ "FILE",	NULL, 	"example of required and any count arg",	JARGF_ANY_COUNT,		handle_print },
};

static void
handle_print(const struct jarg* arg, int argc, char** argv)
{
	printf("[ ");
	for ( int i = 0; i < argc; i++ ) {
		printf("%s -> %s,", arg->identifier, argv[i]);
	}
	printf(" ]\n");
}

static enum jarg_result 
unrecognised_arg(char* cmd)
{
	fprintf(stderr, "ERROR: unrecognised arg %s\n", cmd);
	return JARG_ERROR;
}

int
main(int argc, char** argv)
{
	enum jarg_result res = jarg_handle_args(
		sizeof(jarg_args)/sizeof(jarg_args[0]), 
		unrecognised_arg, 
		argc, 
		argv
	);

	if ( res == JARG_ERROR ) {
		fprintf(stderr, "Error: %s\n", jarg_error_str());
		return 1;
	} else if ( res == JARG_QUIT_EARLY ) {
		return 0;
	}

	/* Do something... */

	return 0;
}
