#include <stdio.h>
#include <stdlib.h>
#include "../include/jarg.h"

static void	handle_print(const struct arg* jarg, int argc, char** argv);

const struct arg jarg_args[] = {
	{ "-p",		"file", JARGF_OPT,						handle_print },
	{ "-x",		NULL, 	JARGF_OPT,						handle_print },
	{ "FILE",	NULL, 	JARGF_REQUIRED|JARGF_ANY_COUNT,	handle_print },
};

static void
handle_print(const struct arg* jarg, int argc, char** argv)
{
	printf("[ ");
	for ( int i = 0; i < argc; i++ ) {
		printf("%s -> %s,", jarg->identifier, argv[i]);
	}
	printf(" ]\n");
}

static void
unrecognised_arg(char* cmd)
{
	fprintf(stderr, "ERROR: unrecognised arg %s\n", cmd);
	exit(1);
}

int
main(int argc, char** argv)
{
	if ( argc < 2 ) {
		jarg_print_usage(argv[0], sizeof(jarg_args)/sizeof(jarg_args[0]));
		return 0;
	}

	if ( !jarg_handle_args(
			sizeof(jarg_args)/sizeof(jarg_args[0]), 
			unrecognised_arg, 
			argc, 
			argv
		) 
	) {
		fprintf(stderr, "Error: %s\n", jarg_error_str());
		return 1;
	}

	return 0;
}
