#include <stdio.h>
#include <stdlib.h>
#include "../include/jarg.h"

static void	handle_print(int argc, char** argv);

const struct arg jarg_args[] = {
	{ "-p",		0,					handle_print, 0 },
	{ "FILE",	JARGF_REQUIRED|JARGF_ANY_COUNT, 	handle_print, 0 },
};

static void
handle_print(int argc, char** argv)
{
	for ( int i = 0; i < argc; i++ ) {
		printf("%s\n", argv[i]);
	}
}

static void
handle_unrecognised_cmd(char* cmd)
{
	fprintf(stderr, "ERROR: unrecognised arg %s\n", cmd);
	exit(1);
}

int
main(int argc, char** argv)
{
	if ( !jarg_handle_args(LEN(jarg_args), handle_unrecognised_cmd, argc, argv) ) {
		fprintf(stderr, "Error: %s\n", jarg_error_str());
		return 1;
	}
	return 0;
}
